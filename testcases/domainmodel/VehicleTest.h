#include "DomainModelTestFactory.h"
#include "Vehicle.h"

/*
 * Tests if vehicle cycles through its route correctly.
 */
bool nextDirectionTest() {
  Vehicle vehicle = createTestVehicle();
  bool correct    = vehicle.getNextDirection() == TurnDirection::RIGHT;
  correct &= vehicle.getNextDirection() == TurnDirection::UTURN;
  correct &= vehicle.getNextDirection() == TurnDirection::STRAIGHT;
  return correct && vehicle.getNextDirection() == TurnDirection::RIGHT;
}

/*
 * Tests if position can be updated.
 */
bool setPositionTest() {
  // set up vehicle and check inital distance:
  Vehicle vehicle            = createTestVehicle();
  Vehicle::Position position = vehicle.getPosition();
  bool correct               = position.getDistance() == 33.3;
  // move with explicit setter and check again:
  vehicle.setPosition(*position.getStreet(), 1, 44.4);
  correct &= vehicle.getPosition().getDistance() == 44.4;
  // move with implicit setter and check again:
  Vehicle::Position newPosition = Vehicle::Position(*position.getStreet(), 1, 55.5);
  vehicle.setPosition(newPosition);
  return correct && vehicle.getPosition().getDistance() == 55.5;
}