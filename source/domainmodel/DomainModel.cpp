#include "DomainModel.h"

Vehicle& DomainModel::addVehicle(Vehicle& vehicle) {
  vehicles.push_back(vehicle);
  vehicles.back().id = vehicles.size() - 1;
  return vehicles.back();
}

Street& DomainModel::addStreet(Street& street) {
  streets.push_back(street);
  streets.back().id = streets.size() - 1;
  return streets.back();
}

Junction& DomainModel::addJunction(Junction& junction) {
  junctions.push_back(junction);
  junctions.back().id = junctions.size() - 1;
  return junctions.back();
}
