#ifndef HEURISTIC_SIMULATOR_H
#define HEURISTIC_SIMULATOR_H

#include "DomainModel.h"

class HeuristicSimulator {
  const DomainModel &domainModel;
  const unsigned carCount;
  const unsigned streetCount;

  // The travel distance per car as simulated by the HeuristicSimulator. This is an upper bound on the actual travel
  // distance reachable in an accurate simulation. This distance is also used as a cars priority.
  std::vector<double> optimalTravelDistancePerCar;

  // Counts for each street and each car how often the car passed through the traffic light at the end of the street.
  // The two directions of a street are considered separately.
  std::vector<std::vector<unsigned>> trafficLightCrossingCountPerCarPerStreet;

public:
  HeuristicSimulator(const DomainModel &domainModel)
      : domainModel(domainModel), carCount(domainModel.getVehicles().size()),
        streetCount(domainModel.getStreets().size()), optimalTravelDistancePerCar(carCount, 0),
        trafficLightCrossingCountPerCarPerStreet(carCount, std::vector<unsigned>(streetCount, 0)) {}

  // Simulated 'stepCount' steps heuristically for each car while ignoring traffic lights and other cars.
  // Store the distance traveled per car and which traffic lights it passed how often.
  void performSteps(const unsigned stepCount) {
    for (unsigned carId = 0; carId < carCount; ++carId) { // for each car
      const Vehicle &car = domainModel.getVehicle(carId);

      Street *currentStreet        = car.getPosition().getStreet();
      double currentDistance       = car.getPosition().getDistance();
      double currentTravelDistance = 0;
      unsigned turnDirectionIndex  = 0;
      const auto &route            = car.getRoute();

      for (unsigned timeStep = 0; timeStep < stepCount; ++timeStep) { // estimate all time steps
        double velocity = std::min(car.getTargetVelocity(), currentStreet->getSpeedLimit());
        currentTravelDistance += velocity;
        currentDistance += velocity;

        // if the car left the current street count it to the current streets throughput and determine the next street
        if (currentDistance >= currentStreet->getLength()) {
          currentDistance -= currentStreet->getLength();                             // update distance
          ++trafficLightCrossingCountPerCarPerStreet[carId][currentStreet->getId()]; // and throughput

          // Route planning: determine the next street
          const Junction &targetJunction = currentStreet->getTargetJunction();
          // determine cardinal direction of the current street
          CardinalDirection sourceDirection;
          for (const auto &connectedStreet : targetJunction.getIncomingStreets()) {
            if (connectedStreet.isConnected() && connectedStreet.getStreet()->getId() == currentStreet->getId()) {
              sourceDirection = connectedStreet.getDirection();
            }
          }

          // determine the preferred cardinal direction of the new street
          TurnDirection turnDirection     = route[turnDirectionIndex % 4];
          CardinalDirection nextDirection = (CardinalDirection)((sourceDirection + turnDirection) % 4);

          // search for the next connected street in the preferred direction
          for (unsigned i = 0; i < 4; ++i) {
            auto outgoingStreet = targetJunction.getOutgoingStreet((CardinalDirection)((nextDirection + i) % 4));
            if (outgoingStreet.isConnected()) {
              currentStreet = outgoingStreet.getStreet();
              break;
            }
          }
        }
      }
      optimalTravelDistancePerCar[carId] = currentTravelDistance; // write back the computed travel distance
    }
  }

  double getTravelDistance(const unsigned carId) const { return optimalTravelDistancePerCar[carId]; }
  // The priority of a car is equivalent to its optimal travel distance.
  double getCarPriority(const unsigned carId) const { return optimalTravelDistancePerCar[carId]; }

  // The throughput of a street is the total number of cars that crossed the streets traffic light during the heuristic
  // simulation.
  unsigned getTrafficLightThroughput(const unsigned streetId) const {
    unsigned throughput = 0;
    for (unsigned carId = 0; carId < carCount; ++carId) {
      throughput += trafficLightCrossingCountPerCarPerStreet[carId][streetId];
    }
    return throughput;
  }
  // The prioritized throughput is the throughput multiplied by the priority of each car.
  double getPrioritizedTrafficLightThroughput(const unsigned streetId) const {
    double prioritizedThroughput = 0;
    for (unsigned carId = 0; carId < carCount; ++carId) {
      prioritizedThroughput +=
          optimalTravelDistancePerCar[carId] * trafficLightCrossingCountPerCarPerStreet[carId][streetId];
    }
    return prioritizedThroughput;
  }

  // Resets results computed by the heuristic simulation
  void reset() {
    std::fill(optimalTravelDistancePerCar.begin(), optimalTravelDistancePerCar.end(), 0.0);
    for (unsigned carId = 0; carId < carCount; ++carId) {
      std::fill(trafficLightCrossingCountPerCarPerStreet[carId].begin(),
          trafficLightCrossingCountPerCarPerStreet[carId].end(), 0);
    }
  }
};

#endif