#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

/*
 * Creates a test vehicle with somewhat realistic data:
 */
Vehicle createTestVehicle() {
  // prepare objects needed to initialize test car
  const std::vector<Junction::Signal> signals;
  Junction from                    = Junction(0, 0, 10, 15, signals);
  Junction to                      = Junction(1, 1, 10, 20, signals);
  Street street                    = Street(0, 1, 50.0, 100.0, from, to);
  const Vehicle::Position position = Vehicle::Position(street, 1, 33.3);
  // create test route and test vehicle:
  const std::vector<TurnDirection> route{TurnDirection::RIGHT, TurnDirection::UTURN, TurnDirection::STRAIGHT};
  return Vehicle(0, 0, 45.0, 1.0, 1.0, 10.0, 5.0, 0.5, route, position);
}

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