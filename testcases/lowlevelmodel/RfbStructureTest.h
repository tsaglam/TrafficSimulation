#ifndef RFB_STRUCTURE_TEST_H
#define RFB_STRUCTURE_TEST_H

#include <../../snowhouse/snowhouse.h>
#include "LowLevelCar.h"

#include "RfbStructureTestUtils.h"
#include "InsertCarTest.h"
#include "GetNextCarTest.h"
#include "AllIterableTest.h"
#include "ConsistencyTest.h"

/*
 * Create a street with the constructor setting laneCount and length.
 * Check the laneCount and length via the getters.
 */
template <template <class Car> class Street>
void constructorAndConstMembersTest() {
  const unsigned int laneCount = 3;
  const double length          = 150;
  const Street<LowLevelCar> street(laneCount, length);
  AssertThat(street.getLaneCount(), Is().EqualTo(laneCount));
  AssertThat(street.getLength(), Is().EqualTo(length));
}

/*
 * Test if the iterator returned by the getNextCarInFront/Behind functions works as intended.
 * (i.e. ?)
 * - check neighbors on both the own lane an neighboring lanes
 * - try different special cases such as no existing neighbors, a neighbor at the same position etc.
 */

template <template <typename Car> typename Street>
void getNextCarIteratorTest1() {
  Street<LowLevelCar> street(3, 10);
  // Lane 0
  street.insertCar(createCar(0, 0, 0));
  street.insertCar(createCar(1, 0, 4));
  street.insertCar(createCar(2, 0, 7.5));
  street.insertCar(createCar(3, 0, 8));
  // Lane 1
  street.insertCar(createCar(4, 1, 0.5));
  street.insertCar(createCar(5, 1, 2.5));
  street.insertCar(createCar(6, 1, 4));
  street.insertCar(createCar(7, 1, 5.5));
  street.insertCar(createCar(8, 1, 7.5));
  // Lane 2
  for (unsigned i = 0; i < 10; ++i) { street.insertCar(createCar(i + 9, 2, i)); }
  street.incorporateInsertedCars();

  const unsigned carCount = street.getCarCount();
  AssertThat(carCount, Is().EqualTo((unsigned)19));
  const unsigned laneCount = 3;
  std::vector<std::vector<unsigned>> visitedCars(carCount);

  for (unsigned carId = 0; carId < carCount; ++carId) {
    auto carIt = street.allIterable().begin();
    while (carIt->getId() != carId) { ++carIt; }
    visitedCars[carId].push_back(carId);
    // Neighbors on own and other lanes
    std::vector<typename Street<LowLevelCar>::const_iterator> inFront(laneCount);
    std::vector<typename Street<LowLevelCar>::const_iterator> behind(laneCount);
    inFront[0] = street.getNextCarInFront(carIt, 0 - carIt->getLane());
    behind[0]  = street.getNextCarBehind(carIt, 0 - carIt->getLane());
    inFront[1] = street.getNextCarInFront(carIt, 1 - carIt->getLane());
    behind[1]  = street.getNextCarBehind(carIt, 1 - carIt->getLane());
    inFront[2] = street.getNextCarInFront(carIt, 2 - carIt->getLane());
    behind[2]  = street.getNextCarBehind(carIt, 2 - carIt->getLane());

    for (unsigned lane = 0; lane < laneCount; ++lane) {
      if (behind[lane] == street.allIterable().end() || inFront[lane] == street.allIterable().end()) { continue; }
      auto inFrontOfBehind = street.getNextCarInFront(behind[lane]);
      auto behindInFront   = street.getNextCarBehind(inFront[lane]);
      if (inFrontOfBehind == street.allIterable().end() || behindInFront == street.allIterable().end()) { continue; }
      if (lane == carIt->getLane()) {
        AssertThat(inFrontOfBehind->getId(), Is().EqualTo(carIt->getId()));
        AssertThat(behindInFront->getId(), Is().EqualTo(carIt->getId()));
      } else {
        AssertThat(inFrontOfBehind->getId(), Is().EqualTo(inFront[lane]->getId()));
        AssertThat(behindInFront->getId(), Is().EqualTo(behind[lane]->getId()));
      }
    }

    // Iterate from neighbors to front of street and mark cars as visited
    for (unsigned lane = 0; lane < laneCount; ++lane) {
      while (inFront[lane] != street.allIterable().end()) {
        visitedCars[carId].push_back(inFront[lane]->getId());
        ++inFront[lane];
      }
    }

    // Iterate from neighbors to back of street and mark cars as visited
    for (unsigned lane = 0; lane < laneCount; ++lane) {
      while (behind[lane] != street.allIterable().end()) {
        visitedCars[carId].push_back(behind[lane]->getId());
        if (behind[lane] == street.allIterable().begin()) { break; }
        --behind[lane];
      }
    }

    checkIterable(street.allIterable(), visitedCars[carId]);
  }
}

#endif