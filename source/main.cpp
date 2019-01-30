#include <exception>
#include <iostream>

#include "BucketList.h"
#include "ConsistencyRoutine.h"
#include "DomainModel.h"
#include "IDMRoutine.h"
#include "InitialTrafficLightStrategies.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "OptimizationRoutine.h"
#include "Optimizer.h"
#include "ParallelConsistencyRoutine.h"
#include "ParallelIDMRoutine.h"
#include "ParallelTrafficLightRoutine.h"
#include "Simulator.h"
#include "TrafficLightRoutine.h"

/**
 * Definitions of template parameters, dependent on compile flags.
 *
 * The definitions are used by both Simulator and Optimizer.
 */

#define RfbStructure NaiveStreetDataStructure

int main_simulate(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  Simulator<RfbStructure, ParallelTrafficLightRoutine, ParallelIDMRoutine, NullRoutine, ParallelConsistencyRoutine>
      simulator(domainModel);
  simulator.performSteps(jsonReader.getTimeSteps());

  jsonWriter.writeVehicles(domainModel);

  return 0;
}

int main_optimize(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  // using InitialTrafficLights = InitialTrafficLightsWithHeuristicSimulator<true>;
  using InitialTrafficLights = InitialTrafficLightsWithHeuristicSimulatorAndIteration<true>;

  InitialTrafficLights()(domainModel, jsonReader.getTimeSteps());

  // Optimizer<NaiveStreetDataStructure, TrafficLightRoutine, ParallelIDMRoutine, OptimizationRoutine,
  //     ParallelConsistencyRoutine, InitialTrafficLights, true>
  //     optimizer(domainModel, jsonReader.getTimeSteps(), jsonReader.getMinTravelDistance(), 1);
  // optimizer.optimizeTrafficLights();
  jsonWriter.writeSignals(domainModel);

  return 0;
}

int main() {
  DomainModel domainModel;

  JSONReader jsonReader(std::cin);
  JSONWriter jsonWriter(std::cout);

  jsonReader.readInto(domainModel);

  switch (jsonReader.getMode()) {
  case JSONReader::SIMULATE: return main_simulate(jsonReader, domainModel, jsonWriter);
  case JSONReader::OPTIMIZE: return main_optimize(jsonReader, domainModel, jsonWriter);
  default: {
    std::cerr << "Unknown execution mode." << std::endl;
    return 1;
  }
  }
}
