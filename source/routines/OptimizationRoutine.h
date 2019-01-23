#ifndef OPTIMIZATION_ROUTINE_H
#define OPTIMIZATION_ROUTINE_H

#include <vector>

#include "DomainModel.h"
#include "DomainModelCommon.h"
#include "Junction.h"
#include "RfbStructureTraits.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class OptimizationRoutine {
  using reverse_category = typename rfbstructure_traits<RfbStructure>::reverse_category;
  using Vehicle          = LowLevelCar;

private:
  SimulationData<RfbStructure> &data;
  // contains for each junction a vector of requested green light directions per time step
  std::vector<std::vector<CardinalDirection>> requestedGreenLights;
  const double trafficLightZoneMultiplier = 1.0; // TODO choose parameters
  // Is multiplied with the speed limit to determine the size of the traffic light zone

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

    auto streetIterable = getLowLevelStreet(streetId).allIterable();
    for (auto carIt = streetIterable.end() - 1; carIt >= streetIterable.begin(); --carIt) {
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

    auto rfb = getLowLevelStreet().getUnderlyingDataStructure();
    for (unsigned lane = 0; lane < getStreet(streetId).getLanes(); ++lane) { // for each lane
      double contextualVelocity = speedLimit;
      double currentDistance    = getLowLevelStreet().getLength();
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
    double maxPotentialTravelDistance = -1.0;
    CardinalDirection requestedDirection;
    for (const auto &street : junction.getIncomingStreets()) {
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

  const std::vector<std::vector<CardinalDirection>> &getRequestedGreenLights() const { return requestedGreenLights; }
};

#endif
