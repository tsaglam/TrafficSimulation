#ifndef GET_NEXT_CAR_TEST_H
#define GET_NEXT_CAR_TEST_H

#include "RfbStructureTestUtils.h"

/*
 * Test if the getNextCarInFront/Behind functions return the correct car.
 * - check neighbors on both the own lane an neighboring lanes
 * - try different special cases such as no existing neighbors, a neighbor at the same position etc.
 */

// Case 1: 1 lane street, multiple cars in succession
template <template <typename Car> typename Street>
void getNextCarTest1() {
  // Set-Up: create street and insert cars
  const size_t carCount = 10;
  Street<LowLevelCar> street(1, carCount);
  for (size_t i = 0; i < carCount; ++i) { street.insertCar(createCar(i, 0, i)); }
  street.incorporateInsertedCars();

  // Define neighbor relationships
  std::vector<NeighborDef> neighbors;
  for (size_t i = 0; i < carCount; ++i) { neighbors.push_back(NeighborDef(i, i - 1, 0, behind)); }
  for (size_t i = 0; i < carCount - 1; ++i) { neighbors.push_back(NeighborDef(i, i + 1, 0, inFront)); }
  neighbors.push_back(NeighborDef(carCount - 1, -1, 0, inFront));
  checkNeighbors(street, neighbors);
}

// Case 2: 3 lane street, one car with a neighbor in each direction and lane
template <template <typename Car> typename Street>
void getNextCarTest2() {
  //   0123456789
  // 0:  1     2
  // 1:  3  0  4
  // 2:  5     6
  Street<LowLevelCar> street(3, 10);
  street.insertCar(createCar(0, 1, 4));
  street.insertCar(createCar(1, 0, 1));
  street.insertCar(createCar(2, 0, 7));
  street.insertCar(createCar(3, 1, 1));
  street.insertCar(createCar(4, 1, 7));
  street.insertCar(createCar(5, 2, 1));
  street.insertCar(createCar(6, 2, 7));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, 2, -1, inFront));
  neighbors.push_back(NeighborDef(0, 4, 0, inFront));
  neighbors.push_back(NeighborDef(0, 6, 1, inFront));
  neighbors.push_back(NeighborDef(0, 1, -1, behind));
  neighbors.push_back(NeighborDef(0, 3, 0, behind));
  neighbors.push_back(NeighborDef(0, 5, 1, behind));
  checkNeighbors(street, neighbors);
}

// Case 3: 3 lane street, one car, no neighbors
template <template <typename Car> typename Street>
void getNextCarTest3() {
  //   0123456789
  // 0:
  // 1:    0
  // 2:
  Street<LowLevelCar> street(3, 10);
  street.insertCar(createCar(0, 1, 4));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, -1, -1, inFront));
  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 1, inFront));
  neighbors.push_back(NeighborDef(0, -1, -1, behind));
  neighbors.push_back(NeighborDef(0, -1, 0, behind));
  neighbors.push_back(NeighborDef(0, -1, 1, behind));
  checkNeighbors(street, neighbors);
}

// Case 4: 1 lane street, three cars at the same position
template <template <typename Car> typename Street>
void getNextCarTest4() {
  Street<LowLevelCar> street(1, 10);
  for (int i = 0; i < 3; ++i) { street.insertCar(createCar(i, 0, 0)); }
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors.push_back(NeighborDef(0, 1, 0, behind));
  neighbors.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors.push_back(NeighborDef(1, 2, 0, behind));
  neighbors.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors.push_back(NeighborDef(2, -1, 0, behind));
  checkNeighbors(street, neighbors);
}

// Case 5: 3 lane street, one car per lane, each car at the same distance
template <template <typename Car> typename Street>
void getNextCarTest5() {
  //   0123456789
  // 0:    0
  // 1:    1
  // 2:    2
  Street<LowLevelCar> street(3, 10);
  street.insertCar(createCar(0, 0, 4));
  street.insertCar(createCar(1, 1, 4));
  street.insertCar(createCar(2, 2, 4));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 0, behind));
  neighbors.push_back(NeighborDef(0, -1, 1, inFront));
  neighbors.push_back(NeighborDef(0, 1, 1, behind));

  neighbors.push_back(NeighborDef(1, 0, -1, inFront));
  neighbors.push_back(NeighborDef(1, -1, -1, behind));
  neighbors.push_back(NeighborDef(1, -1, 0, inFront));
  neighbors.push_back(NeighborDef(1, -1, 0, behind));
  neighbors.push_back(NeighborDef(1, -1, 1, inFront));
  neighbors.push_back(NeighborDef(1, 2, 1, behind));

  neighbors.push_back(NeighborDef(2, 1, -1, inFront));
  neighbors.push_back(NeighborDef(2, -1, -1, behind));
  neighbors.push_back(NeighborDef(2, -1, 0, inFront));
  neighbors.push_back(NeighborDef(2, -1, 0, behind));
  checkNeighbors(street, neighbors);
}

#endif