#include "DomainModel.h"
#include "SimulationData.h"
#include <iostream>

/**
 * @brief      Returns the correlating low level street for the correlating domain model street of a junction signal.
 * @param      signal  Is the junction signal.
 * @param      data    The data is the simulation data for low level model access.
 * @return     The correlating lov level street.
 */
LowLevelStreet getStreetForSignal(Junction::Signal &signal, SimulationData &data) {
  Junction::ConnectedStreet connectedStreet = getIncomingStreet(signal.getDirection());
  Street domainModelStreet                  = connectedStreet.getStreet();
  return data.getStreet(domainModelStreet.getID());
}

/**
 * @brief      Toggles the signal of the low level street correlating to the domain level street of a junction signal.
 * @param      signal  Is the junction signal.
 * @param      data    The data is the simulation data for low level model access.
 */
void toggleStreetForSignal(Junction::Signal &signal, SimulationData &data) {
  LowLevelStreet street = getStreetForSignal(signal, data);
  street.switchSignal();
}

/**
 * @brief      Simulates a single step for all junctions of the domain model.
 * @param      model  The model is the domain model which contains the junctions.
 * @param      data   The data is the simulation data for low level model access.
 */
void simulateStep(DomainModel &model, SimulationData &data) {
  for (Junction &junction : model.getJunctions()) {
    bool lightChanged = junction.nextStep();
    if (lightChanged) {
      // Turn previous red
      Junction::Signal previous = junction.getPreviousSignal();
      toggleStreetForSignal(previous)
          // Turn current green
          Junction::Signal current = junction.getCurrentSignal();
      toggleStreetForSignal(current);
    }
  }
}