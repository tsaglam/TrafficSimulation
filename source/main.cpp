#include <exception>
#include <iostream>

#include "DomainModel.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "Simulator.h"
#include "TrafficLightRoutine.h"
#include "ConsistencyRoutine.h"

int main() {
  DomainModel domainModel;

  JSONReader jsonReader(std::cin);

  jsonReader.readInto(domainModel);

  Simulator<NaiveStreetDataStructure, TrafficLightRoutine, NullRoutine, ConsistencyRoutine> simulator(domainModel);
  simulator.performSteps(jsonReader.getTimeSteps());

  JSONWriter jsonWriter(std::cout);

  jsonWriter.writeVehicles(domainModel);

  return 0;
}
