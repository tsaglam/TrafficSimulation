#include "DomainModelTestFactory.h"
#include "Vehicle.h"
#include <../../snowhouse/snowhouse.h>

using namespace snowhouse;

/*
 * Tests if vehicle cycles through its route correctly.
 */
void nextDirectionTest() {
  Vehicle vehicle = createTestVehicle();
  AssertThat(vehicle.getNextDirection(), Equals(TurnDirection::RIGHT));
  AssertThat(vehicle.getNextDirection(), Equals(TurnDirection::UTURN));
  AssertThat(vehicle.getNextDirection(), Equals(TurnDirection::STRAIGHT));
  AssertThat(vehicle.getNextDirection(), Equals(TurnDirection::RIGHT));
}

/*
 * Tests if position can be updated.
 */
void setPositionTest() {
  // set up vehicle and check inital distance:
  Vehicle vehicle            = createTestVehicle();
  Vehicle::Position position = vehicle.getPosition();
  AssertThat(position.getDistance(), Equals(33.3));
  // move with explicit setter and check again:
  vehicle.setPosition(*position.getStreet(), 1, 44.4);
  AssertThat(vehicle.getPosition().getDistance(), Equals(44.4));
  // move with implicit setter and check again:
  Vehicle::Position newPosition = Vehicle::Position(*position.getStreet(), 1, 55.5);
  vehicle.setPosition(newPosition);
  AssertThat(vehicle.getPosition().getDistance(), Equals(55.5));
}