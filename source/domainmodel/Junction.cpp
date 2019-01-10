#include "Junction.h"
#include <iostream>

/*
 * Class Signal:
 */
Junction::Signal::Signal(CardinalDirection _direction, unsigned int _signalDuration)
    : direction(_direction), signalDuration(_signalDuration) {}

CardinalDirection Junction::Signal::getDirection() const { return direction; }
unsigned int Junction::Signal::getDuration() const { return signalDuration; }

/*
 * Class ConnectedStreet:
 */
Junction::ConnectedStreet::ConnectedStreet(bool _connected, Street *_street, CardinalDirection _direction)
    : connected(_connected), street(_street), direction(_direction) {}

bool Junction::ConnectedStreet::isConnected() const { return connected; }
Street *Junction::ConnectedStreet::getStreet() const { return street; }
CardinalDirection Junction::ConnectedStreet::getDirection() const { return direction; }

/*
 * Class Junction:
 */
Junction::Junction(id_type _id, int _externalId, int _x, int _y, const std::vector<Signal> &_signals)
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {
  initJunction();
}

Junction::Junction(id_type _id, int _externalId, int _x, int _y, std::vector<Signal> &&_signals)
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {
  initJunction();
}

void Junction::initJunction() { // TODO test me!
  signalIndex = 0;
  if (signals.empty()) {
    currentTimer = -1;
  } else {
    currentTimer = getCurrentSignal().getDuration();
  }
}

bool Junction::nextStep() { // TODO test me!
  if (currentTimer == 0) {
    signalIndex  = (signalIndex + 1) % signals.size(); // next signal
    currentTimer = getCurrentSignal().getDuration();   // reset timer
    return true;                                       // indicate signal change
  } else {
    currentTimer--;
    return false;
  }
}

void Junction::addIncomingStreet(Street &_street, CardinalDirection _direction) {
  incomingStreets[_direction].connected = true;
  incomingStreets[_direction].street    = &_street;
}

void Junction::addOutgoingStreet(Street &_street, CardinalDirection _direction) {
  outgoingStreets[_direction].connected = true;
  outgoingStreets[_direction].street    = &_street;
}

// Access methods:
id_type Junction::getId() const { return id; }
int Junction::getExternalId() const { return externalId; }
int Junction::getX() const { return x; }
int Junction::getY() const { return y; }
Junction::Signal Junction::getCurrentSignal() const {
  if (signalIndex == -1) { throw std::out_of_range("Junction has no signals!"); }
  return signals.at(signalIndex);
}
std::vector<Junction::Signal> Junction::getSignals() const { return signals; }
const Junction::ConnectedStreet &Junction::getIncomingStreet(CardinalDirection direction) const {
  return incomingStreets[direction];
}
const Junction::ConnectedStreet &Junction::getOutgoingStreet(CardinalDirection direction) const {
  return outgoingStreets[direction];
}
const std::array<Junction::ConnectedStreet, 4> &Junction::getIncomingStreets() const { return incomingStreets; }
const std::array<Junction::ConnectedStreet, 4> &Junction::getOutgoingStreets() const { return outgoingStreets; }