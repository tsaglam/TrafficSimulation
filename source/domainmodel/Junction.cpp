#include "Junction.h"

Junction::Signal::Signal(CardinalDirection _direction, unsigned int _time)
  : direction(_direction), time(_time) {}

Junction::ConnectedStreet::ConnectedStreet(bool _isConnected, Street* _street,
                                           CardinalDirection _direction)
  : isConnected(_isConnected), street(_street), direction(_direction) {}

Junction::Junction(id_type _id, int _externalId, int _x, int _y,
                   std::vector<Signal>& _signals)
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
