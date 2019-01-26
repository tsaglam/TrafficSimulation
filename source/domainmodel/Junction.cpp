#include "Junction.h"
#include "JunctionException.h"

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

void Junction::initJunction() {
  if (signals.empty()) { // pre-simulation state, is allowed but cannot be simulated.
    signalIndex  = -1;
    currentTimer = -1;
  } else {
    signalIndex  = 0;
    currentTimer = getCurrentSignal().getDuration();
  }
}

bool Junction::nextStep() {
  if (currentTimer == 0) {
    signalIndex  = (signalIndex + 1) % signals.size(); // next signal
    currentTimer = getCurrentSignal().getDuration();   // reset timer
    currentTimer--;                                    // also decrement the timer, one tick
    return true;                                       // indicate signal change
  } else if (currentTimer > 0) {
    currentTimer--;
    return false;
  } else {
    throw JunctionException(*this, "Cannot simulate step on junction without traffic lights!");
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

void Junction::setSignals(std::vector<Signal> newSignals) {
  signals = newSignals;
  initJunction(); // reset timer and current signal index
}

// Access methods:
id_type Junction::getId() const { return id; }
int Junction::getExternalId() const { return externalId; }
int Junction::getX() const { return x; }
int Junction::getY() const { return y; }
Junction::Signal Junction::getCurrentSignal() const {
  if (signalIndex == -1) { throw JunctionException(*this, "Junction has no signals!"); }
  return signals.at(signalIndex);
}
Junction::Signal Junction::getPreviousSignal() const {
  if (signalIndex == -1) { throw JunctionException(*this, "Junction has no signals!"); }
  int indexOfPrevious = (signalIndex + signals.size() - 1) % signals.size();
  return signals.at(indexOfPrevious);
}
const std::vector<Junction::Signal> &Junction::getSignals() const { return signals; }
const Junction::ConnectedStreet &Junction::getIncomingStreet(CardinalDirection direction) const {
  return incomingStreets[direction];
}
const Junction::ConnectedStreet &Junction::getOutgoingStreet(CardinalDirection direction) const {
  return outgoingStreets[direction];
}
const std::array<Junction::ConnectedStreet, 4> &Junction::getIncomingStreets() const { return incomingStreets; }
const std::array<Junction::ConnectedStreet, 4> &Junction::getOutgoingStreets() const { return outgoingStreets; }