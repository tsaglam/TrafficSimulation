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
  const unsigned int lanes;
  const double speedLimit;
  const double length;
  Junction &from;
  Junction &to;

public:
  Street(id_type id, unsigned int lanes, double speedLimit, double length, Junction &from, Junction &to);

  // access methods:
  id_type getId() const;
  unsigned int getLanes() const;
  double getSpeedLimit() const;
  double getLength() const;
  Junction &getTargetJunction() const;
  Junction &getSourceJunction() const;
};

#endif
