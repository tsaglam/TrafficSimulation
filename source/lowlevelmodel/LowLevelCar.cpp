#include "LowLevelCar.h"

LowLevelCar::LowLevelCar(unsigned int _id, double _targetVelocity, double _maxAcceleration, double _targetDeceleration,
    double _minDistance, double _targetHeadway, double _politeness, double _length)
    : id(_id), targetVelocity(_targetVelocity), maxAcceleration(_maxAcceleration),
      targetDeceleration(_targetDeceleration), minDistance(_minDistance), targetHeadway(_targetHeadway),
      politeness(_politeness), length(_length) {}

LowLevelCar::LowLevelCar(unsigned int _id, double _targetVelocity, double _maxAcceleration, double _targetDeceleration,
    double _minDistance, double _targetHeadway, double _politeness, double _length, unsigned int _lane,
    double _distance, double _velocity)
    : LowLevelCar(_id, _targetVelocity, _maxAcceleration, _targetDeceleration, _minDistance, _targetHeadway,
          _politeness, _length) {
  setPosition(_lane, _distance, _velocity);
}

void LowLevelCar::setPosition(unsigned int lane, double distance, double velocity) {
  currentLane     = lane;
  currentDistance = distance;
  currentVelocity = velocity;
}

unsigned int LowLevelCar::getLane() const { return currentLane; }

double LowLevelCar::getDistance() const { return currentDistance; }

double LowLevelCar::getVelocity() const { return currentVelocity; }

void LowLevelCar::applyUpdates() {
  currentLane     = nextLane;
  currentDistance = nextDistance;
  currentVelocity = nextVelocity;
}

unsigned int LowLevelCar::getId() const { return id; }
double LowLevelCar::getTargetVelocity() const { return targetVelocity; }
double LowLevelCar::getMaxAcceleration() const { return maxAcceleration; }
double LowLevelCar::getTargetDeceleration() const { return targetDeceleration; }
double LowLevelCar::getMinDistance() const { return minDistance; }
double LowLevelCar::getTargetHeadway() const { return targetHeadway; }
double LowLevelCar::getPoliteness() const { return politeness; }
double LowLevelCar::getLength() const { return length; }

void LowLevelCar::setNextBaseAcceleration(double acceleration) { nextBaseAcceleration = acceleration; }
double LowLevelCar::getNextBaseAcceleration() const { return nextBaseAcceleration; }
void LowLevelCar::setNext(unsigned int lane, double distance, double velocity) {
  nextLane     = lane;
  nextDistance = distance;
  nextVelocity = velocity;
}
