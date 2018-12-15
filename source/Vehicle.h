#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include "Direction.h"

class Vehicle {
  unsigned int id;
  double targetVelocity;
  double maxAcceleration;
  double targetDeceleration;
  double minDistance;
  double targetHeadway;
  double politeness;
  std::vector<Direction> route;

 public:
  Vehicle(unsigned int id, double targetVelocity, double maxAcceleration,
          double targetDeceleration, double minDistance, double targetHeadway,
          double politeness);
};

#endif
