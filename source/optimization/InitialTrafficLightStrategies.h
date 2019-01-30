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
  void operator()(DomainModel &domainModel) {
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
  void operator()(DomainModel &domainModel, const unsigned stepCount, const double throughputWeight = 5) {
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
      for (auto const &connectedStreet : junction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          double throughput       = getThroughput(simulator, connectedStreet.getStreet()->getId());
          unsigned signalDuration = baseDuration + (throughputWeight * (throughput / totalThroughput));
          assert(baseDuration == 5);
          assert(throughputWeight >= 0);
          assert(throughput >= 0);
          assert(totalThroughput >= 0);
          assert((throughput / totalThroughput) >= 0);
          assert((throughputWeight * (throughput / totalThroughput)) >= 0);
          assert(signalDuration >= 5);
          initialSignals.push_back(Junction::Signal(connectedStreet.getDirection(), signalDuration));
        }
      }
      assert(!initialSignals.empty()); // illegal input
      junction->setSignals(initialSignals);
    }
  }
};

#endif