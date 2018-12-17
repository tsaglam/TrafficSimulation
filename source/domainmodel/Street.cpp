#include "Street.h"

Street::Street(id_type _id, unsigned int _lanes, double _speedLimit,
	           double _length, Junction& _from, Junction& _to)
    : id(_id), lanes(_lanes), speedLimit(_speedLimit), length(_length),
      from(_from), to(_to) {}
