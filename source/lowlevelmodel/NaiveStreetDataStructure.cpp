#include "NaiveStreetDataStructure.h"

template <class Car>
NaiveStreetDataStructure<Car>::NaiveStreetDataStructure() = default;

template <class Car>
NaiveStreetDataStructure<Car>::NaiveStreetDataStructure(unsigned int laneCount, double length)
    : laneCount(laneCount), length(length) {}

template <class Car>
typename NaiveStreetDataStructure<Car>::CarIterator NaiveStreetDataStructure<Car>::getNextCarInFront(
    const NaiveStreetDataStructure<Car>::CarIterator currentCarIt, const int laneOffset) {
  const unsigned int lane = currentCarIt->getLane() + laneOffset;
  // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
  for (CarIterator i = currentCarIt; i != carsOnStreet.end(); ++i) {
    if (i->getLane() == lane) { return i; }
  }
  return carsOnStreet.end(); // return end iterator if no prev car exists on the given lane
}

template <class Car>
typename NaiveStreetDataStructure<Car>::ConstCarIterator NaiveStreetDataStructure<Car>::getNextCarInFront(
    const NaiveStreetDataStructure<Car>::ConstCarIterator currentCarIt, const int laneOffset) const {
  const unsigned int lane = currentCarIt->getLane() + laneOffset;
  // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
  for (ConstCarIterator i = currentCarIt; i != carsOnStreet.end(); ++i) {
    if (i->getLane() == lane) { return i; }
  }
  return carsOnStreet.end(); // return end iterator if no prev car exists on the given lane
}

template <class Car>
typename NaiveStreetDataStructure<Car>::CarIterator NaiveStreetDataStructure<Car>::getNextCarBehind(
    const NaiveStreetDataStructure<Car>::CarIterator currentCarIt, const int laneOffset) {
  const unsigned int lane = currentCarIt->getLane() + laneOffset;
  // iterate all cars behind the current car (on all lanes) return first car on the specified lane
  for (CarIterator i = currentCarIt; i != carsOnStreet.begin(); --i) { // reverse iteration
    if (i->getLane() == lane) { return i; }
  }
  return carsOnStreet.end(); // return end iterator if no next car exists on the given lane
}

template <class Car>
typename NaiveStreetDataStructure<Car>::ConstCarIterator NaiveStreetDataStructure<Car>::getNextCarBehind(
    const NaiveStreetDataStructure<Car>::ConstCarIterator currentCarIt, const int laneOffset) const {
  const unsigned int lane = currentCarIt->getLane() + laneOffset;
  // iterate all cars behind the current car (on all lanes) return first car on the specified lane
  for (ConstCarIterator i = currentCarIt; i != carsOnStreet.begin(); --i) { // reverse iteration
    if (i->getLane() == lane) { return i; }
  }
  return carsOnStreet.end(); // return end iterator if no next car exists on the given lane
}

template <class Car>
void NaiveStreetDataStructure<Car>::incorporateInsertedCars() {
  for (auto newCar : newCars) { newCar.update(); }                         // update all new cars
  carsOnStreet.insert(carsOnStreet.end(), newCars.begin(), newCars.end()); // append all new cars to carsOnStreet
  sort(carsOnStreet.begin(), carsOnStreet.end(), carComperator);           // restore car order (sorted by distance)
}

template <class Car>
void NaiveStreetDataStructure<Car>::updateCarsAndRestoreConsistency() {
  for (auto car : carsOnStreet) { car.update(); }                // update all cars
  sort(carsOnStreet.begin(), carsOnStreet.end(), carComperator); // restore car order (sorted by distance)
}