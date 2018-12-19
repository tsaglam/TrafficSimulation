#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

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

  // Add a new car to the street using move semantics. The car is inserted into the underlying
  // StreetDataStructure while retaining its consistency.
  void insertCar(Car &&car);

  // Remove and return a car from the street at the current iterator position while retaining
  // the consistency of the underlying StreetDataStructure.
  Car removeCar(iterator carIt);
  Car removeCar(const_iterator carIt);

  // Update a cars position on the street while retaining the consistency of the underlying
  // StreetDataStructure. TODO: how to move car to new street if the end of this street is
  // reached (i.e. how does this street know which is the next street?)
  void updateCarPosition(iterator carIt);
  void updateCarPosition(const_iterator carIt);

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