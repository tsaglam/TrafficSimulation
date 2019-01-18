#include "BucketList.h"
#include "NaiveStreetDataStructure.h"
#include "domainmodel/DomainModelTest.h"
#include "domainmodel/JunctionTest.h"
#include "domainmodel/VehicleTest.h"
#include "lowlevelmodel/RfbStructureTest.h"
#include "routines/TrafficLightRoutineTest.h"
#include "routines/ConsistencyRoutineTest.h"
#include <../../snowhouse/snowhouse.h>
#include <iostream>
#include <regex>

using namespace snowhouse;

#define RUN(funk) run(&funk, #funk)

const std::string ANSIred   = "\033[1;31m";
const std::string ANSIgreen = "\033[1;32m";
const std::string ANSIreset = "\033[0m";

int numberOfFailedTests;

// indents and returns every line of a multi-line AssertionException message.
std::string indentMessage(AssertionException exc) {
  std::string result = regex_replace(exc.GetMessage(), std::regex("Expected"), "\tExpected");
  return regex_replace(result, std::regex("Actual"), "\tActual");
}

// Run methods, which is responsible for prining the test results
void run(void (*fun_ptr)(), std::string name) {
  try {
    (*fun_ptr)();
    std::cout << ANSIgreen << " + Test " << name << " passed" << ANSIreset << std::endl;
  } catch (const AssertionException &exception) {
    std::cout << ANSIred << " - Test " << name << " failed: " << ANSIreset << std::endl;
    std::cout << ANSIred << indentMessage(exception) << ANSIreset;
    numberOfFailedTests += 1; // and increase fail count
  }
}

// RUN TEST CASES HERE, USE ONLY THE METHOD NAME AS PARAMETER:
int main() {
  // Vehicle:
  RUN(nextDirectionTest);
  RUN(setPositionTest);
  // Junction:
  RUN(junctionCreationTest);
  RUN(trafficLightTest);
  RUN(junctionWithoutTrafficLightsTest);
  RUN(previousSignalTest);
  // DomainModel:
  RUN(modelCreationTest);
  RUN(modelCreationTest2);
  // Routines:
  RUN(trafficLightRoutineTest);
  RUN(takeTurnTest);
  RUN(calculateOriginDirectionTest);

  // RfbStructure - BucketList
  std::cout << "\n";
  RUN(constructorAndConstMembersTest<BucketList>);
  RUN(allIterableTest<BucketList>);
  RUN(getNextCarTest1<BucketList>);
  RUN(getNextCarIteratorTest<BucketList>);
  RUN(insertCarTest<BucketList>);
  RUN(consistencyTest<BucketList>);
  RUN(beyondsIterableTest<BucketList>);
  RUN(removeBeyondsTest<BucketList>);
  // RfbStructure - NaiveStreetDataStructure
  std::cout << "\n";
  RUN(constructorAndConstMembersTest<NaiveStreetDataStructure>);
  RUN(allIterableTest<NaiveStreetDataStructure>);
  RUN(getNextCarTest1<NaiveStreetDataStructure>);
  RUN(getNextCarIteratorTest<NaiveStreetDataStructure>);
  RUN(insertCarTest<NaiveStreetDataStructure>);
  RUN(consistencyTest<NaiveStreetDataStructure>);
  RUN(beyondsIterableTest<NaiveStreetDataStructure>);
  RUN(removeBeyondsTest<NaiveStreetDataStructure>);

  // Prints the test results and the number of failed tests:
  if (numberOfFailedTests == 0) {
    std::cout << ANSIgreen << "ALL TESTS PASSED!" << ANSIreset << std::endl;
    return 0;
  } else {
    std::cout << ANSIred << "ERROR: " << numberOfFailedTests << " tests failed!" << ANSIreset << std::endl;
    return 1;
  }
}