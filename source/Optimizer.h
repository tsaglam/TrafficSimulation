#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Simulator.h"

template <template <typename Vehicle> typename RfbStructure,
    template <template <typename Vehicle> typename _RfbStructure> typename SignalingRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename IDMRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename ConsistencyRoutine>
class Optimizer {
  DomainModel &domainModel;
  double lastTravelDistance = 0;
  const double minTravelDistance;
  const unsigned stepCount;

private:
  /**
   * Determines simple initial traffic lights and sets them in the domain model.
   * Each street has exactly one green phase with the minimum duration of 5 s.
   * The order of the signals is equivalent to the order returned by Junction::getIncomingStreets().
   * A signal is skipped if there is no connected street in that direction.
   */
  void setInitialTrafficLights() {
    const unsigned signalDuration = 5;
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<Junction::Signal> initialSignals;
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), signalDuration));
        }
      }
      // junction.setSignals(initialSignals); TODO uncomment when implemented in the domain model
    }
  }

  void improveTrafficLights() {
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<Junction::Signal> newSignals;
      // TODO compute new signals based on the old signals and the evaluation
      // junction.setSignals(newSignals); TODO uncomment when implemented in the domain model
    }
  }

  /**
   * Iterate over all cars on all streets and sum their travel distance to the total travel distance.
   * Store the computed distance in lastTravelDistance
   */
  void calculateTravelDistance() {
    double travelDistance = 0;
    // for (auto &street : domainModel.getStreets()) { // TODO fix this
    //   for (const auto &car : street.allIterable()) { travelDistance += car.getTravelDistance(); }
    // }
    lastTravelDistance = travelDistance;
  }

  /**
   * Resets the domain model, initializes a new simulator and runs a simulation of 'stepCount' steps while
   * evaluating the traffic lights. Returns directly if the simulation reached the required minimum travel distance.
   * Otherwise optimizes the traffic lights based on the evaluation.
   */
  void runOptimizationCycle() {
    // domainModel.reset(); // reset cars to initial position TODO uncomment when implemented in the domain model
    // run a complete simulation using a newly initialized simulator, evaluate the traffic lights during the simulation
    Simulator<RfbStructure, SignalingRoutine, IDMRoutine, ConsistencyRoutine> simulator(domainModel);
    simulator.performSteps(stepCount);

    calculateTravelDistance();                              // compute the traveled distance
    if (lastTravelDistance < minTravelDistance) { return; } // check whether the minimum travel distance is reached
    improveTrafficLights();                                 // optimize the traffic lights based on the evaluation
  }

public:
  /**
   * Creates an Optimizer object.
   * @param      _domainModel        The domain model for the simulation
   * @param[in]  _stepCount          The number of steps the simulation is run
   * @param[in]  _minTravelDistance  The minimum travel distance that has to be reached by the sum of all cars
   */
  Optimizer(DomainModel &_domainModel, const unsigned _stepCount, const double _minTravelDistance)
      : domainModel(_domainModel), minTravelDistance(_minTravelDistance), stepCount(_stepCount) {}

  /**
   * Computes a signal order and duration such that all cars in the simulation travel at least a distance of
   * minTravelDistance. Runs a complete simulation and evaluates the traffic lights. Checks whether the minimum travel
   * distance is reached. Otherwise optimizes the traffic lights based on the evaluation, resets the simulation and
   * repeats this optimization cycle.
   */
  void optimizeTrafficLights() {
    setInitialTrafficLights();
    while (lastTravelDistance < minTravelDistance) { runOptimizationCycle(); }
  }
};

#endif
