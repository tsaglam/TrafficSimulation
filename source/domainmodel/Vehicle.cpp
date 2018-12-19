#include "Vehicle.h"

Vehicle::Position::Position(Street& _street, unsigned int _lane,
                            double _distance)
    : street(&_street), lane(_lane), distance(_distance) {}

Vehicle::Vehicle(id_type _id, int _externalId, double _targetVelocity,
                 double _maxAcceleration, double _targetDeceleration,
                 double _minDistance, double _targetHeadway, double _politeness,
                 std::vector<TurnDirection>& _route, Position& _position)
    : id(_id),
      externalId(_externalId),
      targetVelocity(_targetVelocity),
      maxAcceleration(_maxAcceleration),
      targetDeceleration(_targetDeceleration),
      minDistance(_minDistance),
      targetHeadway(_targetHeadway),
      politeness(_politeness),
      route(_route),
      position(_position) {}

void Vehicle::setPosition(Position& _position) { position = _position; }

void Vehicle::setPosition(Street& street, unsigned int lane, double distance) {
  Vehicle::Position position(street, lane, distance);
  setPosition(position);
}

TurnDirection Vehicle::getNextDirection() {
  TurnDirection next = route.at(directionIndex);
  directionIndex = (directionIndex + 1) % route.size();
  return next;
}

id_type Vehicle::getId() { return id; }
int Vehicle::getExternalId() { return externalId; }
double Vehicle::getTargetVelocity() { return targetVelocity; }
double Vehicle::getMaxAcceleration() { return maxAcceleration; }
double Vehicle::getTargetDeceleration() { return targetDeceleration; }
double Vehicle::getMinDistance() { return minDistance; }
double Vehicle::getTargetHeadway() { return targetHeadway; }
double Vehicle::getPoliteness() { return politeness; }
Vehicle::Position& Vehicle::getPosition() { return position; }
