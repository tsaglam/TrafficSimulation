#include "Street.h"

Street::Street(id_type _id, unsigned int _lanes, double _speedLimit,
               double _length, Junction& _from, Junction& _to)
    : id(_id),
      lanes(_lanes),
      speedLimit(_speedLimit),
      length(_length),
      from(_from),
      to(_to) {}

id_type Street::getId() const { return id; }
unsigned int Street::getLanes() const { return lanes; }
double Street::getSpeedLimit() const { return speedLimit; }
double Street::getLength() const { return length; }
Junction& Street::getTargetJunction() const { return to; }
Junction& Street::getSourceJunction() const { return from; }
