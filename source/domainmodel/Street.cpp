#include "Street.h"

Street::Street(id_type _id, unsigned int _lanes, double _speedLimit,
               double _length, Junction& _from, Junction& _to)
    : id(_id),
      lanes(_lanes),
      speedLimit(_speedLimit),
      length(_length),
      from(_from),
      to(_to) {}

id_type Street::getId() { return id; }
unsigned int Street::getLanes() { return lanes; }
double Street::getSpeedLimit() { return speedLimit; }
double Street::getLength() { return length; }
const Junction& Street::getTargetJunction() { return to; }
const Junction& Street::getSourceJunction() { return from; }
