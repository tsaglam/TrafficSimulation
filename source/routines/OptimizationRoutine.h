#ifndef OPTIMIZATION_ROUTINE_H
#define OPTIMIZATION_ROUTINE_H

#include <vector>

#include "DomainModel.h"
#include "DomainModelCommon.h"
#include "Junction.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class OptimizationRoutine {

private:
  SimulationData<RfbStructure> &data;
  // contains for each junction a vector of requested green light directions per time step
  std::vector<std::vector<CardinalDirection>> requestedGreenLights;

  CardinalDirection determineOptimalGreenLight(const Junction &junction) const {
    return junction.getSignals().front().getDirection(); // TODO replace with heuristic
  }

public:
  OptimizationRoutine(SimulationData<RfbStructure> &_data)
      : data(_data), requestedGreenLights(data.getDomainModel().getJunctions().size()) {}

  /**
   * Determine the optimal green light direction for each junction in the current step.
   * Insert these directions into the requestedGreenLights vector.
   */
  void perform() {
    for (auto const &junction : data.getDomainModel().getJunctions()) {
      requestedGreenLights[junction->getId()].push_back(determineOptimalGreenLight(*junction));
    }
  }

  const std::vector<std::vector<CardinalDirection>> &getRequestedGreenLights() const { return requestedGreenLights; }
};

#endif
