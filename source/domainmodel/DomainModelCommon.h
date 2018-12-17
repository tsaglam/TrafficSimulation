#ifndef DOMAINMODELCOMMON_H
#define DOMAINMODELCOMMON_H

#include <cstddef>

using id_type = std::size_t;

enum TurnDirection { UTURN = 0, LEFT = 1, STRAIGHT = 2, RIGHT = 3 };

enum CardinalDirection { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

#endif
