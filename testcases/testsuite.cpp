#include <stdio.h>
#include <iostream>

int failedTests;

// Example code for one basic test case, import and add other test classes
// instead of adding methods here
bool someComponentTest() { return 5 / 2 == 2.5; }
bool someFailingTest() { return 5 / 2 == 2; }

// Run methods, which is responsible for prining the test results
void run(bool (*fun_ptr)(), std::string name) {
  if ((*fun_ptr)()) {  // run test
    std::cout << name << " passed" << std::endl;
  } else {
    std::cout << name << " failed" << std::endl;
    failedTests += 1;  // and increase fail count
  }
}

int main() {
  /*
   * ADD TESTS HERE WITH DESCRIPTION:
   */
  run(&someComponentTest, "someComponentTest");
  run(&someFailingTest, "someFailingTest");

  if (failedTests == 0) {
    std::cout << "ALL TESTS PASSED!" << std::endl;
  } else {
    std::cout << "ERROR: " << failedTests << " tests failed!" << std::endl;
  }
  return 0;
}