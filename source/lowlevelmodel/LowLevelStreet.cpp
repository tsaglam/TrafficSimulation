#include "LowLevelStreet.h"

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getPrevCar(
    LowLevelStreet<Car, StreetDataStructure>::iterator currentCarIt, const int laneOffset) {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._getPrevCar(currentCarIt, laneOffset);
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getPrevCar(
    LowLevelStreet<Car, StreetDataStructure>::const_iterator currentCarIt, const int laneOffset) {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._getPrevCar(currentCarIt, laneOffset);
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getNextCar(
    LowLevelStreet<Car, StreetDataStructure>::iterator currentCarIt, const int laneOffset) {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._getNextCar(currentCarIt, laneOffset);
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getNextCar(
    LowLevelStreet<Car, StreetDataStructure>::const_iterator currentCarIt, const int laneOffset) {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._getNextCar(currentCarIt, laneOffset);
}

template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::insertCar(Car &&car) {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._insertCar(car);
}

template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::incorporateAddedCars() {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._incorporateAddedCars();
}

template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::applyUpdates() {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._applyUpdates();
}

template <class Car, class StreetDataStructure>
std::vector<Car>& LowLevelStreet<Car, StreetDataStructure>::getDepartedCars() {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._getDepartedCars();
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::begin() {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._begin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::end() {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._end();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator
LowLevelStreet<Car, StreetDataStructure>::begin() const {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._begin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator
LowLevelStreet<Car, StreetDataStructure>::end() const {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._end();
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator
LowLevelStreet<Car, StreetDataStructure>::cbegin() const {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._cbegin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator
LowLevelStreet<Car, StreetDataStructure>::cend() const {
  StreetDataStructure &derived = static_cast<StreetDataStructure &>(*this);
  return derived._cend();
}