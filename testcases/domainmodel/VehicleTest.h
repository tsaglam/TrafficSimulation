#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

bool nextDirectionTest() {
  // prepare objects needed to initialize test car
  const std::vector<Junction::Signal> signals;
  Junction junction                = Junction(0, 0, 0, 0, signals);
  Street street                    = Street(0, 1, 0.0, 0.0, junction, junction);
  const Vehicle::Position position = Vehicle::Position(street, 1, 0.0);
  // create test route and test vehicle:
  const std::vector<TurnDirection> route{TurnDirection::RIGHT, TurnDirection::UTURN, TurnDirection::STRAIGHT};
  Vehicle vehicle = Vehicle(0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, route, position);
  // test if correct route is taken:
  bool correct = vehicle.getNextDirection() == TurnDirection::RIGHT;
  correct &= vehicle.getNextDirection() == TurnDirection::UTURN;
  correct &= vehicle.getNextDirection() == TurnDirection::STRAIGHT;
  correct &= vehicle.getNextDirection() == TurnDirection::RIGHT;
  return correct;
}