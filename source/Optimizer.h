#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "DomainModel.h"
#include "Junction.h"
#include "Simulator.h"

#include <iomanip>
#include <iostream>
#include <vector>

template <template <typename Vehicle> typename RfbStructure,
    template <template <typename Vehicle> typename _RfbStructure> typename SignalingRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename IDMRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename OptimizationRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename ConsistencyRoutine,
    typename InitialTrafficLightStrategy, bool debug = false>
class Optimizer {
  using SimulatorType = Simulator<RfbStructure, SignalingRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine>;

  DomainModel &domainModel;
  const double minTravelDistance;
  const unsigned stepCount;
  double lastTravelDistance = 0;
  unsigned maxCycles        = -1;

private:
  /** Set the initial traffic ligths based on the initial traffic light strategy. */
  void setInitialTrafficLights() { InitialTrafficLightStrategy()(domainModel); }

  /**
   * Iterate over all cars on all streets and sum their travel distance to the total travel distance.
   * Store the computed distance in lastTravelDistance
   * @param[in]  simulator          The simulator running the simulation (and holding the low level model)
   */
  void calculateTravelDistance(const SimulatorType &simulator) {
    double travelDistance = 0;
    for (auto &street : simulator.getData().getStreets()) {
      for (const auto &car : street.allIterable()) { travelDistance += car.getTravelDistance(); }
    }
    lastTravelDistance = travelDistance;
  }

  /**
   * Resets the domain model, initializes a new simulator and runs a simulation of 'stepCount' steps while
   * evaluating the traffic lights. Returns directly if the simulation reached the required minimum travel distance.
   * Otherwise optimizes the traffic lights based on the evaluation.
   */
  void runOptimizationCycle() {
    domainModel.resetModel(); // reset cars and signals to initial state
    // run a complete simulation using a newly initialized simulator, evaluate the traffic lights during the simulation
    SimulatorType simulator(domainModel);
    simulator.performSteps(stepCount);

    calculateTravelDistance(simulator);                        // compute the traveled distance
    if (lastTravelDistance >= minTravelDistance) { return; }   // check whether the minimum travel distance is reached
    simulator.getOptimizationRoutine().improveTrafficLights(); // optimize the traffic lights based on the evaluation
  }

public:
  /**
   * Creates an Optimizer object.
   * @param      _domainModel        The domain model for the simulation
   * @param[in]  _stepCount          The number of steps the simulation is run
   * @param[in]  _minTravelDistance  The minimum travel distance that has to be reached by the sum of all cars
   */
  Optimizer(
      DomainModel &_domainModel, const unsigned _stepCount, const double _minTravelDistance, unsigned _maxCycles = -1)
      : domainModel(_domainModel), minTravelDistance(_minTravelDistance), stepCount(_stepCount), maxCycles(_maxCycles) {
  }

  void printOptimizationProgress(const unsigned cycleCount) const {
    std::cerr << "Optimization Cycle " << std::setw(4) << cycleCount << "    travel distance " << std::setw(8)
              << std::fixed << std::setprecision(2) << lastTravelDistance << "\n";
  }

  /**
   * Computes a signal order and duration such that all cars in the simulation travel at least a distance of
   * minTravelDistance. Runs a complete simulation and evaluates the traffic lights. Checks whether the minimum travel
   * distance is reached. Otherwise optimizes the traffic lights based on the evaluation, resets the simulation and
   * repeats this optimization cycle.
   */
  void optimizeTrafficLights() {
    setInitialTrafficLights();
    unsigned optimizationCycleCount = 0;
    while (lastTravelDistance < minTravelDistance && optimizationCycleCount < maxCycles) {
      runOptimizationCycle();
      ++optimizationCycleCount;
      if (debug) { printOptimizationProgress(optimizationCycleCount); }
    }
  }
};

#endif
