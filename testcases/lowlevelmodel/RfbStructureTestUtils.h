#ifndef RFB_STRUCTURE_TEST_UTILS_H
#define RFB_STRUCTURE_TEST_UTILS_H

#include "LowLevelCar.h"

#include <../../snowhouse/snowhouse.h>
#include <cassert>
#include <map>

using namespace snowhouse;

#define createCar(id, lane, distance) LowLevelCar(id, id, 0, 0, 0, 0, 0, 0, 0, lane, distance, 0)

template <class Iterable>
void checkIterable(Iterable iterable, const std::vector<unsigned> &shouldContain,
    const std::vector<unsigned> &mightContain     = std::vector<unsigned>(),
    const std::vector<unsigned> &shouldNotContain = std::vector<unsigned>(), const bool noDuplicates = true) {
  std::map<unsigned, bool> shouldContainMap, mightContainMap;
  for (auto id : shouldContain) { shouldContainMap.insert(std::pair<unsigned, bool>(id, false)); }
  for (auto id : mightContain) { mightContainMap.insert(std::pair<unsigned, bool>(id, false)); }

  for (auto car : iterable) {
    unsigned id = car.getId();
    AssertThat(shouldNotContain, Is().Not().Containing(id)); // does not contain ids it should not contain
    auto findShouldContain = shouldContainMap.find(id);
    auto findMightContain  = mightContainMap.find(id);
    // all contained ids are either necessary or at least allowed to be contained
    assert(findShouldContain != shouldContainMap.end() || findMightContain != mightContainMap.end());

    if (noDuplicates) { // check for duplicates if wanted
      // if the id is in shouldContain, ensure that is is not yet visited
      assert(findShouldContain == shouldContainMap.end() || findShouldContain->second == false);
      // if the id is in mightContain, ensure that is is not yet visited
      assert(findMightContain == mightContainMap.end() || findMightContain->second == false);
    }
    // mark id as visited
    if (findShouldContain != shouldContainMap.end()) { findShouldContain->second = true; }
    if (findMightContain != mightContainMap.end()) { findMightContain->second = true; }
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

#endif