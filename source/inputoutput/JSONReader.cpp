#include "JSONReader.h"

#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <utility>
#include <vector>

#include "json.hpp"

#include "DomainModel.h"
#include "DomainModelCommon.h"
#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

using json = nlohmann::json;

JSONReader::Exception::Exception(const char *_what) : whatStr(_what) {}

const char *JSONReader::Exception::what() const throw() { return whatStr.c_str(); }

JSONReader::Road::Road(
    Junction &_junction1, Junction &_junction2, double _length, unsigned int _lanes, double _speedLimit)
    : junction1(_junction1), junction2(_junction2), length(_length), lanes(_lanes), speedLimit(_speedLimit) {}

JSONReader::JSONReader(std::istream &_in) : in(_in), hasBeenRead(false) {}

unsigned int JSONReader::getTimeSteps() const {
  if (!hasBeenRead) throw JSONReader::Exception("No input has been read yet");

  return timeSteps;
}

void JSONReader::readInto(DomainModel &domainModel) {
  if (hasBeenRead) throw JSONReader::Exception("Attempting to read input although input has already been read");

  json input;

  // May throw exception
  std::cin >> input;

  timeSteps = input["time_steps"].get<unsigned int>();

  std::map<int, Junction *> junctionsMap;
  std::map<int, Vehicle *> vehiclesMap;

  for (const auto &inputJunction : input["junctions"]) {
    Junction junction = readJunction(inputJunction);

    if (junctionsMap.count(junction.getExternalId()) > 0)
      throw JSONReader::Exception("Duplicate Junction ID encountered.");

    Junction &domainJunction                     = domainModel.addJunction(std::move(junction));
    junctionsMap[domainJunction.getExternalId()] = &domainJunction;
  }

  for (const auto &inputRoad : input["roads"]) {
    Road road = readRoad(inputRoad, junctionsMap);

    Street street1(0, road.lanes, road.speedLimit, road.length, road.junction1, road.junction2);
    Street &domainStreet1 = domainModel.addStreet(std::move(street1));

    Street street2(0, road.lanes, road.speedLimit, road.length, road.junction2, road.junction1);
    Street &domainStreet2 = domainModel.addStreet(std::move(street2));

    road.junction1.addOutgoingStreet(domainStreet1, relativeDirection(road.junction1, road.junction2));
    road.junction2.addIncomingStreet(domainStreet1, relativeDirection(road.junction2, road.junction1));

    road.junction2.addOutgoingStreet(domainStreet2, relativeDirection(road.junction2, road.junction1));
    road.junction1.addIncomingStreet(domainStreet2, relativeDirection(road.junction1, road.junction2));
  }

  for (const auto &inputVehicle : input["cars"]) {
    Vehicle vehicle = readVehicle(inputVehicle, junctionsMap);

    if (vehiclesMap.count(vehicle.getExternalId()) > 0)
      throw JSONReader::Exception("Duplicate Vehicle ID encountered.");

    Vehicle &domainVehicle                     = domainModel.addVehicle(std::move(vehicle));
    vehiclesMap[domainVehicle.getExternalId()] = &domainVehicle;
  }

  hasBeenRead = true;
}

Junction JSONReader::readJunction(const json &inputJunction) const {
  int externalId = inputJunction["id"].get<int>();
  int x          = inputJunction["x"].get<int>();
  int y          = inputJunction["y"].get<int>();

  std::vector<Junction::Signal> signals;
  signals.reserve(inputJunction["signals"].size());

  for (const auto &inputSignal : inputJunction["signals"]) {
    // 0->North, 1->East, 2->South, 3->West
    unsigned int direction = inputSignal["dir"].get<unsigned int>();
    if (direction > WEST) throw JSONReader::Exception("Invalid direction in signals list");
    // Input in s
    unsigned int time = inputSignal["time"].get<unsigned int>();

    Junction::Signal signal(static_cast<CardinalDirection>(direction), time);
    signals.push_back(signal);
  }

  return Junction(0, externalId, x, y, std::move(signals));
}

JSONReader::Road JSONReader::readRoad(const json &inputRoad, const std::map<int, Junction *> &junctionsMap) const {
  // junction IDs
  int junctionId1 = inputRoad["junction1"].get<int>();
  int junctionId2 = inputRoad["junction2"].get<int>();
  // Number of lanes into each direction; 1, 2 or 3
  unsigned int lanes = inputRoad["lanes"].get<unsigned int>();
  // Input in km/h
  double speedLimit = kmhToMs(inputRoad["limit"].get<double>());

  Junction *junction1 = junctionsMap.at(junctionId1);
  Junction *junction2 = junctionsMap.at(junctionId2);

  double length = distance(*junction1, *junction2);

  return JSONReader::Road(*junction1, *junction2, length, lanes, speedLimit);
}

Vehicle JSONReader::readVehicle(const json &inputVehicle, const std::map<int, Junction *> &junctionsMap) const {
  int externalId = inputVehicle["id"].get<int>();
  // Input in km/h
  double targetVelocity = kmhToMs(inputVehicle["target_velocity"].get<double>());
  // Input in m/s²
  double maxAcceleration = inputVehicle["max_acceleration"].get<double>();
  // Input in m/s²
  double targetDeceleration = inputVehicle["target_deceleration"].get<double>();
  // Input in m
  double minDistance = inputVehicle["min_distance"].get<double>();
  // Input in s
  double targetHeadway = inputVehicle["target_headway"].get<double>();
  // Input in range [0.0, 1.0]
  double politeness = inputVehicle["politeness"].get<double>();
  if (politeness < 0.0 || politeness > 1.0) throw JSONReader::Exception("Invalid politeness in vehicle details");

  // starting position via junction IDs
  int fromJunctionId = inputVehicle["start"]["from"].get<int>();
  int toJunctionId   = inputVehicle["start"]["to"].get<int>();
  // Lane on street
  unsigned int lane = inputVehicle["start"]["lane"].get<unsigned int>();
  // distance from 'from' junction in m
  double distance = inputVehicle["start"]["distance"].get<double>();

  Junction *fromJunction = junctionsMap.at(fromJunctionId);

  // Iterate over outgoing streets and find the one leading to the
  // junction with externalId toJunctionId
  bool found = false;
  Street *street;
  for (const auto &connectedStreet : fromJunction->getOutgoingStreets()) {
    if (!connectedStreet.isConnected()) continue;

    street = connectedStreet.getStreet();
    if (street->getTargetJunction().getExternalId() == toJunctionId) {
      found = true;
      break;
    }
  }
  if (!found) throw JSONReader::Exception("Unknown Junction ID in vehicle position specification: no such street");

  if (lane >= street->getLanes())
    throw JSONReader::Exception("Invalid lane in vehicle position specification: no such lane on street");

  if (distance < 0 || distance > street->getLength())
    throw JSONReader::Exception("Invalid distance in vehicle position specification: not on street");

  Vehicle::Position position(*street, lane, distance);

  std::vector<TurnDirection> route;
  route.reserve(inputVehicle["route"].size());

  for (const auto &inputTurn : inputVehicle["route"]) {
    // turn order: 0->uturn, 1->left, 2->straight, 3->right
    unsigned int turnDirection = inputTurn.get<unsigned int>();
    if (turnDirection > RIGHT) throw JSONReader::Exception("Invalid turn direction in vehicle route");
    route.push_back(static_cast<TurnDirection>(turnDirection));
  }

  return Vehicle(0, externalId, targetVelocity, maxAcceleration, targetDeceleration, minDistance, targetHeadway,
      politeness, std::move(route), position);
}

double JSONReader::kmhToMs(double speed) const { return speed * 1000.0 / 3600.0; }

double JSONReader::distance(Junction &junction1, Junction &junction2) const {
  double xOffset = static_cast<double>(junction1.getX() - junction2.getX());
  double yOffset = static_cast<double>(junction1.getY() - junction2.getY());

  return std::hypot(xOffset, yOffset) * 100.0;
}

CardinalDirection JSONReader::relativeDirection(Junction &origin, Junction &other) const {
  int xOffset = other.getX() - origin.getX();
  int yOffset = other.getY() - origin.getY();

  // Left-handed cartesian coordinate system, let's transform into right-handed
  yOffset = -yOffset;

  if (xOffset >= 0) {
    if (std::abs(yOffset) < xOffset)
      return EAST;
    else if (yOffset >= 0)
      return NORTH;
    else
      return SOUTH;
  } else {
    if (std::abs(yOffset) < -xOffset)
      return WEST;
    else if (yOffset >= 0)
      return NORTH;
    else
      return SOUTH;
  }
}
