#ifndef ACCELERATION_COMPUTER_H
#define ACCELERATION_COMPUTER_H

#include <cmath>

#include "LowLevelCar.h"
#include "LowLevelStreet.h"

template <template <typename Vehicle> typename RfbStructure>
class AccelerationComputer {
  using car_iterator = typename LowLevelStreet<RfbStructure>::iterator;
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

#endif