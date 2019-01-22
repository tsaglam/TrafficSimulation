#ifndef JUNCTION_H
#define JUNCTION_H

#include <array>
#include <vector>

#include "DomainModelCommon.h"
#include "Street.h"

class Street;

class Junction {
private:
  friend class DomainModel;

public:
  /**
   * @brief      Represents a traffic light for one of the directions of the junction.
   */
  class Signal {
  private:
    CardinalDirection direction;
    unsigned int signalDuration;

  public:
    Signal() = default;
    Signal(CardinalDirection direction, unsigned int signalDuration);
    CardinalDirection getDirection() const;
    unsigned int getDuration() const;
  };

private:
  class ConnectedStreet {
  private:
    friend class Junction;

    bool connected;
    Street *street;
    CardinalDirection direction;

  public:
    ConnectedStreet() = default;
    ConnectedStreet(bool connected, Street *street, CardinalDirection direction);
    bool isConnected() const;
    Street *getStreet() const;
    CardinalDirection getDirection() const;
  };

private:
  id_type id;
  int externalId;
  int x;
  int y;
  std::vector<Signal> signals;
  std::array<ConnectedStreet, 4> incomingStreets = {
      {ConnectedStreet(false, nullptr, NORTH), ConnectedStreet(false, nullptr, EAST),
          ConnectedStreet(false, nullptr, SOUTH), ConnectedStreet(false, nullptr, WEST)}};
  std::array<ConnectedStreet, 4> outgoingStreets = {
      {ConnectedStreet(false, nullptr, NORTH), ConnectedStreet(false, nullptr, EAST),
          ConnectedStreet(false, nullptr, SOUTH), ConnectedStreet(false, nullptr, WEST)}};

  int currentTimer;
  int signalIndex;

  void initJunction();

public:
  Junction(id_type id, int externalId, int x, int y, const std::vector<Signal> &signals);
  Junction(id_type id, int externalId, int x, int y, std::vector<Signal> &&signals);

  /**
   * @brief      Adds an incoming street for a specific direction and marks it internally as connected.
   * @param      street     The incoming street to add.
   * @param[in]  direction  The cardinal direction from where the street is coming in.
   */
  void addIncomingStreet(Street &street, CardinalDirection direction);

  /**
   * @brief      Adds an outgoing street for a specific direction and marks it internally as connected.
   * @param      street     The outgoing street to add.
   * @param[in]  direction  The cardinal direction from where the street is going out.
   */
  void addOutgoingStreet(Street &street, CardinalDirection direction);

  /**
   * @brief      simlates a step. Decreases the traffic light timer. Toggles traffic lights.
   * @return     true if a traffic light was switched, false if only the timer was decreased.
   */
  bool nextStep();

  /**
   * @brief      Gives the junction new signals, resets the current signal and the current timer.
   * @param[in]  newSignals  The new signals.
   */
  void setSignals(std::vector<Signal> newSignals);

  // access methods:
  id_type getId() const;
  int getExternalId() const;
  int getX() const;
  int getY() const;
  Signal getCurrentSignal() const;  // is also the signal that is green
  Signal getPreviousSignal() const; // is also the last signal that has been green before the current
  std::vector<Signal> getSignals() const;
  const ConnectedStreet &getIncomingStreet(CardinalDirection direction) const;
  const ConnectedStreet &getOutgoingStreet(CardinalDirection direction) const;
  const std::array<ConnectedStreet, 4> &getIncomingStreets() const;
  const std::array<ConnectedStreet, 4> &getOutgoingStreets() const;
};

#endif
