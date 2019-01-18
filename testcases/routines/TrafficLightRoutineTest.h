#include "../domainmodel/DomainModelTestFactory.h"
#include <../../snowhouse/snowhouse.h>

#include "LowLevelStreet.h"
#include "ModelSyncer.h"
#include "NaiveStreetDataStructure.h"
#include "TrafficLightRoutine.h"
#include "TrafficLightSignaler.h"

/**
 * @brief      Gets the signal of the low level street of a junctions signal in the domain model.
 * @param      signal    The desired signal.
 * @param[in]  junction  The junction which contains the signal.
 * @param      data      The data which is used for the simulation.
 * @return     The current signal of the low level street that correlates to the junctions signal.
 */
Signal getLowLevelSignal(Junction::Signal signal, const Junction &junction, SimulationData<NaiveStreetDataStructure> &data) {
  Street *domainModelStreet                        = junction.getIncomingStreet(signal.getDirection()).getStreet();
  LowLevelStreet<NaiveStreetDataStructure> &street = data.getStreet(domainModelStreet->getId());
  return street.getSignal();
}

Signal getCurrentLowLevelSignal(const Junction &junction, SimulationData<NaiveStreetDataStructure> &data) {
  return getLowLevelSignal(junction.getCurrentSignal(), junction, data);
}

Signal getPreviousLowLevelSignal(const Junction &junction, SimulationData<NaiveStreetDataStructure> &data) {
  return getLowLevelSignal(junction.getPreviousSignal(), junction, data);
}

/**
 * @brief      Checks whether the traffic light routine works. Checks the changed lights in the domain model and in the
 * low level model. The following Network is build:
 *     x
 *     |
 *  x--x--x
 *     |
 *     x
 */
void trafficLightRoutineTest() {
  // DOMAIN MODEL SETUP:
  DomainModel model;
  Junction &junction = model.addJunction(createTestJunction()); // clang-format off
  for (CardinalDirection direction = CardinalDirection::NORTH; direction <= CardinalDirection::WEST;
       direction = CardinalDirection(direction + 1)) { // clang-format on
    CardinalDirection opposite = (CardinalDirection)((direction + 2) % 4);
    Junction &other            = model.addJunction(Junction(0, 0, 10, 15, {{Junction::Signal(opposite, 40)}}));
    Street &incoming           = model.addStreet(Street(direction, 1, 50.0, 100.0, other, junction));
    Street &outgoing           = model.addStreet(Street(direction + 4, 1, 50.0, 100.0, junction, other));
    junction.addIncomingStreet(incoming, direction);
    junction.addOutgoingStreet(outgoing, direction);
    other.addIncomingStreet(outgoing, opposite);
    other.addOutgoingStreet(incoming, opposite);
  }
  // ROUTINE SETUP:
  SimulationData<NaiveStreetDataStructure> data(model);
  TrafficLightRoutine<NaiveStreetDataStructure> routine(data);
  ModelSyncer<NaiveStreetDataStructure> modelSyncer(data);
  modelSyncer.buildFreshLowLevel();
  // ACTUAL TESTING:
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  AssertThat(getCurrentLowLevelSignal(junction, data), Is().EqualTo(Signal::GREEN));
  AssertThat(getPreviousLowLevelSignal(junction, data), Is().EqualTo(Signal::RED));
  for (int i = 0; i <= 10; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::EAST));
  AssertThat(getCurrentLowLevelSignal(junction, data), Is().EqualTo(Signal::GREEN));
  AssertThat(getPreviousLowLevelSignal(junction, data), Is().EqualTo(Signal::RED));
  for (int i = 0; i <= 20; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::SOUTH));
  AssertThat(getCurrentLowLevelSignal(junction, data), Is().EqualTo(Signal::GREEN));
  AssertThat(getPreviousLowLevelSignal(junction, data), Is().EqualTo(Signal::RED));
  for (int i = 0; i <= 30; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::WEST));
  AssertThat(getCurrentLowLevelSignal(junction, data), Is().EqualTo(Signal::GREEN));
  AssertThat(getPreviousLowLevelSignal(junction, data), Is().EqualTo(Signal::RED));
  for (int i = 0; i <= 40; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  AssertThat(getCurrentLowLevelSignal(junction, data), Is().EqualTo(Signal::GREEN));
  AssertThat(getPreviousLowLevelSignal(junction, data), Is().EqualTo(Signal::RED));
}