#include "domainmodel/DomainModelTest.h"
#include "domainmodel/JunctionTest.h"
#include "domainmodel/VehicleTest.h"
#include <../../snowhouse/snowhouse.h>
#include <iostream>
#include <regex>
#include <stdio.h>

using namespace snowhouse;

#define RUN(funk) run(&funk, #funk)

int numberOfFailedTests;

// Example code for one basic test case, import and add other test classes
// instead of adding methods here. Dont forget to include snowhouse and to use the namespace snowhouse.
void someComponentTest() { AssertThat(6 / 2, Equals(3)); }

// indents and returns every line of a multi-line AssertionException message.
std::string indentMessage(AssertionException exc) {
  std::string result = regex_replace(exc.GetMessage(), std::regex("Expected"), "\tExpected");
  return regex_replace(result, std::regex("Actual"), "\tActual");
}

// Run methods, which is responsible for prining the test results
void run(void (*fun_ptr)(), std::string name) {
  try {
    (*fun_ptr)();
    std::cout << " + Test " << name << " passed" << std::endl;
  } catch (const AssertionException &exception) {
    std::cout << " - Test " << name << " failed: " << std::endl;
    std::cout << indentMessage(exception) << std::endl;
    numberOfFailedTests += 1; // and increase fail count
  }
  // if ((*fun_ptr)()) { // run test
  //   std::cout << " + Test " << name << " passed" << std::endl;
  // } else {
  //   std::cout << " - Test " << name << " failed" << std::endl;
  //   numberOfFailedTests += 1; // and increase fail count
  // }
}

int main() {
  /*
   * RUN TEST CASES HERE, USE ONLY THE METHOD NAME as parameter:
   */
  RUN(someComponentTest);
  // Vehicle:
  RUN(nextDirectionTest);
  RUN(setPositionTest);
  // Junction:
  RUN(junctionCreationTest);
  // DomainModel:
  RUN(modelCreationTest);

  if (numberOfFailedTests == 0) {
    std::cout << "ALL TESTS PASSED!" << std::endl;
  } else {
    std::cout << "ERROR: " << numberOfFailedTests << " tests failed!" << std::endl;
  }
  return 0;
}