#include "DomainModel.h"

#include <memory>

Vehicle& DomainModel::addVehicle(const Vehicle& vehicle) {
  vehicles.emplace_back(new Vehicle(vehicle));
  vehicles.back()->id = vehicles.size() - 1;
  return *vehicles.back();
}

Junction& DomainModel::addJunction(const Junction& junction) {
  junctions.emplace_back(new Junction(junction));
  junctions.back()->id = junctions.size() - 1;
  return *junctions.back();
}

Street& DomainModel::addStreet(const Street& street) {
  streets.emplace_back(new Street(street));
  streets.back()->id = streets.size() - 1;
  return *streets.back();
}

Vehicle& DomainModel::addVehicle(Vehicle&& vehicle) {
  vehicles.emplace_back(new Vehicle(vehicle));
  vehicles.back()->id = vehicles.size() - 1;
  return *vehicles.back();
}

Street& DomainModel::addStreet(Street&& street) {
  streets.emplace_back(new Street(street));
  streets.back()->id = streets.size() - 1;
  return *streets.back();
}

Junction& DomainModel::addJunction(Junction&& junction) {
  junctions.emplace_back(new Junction(junction));
  junctions.back()->id = junctions.size() - 1;
  return *junctions.back();
}

Vehicle& DomainModel::getVehicle(id_type id) { return *vehicles.at(id); }

Street& DomainModel::getStreet(id_type id) { return *streets.at(id); }

Junction& DomainModel::getJunction(id_type id) { return *junctions.at(id); }

/*
 * class VehicleIterator:
 */
VehicleIterator::VehicleIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

const std::vector<std::unique_ptr<Vehicle>>::const_iterator
VehicleIterator::cbegin() {
  return domainModel.vehicles.cbegin();
}

const std::vector<std::unique_ptr<Vehicle>>::const_iterator
VehicleIterator::cend() {
  return domainModel.vehicles.cend();
}

const std::vector<std::unique_ptr<Vehicle>>::iterator VehicleIterator::begin() {
  return domainModel.vehicles.begin();
}

const std::vector<std::unique_ptr<Vehicle>>::iterator VehicleIterator::end() {
  return domainModel.vehicles.end();
}

/*
 * class StreetIterator:
 */
StreetIterator::StreetIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

const std::vector<std::unique_ptr<Street>>::const_iterator
StreetIterator::cbegin() {
  return domainModel.streets.cbegin();
}
const std::vector<std::unique_ptr<Street>>::const_iterator
StreetIterator::cend() {
  return domainModel.streets.cend();
}

const std::vector<std::unique_ptr<Street>>::iterator StreetIterator::begin() {
  return domainModel.streets.begin();
}

const std::vector<std::unique_ptr<Street>>::iterator StreetIterator::end() {
  return domainModel.streets.end();
}

/*
 * class JunctionIterator:
 */
JunctionIterator::JunctionIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

const std::vector<std::unique_ptr<Junction>>::const_iterator
JunctionIterator::cbegin() {
  return domainModel.junctions.cbegin();
}

const std::vector<std::unique_ptr<Junction>>::const_iterator
JunctionIterator::cend() {
  return domainModel.junctions.cend();
}

const std::vector<std::unique_ptr<Junction>>::iterator
JunctionIterator::begin() {
  return domainModel.junctions.begin();
}

const std::vector<std::unique_ptr<Junction>>::iterator JunctionIterator::end() {
  return domainModel.junctions.end();
}
