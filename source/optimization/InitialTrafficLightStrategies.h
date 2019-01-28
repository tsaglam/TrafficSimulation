#ifndef INITIAL_TRAFFIC_LIGTH_STRATEGIES
#define INITIAL_TRAFFIC_LIGTH_STRATEGIES

#include "DomainModel.h"
#include "Junction.h"

struct InitialTrafficLightsAllFive {
  InitialTrafficLightsAllFive() = default;
  /**
   * Determines simple initial traffic lights and sets them in the domain model.
   * Each street has exactly one green phase with the minimum duration of 5 s.
   * The order of the signals is equivalent to the order returned by Junction::getIncomingStreets().
   * A signal is skipped if there is no connected street in that direction.
   */
  void operator()(DomainModel& domainModel) {
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
};

#endif