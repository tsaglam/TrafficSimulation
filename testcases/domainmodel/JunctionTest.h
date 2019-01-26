#include "DomainModelTestFactory.h"
#include "Junction.h"
#include "JunctionException.h"
#include "Street.h"
#include <../../snowhouse/snowhouse.h>

using namespace snowhouse;

/*
 * adds four incoming and four outgoing streets to a junction.
 */
void addStreets(Junction &junction, DomainModel &model) {
  Junction other = createTestJunction();
  // clang-format off
  for (CardinalDirection dir = CardinalDirection::NORTH; dir <= CardinalDirection::WEST;
       dir = CardinalDirection(dir + 1)) { // clang-format on
    Street incoming = Street(dir, 1, 50.0, 100.0, other, junction);
    Street outgoing = Street(dir + 4, 1, 50.0, 100.0, junction, other);
    junction.addIncomingStreet(model.addStreet(incoming), dir);
    junction.addOutgoingStreet(model.addStreet(outgoing), dir);
  }
}

/*
 * Tries to add streets and checks whether they are connected.
 */
void junctionCreationTest() {
  DomainModel model;
  Junction testJunction = createTestJunction();
  addStreets(testJunction, model);
  // clang-format off
  for (CardinalDirection dir = CardinalDirection::NORTH; dir < CardinalDirection::WEST;
       dir = CardinalDirection(dir + 1)) { // clang-format on
    AssertThat(testJunction.getIncomingStreet(dir).isConnected(), Is().EqualTo(true));
    AssertThat(testJunction.getOutgoingStreet(dir).isConnected(), Is().EqualTo(true));
  }
}

/**
 * Cycles through the traffic light states. Simulates a total of 104 steps, and changes green light four times until
 * back to the northern traffic light.
 */
void trafficLightTest() {
  Junction testJunction = createTestJunction();
  // Step 0, special case: Initialisation
  AssertThat(testJunction.nextStep(), Is().EqualTo(false));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  // Steps 1 to 9
  for (int i = 1; i < 10; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
  // Step 10: Switch
  AssertThat(testJunction.nextStep(), Is().EqualTo(true));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::EAST));
  // Steps 11 to 29
  for (int i = 1; i < 20; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
  // Step 30: Switch
  AssertThat(testJunction.nextStep(), Is().EqualTo(true));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::SOUTH));
  // Steps 31 to 59
  for (int i = 1; i < 30; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
  // Step 60: Switch
  AssertThat(testJunction.nextStep(), Is().EqualTo(true));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::WEST));
  // Steps 61 to 99
  for (int i = 1; i < 40; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
  // Step 100: Switch
  AssertThat(testJunction.nextStep(), Is().EqualTo(true));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
}

/**
 * Cycles through the traffic light states. Simulates a total of 104 steps, and changes green light four times until
 * back to the northern traffic light.
 */
void setSignalTest() {
  Junction testJunction = createTestJunction();
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  for (int i = 0; i < 10; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
  AssertThat(testJunction.nextStep(), Is().EqualTo(true));
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::EAST));
  const std::vector<Junction::Signal> signals{Junction::Signal(CardinalDirection::NORTH, 50)};
  testJunction.setSignals(signals);
  AssertThat(testJunction.getCurrentSignal().getDirection(), Is().EqualTo(CardinalDirection::NORTH));
  for (int i = 0; i < 50; ++i) { AssertThat(testJunction.nextStep(), Is().EqualTo(false)); }
}

/**
 * Tests the valid case where a junction has not signals, and certain operations cannot be performed.
 */
void junctionWithoutTrafficLightsTest() {
  const std::vector<Junction::Signal> signals;
  Junction weirdJunction = Junction(0, 0, 10, 15, signals);
  AssertThrows(JunctionException, weirdJunction.nextStep());
  AssertThrows(JunctionException, weirdJunction.getCurrentSignal());
}

/**
 * Tests the valid case where a junction has not signals, and certain operations cannot be performed.
 */
void previousSignalTest() {
  Junction testJunction  = createTestJunction();
  Junction::Signal first = testJunction.getCurrentSignal();
  while (!testJunction.nextStep()) {}
  Junction::Signal second   = testJunction.getCurrentSignal();
  Junction::Signal previous = testJunction.getPreviousSignal();
  AssertThat(first.getDirection(), Is().EqualTo(previous.getDirection()));     // North is previous of east
  AssertThat(first.getDirection(), Is().Not().EqualTo(second.getDirection())); // North is not east
}
