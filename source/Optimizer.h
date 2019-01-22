#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "DomainModel.h"
#include "Junction.h"
#include "Simulator.h"

#include <vector>

template <template <typename Vehicle> typename RfbStructure,
    template <template <typename Vehicle> typename _RfbStructure> typename SignalingRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename IDMRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename OptimizationRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename ConsistencyRoutine>
class Optimizer {
  DomainModel &domainModel;
  double lastTravelDistance = 0;
  const double minTravelDistance;
  const unsigned stepCount;
  const double relativeRescaleDurationLimit = 0;

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
      // junction->setSignals(initialSignals); TODO uncomment when implemented in the domain model
    }
  }

  /**
   * Improves the traffic light durations base on the observations made during the last simulation.
   * Retrieves the requested green lights from the OptimizationRoutine sets the new signal durations as mean of the
   * requests and old duration. If the new duration is less than the minimum duration of 5 or the relative duration
   * defined in 'relativeRescaleDurationLimit' the total duration of that junction is increased.
   */
  void improveTrafficLights(
      const Simulator<RfbStructure, SignalingRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine> &simulator) {
    // get request vectors from the OptimizationRoutine
    std::vector<std::vector<CardinalDirection>> requestedGreenLights;
    // const std::vector<std::vector<CardinalDirection>> &requestedGreenLights =
    //     simulator.getOptimizationRoutine().getRequestedGreenLights(); TODO uncomment when routine is implemented
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<CardinalDirection> &requestedGreenLightDirection = requestedGreenLights[junction->getId()];

      // Determine percentage of green light requests per direction
      std::vector<double> requestPercentage(4, 0);
      for (auto direction : requestedGreenLightDirection) { ++requestPercentage[direction]; }
      for (unsigned i = 0; i < 4; ++i) { requestPercentage[i] /= requestedGreenLightDirection.size(); }

      // Get the old signals and determine their total duration
      std::vector<Junction::Signal> oldSignals = junction->getSignals();
      std::vector<unsigned> signalDurations;
      double totalSignalsDuration = 0;
      for (const auto signal : oldSignals) { totalSignalsDuration += signal.getDuration(); }

      double rescaleValue         = 1.0;
      double absoluteRescaleLimit = std::min(5.0, totalSignalsDuration * relativeRescaleDurationLimit);

      // Determine duration of the new signals
      for (const auto &signal : oldSignals) {
        double oldPercentage = signal.getDuration() / totalSignalsDuration;
        double newPercentage = (oldPercentage + requestPercentage[signal.getDirection()]) * 0.5;
        unsigned newDuration = totalSignalsDuration * newPercentage;
        signalDurations.push_back(newDuration);
        if (newDuration < absoluteRescaleLimit) { rescaleValue = std::max(rescaleValue, std::ceil(5.0 / newDuration)); }
      }

      // Rescale the total duration by the rescaleValue if necessary
      if (rescaleValue != 1.0) {
        for (unsigned i = 0; i < signalDurations.size(); ++i) { signalDurations[i] *= rescaleValue; }
      }

      // Create new signals vector
      std::vector<Junction::Signal> newSignals(signalDurations.size());
      for (unsigned i = 0; i < signalDurations.size(); ++i) {
        newSignals[i] = Junction::Signal(oldSignals[i].getDirection(), signalDurations[i]);
      }

      // junction.setSignals(newSignals); TODO uncomment when implemented in the domain model
    }
  }

  /**
   * Iterate over all cars on all streets and sum their travel distance to the total travel distance.
   * Store the computed distance in lastTravelDistance
   * * @param[in]  simulator          The simulator running the simulation (and holding the low level model)
   */
  void calculateTravelDistance(
      const Simulator<RfbStructure, SignalingRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine> &simulator) {
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
    // domainModel.reset(); // reset cars to initial position TODO uncomment when implemented in the domain model
    // run a complete simulation using a newly initialized simulator, evaluate the traffic lights during the simulation
    Simulator<RfbStructure, SignalingRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine> simulator(
        domainModel);
    simulator.performSteps(stepCount);

    calculateTravelDistance(simulator);                     // compute the traveled distance
    if (lastTravelDistance < minTravelDistance) { return; } // check whether the minimum travel distance is reached
    improveTrafficLights(simulator);                        // optimize the traffic lights based on the evaluation
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
