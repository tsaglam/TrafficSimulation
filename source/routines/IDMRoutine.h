#ifndef IDM_ROUTINE_H
#define IDM_ROUTINE_H

#include <algorithm>
#include <cassert>
#include <cmath>

#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class IDMRoutine {
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

    bool isEnd(const car_iterator &it) const { return it == endIt; }
  };

private:
  SimulationData<RfbStructure> &data;

public:
  IDMRoutine(SimulationData<RfbStructure> &_data) : data(_data) {}
  void perform() {
    for (auto &street : data.getStreets()) { processStreet(street); }
  }

private:
  void processStreet(LowLevelStreet<RfbStructure> &street) {
    // Initialise end iterator for for loops during computation
    car_iterator endIt = street.allIterable().end();

    for (car_iterator carIt = street.allIterable().begin(); carIt != endIt; ++carIt) {
      // TODO
      // Extract into AccelerationComputer
      // BEGIN [Extract into AccelerationComputer]

      car_iterator carInFrontIt = street.getNextCarInFront(carIt, 0);

      LowLevelCar *carInFrontPtr;
      if (carInFrontIt == endIt)
        carInFrontPtr = nullptr;
      else
        carInFrontPtr = *carInFrontIt;

      const double baseAcceleration = computeAcceleration(*carIt, carInFrontPtr);
      // END [Extract into AccelerationComputer]

      carIt->setNextBaseAcceleration(baseAcceleration);
    }

    for (car_iterator carIt = street.allIterable().begin(); carIt != endIt; ++carIt) {
      LaneChangeValues leftLaneChange;
      LaneChangeValues rightLaneChange;

      if (carIt->getLane() >= 0) // if not outermost left lane
        leftLaneChange = computeLaneChangeValues(street, carIt, -1);
      if (carIt->getLane() < street.getLaneCount()) // if not outermost right lane
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
  }

  LaneChangeValues computeLaneChangeValues(
      LowLevelStreet<RfbStructure> &street, car_iterator carIt, const int laneOffset) {
    // TODO OPTIMISATION
    // Cache car in front, car behind

    if (!computeIsSpace(street, carIt, laneOffset)) return LaneChangeValues();

    // TODO
    // Call AccelerationComputer
    // BEGIN [Call AccelerationComputer]

    car_iterator carInFrontIt = street.getNextCarInFront(carIt, laneOffset);

    LowLevelCar *carInFrontPtr;
    // TODO
    // if (accelerationComputer.isEnd(carInFrontIt))
    if (carInFrontIt == street.allIterable().end())
      carInFrontPtr = nullptr;
    else
      carInFrontPtr = *carInFrontIt;

    const double acceleration = computeAcceleration(*carIt, carInFrontPtr);
    // END [Call AccelerationComputer]

    // Compute acceleration deltas of cars behind the car in question.
    // This delta is used in the calculation of the lane change indicator.
    double carBehindAccelerationDeltas = 0.0;

    // Retrieve next car behind the car in question (no lange change). This should not be a "special" car (traffic light
    // car)
    car_iterator carBehindIt = street.getNextCarBehind(carIt, 0);
    assert(!carBehindIt.isSpecial() || "Unexpected special iterator: Car behind should never be a special car!");

    if (carBehindIt == street.allIterable().end()) {
      // TODO
      // end() behaviour: What if carBehindIt is end()?
      // Probably disregard.
    } else {
      const double carBehindAcceleration = computeAcceleration(*carBehindIt, &*carIt);
      carBehindAccelerationDeltas += carBehindAcceleration - carBehindIt->getNextBaseAcceleration();
    }

    // Retrieve next car behind the car in question if a lane change would take place. This should not be a "special"
    // car (traffic light car)
    car_iterator langeChangeCarBehindIt = street.getNextCarBehind(carIt, laneOffset);
    assert(!langeChangeCarBehindIt.isSpecial() ||
           "Unexpected special iterator: Car behind should never be a special car!");

    if (langeChangeCarBehindIt == street.allIterable().end()) {
      // TODO
      // end() behaviour: What if langeChangeCarBehindIt is end()?
      // Probably disregard.
    } else {
      const double laneChangeCarBehindAcceleration = computeAcceleration(*langeChangeCarBehindIt, &*carIt);
      carBehindAccelerationDeltas +=
          laneChangeCarBehindAcceleration - langeChangeCarBehindIt->getNextBaseAcceleration();
    }

    const double indicator =
        acceleration - carIt->getNextBaseAcceleration() + carIt->getPoliteness() * carBehindAccelerationDeltas;

    // If the indicator is smaller equal 1.0, don't indicate lane change
    if (indicator <= 1.0) return LaneChangeValues();

    return LaneChangeValues(acceleration, indicator);
  }

  bool computeIsSpace(LowLevelStreet<RfbStructure> &street, car_iterator carIt, const int laneOffset) const {

    // TODO
    // end() behaviour
    // should endIt be passed?

    // Retrieve next car behind the car in question. This should not be a "special" car (traffic light car)
    car_iterator carBehindIt = street.getNextCarBehind(carIt, laneOffset);
    assert(!carBehindIt.isSpecial() || "Unexpected special iterator: Car behind should never be a special car!");

    if (carIt->getDistance() - carIt->getLength() < carBehindIt->getDistance() + carIt->getMinDistance()) return false;

    // Retrieve next car in front of the car in question which is not a "special" car (traffic light car)
    car_iterator carInFrontIt = carIt;
    do { carInFrontIt = street.getNextCarInFront(carInFrontIt, laneOffset); } while (carInFrontIt.isSpecial());

    if (carInFrontIt->getDistance() - carInFrontIt->getLength() < carIt->getDistance() + carIt->getMinDistance())
      return false;

    return true;
  }

  void computeAndSetDynamics(LowLevelCar &car, const double nextAcceleration, const unsigned int nextLane) {
    const double nextVelocity = std::max(car.getVelocity() + nextAcceleration, 0.0);
    const double nextDistance = car.getDistance() + nextVelocity;
    car.setNext(nextLane, nextDistance, nextVelocity);
  }

  /**
   * Computes the acceleration of a car with respect to the car immediately in front of it.
   *
   * TODO this method should be integrated e.g. with computeAcceleration_formula
   */
  double computeAcceleration(const LowLevelCar &origin, const LowLevelCar *inFront) {
    // TODO
    // What is the acceleration when there is no car in front?
    // Probably use the formula with carInFrontFactor set to 0

    if (inFront == nullptr) {
      return 0.0;
    } else {
      return computeAcceleration_formula(origin, *inFront);
    }
  }

  double computeAcceleration_formula(const LowLevelCar &origin, const LowLevelCar &inFront) const {
    // Distance between the car and the car in front of it.
    const double distanceDelta = inFront.getDistance() - origin.getDistance() - inFront.getLength();
    // Difference of velocity between the car and the car in front of it.
    const double velocityDelta = origin.getVelocity() - inFront.getVelocity();

    // Captures constraints of target velocity, no consideration of car in front ("freie fahrt")
    const double unrestrictedDrivingFactor = 1.0 - std::pow(origin.getVelocity() / origin.getTargetVelocity(), 4);

    // clang-format off
    const double fractionInFraction = (
      (
        origin.getVelocity() * velocityDelta
      ) / (
        2.0 * std::sqrt(
          origin.getMaxAcceleration() * origin.getTargetDeceleration()
        )
      )
    );
    // clang-format on

    const double carInFrontFactorDividend =
        origin.getMinDistance() + origin.getVelocity() * origin.getTargetHeadway() + fractionInFraction;

    // Captures constraints imposed by car in front
    const double carInFrontFactor = std::pow(carInFrontFactorDividend / distanceDelta, 2);

    return origin.getMaxAcceleration() * (unrestrictedDrivingFactor - carInFrontFactor);
  }
};

#endif
