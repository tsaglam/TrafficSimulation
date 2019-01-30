#ifndef INITIAL_TRAFFIC_LIGTH_STRATEGIES
#define INITIAL_TRAFFIC_LIGTH_STRATEGIES

#include "DomainModel.h"
#include "HeuristicSimulator.h"
#include "Junction.h"

struct InitialTrafficLightsAllFive {
  InitialTrafficLightsAllFive() = default;
  /**
   * Determines simple initial traffic lights and sets them in the domain model.
   * Each street has exactly one green phase with the minimum duration of 5 s.
   * The order of the signals is equivalent to the order returned by Junction::getIncomingStreets().
   * A signal is skipped if there is no connected street in that direction.
   */
  void operator()(DomainModel &domainModel) const {
    const unsigned signalDuration = 5;
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<Junction::Signal> initialSignals;
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), signalDuration));
        }
      }
      junction->setSignals(initialSignals);
    }
  }
  void operator()(DomainModel &domainModel, const unsigned) { operator()(domainModel); }
};

template <bool withPriority>
struct InitialTrafficLightsWithHeuristicSimulator {
  InitialTrafficLightsWithHeuristicSimulator() = default;

  double getThroughput(const HeuristicSimulator &simulator, const unsigned streetId) const {
    if constexpr (withPriority) {
      return simulator.getPrioritizedTrafficLightThroughput(streetId);
    } else {
      return simulator.getTrafficLightThroughput(streetId);
    }
  }

  /**
   * Determines initial traffic lights and sets them in the domain model.
   * Runs a heuristic simulation of each car's route without traffic lights and other cars.
   * Assigns the traffic light duration according to their throughput as
   * 5 + x * throughput of this traffic light / total throughput of the junction
   * The parameter x > 0 is specified via the 'throughputWeight' with default 5.
   * A signal is skipped if there is no connected street in that direction.
   */
  void operator()(DomainModel &domainModel, const unsigned stepCount, const double throughputWeight = 5) const {
    HeuristicSimulator simulator(domainModel);
    simulator.performSteps(stepCount);

    const unsigned baseDuration = 5;
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<Junction::Signal> initialSignals;
      double totalThroughput = 0.0;
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          totalThroughput += getThroughput(simulator, connectedStreet.getStreet()->getId());
        }
      }

      // if no car will ever cross this junction the signals are irrelevant.
      // We, therefore, set a signal only for the first connected street with the base duration.
      if (totalThroughput == 0) {
        for (auto const &connectedStreet : junction->getIncomingStreets()) {
          if (connectedStreet.isConnected()) {
            initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), baseDuration));
            break;
          }
        }
      } else { // otherwise determine signal durations based on the relative throughput
        for (auto const &connectedStreet : junction->getIncomingStreets()) {
          if (connectedStreet.isConnected()) {
            double throughput = getThroughput(simulator, connectedStreet.getStreet()->getId());
            if (throughput == 0) { continue; } // skip streets with no throughput
            unsigned signalDuration = baseDuration + (throughputWeight * (throughput / totalThroughput));
            initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), signalDuration));
          }
        }
      }
      junction->setSignals(initialSignals);
    }
  }
};

template <bool withPriority>
struct InitialTrafficLightsWithHeuristicSimulatorAndIteration {
  InitialTrafficLightsWithHeuristicSimulatorAndIteration() = default;

  double getThroughput(const HeuristicSimulator &simulator, const unsigned streetId) const {
    if constexpr (withPriority) {
      return simulator.getPrioritizedTrafficLightThroughput(streetId);
    } else {
      return simulator.getTrafficLightThroughput(streetId);
    }
  }

  std::pair<double, std::vector<unsigned>> determineBestOrder(
      const std::vector<std::vector<TrafficLightCrossing>> &crossingsPerStreet,
      const std::vector<unsigned> &trafficLightDuration, const std::vector<double> *carPriorities) const {
    double minimumWaitTime = -1;
    std::vector<unsigned> bestOrder;

    std::vector<unsigned> currentOrder(trafficLightDuration.size());
    for (unsigned i = 0; i < currentOrder.size(); ++i) { currentOrder[i] = i; }

    do { // determine order with minimal wait time for each possible permutation of the traffic light order
      RateTrafficLights rating(crossingsPerStreet, trafficLightDuration, currentOrder, carPriorities);
      double totalWaitTime = rating.getTotalWaitTimeWithPriority();
      if (minimumWaitTime < 0 || totalWaitTime < minimumWaitTime) {
        minimumWaitTime = totalWaitTime;
        bestOrder       = currentOrder;
      }
    } while (std::next_permutation(currentOrder.begin(), currentOrder.end()));
    return std::make_pair(minimumWaitTime, bestOrder); // return the best order and its total wait time
  }

  void optimizeSignals(const HeuristicSimulator &simulator, std::vector<unsigned> &trafficLightDuration,
      std::vector<unsigned> &trafficLightOrder, const std::vector<unsigned> &streetIds) const {
    const std::vector<double> *carPriorities = simulator.getCarPriorities();

    std::vector<unsigned> bestDuration, bestOrder;
    std::vector<unsigned> currentDuration = trafficLightDuration;
    std::vector<unsigned> currentOrder    = trafficLightOrder;

    double bestRating                        = -1;
    unsigned cyclesWithoutImprovement          = 0;
    const unsigned maxCyclesWithoutImprovement = 5;

    // retrieve information from the simulator
    std::vector<std::vector<TrafficLightCrossing>> crossingsPerStreet(streetIds.size());
    for (unsigned streetIndex = 0; streetIndex < streetIds.size(); ++streetIndex) {
      crossingsPerStreet[streetIndex] = simulator.getTrafficLightCrossings(streetIds[streetIndex]);
    }

    std::cout << "Total wait time: ";
    while (true) {
      // evaluate the current traffic light duration and find the best order
      double totalWaitTime;
      std::tie(totalWaitTime, currentOrder) = determineBestOrder(crossingsPerStreet, currentDuration, carPriorities);
      std::cout << totalWaitTime << " ";

      // if improvement over current optimum set as new optimum and reset cyclesWithoutImprovement count
      if (bestRating < 0 || totalWaitTime < bestRating) {
        cyclesWithoutImprovement = 0;
        bestRating               = totalWaitTime;
        bestDuration             = currentDuration;
        bestOrder                = currentOrder;
      } else { // if no improvement increment unsuccessful cycle count, stop search if maximum cycles reached
        if (++cyclesWithoutImprovement >= maxCyclesWithoutImprovement) { break; }
      }

      if (totalWaitTime <= 0) { break; } // stop if optimal wait time reached

      // improve the traffic lights, use cycle count to diversify solutions when retrying from the same optimum
      RateTrafficLights rating(crossingsPerStreet, currentDuration, currentOrder, carPriorities);
      for (unsigned streetIndex = 0; streetIndex < streetIds.size(); ++streetIndex) {
        double relativeRating = rating.getWaitTimeWithPriority(streetIndex) / totalWaitTime;
        currentDuration[streetIndex] += (5 + cyclesWithoutImprovement) * relativeRating;
        assert(currentDuration[streetIndex] >= 5);
      }
    }
    std::cout << "\n";

    trafficLightDuration = bestDuration;
    trafficLightOrder    = bestOrder;
  }

  /**
   * Determines initial traffic lights and sets them in the domain model.
   * Runs a heuristic simulation of each car's route without traffic lights and other cars.
   * Assigns the traffic light duration according to their throughput as
   * 5 + x * throughput of this traffic light / total throughput of the junction
   * The parameter x > 0 is specified via the 'throughputWeight' with default 5.
   * A signal is skipped if there is no connected street in that direction.
   */
  void operator()(DomainModel &domainModel, const unsigned stepCount, const double throughputWeight = 5) const {
    HeuristicSimulator simulator(domainModel);
    simulator.performSteps(stepCount);

    const unsigned baseDuration = 5;
    for (auto const &junction : domainModel.getJunctions()) {
      std::vector<std::vector<TrafficLightCrossing>> crossingsPerStreet;

      std::vector<Junction::Signal> initialSignals;
      double totalThroughput = 0.0;
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          totalThroughput += getThroughput(simulator, connectedStreet.getStreet()->getId());
        }
      }

      // if no car will ever cross this junction the signals are irrelevant.
      // We, therefore, set a signal only for the first connected street with the base duration.
      if (totalThroughput == 0) {
        for (auto const &connectedStreet : junction->getIncomingStreets()) {
          if (connectedStreet.isConnected()) {
            initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), baseDuration));
            break;
          }
        }
        junction->setSignals(initialSignals);
        continue;
      }

      // otherwise determine initial signal durations based on the relative throughput
      std::vector<unsigned> signalDurations;
      std::vector<unsigned> signalOrder = {0, 1, 2, 3};
      std::vector<unsigned> streetIds;
      std::vector<CardinalDirection> signalDirections;
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          double throughput = getThroughput(simulator, connectedStreet.getStreet()->getId());
          if (throughput == 0) { continue; } // skip streets with no throughput
          unsigned signalDuration = baseDuration + (throughputWeight * (throughput / totalThroughput));
          signalDurations.push_back(signalDuration);
          signalDirections.push_back(connectedStreet.getDirection());
          streetIds.push_back(connectedStreet.getStreet()->getId());
        }
      }
      signalOrder.resize(signalDurations.size());
      // try improving the initial signals
      optimizeSignals(simulator, signalDurations, signalOrder, streetIds);

      for (const auto streetIndex : signalOrder) {
        initialSignals.push_back(Junction::Signal(signalDirections[streetIndex], signalDurations[streetIndex]));
      }
      junction->setSignals(initialSignals);
    }
  }
};

#endif