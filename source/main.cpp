#include <exception>
#include <iostream>

#include "BucketList.h"
#include "ConsistencyRoutine.h"
#include "ParallelConsistencyRoutine.h"
#include "DomainModel.h"
#include "IDMRoutine.h"
#include "InitialTrafficLightStrategies.h"
#include "ParallelIDMRoutine.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "OptimizationRoutine.h"
#include "Optimizer.h"
#include "Simulator.h"
#include "ParallelTrafficLightRoutine.h"
#include "TrafficLightRoutine.h"

int main_simulate(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  Simulator<VectorBucketList, TrafficLightRoutine, IDMRoutine, NullRoutine, ParallelConsistencyRoutine> simulator(domainModel);
  simulator.performSteps(jsonReader.getTimeSteps());

  jsonWriter.writeVehicles(domainModel);

  return 0;
}

int main_optimize(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  const unsigned maximumOptimizationCycles = 100; // TODO remove when debugging finished
  const double minimumTraveldistance       = jsonReader.getMinTravelDistance();

  Optimizer<VectorBucketList, TrafficLightRoutine, IDMRoutine, OptimizationRoutine, ConsistencyRoutine,
      InitialTrafficLightsAllFive, false>
      optimizer(domainModel, jsonReader.getTimeSteps(), minimumTraveldistance, maximumOptimizationCycles);
  optimizer.optimizeTrafficLights();

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
