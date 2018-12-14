#ifndef DOMAINMODEL_H
#define DOMAINMODEL_H

#include <vector>
#include "Junction.h"
#include "Street.h"
#include "Vehicle.h"

class DomainModel {
  std::vector<Vehicle> vehicles;
  std::vector<Street> streets;
  std::vector<Junction> junctions;

 public:
  DomainModel() = default;
  void addVehicle(Vehicle vehicle);
  void addStreet(Street street);
  void addJunction(Junction junction);
};

#endif