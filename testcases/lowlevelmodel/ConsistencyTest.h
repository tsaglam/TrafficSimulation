#ifndef CONSISTENCY_TEST_H
#define CONSISTENCY_TEST_H

#include "RfbStructureTestUtils.h"

/*
 * Move cars by setting a new position, call updateCarsAndRestoreConsistency and test the consistency of the street
 * using the getNextCar functions.
 * Different test cases:
 * - no cars moved -> all cars moved
 * - all cars remain on street -> all cars left the street
 * - no cars on the street -> many cars on the street
 * - no change in the car order -> much change in the car order (per lane/per street)
 */

// Empty 1-lane street, no movement
template <template <typename Car> typename Street>
void consistencyTest1() {
  Street<LowLevelCar> street(1, 10);
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {});
  checkIterable(street.beyondsIterable(), {});

  street.updateCarsAndRestoreConsistency();

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {});
  checkIterable(street.beyondsIterable(), {});
}

// Non-empty 1-lane street but no car movement
template <template <typename Car> typename Street>
void consistencyTest2() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 6));
  street.insertCar(createCar(2, 0, 4));
  street.insertCar(createCar(3, 0, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, 2, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 0, behind));
  neighbors.push_back(NeighborDef(1, 3, 0, inFront));
  neighbors.push_back(NeighborDef(1, 2, 0, behind));
  neighbors.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors.push_back(NeighborDef(2, 0, 0, behind));
  neighbors.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors.push_back(NeighborDef(3, 1, 0, behind));

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3});
  checkIterable(street.beyondsIterable(), {});

  street.updateCarsAndRestoreConsistency();

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3});
  checkIterable(street.beyondsIterable(), {});
}

// 1-lane street, cars move but no change in the car order
template <template <typename Car> typename Street>
void consistencyTest3() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 6));
  street.insertCar(createCar(2, 0, 4));
  street.insertCar(createCar(3, 0, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, 2, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 0, behind));
  neighbors.push_back(NeighborDef(1, 3, 0, inFront));
  neighbors.push_back(NeighborDef(1, 2, 0, behind));
  neighbors.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors.push_back(NeighborDef(2, 0, 0, behind));
  neighbors.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors.push_back(NeighborDef(3, 1, 0, behind));

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 5, 0); }
    if (car.getId() == 1) { car.setNext(0, 7, 0); }
    if (car.getId() == 2) { car.setNext(0, 6, 0); }
    if (car.getId() == 3) { car.setNext(0, 9, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3});
  checkIterable(street.beyondsIterable(), {});
}

// 2-lane street, cars move on one lane, no lane changes
// -> order of cars on different lanes is modified
template <template <typename Car> typename Street>
void consistencyTest4() {
  Street<LowLevelCar> street(2, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 6));
  street.insertCar(createCar(2, 0, 4));
  street.insertCar(createCar(3, 0, 9));
  street.insertCar(createCar(4, 1, 1));
  street.insertCar(createCar(5, 1, 3));
  street.insertCar(createCar(6, 1, 7));
  street.insertCar(createCar(7, 1, 7));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 0, behind));
  neighbors1.push_back(NeighborDef(0, 5, 1, inFront));
  neighbors1.push_back(NeighborDef(0, 4, 1, behind));

  neighbors1.push_back(NeighborDef(1, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 2, 0, behind));
  neighbors1.push_back(NeighborDef(1, 7, 1, inFront));
  neighbors1.push_back(NeighborDef(1, 5, 1, behind));

  neighbors1.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 0, 0, behind));
  neighbors1.push_back(NeighborDef(2, 7, 1, inFront));
  neighbors1.push_back(NeighborDef(2, 5, 1, behind));

  neighbors1.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 1, 0, behind));
  neighbors1.push_back(NeighborDef(3, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(3, 6, 1, behind));

  neighbors1.push_back(NeighborDef(4, 0, -1, inFront));
  neighbors1.push_back(NeighborDef(4, -1, -1, behind));
  neighbors1.push_back(NeighborDef(4, 5, 0, inFront));
  neighbors1.push_back(NeighborDef(4, -1, 0, behind));

  neighbors1.push_back(NeighborDef(5, 2, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 0, -1, behind));
  neighbors1.push_back(NeighborDef(5, 7, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 4, 0, behind));

  neighbors1.push_back(NeighborDef(6, 3, -1, inFront));
  neighbors1.push_back(NeighborDef(6, 1, -1, behind));
  neighbors1.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(6, 7, 0, behind));

  neighbors1.push_back(NeighborDef(7, 3, -1, inFront));
  neighbors1.push_back(NeighborDef(7, 1, -1, behind));
  neighbors1.push_back(NeighborDef(7, 6, 0, inFront));
  neighbors1.push_back(NeighborDef(7, 5, 0, behind));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 3, 0); }
    if (car.getId() == 1) { car.setNext(0, 7, 0); }
    if (car.getId() == 2) { car.setNext(0, 7, 0); }
    if (car.getId() == 3) { car.setNext(0, 9, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 2, 0, inFront));
  neighbors2.push_back(NeighborDef(0, -1, 0, behind));
  neighbors2.push_back(NeighborDef(0, 7, 1, inFront));
  neighbors2.push_back(NeighborDef(0, 5, 1, behind));

  neighbors2.push_back(NeighborDef(1, 3, 0, inFront));
  neighbors2.push_back(NeighborDef(1, 2, 0, behind));
  neighbors2.push_back(NeighborDef(1, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(1, 6, 1, behind));

  neighbors2.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors2.push_back(NeighborDef(2, 0, 0, behind));
  neighbors2.push_back(NeighborDef(2, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(2, 6, 1, behind));

  neighbors2.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(3, 1, 0, behind));
  neighbors2.push_back(NeighborDef(3, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(3, 6, 1, behind));

  neighbors2.push_back(NeighborDef(4, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(4, -1, -1, behind));
  neighbors2.push_back(NeighborDef(4, 5, 0, inFront));
  neighbors2.push_back(NeighborDef(4, -1, 0, behind));

  neighbors2.push_back(NeighborDef(5, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(5, -1, -1, behind));
  neighbors2.push_back(NeighborDef(5, 7, 0, inFront));
  neighbors2.push_back(NeighborDef(5, 4, 0, behind));

  neighbors2.push_back(NeighborDef(6, 2, -1, inFront));
  neighbors2.push_back(NeighborDef(6, 0, -1, behind));
  neighbors2.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(6, 7, 0, behind));

  neighbors2.push_back(NeighborDef(7, 2, -1, inFront));
  neighbors2.push_back(NeighborDef(7, 0, -1, behind));
  neighbors2.push_back(NeighborDef(7, 6, 0, inFront));
  neighbors2.push_back(NeighborDef(7, 5, 0, behind));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7});
  checkIterable(street.beyondsIterable(), {});
}

// 3-lane street, all cars are moved with different velocities, no lane changes
template <template <typename Car> typename Street>
void consistencyTest5() {
  Street<LowLevelCar> street(3, 10);

  street.insertCar(createCar(0, 0, 0));
  street.insertCar(createCar(1, 0, 2));
  street.insertCar(createCar(2, 0, 4));
  street.insertCar(createCar(3, 0, 6));
  street.insertCar(createCar(4, 0, 8));
  street.insertCar(createCar(5, 1, 2));
  street.insertCar(createCar(6, 1, 6));
  street.insertCar(createCar(7, 2, 1));
  street.insertCar(createCar(8, 2, 5));
  street.insertCar(createCar(9, 2, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 0, behind));
  neighbors1.push_back(NeighborDef(0, 5, 1, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 1, behind));

  neighbors1.push_back(NeighborDef(1, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, behind));
  neighbors1.push_back(NeighborDef(1, 6, 1, inFront));
  neighbors1.push_back(NeighborDef(1, 5, 1, behind));

  neighbors1.push_back(NeighborDef(2, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 1, 0, behind));
  neighbors1.push_back(NeighborDef(2, 6, 1, inFront));
  neighbors1.push_back(NeighborDef(2, 5, 1, behind));

  neighbors1.push_back(NeighborDef(3, 4, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 2, 0, behind));
  neighbors1.push_back(NeighborDef(3, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(3, 6, 1, behind));

  neighbors1.push_back(NeighborDef(4, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 3, 0, behind));
  neighbors1.push_back(NeighborDef(4, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(4, 6, 1, behind));

  neighbors1.push_back(NeighborDef(5, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 0, -1, behind));
  neighbors1.push_back(NeighborDef(5, 6, 0, inFront));
  neighbors1.push_back(NeighborDef(5, -1, 0, behind));
  neighbors1.push_back(NeighborDef(5, 8, 1, inFront));
  neighbors1.push_back(NeighborDef(5, 7, 1, behind));

  neighbors1.push_back(NeighborDef(6, 3, -1, inFront));
  neighbors1.push_back(NeighborDef(6, 2, -1, behind));
  neighbors1.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(6, 5, 0, behind));
  neighbors1.push_back(NeighborDef(6, 9, 1, inFront));
  neighbors1.push_back(NeighborDef(6, 8, 1, behind));

  neighbors1.push_back(NeighborDef(7, 5, -1, inFront));
  neighbors1.push_back(NeighborDef(7, -1, -1, behind));
  neighbors1.push_back(NeighborDef(7, 8, 0, inFront));
  neighbors1.push_back(NeighborDef(7, -1, 0, behind));

  neighbors1.push_back(NeighborDef(8, 6, -1, inFront));
  neighbors1.push_back(NeighborDef(8, 5, -1, behind));
  neighbors1.push_back(NeighborDef(8, 9, 0, inFront));
  neighbors1.push_back(NeighborDef(8, 7, 0, behind));

  neighbors1.push_back(NeighborDef(9, -1, -1, inFront));
  neighbors1.push_back(NeighborDef(9, 6, -1, behind));
  neighbors1.push_back(NeighborDef(9, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(9, 8, 0, behind));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 1, 0); }
    if (car.getId() == 1) { car.setNext(0, 4, 0); }
    if (car.getId() == 2) { car.setNext(0, 6, 0); }
    if (car.getId() == 3) { car.setNext(0, 8, 0); }
    if (car.getId() == 4) { car.setNext(0, 9, 0); }
    if (car.getId() == 5) { car.setNext(1, 8, 0); }
    if (car.getId() == 6) { car.setNext(1, 9, 0); }
    if (car.getId() == 7) { car.setNext(2, 1, 0); }
    if (car.getId() == 8) { car.setNext(2, 6, 0); }
    if (car.getId() == 9) { car.setNext(2, 9, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 1, 0, inFront));
  neighbors2.push_back(NeighborDef(0, -1, 0, behind));
  neighbors2.push_back(NeighborDef(0, 5, 1, inFront));
  neighbors2.push_back(NeighborDef(0, -1, 1, behind));

  neighbors2.push_back(NeighborDef(1, 2, 0, inFront));
  neighbors2.push_back(NeighborDef(1, 0, 0, behind));
  neighbors2.push_back(NeighborDef(1, 5, 1, inFront));
  neighbors2.push_back(NeighborDef(1, -1, 1, behind));

  neighbors2.push_back(NeighborDef(2, 3, 0, inFront));
  neighbors2.push_back(NeighborDef(2, 1, 0, behind));
  neighbors2.push_back(NeighborDef(2, 5, 1, inFront));
  neighbors2.push_back(NeighborDef(2, -1, 1, behind));

  neighbors2.push_back(NeighborDef(3, 4, 0, inFront));
  neighbors2.push_back(NeighborDef(3, 2, 0, behind));
  neighbors2.push_back(NeighborDef(3, 6, 1, inFront));
  neighbors2.push_back(NeighborDef(3, 5, 1, behind));

  neighbors2.push_back(NeighborDef(4, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(4, 3, 0, behind));
  neighbors2.push_back(NeighborDef(4, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(4, 6, 1, behind));

  neighbors2.push_back(NeighborDef(5, 3, -1, inFront));
  neighbors2.push_back(NeighborDef(5, 2, -1, behind));
  neighbors2.push_back(NeighborDef(5, 6, 0, inFront));
  neighbors2.push_back(NeighborDef(5, -1, 0, behind));
  neighbors2.push_back(NeighborDef(5, 9, 1, inFront));
  neighbors2.push_back(NeighborDef(5, 8, 1, behind));

  neighbors2.push_back(NeighborDef(6, 4, -1, inFront));
  neighbors2.push_back(NeighborDef(6, 3, -1, behind));
  neighbors2.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(6, 5, 0, behind));
  neighbors2.push_back(NeighborDef(6, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(6, 9, 1, behind));

  neighbors2.push_back(NeighborDef(7, 5, -1, inFront));
  neighbors2.push_back(NeighborDef(7, -1, -1, behind));
  neighbors2.push_back(NeighborDef(7, 8, 0, inFront));
  neighbors2.push_back(NeighborDef(7, -1, 0, behind));

  neighbors2.push_back(NeighborDef(8, 5, -1, inFront));
  neighbors2.push_back(NeighborDef(8, -1, -1, behind));
  neighbors2.push_back(NeighborDef(8, 9, 0, inFront));
  neighbors2.push_back(NeighborDef(8, 7, 0, behind));

  neighbors2.push_back(NeighborDef(9, 6, -1, inFront));
  neighbors2.push_back(NeighborDef(9, 5, -1, behind));
  neighbors2.push_back(NeighborDef(9, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(9, 8, 0, behind));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  checkIterable(street.beyondsIterable(), {});
}

// 3-lane street, all cars are moved with different velocities and lane changes, some cars are at the same position
template <template <typename Car> typename Street>
void consistencyTest6() {
  Street<LowLevelCar> street(3, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 4));
  street.insertCar(createCar(2, 0, 8));
  street.insertCar(createCar(3, 1, 0));
  street.insertCar(createCar(4, 1, 1));
  street.insertCar(createCar(5, 1, 6));
  street.insertCar(createCar(6, 1, 6));
  street.insertCar(createCar(7, 2, 0));
  street.insertCar(createCar(8, 2, 3));
  street.insertCar(createCar(9, 2, 6));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 0, behind));
  neighbors1.push_back(NeighborDef(0, 6, 1, inFront));
  neighbors1.push_back(NeighborDef(0, 4, 1, behind));

  neighbors1.push_back(NeighborDef(1, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, behind));
  neighbors1.push_back(NeighborDef(1, 6, 1, inFront));
  neighbors1.push_back(NeighborDef(1, 4, 1, behind));

  neighbors1.push_back(NeighborDef(2, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 1, 0, behind));
  neighbors1.push_back(NeighborDef(2, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(2, 5, 1, behind));

  neighbors1.push_back(NeighborDef(3, 0, -1, inFront));
  neighbors1.push_back(NeighborDef(3, -1, -1, behind));
  neighbors1.push_back(NeighborDef(3, 4, 0, inFront));
  neighbors1.push_back(NeighborDef(3, -1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 8, 1, inFront));
  neighbors1.push_back(NeighborDef(3, 7, 1, behind));

  neighbors1.push_back(NeighborDef(4, 0, -1, inFront));
  neighbors1.push_back(NeighborDef(4, -1, -1, behind));
  neighbors1.push_back(NeighborDef(4, 6, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 3, 0, behind));
  neighbors1.push_back(NeighborDef(4, 8, 1, inFront));
  neighbors1.push_back(NeighborDef(4, 7, 1, behind));

  neighbors1.push_back(NeighborDef(5, 2, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 1, -1, behind));
  neighbors1.push_back(NeighborDef(5, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 6, 0, behind));
  neighbors1.push_back(NeighborDef(5, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(5, 9, 1, behind));

  neighbors1.push_back(NeighborDef(6, 2, -1, inFront));
  neighbors1.push_back(NeighborDef(6, 1, -1, behind));
  neighbors1.push_back(NeighborDef(6, 5, 0, inFront));
  neighbors1.push_back(NeighborDef(6, 4, 0, behind));
  neighbors1.push_back(NeighborDef(6, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(6, 9, 1, behind));

  neighbors1.push_back(NeighborDef(7, 3, -1, inFront));
  neighbors1.push_back(NeighborDef(7, -1, -1, behind));
  neighbors1.push_back(NeighborDef(7, 8, 0, inFront));
  neighbors1.push_back(NeighborDef(7, -1, 0, behind));

  neighbors1.push_back(NeighborDef(8, 6, -1, inFront));
  neighbors1.push_back(NeighborDef(8, 4, -1, behind));
  neighbors1.push_back(NeighborDef(8, 9, 0, inFront));
  neighbors1.push_back(NeighborDef(8, 7, 0, behind));

  neighbors1.push_back(NeighborDef(9, 6, -1, inFront));
  neighbors1.push_back(NeighborDef(9, 4, -1, behind));
  neighbors1.push_back(NeighborDef(9, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(9, 8, 0, behind));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 4, 0); }
    if (car.getId() == 1) { car.setNext(0, 7, 0); }
    if (car.getId() == 2) { car.setNext(1, 9, 0); }
    if (car.getId() == 3) { car.setNext(1, 3, 0); }
    if (car.getId() == 4) { car.setNext(2, 2, 0); }
    if (car.getId() == 5) { car.setNext(0, 7, 0); }
    if (car.getId() == 6) { car.setNext(1, 8, 0); }
    if (car.getId() == 7) { car.setNext(2, 2, 0); }
    if (car.getId() == 8) { car.setNext(2, 5, 0); }
    if (car.getId() == 9) { car.setNext(1, 8, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 5, 0, inFront));
  neighbors2.push_back(NeighborDef(0, -1, 0, behind));
  neighbors2.push_back(NeighborDef(0, 9, 1, inFront));
  neighbors2.push_back(NeighborDef(0, 3, 1, behind));

  neighbors2.push_back(NeighborDef(1, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(1, 5, 0, behind));
  neighbors2.push_back(NeighborDef(1, 9, 1, inFront));
  neighbors2.push_back(NeighborDef(1, 3, 1, behind));

  neighbors2.push_back(NeighborDef(2, -1, -1, inFront));
  neighbors2.push_back(NeighborDef(2, 1, -1, behind));
  neighbors2.push_back(NeighborDef(2, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(2, 6, 0, behind));
  neighbors2.push_back(NeighborDef(2, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(2, 8, 1, behind));

  neighbors2.push_back(NeighborDef(3, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(3, -1, -1, behind));
  neighbors2.push_back(NeighborDef(3, 9, 0, inFront));
  neighbors2.push_back(NeighborDef(3, -1, 0, behind));
  neighbors2.push_back(NeighborDef(3, 8, 1, inFront));
  neighbors2.push_back(NeighborDef(3, 4, 1, behind));

  neighbors2.push_back(NeighborDef(4, 3, -1, inFront));
  neighbors2.push_back(NeighborDef(4, -1, -1, behind));
  neighbors2.push_back(NeighborDef(4, 8, 0, inFront));
  neighbors2.push_back(NeighborDef(4, 7, 0, behind));

  neighbors2.push_back(NeighborDef(5, 1, 0, inFront));
  neighbors2.push_back(NeighborDef(5, 0, 0, behind));
  neighbors2.push_back(NeighborDef(5, 9, 1, inFront));
  neighbors2.push_back(NeighborDef(5, 3, 1, behind));

  neighbors2.push_back(NeighborDef(6, -1, -1, inFront));
  neighbors2.push_back(NeighborDef(6, 1, -1, behind));
  neighbors2.push_back(NeighborDef(6, 2, 0, inFront));
  neighbors2.push_back(NeighborDef(6, 9, 0, behind));
  neighbors2.push_back(NeighborDef(6, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(6, 8, 1, behind));

  neighbors2.push_back(NeighborDef(7, 3, -1, inFront));
  neighbors2.push_back(NeighborDef(7, -1, -1, behind));
  neighbors2.push_back(NeighborDef(7, 4, 0, inFront));
  neighbors2.push_back(NeighborDef(7, -1, 0, behind));

  neighbors2.push_back(NeighborDef(8, 9, -1, inFront));
  neighbors2.push_back(NeighborDef(8, 3, -1, behind));
  neighbors2.push_back(NeighborDef(8, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(8, 4, 0, behind));

  neighbors2.push_back(NeighborDef(9, -1, -1, inFront));
  neighbors2.push_back(NeighborDef(9, 1, -1, behind));
  neighbors2.push_back(NeighborDef(9, 6, 0, inFront));
  neighbors2.push_back(NeighborDef(9, 3, 0, behind));
  neighbors2.push_back(NeighborDef(9, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(9, 8, 1, behind));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  checkIterable(street.beyondsIterable(), {});
}

// 1-lane street, some cars leave the street, calling remove beyonds multiple times
template <template <typename Car> typename Street>
void consistencyTest7() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 4));
  street.insertCar(createCar(2, 0, 7));
  street.insertCar(createCar(3, 0, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  for (int carId = 0; carId < 4; ++carId) { neighbors1.push_back(NeighborDef(carId, carId - 1, 0, behind)); }
  for (int carId = 0; carId < 3; ++carId) { neighbors1.push_back(NeighborDef(carId, carId + 1, 0, inFront)); }
  neighbors1.push_back(NeighborDef(3, -1, 0, inFront));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 5, 0); }
    if (car.getId() == 1) { car.setNext(0, 8, 0); }
    if (car.getId() == 2) { car.setNext(0, 11, 0); }
    if (car.getId() == 3) { car.setNext(0, 15, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  // in between state
  checkIterable(street.allIterable(), {0, 1}, {2, 3});
  checkIterable(street.beyondsIterable(), {2, 3});

  street.removeBeyonds();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 1, 0, inFront));
  neighbors2.push_back(NeighborDef(0, -1, 0, behind));
  neighbors2.push_back(NeighborDef(1, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(1, 0, 0, behind));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1});
  checkIterable(street.beyondsIterable(), {});

  street.removeBeyonds();

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1});
  checkIterable(street.beyondsIterable(), {});
}

// 1-lane street, all cars leave the street, some are exactly at the end of the street
template <template <typename Car> typename Street>
void consistencyTest8() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 3));
  street.insertCar(createCar(2, 0, 5));
  street.insertCar(createCar(3, 0, 7));
  street.insertCar(createCar(4, 0, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  for (int carId = 0; carId < 5; ++carId) { neighbors1.push_back(NeighborDef(carId, carId - 1, 0, behind)); }
  for (int carId = 0; carId < 4; ++carId) { neighbors1.push_back(NeighborDef(carId, carId + 1, 0, inFront)); }
  neighbors1.push_back(NeighborDef(4, -1, 0, inFront));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 25, 0); }
    if (car.getId() == 1) { car.setNext(0, 10.001, 0); }
    if (car.getId() == 2) { car.setNext(0, 13, 0); }
    if (car.getId() == 3) { car.setNext(0, 10, 0); }
    if (car.getId() == 4) { car.setNext(0, 11, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  // in between state
  checkIterable(street.allIterable(), {}, {0, 1, 2, 3, 4});
  checkIterable(street.beyondsIterable(), {0, 1, 2, 3, 4});

  street.removeBeyonds();

  std::vector<NeighborDef> neighbors2; // empty since all cars left the street

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {});
  checkIterable(street.beyondsIterable(), {});
}

// 3-lane street with lane changes, cars at the same position and cars leaving the street
template <template <typename Car> typename Street>
void consistencyTest9() {
  Street<LowLevelCar> street(3, 10);

  street.insertCar(createCar(0, 0, 4));
  street.insertCar(createCar(1, 1, 1));
  street.insertCar(createCar(2, 2, 4));
  street.insertCar(createCar(3, 0, 7));
  street.insertCar(createCar(4, 1, 6));
  street.insertCar(createCar(5, 2, 8));
  street.insertCar(createCar(6, 0, 2));
  street.insertCar(createCar(7, 2, 0));
  street.insertCar(createCar(8, 1, 3));
  street.insertCar(createCar(9, 2, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(0, 6, 0, behind));
  neighbors1.push_back(NeighborDef(0, 4, 1, inFront));
  neighbors1.push_back(NeighborDef(0, 8, 1, behind));

  neighbors1.push_back(NeighborDef(1, 6, -1, inFront));
  neighbors1.push_back(NeighborDef(1, -1, -1, behind));
  neighbors1.push_back(NeighborDef(1, 8, 0, inFront));
  neighbors1.push_back(NeighborDef(1, -1, 0, behind));
  neighbors1.push_back(NeighborDef(1, 2, 1, inFront));
  neighbors1.push_back(NeighborDef(1, 7, 1, behind));

  neighbors1.push_back(NeighborDef(2, 4, -1, inFront));
  neighbors1.push_back(NeighborDef(2, 8, -1, behind));
  neighbors1.push_back(NeighborDef(2, 5, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 7, 0, behind));

  neighbors1.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 0, 0, behind));
  neighbors1.push_back(NeighborDef(3, -1, 1, inFront));
  neighbors1.push_back(NeighborDef(3, 4, 1, behind));

  neighbors1.push_back(NeighborDef(4, 3, -1, inFront));
  neighbors1.push_back(NeighborDef(4, 0, -1, behind));
  neighbors1.push_back(NeighborDef(4, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 8, 0, behind));
  neighbors1.push_back(NeighborDef(4, 5, 1, inFront));
  neighbors1.push_back(NeighborDef(4, 2, 1, behind));

  neighbors1.push_back(NeighborDef(5, -1, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 4, -1, behind));
  neighbors1.push_back(NeighborDef(5, 9, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 2, 0, behind));

  neighbors1.push_back(NeighborDef(6, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(6, -1, 0, behind));
  neighbors1.push_back(NeighborDef(6, 8, 1, inFront));
  neighbors1.push_back(NeighborDef(6, 1, 1, behind));

  neighbors1.push_back(NeighborDef(7, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(7, -1, -1, behind));
  neighbors1.push_back(NeighborDef(7, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(7, -1, 0, behind));

  neighbors1.push_back(NeighborDef(8, 0, -1, inFront));
  neighbors1.push_back(NeighborDef(8, 6, -1, behind));
  neighbors1.push_back(NeighborDef(8, 4, 0, inFront));
  neighbors1.push_back(NeighborDef(8, 1, 0, behind));
  neighbors1.push_back(NeighborDef(8, 2, 1, inFront));
  neighbors1.push_back(NeighborDef(8, 7, 1, behind));

  neighbors1.push_back(NeighborDef(9, -1, -1, inFront));
  neighbors1.push_back(NeighborDef(9, 4, -1, behind));
  neighbors1.push_back(NeighborDef(9, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(9, 5, 0, behind));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  checkIterable(street.beyondsIterable(), {});

  for (auto &car : street.allIterable()) {
    if (car.getId() == 0) { car.setNext(0, 6, 0); }
    if (car.getId() == 1) { car.setNext(0, 3, 0); }
    if (car.getId() == 2) { car.setNext(1, 6, 0); }
    if (car.getId() == 3) { car.setNext(0, 9.99, 0); }
    if (car.getId() == 4) { car.setNext(1, 15, 0); }
    if (car.getId() == 5) { car.setNext(2, 9.99, 0); }
    if (car.getId() == 6) { car.setNext(1, 5, 0); }
    if (car.getId() == 7) { car.setNext(2, 5, 0); }
    if (car.getId() == 8) { car.setNext(1, 5, 0); }
    if (car.getId() == 9) { car.setNext(2, 10.01, 0); }
  }

  street.updateCarsAndRestoreConsistency();

  // in between state
  checkIterable(street.allIterable(), {0, 1, 2, 3, 5, 6, 7, 8}, {4, 9});
  checkIterable(street.beyondsIterable(), {4, 9});

  street.removeBeyonds();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 3, 0, inFront));
  neighbors2.push_back(NeighborDef(0, 1, 0, behind));
  neighbors2.push_back(NeighborDef(0, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(0, 2, 1, behind));

  neighbors2.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors2.push_back(NeighborDef(1, -1, 0, behind));
  neighbors2.push_back(NeighborDef(1, 8, 1, inFront));
  neighbors2.push_back(NeighborDef(1, -1, 1, behind));

  neighbors2.push_back(NeighborDef(2, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(2, 1, -1, behind));
  neighbors2.push_back(NeighborDef(2, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(2, 6, 0, behind));
  neighbors2.push_back(NeighborDef(2, 5, 1, inFront));
  neighbors2.push_back(NeighborDef(2, 7, 1, behind));

  neighbors2.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(3, 0, 0, behind));
  neighbors2.push_back(NeighborDef(3, -1, 1, inFront));
  neighbors2.push_back(NeighborDef(3, 2, 1, behind));

  neighbors2.push_back(NeighborDef(5, -1, -1, inFront));
  neighbors2.push_back(NeighborDef(5, 2, -1, behind));
  neighbors2.push_back(NeighborDef(5, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(5, 7, 0, behind));

  neighbors2.push_back(NeighborDef(6, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(6, 1, -1, behind));
  neighbors2.push_back(NeighborDef(6, 2, 0, inFront));
  neighbors2.push_back(NeighborDef(6, 8, 0, behind));
  neighbors2.push_back(NeighborDef(6, 5, 1, inFront));
  neighbors2.push_back(NeighborDef(6, 7, 1, behind));

  neighbors2.push_back(NeighborDef(7, 6, -1, inFront));
  neighbors2.push_back(NeighborDef(7, 8, -1, behind));
  neighbors2.push_back(NeighborDef(7, 5, 0, inFront));
  neighbors2.push_back(NeighborDef(7, -1, 0, behind));

  neighbors2.push_back(NeighborDef(8, 0, -1, inFront));
  neighbors2.push_back(NeighborDef(8, 1, -1, behind));
  neighbors2.push_back(NeighborDef(8, 6, 0, inFront));
  neighbors2.push_back(NeighborDef(8, -1, 0, behind));
  neighbors2.push_back(NeighborDef(8, 7, 1, inFront));
  neighbors2.push_back(NeighborDef(8, -1, 1, behind));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 5, 6, 7, 8});
  checkIterable(street.beyondsIterable(), {});
}

#endif