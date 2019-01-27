#ifndef INSERT_CAR_TEST_H
#define INSERT_CAR_TEST_H

#include "RfbStructureTestUtils.h"

/*
 * Add new cars to the street (with insertCar and incorporateInsertedCars). Test whether all cars are at the correct
 * position using the getNextCar functions.
 * Different test cases:
 * - street previously empty -> street previously containing many cars
 * - inserting no cars -> inserting many cars
 * - all new cars at the beginning of the street -> new cars alternating with old cars
 * - sorted/unsorted input of cars
 */

// Empty 1-lane street, insert mutliple cars sorted by id and position. No cars at the same position.
template <template <typename Car> typename Street>
void insertCarTest1() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 1));
  street.insertCar(createCar(1, 0, 3));
  street.insertCar(createCar(2, 0, 5));
  street.insertCar(createCar(3, 0, 7));
  street.insertCar(createCar(4, 0, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  for (int carId = 0; carId < 5; ++carId) { neighbors.push_back(NeighborDef(carId, carId - 1, 0, behind)); }
  for (int carId = 0; carId < 4; ++carId) { neighbors.push_back(NeighborDef(carId, carId + 1, 0, inFront)); }
  neighbors.push_back(NeighborDef(4, -1, 0, inFront));
  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4});
}

// Empty 1-lane street, insert mutliple cars not sorted by id or position. No cars at the same position.
template <template <typename Car> typename Street>
void insertCarTest2() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 9));
  street.insertCar(createCar(1, 0, 7));
  street.insertCar(createCar(2, 0, 3));
  street.insertCar(createCar(3, 0, 5));
  street.insertCar(createCar(4, 0, 1));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(4, -1, 0, behind));
  neighbors.push_back(NeighborDef(2, 4, 0, behind));
  neighbors.push_back(NeighborDef(3, 2, 0, behind));
  neighbors.push_back(NeighborDef(1, 3, 0, behind));
  neighbors.push_back(NeighborDef(0, 1, 0, behind));

  neighbors.push_back(NeighborDef(4, 2, 0, inFront));
  neighbors.push_back(NeighborDef(2, 3, 0, inFront));
  neighbors.push_back(NeighborDef(3, 1, 0, inFront));
  neighbors.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4});
}

// Empty 1-lane street, insert two cars at the same position.
template <template <typename Car> typename Street>
void insertCarTest3() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 4));
  street.insertCar(createCar(1, 0, 4));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  neighbors.push_back(NeighborDef(0, 1, 0, behind));
  neighbors.push_back(NeighborDef(1, -1, 0, behind));

  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors.push_back(NeighborDef(1, 0, 0, inFront));

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1});
}

// Empty 3-lane street, insert mutliple cars with no specific order, some of them at the same distance on different
// lanes, two cars at the same position (i.e. same distance on the same lane)
template <template <typename Car> typename Street>
void insertCarTest4() {
  Street<LowLevelCar> street(3, 10);

  street.insertCar(createCar(0, 0, 7));
  street.insertCar(createCar(1, 0, 7));
  street.insertCar(createCar(2, 0, 1));
  street.insertCar(createCar(3, 0, 3));
  street.insertCar(createCar(4, 0, 4));

  street.insertCar(createCar(5, 1, 4));
  street.insertCar(createCar(6, 1, 7));
  street.insertCar(createCar(7, 1, 0));

  street.insertCar(createCar(8, 2, 7));
  street.insertCar(createCar(9, 2, 7));
  street.insertCar(createCar(10, 2, 2));
  street.insertCar(createCar(11, 2, 4));
  street.insertCar(createCar(12, 2, 9));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors;
  // Lane 0
  neighbors.push_back(NeighborDef(0, 1, 0, behind));
  neighbors.push_back(NeighborDef(0, 6, 1, behind));
  neighbors.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors.push_back(NeighborDef(0, -1, 1, inFront));

  neighbors.push_back(NeighborDef(1, 4, 0, behind));
  neighbors.push_back(NeighborDef(1, 6, 1, behind));
  neighbors.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors.push_back(NeighborDef(1, -1, 1, inFront));

  neighbors.push_back(NeighborDef(2, -1, 0, behind));
  neighbors.push_back(NeighborDef(2, 7, 1, behind));
  neighbors.push_back(NeighborDef(2, 3, 0, inFront));
  neighbors.push_back(NeighborDef(2, 5, 1, inFront));

  neighbors.push_back(NeighborDef(3, 2, 0, behind));
  neighbors.push_back(NeighborDef(3, 7, 1, behind));
  neighbors.push_back(NeighborDef(3, 4, 0, inFront));
  neighbors.push_back(NeighborDef(3, 5, 1, inFront));

  neighbors.push_back(NeighborDef(4, 3, 0, behind));
  neighbors.push_back(NeighborDef(4, 5, 1, behind));
  neighbors.push_back(NeighborDef(4, 1, 0, inFront));
  neighbors.push_back(NeighborDef(4, 6, 1, inFront));

  // Lane 1
  neighbors.push_back(NeighborDef(5, 3, -1, behind));
  neighbors.push_back(NeighborDef(5, 7, 0, behind));
  neighbors.push_back(NeighborDef(5, 11, 1, behind));
  neighbors.push_back(NeighborDef(5, 4, -1, inFront));
  neighbors.push_back(NeighborDef(5, 6, 0, inFront));
  neighbors.push_back(NeighborDef(5, 9, 1, inFront));

  neighbors.push_back(NeighborDef(6, 4, -1, behind));
  neighbors.push_back(NeighborDef(6, 5, 0, behind));
  neighbors.push_back(NeighborDef(6, 8, 1, behind));
  neighbors.push_back(NeighborDef(6, 1, -1, inFront));
  neighbors.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors.push_back(NeighborDef(6, 12, 1, inFront));

  neighbors.push_back(NeighborDef(7, -1, -1, behind));
  neighbors.push_back(NeighborDef(7, -1, 0, behind));
  neighbors.push_back(NeighborDef(7, -1, 1, behind));
  neighbors.push_back(NeighborDef(7, 2, -1, inFront));
  neighbors.push_back(NeighborDef(7, 5, 0, inFront));
  neighbors.push_back(NeighborDef(7, 10, 1, inFront));

  // Lane 2
  neighbors.push_back(NeighborDef(8, 5, -1, behind));
  neighbors.push_back(NeighborDef(8, 9, 0, behind));
  neighbors.push_back(NeighborDef(8, 6, -1, inFront));
  neighbors.push_back(NeighborDef(8, 12, 0, inFront));

  neighbors.push_back(NeighborDef(9, 5, -1, behind));
  neighbors.push_back(NeighborDef(9, 11, 0, behind));
  neighbors.push_back(NeighborDef(9, 6, -1, inFront));
  neighbors.push_back(NeighborDef(9, 8, 0, inFront));

  neighbors.push_back(NeighborDef(10, 7, -1, behind));
  neighbors.push_back(NeighborDef(10, -1, 0, behind));
  neighbors.push_back(NeighborDef(10, 5, -1, inFront));
  neighbors.push_back(NeighborDef(10, 11, 0, inFront));

  neighbors.push_back(NeighborDef(11, 7, -1, behind));
  neighbors.push_back(NeighborDef(11, 10, 0, behind));
  neighbors.push_back(NeighborDef(11, 5, -1, inFront));
  neighbors.push_back(NeighborDef(11, 9, 0, inFront));

  neighbors.push_back(NeighborDef(12, 6, -1, behind));
  neighbors.push_back(NeighborDef(12, 8, 0, behind));
  neighbors.push_back(NeighborDef(12, -1, -1, inFront));
  neighbors.push_back(NeighborDef(12, -1, 0, inFront));

  checkNeighbors(street, neighbors);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
}

// 1-lane street, insert cars in two steps, the cars of the last step are behind all cars inserted in the first step (in
// terms of distance)
template <template <typename Car> typename Street>
void insertCarTest5() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 7));
  street.insertCar(createCar(1, 0, 4));
  street.insertCar(createCar(2, 0, 5));
  street.insertCar(createCar(3, 0, 8));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 2, 0, behind));
  neighbors1.push_back(NeighborDef(1, -1, 0, behind));
  neighbors1.push_back(NeighborDef(2, 1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 0, 0, behind));

  neighbors1.push_back(NeighborDef(0, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(3, -1, 0, inFront));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3});

  street.insertCar(createCar(4, 0, 2));
  street.insertCar(createCar(5, 0, 0));
  street.insertCar(createCar(6, 0, 3));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors2;
  neighbors2.push_back(NeighborDef(0, 2, 0, behind));
  neighbors2.push_back(NeighborDef(1, 6, 0, behind));
  neighbors2.push_back(NeighborDef(2, 1, 0, behind));
  neighbors2.push_back(NeighborDef(3, 0, 0, behind));
  neighbors2.push_back(NeighborDef(4, 5, 0, behind));
  neighbors2.push_back(NeighborDef(5, -1, 0, behind));
  neighbors2.push_back(NeighborDef(6, 4, 0, behind));

  neighbors2.push_back(NeighborDef(0, 3, 0, inFront));
  neighbors2.push_back(NeighborDef(1, 2, 0, inFront));
  neighbors2.push_back(NeighborDef(2, 0, 0, inFront));
  neighbors2.push_back(NeighborDef(3, -1, 0, inFront));
  neighbors2.push_back(NeighborDef(4, 6, 0, inFront));
  neighbors2.push_back(NeighborDef(5, 4, 0, inFront));
  neighbors2.push_back(NeighborDef(6, 1, 0, inFront));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6});
}

// 1-lane street, insert cars in two steps, the cars of the last step inserted alternatingly with the cars inserted in
// the first step (in terms of distance)
template <template <typename Car> typename Street>
void insertCarTest6() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 9));
  street.insertCar(createCar(1, 0, 7));
  street.insertCar(createCar(2, 0, 3));
  street.insertCar(createCar(3, 0, 5));
  street.insertCar(createCar(4, 0, 1));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 1, 0, behind));
  neighbors1.push_back(NeighborDef(1, 3, 0, behind));
  neighbors1.push_back(NeighborDef(2, 4, 0, behind));
  neighbors1.push_back(NeighborDef(3, 2, 0, behind));
  neighbors1.push_back(NeighborDef(4, -1, 0, behind));

  neighbors1.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 2, 0, inFront));
  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4});

  street.insertCar(createCar(5, 0, 6));
  street.insertCar(createCar(6, 0, 0));
  street.insertCar(createCar(7, 0, 2));
  street.insertCar(createCar(8, 0, 8));
  street.insertCar(createCar(9, 0, 4));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors2;
  neighbors1.push_back(NeighborDef(0, 8, 0, behind));
  neighbors1.push_back(NeighborDef(1, 5, 0, behind));
  neighbors1.push_back(NeighborDef(2, 7, 0, behind));
  neighbors1.push_back(NeighborDef(3, 9, 0, behind));
  neighbors1.push_back(NeighborDef(4, 6, 0, behind));
  neighbors1.push_back(NeighborDef(5, 3, 0, behind));
  neighbors1.push_back(NeighborDef(6, -1, 0, behind));
  neighbors1.push_back(NeighborDef(7, 4, 0, behind));
  neighbors1.push_back(NeighborDef(8, 1, 0, behind));
  neighbors1.push_back(NeighborDef(9, 2, 0, behind));

  neighbors1.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 8, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 9, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 5, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 7, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(6, 4, 0, inFront));
  neighbors1.push_back(NeighborDef(7, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(8, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(9, 3, 0, inFront));
  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
}

// 1-lane street, insert cars in two steps, the cars of the last step inserted alternatingly with the cars inserted in
// the first step (in terms of distance)
template <template <typename Car> typename Street>
void insertCarTest7() {
  Street<LowLevelCar> street(1, 10);

  street.insertCar(createCar(0, 0, 4));
  street.insertCar(createCar(1, 0, 4));
  street.insertCar(createCar(2, 0, 1));
  street.insertCar(createCar(3, 0, 7));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 1, 0, behind));
  neighbors1.push_back(NeighborDef(1, 2, 0, behind));
  neighbors1.push_back(NeighborDef(2, -1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 0, 0, behind));

  neighbors1.push_back(NeighborDef(0, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(3, -1, 0, inFront));
  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3});

  street.insertCar(createCar(4, 0, 4));
  street.insertCar(createCar(5, 0, 7));
  street.insertCar(createCar(6, 0, 9));
  street.insertCar(createCar(7, 0, 9));
  street.insertCar(createCar(8, 0, 2));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors2;
  neighbors1.push_back(NeighborDef(0, 1, 0, behind));
  neighbors1.push_back(NeighborDef(1, 4, 0, behind));
  neighbors1.push_back(NeighborDef(2, -1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 5, 0, behind));
  neighbors1.push_back(NeighborDef(4, 8, 0, behind));
  neighbors1.push_back(NeighborDef(5, 0, 0, behind));
  neighbors1.push_back(NeighborDef(6, 7, 0, behind));
  neighbors1.push_back(NeighborDef(7, 3, 0, behind));
  neighbors1.push_back(NeighborDef(8, 2, 0, behind));

  neighbors1.push_back(NeighborDef(0, 5, 0, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 8, 0, inFront));
  neighbors1.push_back(NeighborDef(3, 7, 0, inFront));
  neighbors1.push_back(NeighborDef(4, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 3, 0, inFront));
  neighbors1.push_back(NeighborDef(6, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(7, 6, 0, inFront));
  neighbors1.push_back(NeighborDef(8, 4, 0, inFront));
  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8});
}

// 3-lane street, insert cars in two steps, some cars are inserted at the same position
template <template <typename Car> typename Street>
void insertCarTest8() {
  Street<LowLevelCar> street(3, 10);

  street.insertCar(createCar(0, 1, 7));
  street.insertCar(createCar(1, 1, 7));
  street.insertCar(createCar(2, 0, 3));
  street.insertCar(createCar(3, 2, 1));
  street.insertCar(createCar(4, 2, 5));
  street.insertCar(createCar(5, 1, 3));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors1;
  neighbors1.push_back(NeighborDef(0, 2, -1, behind));
  neighbors1.push_back(NeighborDef(0, 1, 0, behind));
  neighbors1.push_back(NeighborDef(0, 4, 1, behind));
  neighbors1.push_back(NeighborDef(0, -1, -1, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 1, inFront));

  neighbors1.push_back(NeighborDef(1, 2, -1, behind));
  neighbors1.push_back(NeighborDef(1, 5, 0, behind));
  neighbors1.push_back(NeighborDef(1, 4, 1, behind));
  neighbors1.push_back(NeighborDef(1, -1, -1, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(1, -1, 1, inFront));

  neighbors1.push_back(NeighborDef(2, -1, 0, behind));
  neighbors1.push_back(NeighborDef(2, 5, 1, behind));
  neighbors1.push_back(NeighborDef(2, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 1, 1, inFront));

  neighbors1.push_back(NeighborDef(3, -1, -1, behind));
  neighbors1.push_back(NeighborDef(3, -1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 5, -1, inFront));
  neighbors1.push_back(NeighborDef(3, 4, 0, inFront));

  neighbors1.push_back(NeighborDef(4, 5, -1, behind));
  neighbors1.push_back(NeighborDef(4, 3, 0, behind));
  neighbors1.push_back(NeighborDef(4, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(4, -1, 0, inFront));

  neighbors1.push_back(NeighborDef(5, -1, -1, behind));
  neighbors1.push_back(NeighborDef(5, -1, 0, behind));
  neighbors1.push_back(NeighborDef(5, 3, 1, behind));
  neighbors1.push_back(NeighborDef(5, 2, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 4, 1, inFront));

  checkNeighbors(street, neighbors1);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5});

  street.insertCar(createCar(6, 0, 3));
  street.insertCar(createCar(7, 0, 9));
  street.insertCar(createCar(8, 2, 7));
  street.insertCar(createCar(9, 2, 7));
  street.incorporateInsertedCars();

  std::vector<NeighborDef> neighbors2;
  neighbors1.push_back(NeighborDef(0, 2, -1, behind));
  neighbors1.push_back(NeighborDef(0, 1, 0, behind));
  neighbors1.push_back(NeighborDef(0, 8, 1, behind));
  neighbors1.push_back(NeighborDef(0, 7, -1, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(0, -1, 1, inFront));

  neighbors1.push_back(NeighborDef(1, 2, -1, behind));
  neighbors1.push_back(NeighborDef(1, 5, 0, behind));
  neighbors1.push_back(NeighborDef(1, 8, 1, behind));
  neighbors1.push_back(NeighborDef(1, 7, -1, inFront));
  neighbors1.push_back(NeighborDef(1, 0, 0, inFront));
  neighbors1.push_back(NeighborDef(1, -1, 1, inFront));

  neighbors1.push_back(NeighborDef(2, 6, 0, behind));
  neighbors1.push_back(NeighborDef(2, 5, 1, behind));
  neighbors1.push_back(NeighborDef(2, 7, 0, inFront));
  neighbors1.push_back(NeighborDef(2, 1, 1, inFront));

  neighbors1.push_back(NeighborDef(3, -1, -1, behind));
  neighbors1.push_back(NeighborDef(3, -1, 0, behind));
  neighbors1.push_back(NeighborDef(3, 5, -1, inFront));
  neighbors1.push_back(NeighborDef(3, 4, 0, inFront));

  neighbors1.push_back(NeighborDef(4, 5, -1, behind));
  neighbors1.push_back(NeighborDef(4, 3, 0, behind));
  neighbors1.push_back(NeighborDef(4, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(4, 9, 0, inFront));

  neighbors1.push_back(NeighborDef(5, 6, -1, behind));
  neighbors1.push_back(NeighborDef(5, -1, 0, behind));
  neighbors1.push_back(NeighborDef(5, 3, 1, behind));
  neighbors1.push_back(NeighborDef(5, 2, -1, inFront));
  neighbors1.push_back(NeighborDef(5, 1, 0, inFront));
  neighbors1.push_back(NeighborDef(5, 4, 1, inFront));

  neighbors1.push_back(NeighborDef(6, -1, 0, behind));
  neighbors1.push_back(NeighborDef(6, -1, 1, behind));
  neighbors1.push_back(NeighborDef(6, 2, 0, inFront));
  neighbors1.push_back(NeighborDef(6, 5, 1, inFront));

  neighbors1.push_back(NeighborDef(7, 2, 0, behind));
  neighbors1.push_back(NeighborDef(7, 0, 1, behind));
  neighbors1.push_back(NeighborDef(7, -1, 0, inFront));
  neighbors1.push_back(NeighborDef(7, -1, 1, inFront));

  neighbors1.push_back(NeighborDef(8, 5, -1, behind));
  neighbors1.push_back(NeighborDef(8, 9, 0, behind));
  neighbors1.push_back(NeighborDef(8, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(8, -1, 0, inFront));

  neighbors1.push_back(NeighborDef(9, 5, -1, behind));
  neighbors1.push_back(NeighborDef(9, 4, 0, behind));
  neighbors1.push_back(NeighborDef(9, 1, -1, inFront));
  neighbors1.push_back(NeighborDef(9, 8, 0, inFront));

  checkNeighbors(street, neighbors2);
  checkIterable(street.allIterable(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
}

#endif