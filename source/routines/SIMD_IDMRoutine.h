#ifndef SMDI_IDM_ROUTINE_H
#define SMDI_IDM_ROUTINE_H

#include <algorithm>
#include <cassert>
#include <cmath>

#include <immintrin.h>

#include "AccelerationComputer.h"
#include "IDMRoutine.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class SIMD_IDMRoutine : public IDMRoutine<RfbStructure> {
  using car_iterator = typename LowLevelStreet<RfbStructure>::iterator;
  using AccelerationComputerRfb = AccelerationComputer<RfbStructure>;
  using LaneChangeValues = typename IDMRoutine<RfbStructure>::LaneChangeValues;

public:
  using IDMRoutine<RfbStructure>::IDMRoutine;

protected:
  void processStreet(LowLevelStreet<RfbStructure> &street) {
    // Initialise acceleration computer for use during computation
    AccelerationComputerRfb accelerationComputer(street);

    car_iterator carIt = street.allIterable().begin();
    for (int i = 0; i < (int)street.getCarCount() - 4; i += 4) {          // for all blocks of four cars on the street
      carIt = compute4BaseAccelerationsSIMD(accelerationComputer, carIt); // compute and set base acceleration
    }
    computeBaseAcceleration(accelerationComputer, carIt); // compute and set base acceleration of all remaining cars

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

      this->computeAndSetDynamics(*carIt, nextAcceleration, carIt->getLane() + laneOffset);
    }
  }

  /**
   * Computes and sets the base acceleration from the given begin to accelerationComputer.end() using the acceleration
   * computer. Returns the iterator of the next car to process (= end).
   * @param[in]  accelerationComputer  The acceleration computer
   * @param[in]  begin                 The car to begin from
   * @return     The iterator of the next car to process (= end)
   */
  car_iterator computeBaseAcceleration(AccelerationComputerRfb accelerationComputer, car_iterator begin) {
    for (car_iterator carIt = begin; accelerationComputer.isNotEnd(carIt); ++carIt) {
      const double baseAcceleration = accelerationComputer(carIt, 0);
      carIt->setNextBaseAcceleration(baseAcceleration);
    }
    return accelerationComputer.end();
  }

  /**
   * Computes four base accelerations from begin to begin + 4 using SIMD, the iterators must exists and be != end.
   * Returns the iterator of the next car to process (= end).
   * @param[in]  accelerationComputer  The acceleration computer
   * @param[in]  begin                 The car to begin from
   * @return     The iterator of the next car to process (= end)
   */
  car_iterator compute4BaseAccelerationsSIMD(AccelerationComputerRfb accelerationComputer, car_iterator begin) {
    LowLevelStreet<RfbStructure> &street = accelerationComputer.getStreet();
    std::array<car_iterator, 4> cars, carsInFront;
    car_iterator carIt = begin;
    for (unsigned i = 0; i < 4; ++i) {
      cars[i]        = carIt;
      carsInFront[i] = street.getNextCarInFront(carIt);
      carIt++;
    }
    std::array<double, 4> accelerations =
        computeAccelerationSIMD(street.getSpeedLimit(), cars, carsInFront, accelerationComputer.end());

    carIt = begin;
    for (unsigned i = 0; i < 4; ++i) { (carIt++)->setNextBaseAcceleration(accelerations[i]); }
    return carIt;
  }

  /**
   * Calculates the acceleration using avx SIMD instructions of the four cars. Ignores end iterator cars.
   *
   * @param[in]  speedLimit      The speed limit on the current street
   * @param[in]  car_it          The iterators of the cars whose acceleration should be computed
   * @param[in]  car_inFront_it  The iterators of the cars in front
   * @param[in]  end             The end iterator
   *
   * @return     The acceleration.
   */
  std::array<double, 4> computeAccelerationSIMD(const double speedLimit, std::array<car_iterator, 4> car_it,
      std::array<car_iterator, 4> car_inFront_it, car_iterator end) const {
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

    for (unsigned i = 0; i < arrayLength; ++i) { // [3] is unused and thus not set
      if (car_it[i] == end) { continue; }        // skip nonexistent cars
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
        inFrontMultiplier[i] = 1;
        inFrontLength[i]     = car_inFront_it[i]->getLength();
        inFrontVelocity[i]   = car_inFront_it[i]->getVelocity();
        inFrontPosition[i]   = car_inFront_it[i]->getDistance();
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
    // current velocity / target velocity
    __m256d vec_unrestrictedDrivingFactor = _mm256_div_pd(vec_currentVelocity, vec_targetVelocity);
    // (current velocity / target velocity)^2
    vec_unrestrictedDrivingFactor = _mm256_mul_pd(vec_unrestrictedDrivingFactor, vec_unrestrictedDrivingFactor);
    // (current velocity / target velocity)^4
    vec_unrestrictedDrivingFactor = _mm256_mul_pd(vec_unrestrictedDrivingFactor, vec_unrestrictedDrivingFactor);
    // 1 - (current velocity / target velocity)^4
    vec_unrestrictedDrivingFactor = _mm256_sub_pd(vec_ONE, vec_unrestrictedDrivingFactor);

    // in front distance - in front length
    __m256d vec_distanceDelta = _mm256_sub_pd(vec_inFrontPosition, vec_inFrontLength);
    // (in front distance - in front length) - own distance
    vec_distanceDelta = _mm256_sub_pd(vec_distanceDelta, vec_currentPosition);

    // own velocity - in front velocity
    __m256d vec_velocityDelta = _mm256_sub_pd(vec_currentVelocity, vec_inFrontVelocity);

    // own velocity * velocity delta
    __m256d vec_fractionInFraction = _mm256_mul_pd(vec_currentVelocity, vec_velocityDelta);
    // (own velocity * velocity delta) / acceleration divisor
    vec_fractionInFraction = _mm256_div_pd(vec_fractionInFraction, vec_accelerationDivisor);

    // velocity * target headway
    __m256d vec_carInFrontFactorDividend = _mm256_mul_pd(vec_currentVelocity, vec_targetHeadway);
    // min distance + (velocity * target headway)
    vec_carInFrontFactorDividend = _mm256_add_pd(vec_minDistance, vec_carInFrontFactorDividend);
    // min distance + (velocity * target headway) + fraction in fraction
    vec_carInFrontFactorDividend = _mm256_add_pd(vec_carInFrontFactorDividend, vec_fractionInFraction);

    // car in front dividend / distance delta
    __m256d vec_carInFrontFactor = _mm256_div_pd(vec_carInFrontFactorDividend, vec_distanceDelta);
    // (car in front dividend / distance delta)^2
    vec_carInFrontFactor = _mm256_mul_pd(vec_carInFrontFactor, vec_carInFrontFactor);
    // set to 0 if there is no car in front
    vec_carInFrontFactor = _mm256_mul_pd(vec_inFrontMultiplier, vec_carInFrontFactor);

    // acceleration from partial results
    // (unrestricted driving factor - car in front factor)
    __m256d vec_acceleration = _mm256_sub_pd(vec_unrestrictedDrivingFactor, vec_carInFrontFactor);
    // max acceleration * (unrestricted driving factor - car in front factor)
    vec_acceleration = _mm256_mul_pd(vec_acceleration, vec_maxAcceleration);

    // store results in array and return
    std::array<double, arrayLength> accelerations;
    _mm256_store_pd(&accelerations[0], vec_acceleration);
    return accelerations;
  }

  LaneChangeValues computeLaneChangeValues(
      AccelerationComputerRfb accelerationComputer, car_iterator carIt, const int laneOffset) const {
    LowLevelStreet<RfbStructure> &street = accelerationComputer.getStreet();

    // PENDING [computeIsSpaceTrafficLightCar]

    // Retrieve next car behind the car in question if a lane change would take place.
    car_iterator laneChangeCarBehindIt = street.getNextCarBehind(carIt, laneOffset);
    // Retrieve next car in front of the car in question if a lane change would take place.
    car_iterator laneChangeCarInFrontIt = street.getNextCarInFront(carIt, laneOffset);

    if (!this->computeIsSpace(accelerationComputer, carIt, laneChangeCarBehindIt, laneChangeCarInFrontIt))
      return LaneChangeValues();

    // Retrieve next car in front of the car in question (no lane change).
    car_iterator carInFrontIt = street.getNextCarInFront(carIt, 0);
    // Retrieve next car behind the car in question (no lane change).
    car_iterator carBehindIt = street.getNextCarBehind(carIt, 0);

    // compute without SIMD if only one acceleration is computed (because there are no cars behind on both lanes)
    std::array<double, 4> accelerations = {0, 0, 0, 0};
    if (accelerationComputer.isEnd(carBehindIt) && accelerationComputer.isEnd(laneChangeCarBehindIt)) {
      accelerations[0] = accelerationComputer(carIt, laneChangeCarInFrontIt);
    } else { // use SIMD to compute two or more accelerations
      std::array<car_iterator, 4> cars        = {carIt, carBehindIt, laneChangeCarBehindIt, accelerationComputer.end()};
      std::array<car_iterator, 4> carsInFront = {
          laneChangeCarInFrontIt, carInFrontIt, laneChangeCarInFrontIt, accelerationComputer.end()};
      accelerations = computeAccelerationSIMD(street.getSpeedLimit(), cars, carsInFront, accelerationComputer.end());
    }

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
};

#endif
