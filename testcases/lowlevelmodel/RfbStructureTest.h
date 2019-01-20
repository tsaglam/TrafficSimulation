#ifndef RFB_STRUCTURE_TEST_H
#define RFB_STRUCTURE_TEST_H

#include <../../snowhouse/snowhouse.h>
#include <cassert>
#include <map>

#include "../randomUtils.h"
#include "LowLevelCar.h"

using namespace snowhouse;

#define createCar(id, lane, distance) LowLevelCar(id, id, 0, 0, 0, 0, 0, 0, 0, lane, distance, 0)

template <class Iterable>
void checkIterable(Iterable iterable, const std::vector<unsigned> &shouldContain,
    const std::vector<unsigned> &mightContain     = std::vector<unsigned>(),
    const std::vector<unsigned> &shouldNotContain = std::vector<unsigned>(), const bool noDuplicates = true) {
  std::map<unsigned, bool> shouldContainMap;
  for (auto id : shouldContain) { shouldContainMap.insert(std::pair<unsigned, bool>(id, false)); }

  for (auto car : iterable) {
    unsigned id = car.getId();
    AssertThat(shouldNotContain, Is().Not().Containing(id)); // does not contain ids it should not contain
    // all contained ids are either necessary or at least allowed to be contained
    auto findShouldContain = shouldContainMap.find(id);
    assert(findShouldContain != shouldContainMap.end() ||
           std::find(mightContain.begin(), mightContain.end(), id) != mightContain.end());
    if (noDuplicates) { AssertThat(findShouldContain->second, Is().False()); }
    findShouldContain->second = true; // mark contained ids
  }

  for (auto const &kv : shouldContainMap) { AssertThat(kv.second, Is().True()); }
}

enum NeighborState { inFront = 1, behind = -1 };

struct NeighborDef {
  unsigned carId;
  unsigned neighborId;
  int laneOffset;
  NeighborState state;

  NeighborDef(unsigned carId, unsigned neighborId, int laneOffset, NeighborState state)
      : carId(carId), neighborId(neighborId), laneOffset(laneOffset), state(state) {}
};

template <template <class Car> class Street>
void assertNeighborDef(
    const Street<LowLevelCar> &street, typename Street<LowLevelCar>::const_iterator carIt, const NeighborDef def) {
  AssertThat(carIt->getId(), Is().EqualTo(def.carId));
  auto neighbor = (def.state == inFront) ? street.getNextCarInFront(carIt, def.laneOffset)
                                         : street.getNextCarBehind(carIt, def.laneOffset);
  if (def.neighborId == (unsigned)-1) {
    AssertThat(neighbor, Is().EqualTo(street.allIterable().end()));
  } else {
    AssertThat(neighbor, Is().Not().EqualTo(street.allIterable().end()));
    AssertThat(neighbor->getId(), Is().EqualTo(def.neighborId));
  }
}

template <template <class Car> class Street>
void checkNeighbors(const Street<LowLevelCar> &street, const std::vector<NeighborDef> &neighborDefinitionsVector) {
  // create map from vector using the carId as key
  std::map<unsigned, std::vector<NeighborDef>> neighborDefinitionsMap;
  for (const NeighborDef &def : neighborDefinitionsVector) {
    if (neighborDefinitionsMap.count(def.carId) == 0) {
      std::pair<unsigned, std::vector<NeighborDef>> newElement(def.carId, std::vector<NeighborDef>(1, def));
      neighborDefinitionsMap.insert(newElement);
    } else {
      neighborDefinitionsMap.find(def.carId)->second.push_back(def);
    }
  }

  // Check the neighbors for all cars
  for (auto carIt = street.allIterable().begin(); carIt != street.allIterable().end(); ++carIt) {
    if (neighborDefinitionsMap.count(carIt->getId()) == 0) { continue; }
    auto neighbors = neighborDefinitionsMap.find(carIt->getId())->second;
    for (auto it = neighbors.begin(); it != neighbors.end(); ++it) { assertNeighborDef(street, carIt, *it); }
  }
}

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
 * Ensure that the allIterable visits each car on the street exactly once and that the number of visited cars is equal
 * to getCarCount. Try different states of the street including special cases and in-between states.
 */
template <template <typename Car> typename Street>
void allIterableTest() {
  Street<LowLevelCar> street(1, 10);
  std::vector<unsigned> expectedCarIds(10);
  for (int i = 0; i < 10; ++i) {
    street.insertCar(createCar(i, 0, i));
    expectedCarIds[i] = i;
  }
  street.incorporateInsertedCars();
  checkIterable(street.allIterable(), expectedCarIds);
}

/*
 * Test if the getNextCarInFront/Behind functions return the correct car.
 * - check neighbors on both the own lane an neighboring lanes
 * - try different special cases such as no existing neighbors, a neighbor at the same position etc.
 */
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

/*
 * Add new cars to the street (with insertCar and incorporateInsertedCars). Test whether all cars are at the correct
 * position using the getNextCar functions.
 * Different test cases:
 * - street previously empty -> street previously containing many cars
 * - inserting no cars -> inserting many cars
 * - all new cars at the beginning of the street -> new cars alternating with old cars
 * - sorted/unsorted input of cars
 */
template <template <typename Car> typename Street>
void insertCarTest() {}

/*
 * Move cars by setting a new position, call updateCarsAndRestoreConsistency and test the consistency of the street
 * using the getNextCar functions.
 * Different test cases:
 * - no cars moved -> all cars moved
 * - all cars remain on street -> all cars left the street
 * - no cars on the street -> many cars on the street
 * - no change in the car order -> much change in the car order (per lane/per street)
 */
template <template <typename Car> typename Street>
void consistencyTest() {}

/*
 * Test if the remove beyonds iterator contains all cars stored in this street with distance >= streetLength
 * Test cases:
 * - no/all cars departed
 * - car exactly at distance == streetLength
 * - different cars on different lanes with different beyonds distances
 */
template <template <typename Car> typename Street>
void beyondsIterableTest() {}

/*
 * After calling removeBeyonds, the beyonds iterator is empty an all departed cars are removed from the street, i.e.
 * also not in the allIterable.
 */
template <template <typename Car> typename Street>
void removeBeyondsTest() {}

#endif