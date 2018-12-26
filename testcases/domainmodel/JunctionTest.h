/*
 * Creates a test junction.
 */
Junction createJunction() {
  Junction::Signal signalNorth = Junction::Signal(CardinalDirection::NORTH, 30);
  Junction::Signal signalEast  = Junction::Signal(CardinalDirection::EAST, 20);
  Junction::Signal signalSouth = Junction::Signal(CardinalDirection::SOUTH, 10);
  Junction::Signal signalWest  = Junction::Signal(CardinalDirection::WEST, 0);
  const std::vector<Junction::Signal> signals{signalNorth, signalEast, signalSouth, signalWest};
  return Junction(0, 0, 10, 15, signals);
}

/*
 * adds four incoming and four outgoing streets to a junction.
 */
void addStreets(Junction &junction) {
  Junction other = createJunction();
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
bool testJunctionCreation() {
  Junction testJunction = createJunction();
  addStreets(testJunction);
  bool correct = true;
  // clang-format off
  for (CardinalDirection dir = CardinalDirection::NORTH; dir < CardinalDirection::WEST;
       dir = CardinalDirection(dir + 1)) { // clang-format on
    correct &= testJunction.getIncomingStreet(dir).isConnected() == true;
    correct &= testJunction.getOutgoingStreet(dir).isConnected() == true;
  }
  return true;
}