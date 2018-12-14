#ifndef STREET_H
#define STREET_H

#include "Junction.h"

class Junction;

class Street {
  Junction& from;
  Junction& to;
  unsigned int lanes;
  double speedLimit;

 public:
  Street(unsigned int lanes, double speedLimit, Junction& from, Junction& to);
};

#endif