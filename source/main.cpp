#include <exception>
#include <iostream>

#include "ConsistencyRoutine.h"
#include "DomainModel.h"
#include "IDMRoutine.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "NaiveStreetDataStructure.h"
#include "NullRoutine.h"
#include "Simulator.h"
#include "TrafficLightRoutine.h"

int main() {
  DomainModel domainModel;

  JSONReader jsonReader(std::cin);

  jsonReader.readInto(domainModel);

  Simulator<NaiveStreetDataStructure, TrafficLightRoutine, IDMRoutine, ConsistencyRoutine> simulator(domainModel);
  simulator.performSteps(jsonReader.getTimeSteps());

  JSONWriter jsonWriter(std::cout);

  jsonWriter.writeVehicles(domainModel);

  return 0;
}
