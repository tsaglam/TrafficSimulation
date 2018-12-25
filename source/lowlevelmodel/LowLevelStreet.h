#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

#include <vector>

// Base class for all low level street implementations. Uses CRTP for inheritance without virtual functions.
template <class Car, class StreetDataStructure>
class LowLevelStreet {
public:
  // use iterator types provided by the underlying StreetDataStructure
  using iterator       = typename StreetDataStructure::iterator;
  using const_iterator = typename StreetDataStructure::const_iterator;

  // Find the next car in front of the current car on the current lane (if laneOffset == 0)
  // or the next lane to the left (-1) / right (+1) of the current lane.
  // All cars are represented by iterators.
  iterator getPrevCar(iterator currentCarIt, const int laneOffset = 0);
  iterator getPrevCar(const_iterator currentCarIt, const int laneOffset = 0);

  // Find the next car in behind the current car on the current lane (if laneOffset == 0)
  // or the next lane to the left (-1) / right (+1) of the current lane.
  // All cars are represented by iterators.
  iterator getNextCar(iterator currentCarIt, const int laneOffset = 0);
  iterator getNextCar(const_iterator currentCarIt, const int laneOffset = 0);

  // Add a new car to the street using move semantics. The inserted cars are initially collected
  // internally and only inserted into the underlying StreetDataStructure once the
  // incorporateAddedCars function is called.
  void insertCar(Car &&car);

  // Insert all cars into the underlying StreetDataStructure that were inserted after the last call
  // to incorporateAddedCars. The consistency of the data structure after the function call is ensured.
  void incorporateAddedCars();

  // Update the position of **all** cars on this street while retaining the consistency of the
  // underlying StreetDataStructure. Cars that reached the end of this street are collected
  // internally in a vector which can be accessed via the getDepartedCars function.
  void applyUpdates();

  // Access the cars which left the current street (as determined by the applyUpdates function).
  std::vector<Car>& getDepartedCars();

  // Begin and end iterators to iterate over all cars on this street. Usually in order of
  // increasing distance from the start of the street, but no specific order is guaranteed.
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

private:
  LowLevelStreet(){};
  friend StreetDataStructure;
};

#endif