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
  std::vector<std::unique_ptr<Vehicle>> vehicles;
  std::vector<std::unique_ptr<Street>> streets;
  std::vector<std::unique_ptr<Junction>> junctions;

public:
  DomainModel() = default;

  void resetVehiclePositions();

  // methods for building the domain model:
  Vehicle &addVehicle(const Vehicle &vehicle);
  Street &addStreet(const Street &street);
  Junction &addJunction(const Junction &junction);
  Vehicle &addVehicle(Vehicle &&vehicle);
  Street &addStreet(Street &&street);
  Junction &addJunction(Junction &&junction);

  // access methods for the unique elements:
  Vehicle &getVehicle(id_type id);
  Street &getStreet(id_type id);
  Junction &getJunction(id_type id);
  const Vehicle &getVehicle(id_type id) const;
  const Street &getStreet(id_type id) const;
  const Junction &getJunction(id_type id) const;

  // access methods for the full sets:
  const std::vector<std::unique_ptr<Vehicle>> &getVehicles() const;
  const std::vector<std::unique_ptr<Street>> &getStreets() const;
  const std::vector<std::unique_ptr<Junction>> &getJunctions() const;
};

#endif
