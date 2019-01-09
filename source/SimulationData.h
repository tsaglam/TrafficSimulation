#ifndef SIMULATION_DATA_H
#define SIMULATION_DATA_H

#include <vector>

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"

/**
 * The SimulationData class holds all persistent data required during simulation.
 * Computation routines should exclusively use SimulationData to operate on domain model as well as low level
 * representation.
 */
template <template <typename Vehicle> RfbStructure>
class SimulationData {
public:
  using Car                  = LowLevelCar;
  using Street               = typename LowLevelStreet<RfbStructure>;
  using ConcreteRfbStructure = typename RfbStructure<Car>;

private:
  DomainModel &domainModel;
  std::vector<Street> streets;

public:
  SimulationData(DomainModel &_domainModel) : domainModel(_domainModel) {}

  Street &getStreet(unsigned int id) const { return streets.at(id); }
  std::vector<Street> &getStreets() const { return streets; }
  DomainModel &getDomainModel() const { return domainModel; }
};

#endif
