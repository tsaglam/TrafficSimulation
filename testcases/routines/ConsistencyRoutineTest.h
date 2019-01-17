#include "../domainmodel/DomainModelTestFactory.h"
#include "ConsistencyRoutine.h"
#include "LowLevelCar.h"
#include "NaiveStreetDataStructure.h"
#include <../../snowhouse/snowhouse.h>
#include <iostream>

void takeTurnTest() {
  // SETUP:
  DomainModel model;
  SimulationData<NaiveStreetDataStructure> data(model);
  ConsistencyRoutine<NaiveStreetDataStructure> routine(data);
  // TESTS:
  CardinalDirection result = routine.takeTurn(CardinalDirection::NORTH, TurnDirection::STRAIGHT);
  AssertThat(result, Is().EqualTo(CardinalDirection::SOUTH));
  result = routine.takeTurn(CardinalDirection::WEST, TurnDirection::STRAIGHT);
  AssertThat(result, Is().EqualTo(CardinalDirection::EAST));
  result = routine.takeTurn(CardinalDirection::WEST, TurnDirection::LEFT);
  AssertThat(result, Is().EqualTo(CardinalDirection::NORTH));
  result = routine.takeTurn(CardinalDirection::EAST, TurnDirection::RIGHT);
  AssertThat(result, Is().EqualTo(CardinalDirection::NORTH));
  result = routine.takeTurn(CardinalDirection::SOUTH, TurnDirection::UTURN);
  AssertThat(result, Is().EqualTo(CardinalDirection::SOUTH));
}

void calculateOriginDirectionTest() {
  // ROUTINE SETUP:
  DomainModel model;
  SimulationData<NaiveStreetDataStructure> data(model);
  ConsistencyRoutine<NaiveStreetDataStructure> routine(data);
  // DOMAIN MODEL SETUP:
  Junction other    = createTestJunction();
  Junction junction = createTestJunction();
  // clang-format off
  for (CardinalDirection direction = CardinalDirection::NORTH; direction <= CardinalDirection::WEST;
       direction = CardinalDirection(direction + 1)) { // clang-format on
    Street incoming = Street(direction, 1, 50.0, 100.0, other, junction);
    Street outgoing = Street(direction + 4, 1, 50.0, 100.0, junction, other);

    junction.addIncomingStreet(model.addStreet(incoming), direction);
    junction.addOutgoingStreet(model.addStreet(outgoing), direction);
  }
  // TESTS:
  // clang-format off
  for (CardinalDirection direction = CardinalDirection::NORTH; direction <= CardinalDirection::WEST;
       direction = CardinalDirection(direction + 1)) { // clang-format on
    Street &street           = *junction.getIncomingStreet(direction).getStreet();
    CardinalDirection result = routine.calculateOriginDirection(junction, street);
    AssertThat(result, Is().EqualTo(direction));
  }
}
