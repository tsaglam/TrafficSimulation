#ifndef SMDI_IDM_ROUTINE_H
#define SMDI_IDM_ROUTINE_H

#include <algorithm>
#include <cassert>
#include <cmath>

#include <iomanip>  // TODO remove
#include <iostream> // TODO remove

#include <immintrin.h>

#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class SMDI_IDMRoutine {
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
  SimulationData<RfbStructure> &data;

public:
  SMDI_IDMRoutine(SimulationData<RfbStructure> &_data) : data(_data) {}
  void perform() {
    for (auto &street : data.getStreets()) { processStreet(street); }
  }

private:
  void processStreet(LowLevelStreet<RfbStructure> &street) {
    // Initialise acceleration computer for use during computation
    AccelerationComputer accelerationComputer(street);

    for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
      const double baseAcceleration = accelerationComputer(carIt, 0);
      carIt->setNextBaseAcceleration(baseAcceleration);
    }

    for (car_iterator carIt = street.allIterable().begin(); accelerationComputer.isNotEnd(carIt); ++carIt) {
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
  }

  std::array<double, 4> computeLaneChangeAccelerationSIMD(const double speedLimit, std::array<car_iterator, 3> car_it,
      std::array<car_iterator, 3> car_inFront_it, car_iterator end) const {
    // Compute the acceleration of the car on the new lane and its behind neighbors on the old and new lane in case of a
    // lane change using SIMD instructions.
    // [0]: car itself, [1]: behind on old lane, [2]: behind on new lane, [3]: unused

    constexpr unsigned arrayLength = 4;
    std::array<double, arrayLength> ONE;
    std::array<double, arrayLength> inFrontMultiplier; // set to 0 if no car in front, 1 otherwise
    std::array<double, arrayLength> maxAcceleration;
    std::array<double, arrayLength> currentVelocity;
    std::array<double, arrayLength> targetVelocity;
    std::array<double, arrayLength> minDistance;
    std::array<double, arrayLength> targetHeadway;
    std::array<double, arrayLength> accelerationDivisor;
    std::array<double, arrayLength> currentPosition;
    std::array<double, arrayLength> inFrontLength;
    std::array<double, arrayLength> inFrontVelocity;
    std::array<double, arrayLength> inFrontPosition;

    // fill input arrays
    for (unsigned i = 0; i < arrayLength; ++i) { ONE[i] = 1; }

    for (unsigned i = 0; i < arrayLength - 1; ++i) { // [3] is unused and thus not set
      if (car_it[i] == end) { continue; }            // skip nonexistent cars
      maxAcceleration[i]     = car_it[i]->getMaxAcceleration();
      currentVelocity[i]     = car_it[i]->getVelocity();
      targetVelocity[i]      = std::min(car_it[i]->getTargetVelocity(), speedLimit);
      minDistance[i]         = car_it[i]->getMinDistance();
      targetHeadway[i]       = car_it[i]->getTargetHeadway();
      accelerationDivisor[i] = car_it[i]->getAccelerationDivisor();
      currentPosition[i]     = car_it[i]->getDistance();

      if (car_inFront_it[i] == end) {
        inFrontMultiplier[i] = 0;
        inFrontLength[i]     = 1;
        inFrontVelocity[i]   = 1;
        inFrontPosition[i]   = 1;
      } else {
        inFrontLength[i]     = car_inFront_it[i]->getLength();
        inFrontVelocity[i]   = car_inFront_it[i]->getVelocity();
        inFrontPosition[i]   = car_inFront_it[i]->getDistance();
        inFrontMultiplier[i] = 1;
      }
    }

    // load vectors
    __m256d vec_ONE                 = _mm256_loadu_pd(&ONE[0]);
    __m256d vec_inFrontMultiplier   = _mm256_loadu_pd(&inFrontMultiplier[0]);
    __m256d vec_maxAcceleration     = _mm256_loadu_pd(&maxAcceleration[0]);
    __m256d vec_currentVelocity     = _mm256_loadu_pd(&currentVelocity[0]);
    __m256d vec_targetVelocity      = _mm256_loadu_pd(&targetVelocity[0]);
    __m256d vec_minDistance         = _mm256_loadu_pd(&minDistance[0]);
    __m256d vec_targetHeadway       = _mm256_loadu_pd(&targetHeadway[0]);
    __m256d vec_accelerationDivisor = _mm256_loadu_pd(&accelerationDivisor[0]);
    __m256d vec_currentPosition     = _mm256_loadu_pd(&currentPosition[0]);
    __m256d vec_inFrontLength       = _mm256_loadu_pd(&inFrontLength[0]);
    __m256d vec_inFrontVelocity     = _mm256_loadu_pd(&inFrontVelocity[0]);
    __m256d vec_inFrontPosition     = _mm256_loadu_pd(&inFrontPosition[0]);

    // compute partial results
    __m256d vec_velocityTargetHeadwayMultiplier = _mm256_mul_pd(vec_currentVelocity, vec_targetHeadway);

    __m256d vec_nestedFraction = _mm256_sub_pd(vec_currentVelocity, vec_inFrontVelocity);
    vec_nestedFraction         = _mm256_mul_pd(vec_currentVelocity, vec_nestedFraction);
    vec_nestedFraction         = _mm256_div_pd(vec_nestedFraction, vec_accelerationDivisor);

    __m256d vec_denominatorSum = _mm256_add_pd(vec_minDistance, vec_velocityTargetHeadwayMultiplier);
    vec_denominatorSum         = _mm256_add_pd(vec_denominatorSum, vec_nestedFraction);

    __m256d vec_distanceToInFront = _mm256_sub_pd(vec_currentPosition, vec_inFrontPosition);
    vec_distanceToInFront         = _mm256_sub_pd(vec_distanceToInFront, vec_inFrontLength);

    __m256d vec_secondFraction        = _mm256_div_pd(vec_denominatorSum, vec_distanceToInFront);
    __m256d vec_secondFractionSquared = _mm256_mul_pd(vec_secondFraction, vec_secondFraction);
    vec_secondFractionSquared         = _mm256_mul_pd(vec_secondFractionSquared, vec_inFrontMultiplier);

    __m256d vec_firstFraction     = _mm256_div_pd(vec_currentVelocity, vec_targetVelocity);
    __m256d vec_firstFractionPow4 = _mm256_mul_pd(vec_firstFraction, vec_firstFraction);
    vec_firstFractionPow4         = _mm256_mul_pd(vec_firstFractionPow4, vec_firstFractionPow4);

    // acceleration from partial results
    __m256d vec_acceleration = _mm256_sub_pd(vec_ONE, vec_firstFractionPow4);
    vec_acceleration         = _mm256_sub_pd(vec_acceleration, vec_secondFractionSquared);
    vec_acceleration         = _mm256_mul_pd(vec_acceleration, vec_maxAcceleration);

    // store results in array and return
    std::array<double, arrayLength> accelerations;
    _mm256_store_pd(&accelerations[0], vec_acceleration);
    return accelerations;
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

    // Retrieve next car in front of the car in question (no lane change).
    car_iterator carInFrontIt = street.getNextCarInFront(carIt, 0);
    // Retrieve next car behind the car in question (no lane change).
    car_iterator carBehindIt = street.getNextCarBehind(carIt, 0);

    // TODO decide when to use SIMD
    std::array<car_iterator, 3> cars        = {carIt, carBehindIt, laneChangeCarBehindIt};
    std::array<car_iterator, 3> carsInFront = {laneChangeCarInFrontIt, carInFrontIt, laneChangeCarInFrontIt};
    std::array<double, 4> accelerations =
        computeLaneChangeAccelerationSIMD(street.getSpeedLimit(), cars, carsInFront, accelerationComputer.end());

    // If the acceleration after a lane change is smaller equal the base acceleration, don't indicate lane change
    if (accelerations[0] <= carIt->getNextBaseAcceleration()) return LaneChangeValues();

    // Compute acceleration deltas of cars behind the car in question.
    // This delta is used in the calculation of the lane change indicator.
    double carBehindAccelerationDeltas = 0.0;

    if (accelerationComputer.isNotEnd(carBehindIt)) {
      // If there is a car behind, then consider it in the acceleration delta
      carBehindAccelerationDeltas += accelerations[1] - carBehindIt->getNextBaseAcceleration();
    }

    if (accelerationComputer.isNotEnd(laneChangeCarBehindIt)) {
      // If there is a car behind, then consider it in the acceleration delta
      carBehindAccelerationDeltas += accelerations[2] - laneChangeCarBehindIt->getNextBaseAcceleration();
    }

    const double indicator =
        accelerations[0] - carIt->getNextBaseAcceleration() + carIt->getPoliteness() * carBehindAccelerationDeltas;

    // If the indicator is smaller equal 1.0, don't indicate lane change
    if (indicator <= 1.0) return LaneChangeValues();

    return LaneChangeValues(accelerations[0], indicator);
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
