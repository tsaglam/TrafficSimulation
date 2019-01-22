#include "BucketList.h"
#include "NaiveStreetDataStructure.h"
#include "domainmodel/DomainModelTest.h"
#include "domainmodel/JunctionTest.h"
#include "domainmodel/VehicleTest.h"
#include "lowlevelmodel/RfbStructureTest.h"
#include "routines/ConsistencyRoutineTest.h"
#include "routines/TrafficLightRoutineTest.h"
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
  RUN(resetAllVehiclesTest);
  // Routines:
  RUN(trafficLightRoutineTest);
  RUN(takeTurnTest);
  RUN(calculateOriginDirectionTest);

  // RfbStructure - BucketList
  std::cout << "\n   BucketList\n";
  RUN(constructorAndConstMembersTest<BucketList>);
  std::cout << "\n";
  RUN(allIterableTest1<BucketList>);
  RUN(allIterableTest2<BucketList>);
  RUN(allIterableTest3<BucketList>);
  RUN(allIterableTest4<BucketList>);
  RUN(allIterableTest5<BucketList>);
  RUN(allIterableTest6<BucketList>);
  std::cout << "\n";
  RUN(getNextCarTest1<BucketList>);
  RUN(getNextCarTest2<BucketList>);
  RUN(getNextCarTest3<BucketList>);
  RUN(getNextCarTest4<BucketList>);
  RUN(getNextCarTest5<BucketList>);
  std::cout << "\n";
  RUN(getNextCarIteratorTest1<BucketList>);
  RUN(insertCarTest<BucketList>);
  RUN(consistencyTest<BucketList>);
  RUN(beyondsIterableTest<BucketList>);
  RUN(removeBeyondsTest<BucketList>);

  // RfbStructure - NaiveStreetDataStructure
  std::cout << "\n   NaiveStreetDataStructure\n";
  RUN(constructorAndConstMembersTest<NaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(allIterableTest1<NaiveStreetDataStructure>);
  RUN(allIterableTest2<NaiveStreetDataStructure>);
  RUN(allIterableTest3<NaiveStreetDataStructure>);
  RUN(allIterableTest4<NaiveStreetDataStructure>);
  RUN(allIterableTest5<NaiveStreetDataStructure>);
  RUN(allIterableTest6<NaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(getNextCarTest1<NaiveStreetDataStructure>);
  RUN(getNextCarTest2<NaiveStreetDataStructure>);
  RUN(getNextCarTest3<NaiveStreetDataStructure>);
  RUN(getNextCarTest4<NaiveStreetDataStructure>);
  RUN(getNextCarTest5<NaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(getNextCarIteratorTest1<NaiveStreetDataStructure>);
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