#include "Vehicle.h"

Vehicle::Position::Position(Street& _street, unsigned int _lane,
                            double _distance)
  : street(&_street), lane(_lane), distance(_distance) {}

Vehicle::Vehicle(id_type _id, int _externalId, double _targetVelocity,
          double _maxAcceleration, double _targetDeceleration,
          double _minDistance, double _targetHeadway, double _politeness,
          std::vector<TurnDirection>& _route, Position& _position)
  : id(_id), externalId(_externalId), targetVelocity(_targetVelocity),
    maxAcceleration(_maxAcceleration),
    targetDeceleration(_targetDeceleration), minDistance(_minDistance),
    targetHeadway(_targetHeadway), politeness(_politeness), route(_route),
    position(_position) {}

void Vehicle::setPosition(Position& _position) {
  position = _position;
}

void Vehicle::setPosition(Street& street, unsigned int lane, double distance) {
  Vehicle::Position position(street, lane, distance);
  setPosition(position);
}
