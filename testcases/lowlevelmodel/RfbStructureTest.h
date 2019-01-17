#ifndef RFB_STRUCTURE_TEST_H
#define RFB_STRUCTURE_TEST_H

#include <../../snowhouse/snowhouse.h>

#include "LowLevelCar.h"

using namespace snowhouse;
/*
 * Create a street with the constructor setting laneCount and length.
 * Check the laneCount and length via the getters.
 */
template <template <class Car> class Street>
void constructorAndConstMembersTest() {}

/*
 * Ensure that the allIterable visits each car on the street exactly once and that the number of visited cars is equal
 * to getCarCount. Try different states of the street including special cases and inbetween states.
 */
template <template <typename Car> typename Street>
void allIterableTest() {}

/*
 * Test if the getNextCarInFront/Behind functions return the correct car.
 * - check neighbors on both the own lane an neighboring lanes
 * - try different special cases such as no existing neighbors, a neighbor at the same position etc.
 */
template <template <typename Car> typename Street>
void getNextCarTest() {}

/*
 * Test if the iterator returned by the getNextCarInFront/Behind functions works as intendet.
 * (i.e. ?)
 * - check neighbors on both the own lane an neighboring lanes
 * - try different special cases such as no existing neighbors, a neighbor at the same position etc.
 */
template <template <typename Car> typename Street>
void getNextCarIteratorTest() {}

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