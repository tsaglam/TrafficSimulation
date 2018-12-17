#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include "DomainModelCommon.h"
#include "Street.h"

class Vehicle {
private:
  friend class DomainModel;

public:
  class Position {
  private:
    Street* street;
    unsigned int lane;
    double distance;
  public:
    Position(Street& street, unsigned int lane, double distance);
  };

private:
  id_type id;
  int externalId;
  double targetVelocity;
  double maxAcceleration;
  double targetDeceleration;
  double minDistance;
  double targetHeadway;
  double politeness;
  std::vector<TurnDirection> route;
  Position position;

 public:
  Vehicle(id_type id, int externalId, double targetVelocity,
          double maxAcceleration, double targetDeceleration,
          double minDistance, double targetHeadway, double politeness,
          std::vector<TurnDirection>& route, Position& position);
  void setPosition(Position& position);
  void setPosition(Street& street, unsigned int lane, double distance);
};

#endif
