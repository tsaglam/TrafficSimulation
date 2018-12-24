#ifndef DOMAINMODEL_H
#define DOMAINMODEL_H

#include <memory>
#include <vector>

#include "DomainModelCommon.h"
#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

class DomainModel {
 private:
 public:
  std::vector<std::unique_ptr<Vehicle>>
      vehicles;  // TODO make private and use getter
  std::vector<std::unique_ptr<Street>>
      streets;  // TODO make private and use getter
  std::vector<std::unique_ptr<Junction>>
      junctions;  // TODO make private and use getter

  DomainModel() = default;
  Vehicle& addVehicle(const Vehicle& vehicle);
  Street& addStreet(const Street& street);
  Junction& addJunction(const Junction& junction);
  Vehicle& addVehicle(Vehicle&& vehicle);
  Street& addStreet(Street&& street);
  Junction& addJunction(Junction&& junction);

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
  const std::vector<std::unique_ptr<Vehicle>>::const_iterator cbegin();
  const std::vector<std::unique_ptr<Vehicle>>::const_iterator cend();
  const std::vector<std::unique_ptr<Vehicle>>::iterator begin();
  const std::vector<std::unique_ptr<Vehicle>>::iterator end();
};

class StreetIterator {
 private:
  DomainModel& domainModel;

 public:
  StreetIterator(DomainModel& domainModel);
  const std::vector<std::unique_ptr<Street>>::const_iterator cbegin();
  const std::vector<std::unique_ptr<Street>>::const_iterator cend();
  const std::vector<std::unique_ptr<Street>>::iterator begin();
  const std::vector<std::unique_ptr<Street>>::iterator end();
};

class JunctionIterator {
 private:
  DomainModel& domainModel;

 public:
  JunctionIterator(DomainModel& domainModel);
  const std::vector<std::unique_ptr<Junction>>::const_iterator cbegin();
  const std::vector<std::unique_ptr<Junction>>::const_iterator cend();
  const std::vector<std::unique_ptr<Junction>>::iterator begin();
  const std::vector<std::unique_ptr<Junction>>::iterator end();
};

#endif
