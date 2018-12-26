#include "Junction.h"

/*
 * Class Signal:
 */
Junction::Signal::Signal(CardinalDirection _direction, unsigned int _time) : direction(_direction), time(_time) {}

CardinalDirection Junction::Signal::getDirection() const { return direction; }
unsigned int Junction::Signal::getTime() const { return time; }

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
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {}

Junction::Junction(id_type _id, int _externalId, int _x, int _y, std::vector<Signal> &&_signals)
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {}

void Junction::addIncomingStreet(Street &_street, CardinalDirection _direction) {
  incomingStreets[_direction].connected = true;
  incomingStreets[_direction].street    = &_street;
}

void Junction::addOutgoingStreet(Street &_street, CardinalDirection _direction) {
  outgoingStreets[_direction].connected = true;
  outgoingStreets[_direction].street    = &_street;
}

const std::array<Junction::ConnectedStreet, 4> &Junction::getIncomingStreets() const { return incomingStreets; }
const std::array<Junction::ConnectedStreet, 4> &Junction::getOutgoingStreets() const { return outgoingStreets; }

// Access methods:
id_type Junction::getId() const { return id; }
int Junction::getExternalId() const { return externalId; }
int Junction::getX() const { return x; }
int Junction::getY() const { return y; }
std::vector<Junction::Signal> Junction::getSignals() const { return signals; }
const Junction::ConnectedStreet &Junction::getIncomingStreet(CardinalDirection direction) const {
  return incomingStreets[direction];
}
const Junction::ConnectedStreet &Junction::getOutgoingStreet(CardinalDirection direction) const {
  return outgoingStreets[direction];
}