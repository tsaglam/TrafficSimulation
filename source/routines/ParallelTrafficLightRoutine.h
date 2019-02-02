#ifndef P_TRAFFIC_LIGHT_ROUTINE_H
#define P_TRAFFIC_LIGHT_ROUTINE_H

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "RfbStructure.h"
#include "SimulationData.h"
#include <iostream>

template <template <typename Vehicle> typename RfbStructure>
class ParallelTrafficLightRoutine {
public:
  /**
   * Determines when it is actually better to use the parallel traffic light routine.
   */
  const unsigned long PARALLEL_THRESHOLD = 2000;

  /**
   * @brief      Creates the traffic light routine, sets the simulation data.
   *
   * @param      data  Is the simulaton data for the routine.
   */
  ParallelTrafficLightRoutine(SimulationData<RfbStructure> &data) : data(data) {}

  /**
   * @brief      Simulates a single step for all junctions of the domain model.
   */
  void perform() {
    DomainModel &model    = data.getDomainModel();
    const auto &junctions = model.getJunctions();
    if (junctions.size() > PARALLEL_THRESHOLD) {
      performParallel(junctions);
    } else {
      performSequential(junctions);
    }
  }

  void performParallel(const std::vector<std::unique_ptr<Junction>> &junctions) {
    // TODO static? tests?
#pragma omp parallel for shared(junctions)
    for (std::size_t i = 0; i < junctions.size(); i++) {
      const auto &junction = junctions[i];
      perform(*junction);
    }
  }

  void performSequential(const std::vector<std::unique_ptr<Junction>> &junctions) {
    for (auto const &junction : junctions) { perform(*junction); }
  }

  void perform(Junction &junction) {
    bool lightChanged = junction.nextStep();
    if (lightChanged) {
      // Turn previous red:
      Junction::Signal previous = junction.getPreviousSignal();
      toggleStreetForSignal(previous, junction);
      // Turn current green:
      Junction::Signal current = junction.getCurrentSignal();
      toggleStreetForSignal(current, junction);
    }
  }

private:
  /**
   * @brief      Toggles the signal of the low level street correlating to the domain level street of a junction signal.
   * @param      signal  Is the specific signal.
   * @param      junction  Is the junction of the signal.
   */
  void toggleStreetForSignal(Junction::Signal signal, const Junction &junction) {
    auto connectedStreet = junction.getIncomingStreet(signal.getDirection());
    if (connectedStreet.isConnected()) {
      Street *domainModelStreet            = connectedStreet.getStreet();
      LowLevelStreet<RfbStructure> &street = data.getStreet(domainModelStreet->getId());
      street.switchSignal();
    }
  }

  SimulationData<RfbStructure> &data;
};

#endif
