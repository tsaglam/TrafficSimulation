#ifndef OPTIMIZATION_ROUTINE_H
#define OPTIMIZATION_ROUTINE_H

#include <vector>

#include "DomainModel.h"
#include "DomainModelCommon.h"
#include "Junction.h"
#include "RfbStructureTraits.h"
#include "SimulationData.h"

/**
 * Routine to improve the duration and order of the traffic light signals to increase the total distance traveled by all
 * cars.
 *
 * @tparam     RfbStructure  The underlying data structure of the low level street
 *
 * Information for implementing alternative optimization routines:
 * The following functions are required:
 * OptimizationRoutine(SimulationData<RfbStructure> &data): constructor used by the simulator
 * void perform(): is called once in each simulation step after the IDM and before the consistency routine
 * void improveTrafficLights(): is called once per optimization cycle and should improve the traffic light signals in
 * the domain model junctions to increase the total travel distance of all cars. May use information previously
 * collected by perform() during the simulation.
 * The OptimizationRoutine is reinitialized after each optimization cycle.
 */
template <template <typename Vehicle> typename RfbStructure>
class OptimizationRoutine {
  using reverse_category = typename rfbstructure_traits<RfbStructure>::reverse_category;
  using Vehicle          = LowLevelCar;

private:
  SimulationData<RfbStructure> &data;
  // contains for each junction a vector of requested green light directions per time step
  std::vector<std::vector<CardinalDirection>> requestedGreenLights;
  const double trafficLightZoneMultiplier = 10.0; // TODO choose parameters
  // Is multiplied with the speed limit to determine the size of the traffic light zone

  const double relativeRescaleDurationLimit = 0;

  // Simplified getters for the street and low level street by id
  const Street &getStreet(const unsigned streetId) const { return data.getDomainModel().getStreet(streetId); }
  const LowLevelStreet<RfbStructure> &getLowLevelStreet(const unsigned streetId) const {
    return data.getStreet(streetId);
  }

  /**
   * Returns whether the given car on the given street is within the zone affected by the traffic light.
   * This zone is defined as the area beginning at position x before the traffic light with
   * x = speed limit of the street * trafficLightZoneMultiplier
   * The zone ends at the end of the street.
   * @param[in]  car     The car
   * @param[in]  street  The street containing the given car
   * @return     True if in traffic light zone, False otherwise.
   */
  bool isInTrafficLightZone(const double distance, const unsigned streetId) const {
    double trafficLightPosition      = getLowLevelStreet(streetId).getTrafficLightPosition();
    double maxDistanceToTrafficLight = trafficLightZoneMultiplier * getStreet(streetId).getSpeedLimit();
    return (trafficLightPosition - maxDistanceToTrafficLight) <= distance;
  }

  /**
   * Determines the potential travel distance of all cars within the traffic light zone on the given street.
   * The potential travel distance is defined as the sum of the actual travel distance and the potentially lost
   * distance. The potentially lost distance is the difference between the contextual velocity and the actual velocity
   * in the current time step. Negative potentially lost distances are set to 0. The contextual velocity of a car is
   * defined as the minimum of the target velocity, the speed limit and the contextual velocity of the car in front.
   * Computing the potential travel distance of a car can be simplified to max(actual velocity, contextual velocity).
   * The traffic light zone is defined by the isInTrafficLightZone() function.
   * @param[in]  streetId   The street identifier
   * @param[in]  <unnamed>  Trait to allow different implementations for different RfbStructures
   * @return     The potential travel distance of all cars within the traffic light zone on the given street.
   */
  double determinePotentialTravelDistance(const unsigned streetId, rfbstructure_reversible_sorted_iterator_tag) const {
    double potentialTravelDistance = 0;
    double speedLimit              = getStreet(streetId).getSpeedLimit();
    std::vector<double> contextualVelocity(getStreet(streetId).getLanes(), speedLimit);

    auto streetIterable = getLowLevelStreet(streetId).getUnderlyingDataStructure().allIterable();
    for (auto carIt = streetIterable.rbegin(); carIt != streetIterable.rend(); ++carIt) {
      // stop once the end of the traffic light zone is reached
      if (!isInTrafficLightZone(carIt->getDistance(), streetId)) { break; }
      unsigned lane = carIt->getLane();

      // compute contextual and actual velocity
      double targetVelocity    = carIt->getTargetVelocity();
      contextualVelocity[lane] = std::min(targetVelocity, contextualVelocity[lane]);
      double actualVelocity    = carIt->getNextVelocity();

      // compute potential travel distance in the current step
      potentialTravelDistance += std::max(actualVelocity, contextualVelocity[lane]);
    }
    return potentialTravelDistance;
  }

  /**
   * This is a specialization of determinePotentialTravelDistance for bucket-based RfbStructures.
   * As before, the potential travel distance of all cars within the traffic light zone on the given street is computed.
   * However, the contextual velocity is computed bucket-wise to avoid unnecessary sorting of the unsorted buckets.
   * @param[in]  streetId   The street identifier
   * @param[in]  <unnamed>  Trait to allow different implementations for different RfbStructures
   * @return     The potential travel distance of all cars within the traffic light zone on the given street.
   */
  double determinePotentialTravelDistance(const unsigned streetId, rfbstructure_buckets_tag) const {
    double potentialTravelDistance = 0;
    const double speedLimit        = getStreet(streetId).getSpeedLimit();

    auto rfb = getLowLevelStreet(streetId).getUnderlyingDataStructure();
    for (unsigned lane = 0; lane < getStreet(streetId).getLanes(); ++lane) { // for each lane
      double contextualVelocity = speedLimit;
      double currentDistance    = getLowLevelStreet(streetId).getLength();
      // for each section, i.e. each bucket in the current lane
      for (unsigned section = rfb.getSectionCount() - 1; section >= 0; --section) {
        auto bucket = rfb.getBucket(section, lane); // retrieve the current bucket

        // stop once the end of the traffic light zone is reached, i.e. the bucket does not overlap with the zone
        currentDistance -= rfb.getSectionLength();
        if (!isInTrafficLightZone(currentDistance, streetId)) { break; }

        // compute bucket wise contextual velocity
        for (auto car : bucket) { contextualVelocity = std::min(contextualVelocity, car.getTargetVelocity()); }
        // compute potential travel distance in the current step and bucket
        for (auto car : bucket) {
          double actualVelocity = car.getNextVelocity();
          potentialTravelDistance += std::max(actualVelocity, contextualVelocity);
        }
      }
    }
    return potentialTravelDistance;
  }

  /**
   * Determines the optimal green light for a given junction in the current step based o the potential travel distance.
   * @param[in]  junction  The junction
   * @return     The direction of the incoming street with maximal potential travel distance.
   */
  CardinalDirection determineOptimalGreenLight(const Junction &junction) const {
    double maxPotentialTravelDistance    = -1.0;
    CardinalDirection requestedDirection = NORTH;
    for (const auto &street : junction.getIncomingStreets()) {
      if (!street.isConnected()) { continue; } // skip non-existent streets
      double potentialTravelDistance =
          determinePotentialTravelDistance(street.getStreet()->getId(), reverse_category());
      if (potentialTravelDistance > maxPotentialTravelDistance) {
        maxPotentialTravelDistance = potentialTravelDistance;
        requestedDirection         = street.getDirection();
      }
    }
    return requestedDirection;
  }

public:
  OptimizationRoutine(SimulationData<RfbStructure> &_data)
      : data(_data), requestedGreenLights(data.getDomainModel().getJunctions().size()) {}

  /**
   * Determine the optimal green light direction for each junction in the current step.
   * Insert these directions into the requestedGreenLights vector.
   */
  void perform() {
    for (auto const &junction : data.getDomainModel().getJunctions()) {
      requestedGreenLights[junction->getId()].push_back(determineOptimalGreenLight(*junction));
    }
  }

  /**
   * Improves the traffic light durations base on the observations made during the last simulation.
   * Retrieves the requested green lights from the OptimizationRoutine sets the new signal durations as mean of the
   * requests and old duration. If the new duration is less than the minimum duration of 5 or the relative duration
   * defined in 'relativeRescaleDurationLimit' the total duration of that junction is increased.
   */
  void improveTrafficLights() {
    for (auto const &junction : data.getDomainModel().getJunctions()) {
      const std::vector<CardinalDirection> &requestedGreenLightDirection = requestedGreenLights[junction->getId()];

      // Determine percentage of green light requests per direction
      std::vector<double> requestPercentage(4, 0);
      for (auto direction : requestedGreenLightDirection) { ++requestPercentage[direction]; }
      for (unsigned i = 0; i < 4; ++i) { requestPercentage[i] /= requestedGreenLightDirection.size(); }

      // Get the old signals and determine their total duration
      std::vector<Junction::Signal> oldSignals = junction->getSignals();
      std::vector<unsigned> signalDurations;
      double totalSignalsDuration = 0;
      for (const auto signal : oldSignals) { totalSignalsDuration += signal.getDuration(); }

      const double rescaleValue        = 1.3;
      const double requestImpactFactor = 0.1;
      bool rescale                     = false;
      double absoluteRescaleLimit      = std::max(5.0, totalSignalsDuration * relativeRescaleDurationLimit);

      // Determine duration of the new signals
      for (const auto &signal : oldSignals) {
        double oldPercentage = signal.getDuration() / totalSignalsDuration;
        double newPercentage =
            (1 - requestImpactFactor) * oldPercentage + requestImpactFactor * requestPercentage[signal.getDirection()];
        unsigned newDuration = std::round(totalSignalsDuration * newPercentage);
        signalDurations.push_back(newDuration);
        if (newDuration < absoluteRescaleLimit) { rescale = true; }
      }

      // Rescale the total duration by the rescaleValue if necessary
      if (rescale) {
        for (unsigned i = 0; i < signalDurations.size(); ++i) {
          signalDurations[i] = std::max(5.0, signalDurations[i] * rescaleValue);
        }
      }

      // Create new signals vector
      std::vector<Junction::Signal> newSignals(signalDurations.size());
      for (unsigned i = 0; i < signalDurations.size(); ++i) {
        newSignals[i] = Junction::Signal(oldSignals[i].getDirection(), signalDurations[i]);
      }

      junction->setSignals(newSignals);
    }
  }
};

#endif
