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
    // TODO
  }

  double getTravelDistance(const unsigned carId) const { return optimalTravelDistancePerCar[carId]; }
  // The priority of a car is equivalent to its optimal travel distance.
  double getCarPriority(const unsigned carId) const { return optimalTravelDistancePerCar[carId]; }

  // The throughput of a street is the total number of cars that crossed the streets traffic light during the heuristic
  // simulation.
  unsigned getTrafficLightThroughput(const unsigned streetId) const {
    unsigned throughput = 0;
    for (int carId = 0; carId < carCount; ++carId) {
      throughput += trafficLightCrossingCountPerCarPerStreet[carId][streetId];
    }
    return throughput;
  }
  // The prioritized throughput is the throughput multiplied by the priority of each car.
  double getPrioritizedTrafficLightThroughput(const unsigned streetId) const {
    double prioritizedThroughput = 0;
    for (int carId = 0; carId < carCount; ++carId) {
      throughput += optimalTravelDistancePerCar[carId] * trafficLightCrossingCountPerCarPerStreet[carId][streetId];
    }
    return prioritizedThroughput;
  }

  // Resets results computed by the heuristic simulation
  void reset() {
    std::fill(optimalTravelDistancePerCar.begin(), optimalTravelDistancePerCar.end(), 0.0);
    for (int streetId = 0; streetId < streetCount; ++streetId) {
      std::fill(trafficLightCrossingCountPerCarPerStreet.begin(), trafficLightCrossingCountPerCarPerStreet.end(), 0);
    }
  }
};

#endif