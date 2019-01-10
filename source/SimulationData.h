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
template <template <typename Vehicle> typename RfbStructure>
class SimulationData {
public:
  using Vehicle              = LowLevelCar;
  using Street               = LowLevelStreet<RfbStructure>;
  using ConcreteRfbStructure = RfbStructure<Vehicle>;

private:
  DomainModel &domainModel;
  std::vector<Street> streets;

public:
  SimulationData(DomainModel &_domainModel) : domainModel(_domainModel) {}

  Street &getStreet(unsigned int id) { return streets.at(id); }
  const Street &getStreet(unsigned int id) const { return streets.at(id); }
  std::vector<Street> &getStreets() { return streets; }
  const std::vector<Street> &getStreets() const { return streets; }
  DomainModel &getDomainModel() { return domainModel; }
  const DomainModel &getDomainModel() const { return domainModel; }
};

#endif
