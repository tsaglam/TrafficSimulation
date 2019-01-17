#include "../domainmodel/DomainModelTestFactory.h"
#include <../../snowhouse/snowhouse.h>

#include "ModelSyncer.h"
#include "NaiveStreetDataStructure.h"
#include "TrafficLightRoutine.h"
#include <iostream>

/**
 *	Checks whether the traffic light routine works.
 * The following Network is build:
 *     x
 *     |
 *  x--x--x
 *     |
 *     x
 */
void trafficLightRoutineTest() {
  // DOMAIN MODEL SETUP:
  DomainModel model;
  Junction &junction = model.addJunction(createTestJunction());
  // clang-format off
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
  for (int i = 0; i <= 10; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::EAST));
  for (int i = 0; i <= 20; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::SOUTH));
  for (int i = 0; i <= 30; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::WEST));
  for (int i = 0; i <= 40; ++i) { routine.perform(); }
  AssertThat(junction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  // TODO check low level street
}