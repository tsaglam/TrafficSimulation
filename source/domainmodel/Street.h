#ifndef STREET_H
#define STREET_H

#include "DomainModelCommon.h"
#include "Junction.h"

class Junction;

class Street {
 private:
  friend class DomainModel;

 private:
  id_type id;
  unsigned int lanes;
  double speedLimit;
  double length;
  Junction& from;
  Junction& to;

 public:
  Street(id_type id, unsigned int lanes, double speedLimit, double length,
         Junction& from, Junction& to);

  id_type getId();
  unsigned int getLanes();
  double getSpeedLimit();
  double getLength();
  const Junction& getTargetJunction();
  const Junction& getSourceJunction();
};

#endif
