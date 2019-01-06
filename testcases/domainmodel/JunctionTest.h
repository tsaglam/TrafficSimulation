#include "DomainModelTestFactory.h"
#include "Junction.h"
#include "Street.h"
#include <../../snowhouse/snowhouse.h>

using namespace snowhouse;

/*
 * adds four incoming and four outgoing streets to a junction.
 */
void addStreets(Junction &junction) {
  Junction other = createTestJunction();
  // clang-format off
  for (CardinalDirection dir = CardinalDirection::NORTH; dir < CardinalDirection::WEST;
       dir = CardinalDirection(dir + 1)) { // clang-format on
    Street incoming = Street(dir, 1, 50.0, 100.0, other, junction);
    Street outgoing = Street(dir + 4, 1, 50.0, 100.0, junction, other);
    junction.addIncomingStreet(incoming, dir);
    junction.addOutgoingStreet(outgoing, dir);
  }
}

/*
 * Tries to add streets and checks whether they are connected.
 */
void junctionCreationTest() {
  Junction testJunction = createTestJunction();
  addStreets(testJunction);
  // clang-format off
  for (CardinalDirection dir = CardinalDirection::NORTH; dir < CardinalDirection::WEST;
       dir = CardinalDirection(dir + 1)) { // clang-format on
    AssertThat(testJunction.getIncomingStreet(dir).isConnected(), Is().EqualTo(true));
    AssertThat(testJunction.getOutgoingStreet(dir).isConnected(), Is().EqualTo(true));
  }
}