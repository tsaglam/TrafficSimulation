#include "Junction.h"

Junction::Signal::Signal(CardinalDirection _direction, unsigned int _time)
    : direction(_direction), time(_time) {}

Junction::ConnectedStreet::ConnectedStreet(bool _isConnected, Street* _street,
                                           CardinalDirection _direction)
    : isConnected(_isConnected), street(_street), direction(_direction) {}

Junction::Junction(id_type _id, int _externalId, int _x, int _y,
                   const std::vector<Signal>& _signals)
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {}

Junction::Junction(id_type _id, int _externalId, int _x, int _y,
                   std::vector<Signal>&& _signals)
    : id(_id), externalId(_externalId), x(_x), y(_y), signals(_signals) {}

void Junction::addIncomingStreet(Street& _street,
                                 CardinalDirection _direction) {
  incomingStreets[_direction].isConnected = true;
  incomingStreets[_direction].street = &_street;
}

void Junction::addOutgoingStreet(Street& _street,
                                 CardinalDirection _direction) {
  outgoingStreets[_direction].isConnected = true;
  outgoingStreets[_direction].street = &_street;
}

id_type Junction::getId() { return id; }
int Junction::getExternalId() { return externalId; }
int Junction::getX() { return x; }
int Junction::getY() { return y; }
std::vector<Junction::Signal> Junction::getSignals() { return signals; }
const Junction::ConnectedStreet& Junction::getIncomingStreet(
    CardinalDirection direction) {
  return incomingStreets[direction];
}
const Junction::ConnectedStreet& Junction::getOutgoingStreet(
    CardinalDirection direction) {
  return outgoingStreets[direction];
}