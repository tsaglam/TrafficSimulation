#include "Vehicle.h"

Vehicle::Position::Position(Street& _street, unsigned int _lane,
                            double _distance)
    : street(&_street), lane(_lane), distance(_distance) {}

Street* Vehicle::Position::getStreet() const { return street; }
unsigned int Vehicle::Position::getLane() const { return lane; }
double Vehicle::Position::getDistance() const { return distance; }

Vehicle::Vehicle(id_type _id, int _externalId, double _targetVelocity,
                 double _maxAcceleration, double _targetDeceleration,
                 double _minDistance, double _targetHeadway, double _politeness,
                 const std::vector<TurnDirection>& _route,
                 const Position& _position)
    : id(_id),
      externalId(_externalId),
      targetVelocity(_targetVelocity),
      maxAcceleration(_maxAcceleration),
      targetDeceleration(_targetDeceleration),
      minDistance(_minDistance),
      targetHeadway(_targetHeadway),
      politeness(_politeness),
      route(_route),
      position(_position) {
  directionIndex = 0;
}

Vehicle::Vehicle(id_type _id, int _externalId, double _targetVelocity,
                 double _maxAcceleration, double _targetDeceleration,
                 double _minDistance, double _targetHeadway, double _politeness,
                 std::vector<TurnDirection>&& _route, const Position& _position)
    : id(_id),
      externalId(_externalId),
      targetVelocity(_targetVelocity),
      maxAcceleration(_maxAcceleration),
      targetDeceleration(_targetDeceleration),
      minDistance(_minDistance),
      targetHeadway(_targetHeadway),
      politeness(_politeness),
      route(_route),
      position(_position) {
  directionIndex = 0;
}

void Vehicle::setPosition(const Position& _position) { position = _position; }

void Vehicle::setPosition(Street& street, unsigned int lane, double distance) {
  Vehicle::Position position(street, lane, distance);
  setPosition(position);
}

TurnDirection Vehicle::getNextDirection() {
  TurnDirection next = route.at(directionIndex);
  directionIndex = (directionIndex + 1) % route.size();
  return next;
}

id_type Vehicle::getId() const { return id; }
int Vehicle::getExternalId() const { return externalId; }
double Vehicle::getTargetVelocity() const { return targetVelocity; }
double Vehicle::getMaxAcceleration() const { return maxAcceleration; }
double Vehicle::getTargetDeceleration() const { return targetDeceleration; }
double Vehicle::getMinDistance() const { return minDistance; }
double Vehicle::getTargetHeadway() const { return targetHeadway; }
double Vehicle::getPoliteness() const { return politeness; }
const Vehicle::Position& Vehicle::getPosition() const { return position; }
