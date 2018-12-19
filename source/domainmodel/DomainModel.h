#ifndef DOMAINMODEL_H
#define DOMAINMODEL_H

#include <vector>
#include "DomainModelCommon.h"
#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

class DomainModel {
 private:
 public:
  std::vector<Vehicle> vehicles;    // TODO make private and use getter
  std::vector<Street> streets;      // TODO make private and use getter
  std::vector<Junction> junctions;  // TODO make private and use getter

  DomainModel() = default;
  Vehicle& addVehicle(Vehicle& vehicle);
  Street& addStreet(Street& street);
  Junction& addJunction(Junction& junction);

  Vehicle& getVehicle(id_type id);
  Street& getStreet(id_type id);
  Junction& getJunction(id_type id);
};

/**
 * @TODO Naming, Iterator is what is returned by the method defined on this
 *       type. Hence, this type should not be named Iterator.
 *       https://en.cppreference.com/w/cpp/iterator
 */
class VehicleIterator {
 private:
  DomainModel& domainModel;

 public:
  VehicleIterator(DomainModel& domainModel);
  std::vector<Vehicle>::const_iterator cbegin();
  std::vector<Vehicle>::const_iterator cend();
  std::vector<Vehicle>::iterator begin();
  std::vector<Vehicle>::iterator end();
};

class StreetIterator {
 private:
  DomainModel& domainModel;

 public:
  StreetIterator(DomainModel& domainModel);
  std::vector<Street>::const_iterator cbegin();
  std::vector<Street>::const_iterator cend();
  std::vector<Street>::iterator begin();
  std::vector<Street>::iterator end();
};

class JunctionIterator {
 private:
  DomainModel& domainModel;

 public:
  JunctionIterator(DomainModel& domainModel);
  std::vector<Junction>::const_iterator cbegin();
  std::vector<Junction>::const_iterator cend();
  std::vector<Junction>::iterator begin();
  std::vector<Junction>::iterator end();
};

#endif
