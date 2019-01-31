#ifndef TRAFFIC_LIGHT_ROUTINE_H
#define TRAFFIC_LIGHT_ROUTINE_H

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "RfbStructure.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class TrafficLightRoutine {
public:
  /**
   * @brief      Creates the traffic light routine, sets the simulation data.
   *
   * @param      data  Is the simulaton data for the routine.
   */
  TrafficLightRoutine(SimulationData<RfbStructure> &data) : data(data) {}

  /**
   * @brief      Simulates a single step for all junctions of the domain model.
   */
  void perform() {
    DomainModel &model = data.getDomainModel();
    if (model.isGreenWave()) { // debug mode
      for (auto const &junction : model.getJunctions()) {
        junction->nextStep();                              // simulate step anyway
        for (auto const signal : junction->getSignals()) { // set all signals green:
          setStreetForSignal(Signal::GREEN, signal, *junction);
        }
      }
    } else {
      for (auto const &junction : model.getJunctions()) {
        bool lightChanged = junction->nextStep();
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

  /**
   * @brief      Sets the signal of the low level street correlating to the domain level street of a junction signal
   * to a specific value.
   * @param      value  Is the specific signal value to set.
   * @param      signal  Is the specific signal.
   * @param      junction  Is the junction of the signal.
   */
  void setStreetForSignal(Signal value, Junction::Signal signal, const Junction &junction) {
    auto connectedStreet = junction.getIncomingStreet(signal.getDirection());
    if (connectedStreet.isConnected()) {
      Street *domainModelStreet            = connectedStreet.getStreet();
      LowLevelStreet<RfbStructure> &street = data.getStreet(domainModelStreet->getId());
      street.setSignal(value);
    }
  }

  SimulationData<RfbStructure> &data;
};

#endif
