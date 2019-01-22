#include <exception>
#include <iostream>

#include "ConsistencyRoutine.h"
#include "DomainModel.h"
#include "IDMRoutine.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "Optimizer.h"
#include "Simulator.h"
#include "TrafficLightRoutine.h"

int main() {
  DomainModel domainModel;

  JSONReader jsonReader(std::cin);

  jsonReader.readInto(domainModel);

  bool optimizeTrafficLights = false; // TODO replace with enum and set based on the input
  if (optimizeTrafficLights) {
    // Optimizer<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, ConsistencyRoutine> optimizer(
    //     domainModel, jsonReader.getTimeSteps(), jsonReader.getMinTravelDistance());
    Optimizer<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, NullRoutine, ConsistencyRoutine> optimizer(
        domainModel, jsonReader.getTimeSteps(), 0); // TODO replace with above when implemented
    optimizer.optimizeTrafficLights();

    JSONWriter jsonWriter(std::cout);

    // jsonWriter.writeSignals(domainModel); TODO uncomment when implemented
  } else { // run a standard simulation
    Simulator<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, NullRoutine, ConsistencyRoutine> simulator(domainModel);
    simulator.performSteps(jsonReader.getTimeSteps());

    JSONWriter jsonWriter(std::cout);

    jsonWriter.writeVehicles(domainModel);
  }

  return 0;
}
