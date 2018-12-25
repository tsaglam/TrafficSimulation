#include "DomainModel.h"

#include <memory>

Vehicle &DomainModel::addVehicle(const Vehicle &vehicle) {
  vehicles.emplace_back(new Vehicle(vehicle));
  vehicles.back()->id = vehicles.size() - 1;
  return *vehicles.back();
}

Junction &DomainModel::addJunction(const Junction &junction) {
  junctions.emplace_back(new Junction(junction));
  junctions.back()->id = junctions.size() - 1;
  return *junctions.back();
}

Street &DomainModel::addStreet(const Street &street) {
  streets.emplace_back(new Street(street));
  streets.back()->id = streets.size() - 1;
  return *streets.back();
}

Vehicle &DomainModel::addVehicle(Vehicle &&vehicle) {
  vehicles.emplace_back(new Vehicle(vehicle));
  vehicles.back()->id = vehicles.size() - 1;
  return *vehicles.back();
}

Street &DomainModel::addStreet(Street &&street) {
  streets.emplace_back(new Street(street));
  streets.back()->id = streets.size() - 1;
  return *streets.back();
}

Junction &DomainModel::addJunction(Junction &&junction) {
  junctions.emplace_back(new Junction(junction));
  junctions.back()->id = junctions.size() - 1;
  return *junctions.back();
}

/*
 * Access methods:
 */
Vehicle &DomainModel::getVehicle(id_type id) const { return *vehicles.at(id); }
Street &DomainModel::getStreet(id_type id) const { return *streets.at(id); }
Junction &DomainModel::getJunction(id_type id) const { return *junctions.at(id); }
const std::vector<std::unique_ptr<Vehicle>> &DomainModel::getVehicles() const { return vehicles; }
const std::vector<std::unique_ptr<Street>> &DomainModel::getStreets() const { return streets; }
const std::vector<std::unique_ptr<Junction>> &DomainModel::getJunctions() const { return junctions; }
