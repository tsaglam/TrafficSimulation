#ifndef P_IDM_ROUTINE_H
#define P_IDM_ROUTINE_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <thread>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "AccelerationComputer.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"
#include "Timer.h"

template <template <typename Vehicle> typename RfbStructure>
class ParallelIDMRoutine {
private:
  using car_iterator = typename LowLevelStreet<RfbStructure>::iterator;
  using AccelerationComputerRfb = AccelerationComputer<RfbStructure>;

private:
  class LaneChangeValues {
  public:
    bool valid;          // if true, the further fields are valid
    double acceleration; // a in case of a lane change
    double indicator;    // m_alpha

    LaneChangeValues() : valid(false), acceleration(0.0), indicator(0.0) {}
    LaneChangeValues(double _acceleration, double _indicator)
        : valid(true), acceleration(_acceleration), indicator(_indicator) {}
  };

private:
  const unsigned long PARALLEL_THRESHOLD = 100;
  SimulationData<RfbStructure> &data;
  std::vector<unsigned int> carWise;
  std::vector<unsigned int> streetWise;

public:
  ParallelIDMRoutine(SimulationData<RfbStructure> &_data) : data(_data) {}
  void perform() {
#ifdef TIMER
    IDMRoutine_thresholdSorting_timer.start();
#endif
    for (auto &street : data.getStreets()) {
      unsigned int carCount = street.getCarCount();
      if (carCount > PARALLEL_THRESHOLD) {
        carWise.push_back(street.getId());
      } else if (carCount > 0) { // only push non-empty streets
        streetWise.push_back(street.getId());
      }
    }

#ifdef TIMER
    IDMRoutine_thresholdSorting_timer.stop();
    IDMRoutine_performStreetWise_timer.start();
    performStreetWise(streetWise);
    IDMRoutine_performStreetWise_timer.stop();
    IDMRoutine_performCarWise_timer.start();
    performCarWise(carWise);
    IDMRoutine_performCarWise_timer.stop();
#else
    performStreetWise(streetWise);
    performCarWise(carWise);
#endif

    carWise.clear();
    streetWise.clear();
  }

  void performSequential(std::vector<unsigned int> &streetIds) {
    for (auto streetId : streetIds) {
      auto &street = data.getStreet(streetId);
      // Initialise acceleration computer for use during computation
      AccelerationComputer accelerationComputer(street);
      // compute all accelerations:
      for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
        const double baseAcceleration = accelerationComputer(carIt, 0);
        carIt->setNextBaseAcceleration(baseAcceleration);
      }
      for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
        processLaneDecision(carIt, street);
      }
    }
  }

private:
  void performStreetWise(std::vector<unsigned int> &streetIds) {
// #ifdef _OPENMP
//     unsigned int customBlockSize = streetIds.size() / omp_get_max_threads();
// #endif
#pragma omp parallel for shared(data) schedule(static) //, customBlockSize)
    for (std::size_t i = 0; i < streetIds.size(); i++) {
      // get the right street
      auto &street = data.getStreet(streetIds[i]);
      // Initialise acceleration computer for use during computation
      AccelerationComputerRfb accelerationComputer(street);
      // compute all accelerations:
      for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
        const double baseAcceleration = accelerationComputer(carIt, 0);
        carIt->setNextBaseAcceleration(baseAcceleration);
      }
      for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
        processLaneDecision(carIt, street);
      }
    }
  }

  void performCarWise(std::vector<unsigned int> &streetIds) {
    for (auto streetId : streetIds) {
      auto &street = data.getStreet(streetId);
      // Initialise acceleration computer for use during computation
      AccelerationComputerRfb accelerationComputer(street);
      // compute all accelerations:
      auto streetIterable = street.allIterable();
#pragma omp parallel for shared(street) schedule(static)
      for (unsigned i = 0; i < street.getCarCount(); ++i) {
        auto carIt                    = streetIterable.begin() + i;
        const double baseAcceleration = accelerationComputer(carIt, 0);
        carIt->setNextBaseAcceleration(baseAcceleration);
      }
#pragma omp parallel for shared(street) schedule(static)
      for (unsigned i = 0; i < street.getCarCount(); ++i) {
        auto carIt = streetIterable.begin() + i;
        processLaneDecision(carIt, street);
      }
    }
  }

  void processLaneDecision(car_iterator &carIt, LowLevelStreet<RfbStructure> &street) {
    LaneChangeValues leftLaneChange;
    LaneChangeValues rightLaneChange;

    if (carIt->getLane() > 0) // if not outermost left lane
      leftLaneChange = computeLaneChangeValues(street, carIt, -1);
    if (carIt->getLane() < street.getLaneCount() - 1) // if not outermost right lane
      rightLaneChange = computeLaneChangeValues(street, carIt, +1);

    double laneOffset       = 0;
    double nextAcceleration = carIt->getNextBaseAcceleration();
    if (leftLaneChange.valid) {
      if (rightLaneChange.valid && rightLaneChange.indicator > leftLaneChange.indicator) {
        laneOffset       = +1;
        nextAcceleration = rightLaneChange.acceleration;
      } else {
        laneOffset       = -1;
        nextAcceleration = leftLaneChange.acceleration;
      }
    } else if (rightLaneChange.valid) {
      laneOffset       = +1;
      nextAcceleration = rightLaneChange.acceleration;
    }

    computeAndSetDynamics(*carIt, nextAcceleration, carIt->getLane() + laneOffset);
  }

  LaneChangeValues computeLaneChangeValues(
      AccelerationComputerRfb accelerationComputer, car_iterator carIt, const int laneOffset) {
    LowLevelStreet<RfbStructure> &street = accelerationComputer.getStreet();

    // Retrieve next car behind the car in question if a lane change would take place.
    car_iterator laneChangeCarBehindIt = street.getNextCarBehind(carIt, laneOffset);
    // Retrieve next car in front of the car in question if a lane change would take place.
    car_iterator laneChangeCarInFrontIt = street.getNextCarInFront(carIt, laneOffset);

    if (!computeIsSpace(accelerationComputer, carIt, laneChangeCarBehindIt.getThisOrNotSpecialCarBehind(),
            laneChangeCarInFrontIt.getThisOrNotSpecialCarInFront()))
      return LaneChangeValues();

    const double acceleration = accelerationComputer(carIt, laneChangeCarInFrontIt);

    // If the acceleration after a lane change is smaller equal the base acceleration, don't indicate lane change
    if (acceleration <= carIt->getNextBaseAcceleration()) return LaneChangeValues();

    // Compute acceleration deltas of cars behind the car in question.
    // This delta is used in the calculation of the lane change indicator.
    double carBehindAccelerationDeltas = 0.0;

    // Retrieve next car in front of the car in question (no lange change).
    car_iterator carInFrontIt = street.getNextCarInFront(carIt, 0);
    // Retrieve next car behind the car in question (no lange change).
    car_iterator carBehindIt = street.getNextCarBehind(carIt, 0);

    if (accelerationComputer.isNotEnd(carBehindIt)) {
      // If there is a car behind, then consider it in the acceleration delta
      const double carBehindAcceleration = accelerationComputer(carBehindIt, carInFrontIt);
      carBehindAccelerationDeltas += carBehindAcceleration - carBehindIt->getNextBaseAcceleration();
    }

    if (accelerationComputer.isNotEnd(laneChangeCarBehindIt)) {
      // If there is a car behind, then consider it in the acceleration delta
      const double laneChangeCarBehindAcceleration = accelerationComputer(*laneChangeCarBehindIt, &*carIt);
      carBehindAccelerationDeltas += laneChangeCarBehindAcceleration - laneChangeCarBehindIt->getNextBaseAcceleration();
    }

    const double indicator =
        acceleration - carIt->getNextBaseAcceleration() + carIt->getPoliteness() * carBehindAccelerationDeltas;

    // If the indicator is smaller equal 1.0, don't indicate lane change
    if (indicator <= 1.0) return LaneChangeValues();

    return LaneChangeValues(acceleration, indicator);
  }

  bool computeIsSpace(AccelerationComputerRfb accelerationComputer, car_iterator carIt, car_iterator carBehindIt,
      car_iterator carInFrontIt) const {

    if (accelerationComputer.isNotEnd(carBehindIt) &&
        carIt->getDistance() - carIt->getLength() < carBehindIt->getDistance() + carIt->getMinDistance())
      return false;

    if (accelerationComputer.isNotEnd(carInFrontIt) &&
        carInFrontIt->getDistance() - carInFrontIt->getLength() < carIt->getDistance() + carIt->getMinDistance())
      return false;

    return true;
  }

  void computeAndSetDynamics(LowLevelCar &car, const double nextAcceleration, const unsigned int nextLane) {
    const double nextVelocity = std::max(car.getVelocity() + nextAcceleration, 0.0);
    const double nextDistance = car.getDistance() + nextVelocity;
    car.setNext(nextLane, nextDistance, nextVelocity);
    car.updateTravelDistance(nextVelocity); // in this step, the car traveled a distance of 'nextVelocity' meters
  }
};

#endif
