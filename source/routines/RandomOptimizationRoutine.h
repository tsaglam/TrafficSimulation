#ifndef RANDOM_OPTIMIZATION_ROUTINE_H
#define RANDOM_OPTIMIZATION_ROUTINE_H

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

#include "DomainModel.h"
#include "Junction.h"
#include "SimulationData.h"

/**
 * Routine choosing random signals at each optimization iteration.
 *
 * @tparam     RfbStructure  The underlying data structure of the low level street
 */
template <template <typename Vehicle> typename RfbStructure>
class RandomOptimizationRoutine {

private:
  SimulationData<RfbStructure> &data;

public:
  RandomOptimizationRoutine(SimulationData<RfbStructure> &_data) : data(_data) {}

  void perform() {}

  void improveTrafficLights() {
    std::vector<Junction::Signal> newSignals;

    // Initialize random engine from clock.
    std::default_random_engine rng;
    rng.seed(static_cast<std::default_random_engine::result_type>(
        std::chrono::system_clock::now().time_since_epoch().count()));

    std::uniform_int_distribution<unsigned int> dist(5, 20);

    for (auto const &junction : data.getDomainModel().getJunctions()) {

      for (const auto &incomingStreet : junction->getIncomingStreets()) {
        if (incomingStreet.isConnected()) {
          // Draw random signalDuration for each connected street
          unsigned int signalDuration = dist(rng);
          newSignals.emplace_back(incomingStreet.getDirection(), signalDuration);
        }
      }

      // Shuffle signals (permutation)
      std::shuffle(newSignals.begin(), newSignals.end(), rng);

      junction->setSignals(std::move(newSignals));
      newSignals.clear();
    }
  }
};

#endif
