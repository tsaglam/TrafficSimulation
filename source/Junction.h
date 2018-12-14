#ifndef JUNCTION_H
#define JUNCTION_H

#include "Street.h"

class Street;

class Junction {
  unsigned int id;
  int x;
  int y;
  unsigned int timer;

 public:
  Junction(unsigned int id, int x, int y, unsigned int initalTimer);
};

#endif