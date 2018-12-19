#include "LowLevelStreet.h"

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getPrevCar(LowLevelStreet<Car, StreetDataStructure>::iterator currentCarIt, const int laneOffset) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._getPrevCar(currentCarIt, laneOffset);
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getPrevCar(LowLevelStreet<Car, StreetDataStructure>::const_iterator currentCarIt, const int laneOffset) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._getPrevCar(currentCarIt, laneOffset);
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getNextCar(LowLevelStreet<Car, StreetDataStructure>::iterator currentCarIt, const int laneOffset) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._getNextCar(currentCarIt, laneOffset);
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::getNextCar(LowLevelStreet<Car, StreetDataStructure>::const_iterator currentCarIt, const int laneOffset) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._getNextCar(currentCarIt, laneOffset);
}

template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::insertCar(Car &&car) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._insertCar(car);
}

template <class Car, class StreetDataStructure>
Car LowLevelStreet<Car, StreetDataStructure>::removeCar(LowLevelStreet<Car, StreetDataStructure>::iterator carIt) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._removeCar(carIt);
}
template <class Car, class StreetDataStructure>
Car LowLevelStreet<Car, StreetDataStructure>::removeCar(LowLevelStreet<Car, StreetDataStructure>::const_iterator carIt) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._removeCar(carIt);
}

template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::updateCarPosition(LowLevelStreet<Car, StreetDataStructure>::iterator carIt) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._updateCarPosition(carIt);
}
template <class Car, class StreetDataStructure>
void LowLevelStreet<Car, StreetDataStructure>::updateCarPosition(LowLevelStreet<Car, StreetDataStructure>::const_iterator carIt) {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._updateCarPosition(carIt);
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::begin() {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._begin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::iterator LowLevelStreet<Car, StreetDataStructure>::end() {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._end();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator LowLevelStreet<Car, StreetDataStructure>::begin() const {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._begin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator LowLevelStreet<Car, StreetDataStructure>::end() const {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._end();
}

template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator LowLevelStreet<Car, StreetDataStructure>::cbegin() const {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._cbegin();
}
template <class Car, class StreetDataStructure>
typename LowLevelStreet<Car, StreetDataStructure>::const_iterator LowLevelStreet<Car, StreetDataStructure>::cend() const {
  StreetDataStructure& derived = static_cast<StreetDataStructure&>(*this);
  return derived._cend();
}