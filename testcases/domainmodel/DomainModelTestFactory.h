#ifndef DOMAINMODELTESTFACTORY_H

#define DOMAINMODELTESTFACTORY_H

#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

/*
 * Creates a test street with two junctions and no signals:
 */
Street createTestStreet() {
  const std::vector<Junction::Signal> signals;
  Junction from = Junction(0, 0, 10, 15, signals);
  Junction to   = Junction(1, 1, 10, 20, signals);
  return Street(0, 1, 50.0, 100.0, from, to);
}

/*
 * Creates a test vehicle with somewhat realistic data:
 */
Vehicle createTestVehicle() {
  Street street                    = createTestStreet();
  const Vehicle::Position position = Vehicle::Position(street, 1, 33.3);
  // create test route and test vehicle:
  const std::vector<TurnDirection> route{TurnDirection::RIGHT, TurnDirection::UTURN, TurnDirection::STRAIGHT};
  return Vehicle(0, 0, 45.0, 1.0, 1.0, 10.0, 5.0, 0.5, route, position);
}

/*
 * Creates a test junction with four valid signals:
 */
Junction createTestJunction() {
  Junction::Signal signalNorth = Junction::Signal(CardinalDirection::NORTH, 30);
  Junction::Signal signalEast  = Junction::Signal(CardinalDirection::EAST, 20);
  Junction::Signal signalSouth = Junction::Signal(CardinalDirection::SOUTH, 10);
  Junction::Signal signalWest  = Junction::Signal(CardinalDirection::WEST, 0);
  const std::vector<Junction::Signal> signals{signalNorth, signalEast, signalSouth, signalWest};
  return Junction(0, 0, 10, 15, signals);
}

#endif