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
    Street *street;
    unsigned int lane;
    double distance;

  public:
    Position(Street &street, unsigned int lane, double distance);
    Street *getStreet() const;
    unsigned int getLane() const;
    double getDistance() const;
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
  int directionIndex;

public:
  Vehicle(id_type id, int externalId, double targetVelocity, double maxAcceleration, double targetDeceleration,
      double minDistance, double targetHeadway, double politeness, const std::vector<TurnDirection> &route,
      const Position &position);
  Vehicle(id_type id, int externalId, double targetVelocity, double maxAcceleration, double targetDeceleration,
      double minDistance, double targetHeadway, double politeness, std::vector<TurnDirection> &&route,
      const Position &position);

  /**
   * @brief      Updates the position of the car.
   * @param[in]  position  is the new position which contains street, lane and distance.
   */
  void setPosition(const Position &position);

  /**
   * @brief      Updates the position of the car. Conviencience method of setPosition(const Position &position) which
   * creates a position object.
   * @param      street    is the new street.
   * @param[in]  lane      is the new lane number.
   * @param[in]  distance  is the new distance on the street in meters.
   */
  void setPosition(Street &street, unsigned int lane, double distance);

  /**
   * @brief      Returns the next direction of the route. Keeps automatically track of the route cycle, therefore this
   * method should only be called if the vehicle is actually moved.
   * @return     The next TurnDirection of the route.
   */
  TurnDirection getNextDirection();

  // access methods:
  id_type getId() const;
  int getExternalId() const;
  double getTargetVelocity() const;
  double getMaxAcceleration() const;
  double getTargetDeceleration() const;
  double getMinDistance() const;
  double getTargetHeadway() const;
  double getPoliteness() const;
  const Position &getPosition() const;
};

#endif
