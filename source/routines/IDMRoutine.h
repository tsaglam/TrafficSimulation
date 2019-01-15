#ifndef NULL_ROUTINE_H
#define NULL_ROUTINE_H

#include <algorithm>
#include
<cassert>
#include <cmath>

#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"

    template <template <typename Vehicle> typename RfbStructure>
    class IDMRoutine {
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
    LowLevelStreet<RfbStructure>::iterator endIt = street.allIterable().end();

    for (LowLevelStreet<RfbStructure>::iterator carIt = street.allIterable().begin(); carIt != endIt; ++carIt) {
      const double baseAcceleration = computeAcceleration(*carIt, street.getNextCarInFront(carIt, 0));
      carIt->setNextBaseAcceleration(baseAcceleration);
    }

    for (LowLevelStreet<RfbStructure>::iterator carIt = street.allIterable().begin(); carIt != endIt; ++carIt) {
      std::pair<bool, double> leftLaneAcceleration(false, 0.0);
      std::pair<bool, double> rightLaneAcceleration(false, 0.0);

      if (carIt->getLane() >= 0) { // if not outermost left lane
        // Check if there is space
        const bool isSpace = computeIsSpace(street, carIt, -1);

        // Compute left lane acceleration
        leftLaneAcceleration.first_type  = true;
        leftLaneAcceleration.second_type = computeAcceleration(*carIt, street.getNextCarInFront(carIt, -1));
      }
      if (lane < street.getLaneCount()) { // if not outermost right lane
        // Check if there is place
        const bool isSpace = computeIsSpace(street, carIt, +1);

        // Compute right lane acceleration
        rightLaneAcceleration.first_type  = true;
        rightLaneAcceleration.second_type = computeAcceleration(*carIt, street.getNextCarInFront(carIt, +1));
      }
    }
  }

  bool computeIsSpace(
      LowLevelStreet<RfbStructure> &street, LowLevelStreet<RfbStructure>::iterator carIt, const int laneOffset) const {

    // Retrieve next car behind the car in question which is not a "special" car (traffic light car)
    LowLevelStreet<RfbStructure>::iterator carBehindIt = street.getNextCarBehind(carIt, laneOffset);
    assert(!carBehindIt.isSpecial() || "Unexpected special iterator: Car behind should never be a special car!");

    if (carIt->getDistance() - carIt->getLength() < carBehindIt->getDistance() + carIt->getMinDistance()) return false;

    // Retrieve next car in front of the car in question which is not a "special" car (traffic light car)
    LowLevelStreet<RfbStructure>::iterator carInFrontIt = carIt;
    do { carInFrontIt = street.getNextCarInFront(carInFrontIt, laneOffset); } while (carInFrontIt.isSpecial());

    if (carInFrontIt->getDistance() - carInFrontIt->getLength() < carIt->getDistance() + carIt->getMinDistance())
      return false;

    return true;
  }

  void computeAndSetDynamics(LowLevelCar &car, const double nextAcceleration, const unsigned int nextLane) {
    const double nextVelocity = std::max(car.getVelocity() + nextAcceleration, 0);
    const double nextDistance = car.getDistance() + newVelocity;
    car.setNext(nextLane, nextDistance, nextVelocity);
  }

  double computeAcceleration(const LowLevelCar &origin, const LowLevelCar &inFront) const {
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
