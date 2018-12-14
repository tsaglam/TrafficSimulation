#include "Street.h"

Street::Street(unsigned int lanes, double speedLimit, Junction& from,
               Junction& to)
    : lanes(lanes), speedLimit(speedLimit), from(from), to(to) {}