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
   * Plays a role in determining the minimal workload block size. The larger this value gets, the smaller a block can
   * get.
   */
  const int BLOCK_SIZE_FACTOR = 5;

  /**
   * @brief      Creates the traffic light routine, sets the simulation data.
   *
   * @param      data  Is the simulaton data for the routine.
   */
  ParallelTrafficLightRoutine(SimulationData<RfbStructure> &data) : data(data) {}

  /**
   * @brief      Simulates a single step for all junctions of the domain model.
   */
  void perform() { // TODO if cars > 10000 then do parallel
    DomainModel &model   = data.getDomainModel();
    const auto &junctions = model.getJunctions();
#pragma omp parallel for
    for (std::size_t i = 0; i < junctions.size(); i++) {
      const auto &junction = junctions[i];
      bool lightChanged    = junction->nextStep();
      if (lightChanged) {
        // Turn previous red:
        Junction::Signal previous = junction->getPreviousSignal();
        toggleStreetForSignal(previous, *junction);
        // Turn current green:
        Junction::Signal current = junction->getCurrentSignal();
        toggleStreetForSignal(current, *junction);
      }
    }
  }

private:
  /**
   * @brief      Toggles the signal of the low level street correlating to the domain level street of a junction signal.
   * @param      signal  Is the specific signal.
   * @param      junction  Is the junction of the signal.
   */
  void toggleStreetForSignal(Junction::Signal signal, const Junction &junction) {
    Street *domainModelStreet            = junction.getIncomingStreet(signal.getDirection()).getStreet();
    LowLevelStreet<RfbStructure> &street = data.getStreet(domainModelStreet->getId());
    street.switchSignal();
  }

  SimulationData<RfbStructure> &data;
};

#endif
