#ifndef P_IDM_ROUTINE_H
#define P_IDM_ROUTINE_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <thread>

#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class ParallelIDMRoutine {
private:
  using car_iterator = typename LowLevelStreet<RfbStructure>::iterator;

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

  class AccelerationComputer {
  private:
    LowLevelStreet<RfbStructure> &street;
    car_iterator endIt;

  public:
    AccelerationComputer(LowLevelStreet<RfbStructure> &_street) : street(_street), endIt(_street.allIterable().end()) {}

    double operator()(const car_iterator &carIt, const int laneOffset) const {
      return computeAcceleration(carIt, laneOffset);
    }

    double operator()(const car_iterator &carIt, const car_iterator &carInFrontIt) const {
      return computeAcceleration(carIt, carInFrontIt);
    }

    double operator()(const LowLevelCar &car, const LowLevelCar *inFront) const {
      return computeAcceleration(car, inFront);
    }

    double computeAcceleration(const car_iterator &carIt, const int laneOffset) const {
      return computeAcceleration(carIt, street.getNextCarInFront(carIt, laneOffset));
    }

    double computeAcceleration(const car_iterator &carIt, const car_iterator &carInFrontIt) const {
      LowLevelCar *carInFrontPtr;
      if (isEnd(carInFrontIt))
        carInFrontPtr = nullptr;
      else
        carInFrontPtr = &*carInFrontIt;

      return computeAcceleration(*carIt, carInFrontPtr);
    }

    double computeAcceleration(const LowLevelCar &car, const LowLevelCar *inFront) const {
      const double targetVelocity = std::min(car.getTargetVelocity(), street.getSpeedLimit());

      // Captures constraints of target velocity, no consideration of car in front ("freie fahrt")
      const double unrestrictedDrivingFactor = 1.0 - std::pow(car.getVelocity() / targetVelocity, 4);

      double carInFrontFactor = 0.0;
      if (inFront != nullptr) {
        // Distance between the car and the car in front of it.
        const double distanceDelta = inFront->getDistance() - inFront->getLength() - car.getDistance();
        // Difference of velocity between the car and the car in front of it.
        const double velocityDelta = car.getVelocity() - inFront->getVelocity();

        // clang-format off
        const double fractionInFraction = (
          (
            car.getVelocity() * velocityDelta
          ) / (
            car.getAccelerationDivisor()
          )
        );
        // clang-format on

        const double carInFrontFactorDividend =
            car.getMinDistance() + car.getVelocity() * car.getTargetHeadway() + fractionInFraction;

        // Captures constraints imposed by car in front
        carInFrontFactor = std::pow(carInFrontFactorDividend / distanceDelta, 2);
      }

      return car.getMaxAcceleration() * (unrestrictedDrivingFactor - carInFrontFactor);
    }

    car_iterator end() const { return endIt; }
    bool isEnd(const car_iterator &it) const { return it == endIt; }
    bool isNotEnd(const car_iterator &it) const { return it != endIt; }

    LowLevelStreet<RfbStructure> &getStreet() const { return street; }
  };

private:
  const unsigned long PARALLEL_THRESHOLD = 50;
  SimulationData<RfbStructure> &data;

public:
  ParallelIDMRoutine(SimulationData<RfbStructure> &_data) : data(_data) {}
  void perform() {
    std::vector<unsigned int> carWise;
    std::vector<unsigned int> streetWise;
    for (auto &street : data.getStreets()) {
      unsigned int carCount = street.getCarCount();
      if (carCount > PARALLEL_THRESHOLD) {
        carWise.push_back(street.getId());
      } else if (carCount > 0) { // only push non-empty streets
        streetWise.push_back(street.getId());
      }
    }
    performStreetWise(streetWise);
    performCarWise(carWise);
  }

private:
  void performStreetWise(std::vector<unsigned int> &streetIds) {
#ifdef _OPENMP
    unsigned int customBlockSize = streetIds.size() / std::thread::hardware_concurrency();
#endif
#pragma omp parallel for shared(data) schedule(static, customBlockSize)
    for (std::size_t i = 0; i < streetIds.size(); i++) {
      // get the right street
      auto &street = data.getStreet(streetIds[i]);
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

  void performCarWise(std::vector<unsigned int> &streetIds) {
    for (auto streetId : streetIds) {
      auto &street = data.getStreet(streetId);
      // Initialise acceleration computer for use during computation
      AccelerationComputer accelerationComputer(street);
      // compute all accelerations:
      auto streetIterable = street.allIterable();
#pragma omp parallel for shared(street)
      for (unsigned i = 0; i < street.getCarCount(); ++i) {
        auto carIt                    = streetIterable.begin() + i;
        const double baseAcceleration = accelerationComputer(carIt, 0);
        carIt->setNextBaseAcceleration(baseAcceleration);
      }
#pragma omp parallel for shared(street)
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
      AccelerationComputer accelerationComputer, car_iterator carIt, const int laneOffset) {
    LowLevelStreet<RfbStructure> &street = accelerationComputer.getStreet();

    // PENDING [computeIsSpaceTrafficLightCar]

    // Retrieve next car behind the car in question if a lane change would take place.
    car_iterator laneChangeCarBehindIt = street.getNextCarBehind(carIt, laneOffset);
    // Retrieve next car in front of the car in question if a lane change would take place.
    car_iterator laneChangeCarInFrontIt = street.getNextCarInFront(carIt, laneOffset);

    if (!computeIsSpace(accelerationComputer, carIt, laneChangeCarBehindIt, laneChangeCarInFrontIt))
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

  bool computeIsSpace(AccelerationComputer accelerationComputer, car_iterator carIt, car_iterator carBehindIt,
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
