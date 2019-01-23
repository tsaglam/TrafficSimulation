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
      double targetVelocity    = data.getDomainModel().getVehicle(carIt->getId()).getTargetVelocity();
      contextualVelocity[lane] = std::min(targetVelocity, contextualVelocity[lane]);
      double actualVelocity    = carIt->getNextVelocity();

      // compute potential travel distance in the current step
      potentialTravelDistance += std::max(actualVelocity, contextualVelocity[lane]);
    }
    return potentialTravelDistance;
  }

  double determinePotentialTravelDistance(const LowLevelStreet<RfbStructure> &street, rfbstructure_buckets_tag) const {
    // TODO implement this
    return street.getLength();
  }

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
