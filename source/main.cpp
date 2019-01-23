#include <exception>
#include <iostream>

#include "ConsistencyRoutine.h"
#include "DomainModel.h"
#include "IDMRoutine.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "OptimizationRoutine.h"
#include "Optimizer.h"
#include "Simulator.h"
#include "TrafficLightRoutine.h"

int main() {
  DomainModel domainModel;

  JSONReader jsonReader(std::cin);

  jsonReader.readInto(domainModel);

  bool optimizeTrafficLights = false; // TODO replace with enum and set based on the input
  if (optimizeTrafficLights) {
    const unsigned maximumOptimizationCycles = 10;  // TODO remove when debugging finished
    const double minimumTraveldistance       = 100; // TODO replace when implemented
    Optimizer<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine, false>
        optimizer(domainModel, jsonReader.getTimeSteps(), minimumTraveldistance, maximumOptimizationCycles);
    optimizer.optimizeTrafficLights();

    JSONWriter jsonWriter(std::cout);

    // jsonWriter.writeSignals(domainModel); TODO uncomment when implemented
  } else { // run a standard simulation
    Simulator<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, NullRoutine, ConsistencyRoutine> simulator(
        domainModel);
    simulator.performSteps(jsonReader.getTimeSteps());

    JSONWriter jsonWriter(std::cout);

    jsonWriter.writeVehicles(domainModel);
  }

  return 0;
}
