#include "DomainModel.h"

void DomainModel::addVehicle(Vehicle vehicle) {
  vehicles.push_back(vehicle);
}
void DomainModel::addStreet(Street street) {
  streets.push_back(street);
}
void DomainModel::addJunction(Junction junction) {
  junctions.push_back(junction);
}
