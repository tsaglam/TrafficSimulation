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
#include "SIMD_IDMRoutine.h"
#include "Simulator.h"
#include "Timer.h"
#include "TrafficLightRoutine.h"

void printTimes() {
  printTimerHeader();
  printTimer(signalingRoutineTimer, "signalingRoutine");

  printTimer(idmRoutineTimer, "idmRoutine");
  printTimer(IDMRoutine_thresholdSorting_timer, "IDMRoutine_thresholdSorting");
  printTimer(IDMRoutine_performStreetWise_timer, "IDMRoutine_performStreetWise");
  printTimer(IDMRoutine_performCarWise_timer, "IDMRoutine_performCarWise");

  printTimer(optimizationRoutineTimer, "optimizationRoutine");

  printTimer(consistencyRoutineTimer, "consistencyRoutine");
  printTimer(consistencyRoutine_restoreConsistency_timer, "consistencyRoutine_restoreConsistency");
  printTimer(consistencyRoutine_relocateCars_timer, "consistencyRoutine_relocateCars");
  printTimer(consistencyRoutine_incorporateCars_timer, "consistencyRoutine_incorporateCars");
}

/**
 * Definitions of template parameters, dependent on compile flags.
 *
 * The definitions are used by both Simulator and Optimizer.
 */

#define RfbStructure NaiveStreetDataStructure
using InitialTrafficLights = InitialTrafficLightsWithHeuristicSimulatorAndIteration<false>;

int main_simulate(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  Simulator<RfbStructure, ParallelTrafficLightRoutine, ParallelIDMRoutine, NullRoutine, ParallelConsistencyRoutine>
      simulator(domainModel);
  simulator.performSteps(jsonReader.getTimeSteps());

  jsonWriter.writeVehicles(domainModel);

#ifdef TIMER
  printTimes();
#endif
  return 0;
}

int main_optimize(JSONReader &jsonReader, DomainModel &domainModel, JSONWriter &jsonWriter) {
  InitialTrafficLights()(domainModel, jsonReader.getTimeSteps());
  jsonWriter.writeSignals(domainModel);
#ifdef TIMER
  printTimes();
#endif
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
