#include "DomainModel.h"

Vehicle& DomainModel::addVehicle(const Vehicle& vehicle) {
  vehicles.push_back(vehicle);
  vehicles.back().id = vehicles.size() - 1;
  return vehicles.back();
}

Street& DomainModel::addStreet(const Street& street) {
  streets.push_back(street);
  streets.back().id = streets.size() - 1;
  return streets.back();
}

Junction& DomainModel::addJunction(const Junction& junction) {
  junctions.push_back(junction);
  junctions.back().id = junctions.size() - 1;
  return junctions.back();
}

Vehicle& DomainModel::getVehicle(id_type id) { return vehicles.at(id); }

Street& DomainModel::getStreet(id_type id) { return streets.at(id); }

Junction& DomainModel::getJunction(id_type id) { return junctions.at(id); }

/*
 * class VehicleIterator:
 */
VehicleIterator::VehicleIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

std::vector<Vehicle>::const_iterator VehicleIterator::cbegin() {
  return domainModel.vehicles.cbegin();
}

std::vector<Vehicle>::const_iterator VehicleIterator::cend() {
  return domainModel.vehicles.cend();
}

std::vector<Vehicle>::iterator VehicleIterator::begin() {
  return domainModel.vehicles.begin();
}

std::vector<Vehicle>::iterator VehicleIterator::end() {
  return domainModel.vehicles.end();
}

/*
 * class StreetIterator:
 */
StreetIterator::StreetIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

std::vector<Street>::const_iterator StreetIterator::cbegin() {
  return domainModel.streets.cbegin();
}

std::vector<Street>::const_iterator StreetIterator::cend() {
  return domainModel.streets.cend();
}

std::vector<Street>::iterator StreetIterator::begin() {
  return domainModel.streets.begin();
}

std::vector<Street>::iterator StreetIterator::end() {
  return domainModel.streets.end();
}

/*
 * class JunctionIterator:
 */
JunctionIterator::JunctionIterator(DomainModel& domainModel)
    : domainModel(domainModel) {}

std::vector<Junction>::const_iterator JunctionIterator::cbegin() {
  return domainModel.junctions.cbegin();
}

std::vector<Junction>::const_iterator JunctionIterator::cend() {
  return domainModel.junctions.cend();
}

std::vector<Junction>::iterator JunctionIterator::begin() {
  return domainModel.junctions.begin();
}

std::vector<Junction>::iterator JunctionIterator::end() {
  return domainModel.junctions.end();
}
