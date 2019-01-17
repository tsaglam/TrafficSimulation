#include "ConsistencyRoutine.h"
#include "LowLevelCar.h"
#include "NaiveStreetDataStructure.h"
#include <../../snowhouse/snowhouse.h>

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
