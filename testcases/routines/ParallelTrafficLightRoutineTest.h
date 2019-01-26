#include "../domainmodel/DomainModelTestFactory.h"
#include <../../snowhouse/snowhouse.h>

#include "LowLevelStreet.h"
#include "ModelSyncer.h"
#include "NaiveStreetDataStructure.h"
#include "ParallelTrafficLightRoutine.h"
#include "TrafficLightRoutineTest.h"
#include "TrafficLightSignaler.h"

/**
 * @brief      Checks whether the traffic light routine works. Checks the changed lights in the domain model and in the
 * low level model. The following Network is build:
 *     x
 *     |
 *  x--x--x
 *     |
 *     x
 */
void parallelTrafficLightRoutineTest() {
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
  ParallelTrafficLightRoutine<NaiveStreetDataStructure> routine(data);
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