#include "BucketList.h"
#include "CircularNaiveStreetDataStructure.h"
#include "NaiveStreetDataStructure.h"
#include "domainmodel/DomainModelTest.h"
#include "domainmodel/JunctionTest.h"
#include "domainmodel/VehicleTest.h"
#include "lowlevelmodel/RfbStructureTest.h"
#include "routines/ConsistencyRoutineTest.h"
#include "routines/ParallelTrafficLightRoutineTest.h"
#include <../../snowhouse/snowhouse.h>
#include <iostream>
#include <regex>

using namespace snowhouse;

#define RUN(funk) run(&funk, #funk)

const std::string ANSIred   = "\033[1;31m";
const std::string ANSIgreen = "\033[1;32m";
const std::string ANSIreset = "\033[0m";

int numberOfFailedTests;
int numberOfPassedTests;

// indents and returns every line of a multi-line AssertionException message.
std::string indentMessage(AssertionException exc) {
  std::string result = regex_replace(exc.GetMessage(), std::regex("Expected"), "\tExpected");
  return regex_replace(result, std::regex("Actual"), "\tActual");
}

// Run methods, which is responsible for prining the test results
void run(void (*fun_ptr)(), std::string name) {
  try {
    (*fun_ptr)();
    numberOfPassedTests += 1;
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
  RUN(setSignalTest);
  // DomainModel:
  RUN(modelCreationTest);
  RUN(modelCreationTest2);
  RUN(resetAllVehiclesTest);
  // Routines:
  RUN(trafficLightRoutineTest);
  RUN(parallelTrafficLightRoutineTest);
  RUN(takeTurnTest);
  RUN(calculateOriginDirectionTest);

  // RfbStructure - BucketList
  std::cout << "\n   VectorBucketList\n";
  RUN(constructorAndConstMembersTest<VectorBucketList>);
  RUN(getNextCarIteratorTest1<VectorBucketList>);
  std::cout << "\n";
  RUN(allIterableTest1<VectorBucketList>);
  RUN(allIterableTest2<VectorBucketList>);
  RUN(allIterableTest3<VectorBucketList>);
  RUN(allIterableTest4<VectorBucketList>);
  RUN(allIterableTest5<VectorBucketList>);
  RUN(allIterableTest6<VectorBucketList>);
  std::cout << "\n";
  RUN(getNextCarTest1<VectorBucketList>);
  RUN(getNextCarTest2<VectorBucketList>);
  RUN(getNextCarTest3<VectorBucketList>);
  RUN(getNextCarTest4<VectorBucketList>);
  RUN(getNextCarTest5<VectorBucketList>);
  std::cout << "\n";
  RUN(insertCarTest1<VectorBucketList>);
  RUN(insertCarTest2<VectorBucketList>);
  RUN(insertCarTest3<VectorBucketList>);
  RUN(insertCarTest4<VectorBucketList>);
  RUN(insertCarTest5<VectorBucketList>);
  RUN(insertCarTest6<VectorBucketList>);
  RUN(insertCarTest7<VectorBucketList>);
  RUN(insertCarTest8<VectorBucketList>);
  std::cout << "\n";
  RUN(consistencyTest1<VectorBucketList>);
  RUN(consistencyTest2<VectorBucketList>);
  RUN(consistencyTest3<VectorBucketList>);
  RUN(consistencyTest4<VectorBucketList>);
  RUN(consistencyTest5<VectorBucketList>);
  RUN(consistencyTest6<VectorBucketList>);
  RUN(consistencyTest7<VectorBucketList>);
  RUN(consistencyTest8<VectorBucketList>);
  RUN(consistencyTest9<VectorBucketList>);

  std::cout << "\n   FreeListBucketList\n";
  RUN(constructorAndConstMembersTest<FreeListBucketList>);
  RUN(getNextCarIteratorTest1<FreeListBucketList>);
  std::cout << "\n";
  RUN(allIterableTest1<FreeListBucketList>);
  RUN(allIterableTest2<FreeListBucketList>);
  RUN(allIterableTest3<FreeListBucketList>);
  RUN(allIterableTest4<FreeListBucketList>);
  RUN(allIterableTest5<FreeListBucketList>);
  RUN(allIterableTest6<FreeListBucketList>);
  std::cout << "\n";
  RUN(getNextCarTest1<FreeListBucketList>);
  RUN(getNextCarTest2<FreeListBucketList>);
  RUN(getNextCarTest3<FreeListBucketList>);
  RUN(getNextCarTest4<FreeListBucketList>);
  RUN(getNextCarTest5<FreeListBucketList>);
  std::cout << "\n";
  RUN(insertCarTest1<FreeListBucketList>);
  RUN(insertCarTest2<FreeListBucketList>);
  RUN(insertCarTest3<FreeListBucketList>);
  RUN(insertCarTest4<FreeListBucketList>);
  RUN(insertCarTest5<FreeListBucketList>);
  RUN(insertCarTest6<FreeListBucketList>);
  RUN(insertCarTest7<FreeListBucketList>);
  RUN(insertCarTest8<FreeListBucketList>);
  std::cout << "\n";
  RUN(consistencyTest1<FreeListBucketList>);
  RUN(consistencyTest2<FreeListBucketList>);
  RUN(consistencyTest3<FreeListBucketList>);
  RUN(consistencyTest4<FreeListBucketList>);
  RUN(consistencyTest5<FreeListBucketList>);
  RUN(consistencyTest6<FreeListBucketList>);
  RUN(consistencyTest7<FreeListBucketList>);
  RUN(consistencyTest8<FreeListBucketList>);
  RUN(consistencyTest9<FreeListBucketList>);

  // RfbStructure - NaiveStreetDataStructure
  std::cout << "\n   NaiveStreetDataStructure\n";
  RUN(constructorAndConstMembersTest<NaiveStreetDataStructure>);
  RUN(getNextCarIteratorTest1<NaiveStreetDataStructure>);
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
  RUN(insertCarTest1<NaiveStreetDataStructure>);
  RUN(insertCarTest2<NaiveStreetDataStructure>);
  RUN(insertCarTest3<NaiveStreetDataStructure>);
  RUN(insertCarTest4<NaiveStreetDataStructure>);
  RUN(insertCarTest5<NaiveStreetDataStructure>);
  RUN(insertCarTest6<NaiveStreetDataStructure>);
  RUN(insertCarTest7<NaiveStreetDataStructure>);
  RUN(insertCarTest8<NaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(consistencyTest1<NaiveStreetDataStructure>);
  RUN(consistencyTest2<NaiveStreetDataStructure>);
  RUN(consistencyTest3<NaiveStreetDataStructure>);
  RUN(consistencyTest4<NaiveStreetDataStructure>);
  RUN(consistencyTest5<NaiveStreetDataStructure>);
  RUN(consistencyTest6<NaiveStreetDataStructure>);
  RUN(consistencyTest7<NaiveStreetDataStructure>);
  RUN(consistencyTest8<NaiveStreetDataStructure>);
  RUN(consistencyTest9<NaiveStreetDataStructure>);

  // RfbStructure - CircularNaiveStreetDataStructure
  std::cout << "\n   CircularNaiveStreetDataStructure\n";
  RUN(constructorAndConstMembersTest<CircularNaiveStreetDataStructure>);
  RUN(getNextCarIteratorTest1<CircularNaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(allIterableTest1<CircularNaiveStreetDataStructure>);
  RUN(allIterableTest2<CircularNaiveStreetDataStructure>);
  RUN(allIterableTest3<CircularNaiveStreetDataStructure>);
  RUN(allIterableTest4<CircularNaiveStreetDataStructure>);
  RUN(allIterableTest5<CircularNaiveStreetDataStructure>);
  RUN(allIterableTest6<CircularNaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(getNextCarTest1<CircularNaiveStreetDataStructure>);
  RUN(getNextCarTest2<CircularNaiveStreetDataStructure>);
  RUN(getNextCarTest3<CircularNaiveStreetDataStructure>);
  RUN(getNextCarTest4<CircularNaiveStreetDataStructure>);
  RUN(getNextCarTest5<CircularNaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(insertCarTest1<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest2<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest3<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest4<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest5<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest6<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest7<CircularNaiveStreetDataStructure>);
  RUN(insertCarTest8<CircularNaiveStreetDataStructure>);
  std::cout << "\n";
  RUN(consistencyTest1<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest2<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest3<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest4<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest5<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest6<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest7<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest8<CircularNaiveStreetDataStructure>);
  RUN(consistencyTest9<CircularNaiveStreetDataStructure>);

  // Prints the test results and the number of failed tests:
  if (numberOfFailedTests == 0) {
    std::cout << ANSIgreen << "ALL " << numberOfPassedTests << " TESTS PASSED!" << ANSIreset << std::endl;
    return 0;
  } else {
    std::cout << ANSIred << "ERROR: " << numberOfFailedTests << " tests failed!" << ANSIreset << std::endl;
    return 1;
  }
}
