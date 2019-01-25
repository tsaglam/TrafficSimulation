#ifndef ALL_ITERABLE_TEST_H
#define ALL_ITERABLE_TEST_H

#include "RfbStructureTestUtils.h"

/*
 * Ensure that the allIterable visits each car on the street exactly once and that the number of visited cars is equal
 * to getCarCount. Try different states of the street including special cases and in-between states.
 */

// Case 1: Empty street
template <template <typename Car> typename Street>
void allIterableTest1() {
  Street<LowLevelCar> street(3, 10);
  std::vector<unsigned> expectedCarIds;
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);
}
// Case 2: 1 lane street, multiple cars in succession
template <template <typename Car> typename Street>
void allIterableTest2() {
  Street<LowLevelCar> street(1, 10);
  std::vector<unsigned> expectedCarIds(10);
  for (int i = 0; i < 10; ++i) {
    street.insertCar(createCar(i, 0, i));
    expectedCarIds[i] = i;
  }
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);
}
// Case 3: 3 lane street, multiple cars in succession on each lane
template <template <typename Car> typename Street>
void allIterableTest3() {
  const unsigned carCount     = 30;
  const unsigned laneCount    = 3;
  const unsigned streetLength = 10;
  Street<LowLevelCar> street(laneCount, streetLength);
  std::vector<unsigned> expectedCarIds(carCount);
  for (unsigned i = 0; i < carCount; ++i) {
    street.insertCar(createCar(i, i % laneCount, i % streetLength));
    expectedCarIds[i] = i;
  }
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);
}
// Case 4: 3 lane street, multiple overlapping cars on each lane
template <template <typename Car> typename Street>
void allIterableTest4() {
  const unsigned carCount     = 30;
  const unsigned laneCount    = 3;
  const unsigned streetLength = 10;
  Street<LowLevelCar> street(laneCount, streetLength);
  std::vector<unsigned> expectedCarIds(carCount);
  for (unsigned i = 0; i < carCount; ++i) {
    street.insertCar(createCar(i, i % laneCount, laneCount));
    expectedCarIds[i] = i;
  }
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);
}
// Case 5: 3 lane street, interim state: some cars are inserted but not yet incorporated while others are fully
// incorporated
template <template <typename Car> typename Street>
void allIterableTest5() {
  const unsigned carCount     = 30;
  const unsigned laneCount    = 3;
  const unsigned streetLength = 10;
  Street<LowLevelCar> street(laneCount, streetLength);
  std::vector<unsigned> expectedCarIds, allowedCarIds;
  for (unsigned i = 0; i < carCount / 2; ++i) {
    street.insertCar(createCar(i, i % laneCount, laneCount));
    expectedCarIds.push_back(i);
  }
  street.incorporateInsertedCars();
  for (unsigned i = carCount / 2; i < carCount; ++i) {
    street.insertCar(createCar(i, i % laneCount, laneCount));
    allowedCarIds.push_back(i);
  }
  checkIterable(street.allIterable(), expectedCarIds, allowedCarIds);
}
// Case 6: 3 lane street, interim state: some cars are departed but not yet removed
// incorporated
template <template <typename Car> typename Street>
void allIterableTest6() {
  Street<LowLevelCar> street(1, 10);
  std::vector<unsigned> expectedCarIds, allowedCarIds;
  expectedCarIds = {0, 1, 2, 3, 4};

  for (int i = 0; i < 5; ++i) { street.insertCar(createCar(i, 0, 2 * i)); }
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);

  expectedCarIds = {0, 1, 2};
  allowedCarIds  = {3, 4};
  for (LowLevelCar &car : street.allIterable()) { car.setNext(0, car.getDistance() * 2, 0); }
  street.updateCarsAndRestoreConsistency();
  checkIterable(street.allIterable(), expectedCarIds, allowedCarIds);
}

#endif