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
template <typename RfbStructure>
class SimulationData {
public:
  typedef LowLevelCar Car;
  typedef typename LowLevelStreet<RfbStructure> Street;
  typedef typename RfbStructure<Car> ConcreteRfbStructure;

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
