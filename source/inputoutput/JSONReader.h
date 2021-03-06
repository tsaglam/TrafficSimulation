#ifndef JSONREADER_H
#define JSONREADER_H

#include <exception>
#include <istream>
#include <string>

#include "../../json/single_include/nlohmann/json.hpp"

#include "DomainModel.h"
#include "Junction.h"

class JSONReader {
public:
  class Exception : public std::exception {
  protected:
    std::string whatStr;

  public:
    Exception(const char *what);
    virtual const char *what() const throw();
  };

  enum Mode { SIMULATE, OPTIMIZE };

private:
  /**
   * Represents a two-directional road as read from the JSON input.
   */
  class Road {
  private:
    friend class JSONReader;

    Junction &junction1;
    Junction &junction2;
    double length;
    unsigned int lanes;
    double speedLimit;

  public:
    Road(Junction &junction1, Junction &junction2, double length, unsigned int lanes, double speedLimit);
  };

private:
  std::istream &in;
  bool hasBeenRead;
  unsigned int timeSteps;
  unsigned int minTravelDistance;
  Mode mode;

private:
  /**
   * Converts a speed in km/h to m/s.
   */
  double kmhToMs(double speed) const;
  /**
   * Computes the distance between two junctions in metres.
   * The junctions are not assumed to be aligned orthogonally.
   */
  double distance(Junction &junction1, Junction &junction2) const;
  /**
   * Computes the relative direction (CardinalDirection) from an origin
   * junction towards another junction, i.e. what direction to look to from
   * origin to see the other junction.
   */
  CardinalDirection relativeDirection(Junction &origin, Junction &other) const;

  Junction readJunction(const nlohmann::json &inputJunction, Mode mode) const;
  Road readRoad(const nlohmann::json &inputRoad, const std::map<int, Junction *> &junctionsMap) const;
  Vehicle readVehicle(const nlohmann::json &inputVehicle, const std::map<int, Junction *> &junctionsMap) const;

public:
  JSONReader(std::istream &in);
  void readInto(DomainModel &domainModel);
  /**
   * Returns the time_steps specified in a file read.
   * Calling this method is only valid after a file has been read.
   */
  unsigned int getTimeSteps() const;
  /**
   * Returns the execution mode specified by the file read.
   * Calling this method is only valid after a file has been read.
   */
  Mode getMode() const;
  /**
   * Returns the min_travel_distance specified in a file read.
   * Calling this method is only valid after a file has been read and the mode is OPTIMIZE.
   */
  unsigned int getMinTravelDistance() const;
};

#endif
