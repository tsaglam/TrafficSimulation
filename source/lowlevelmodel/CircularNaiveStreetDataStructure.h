#ifndef CIRCULAR_NAIVE_STREET_DATA_STRUCTURE_H
#define CIRCULAR_NAIVE_STREET_DATA_STRUCTURE_H

#include <algorithm>

#include "CircularVector.h"
#include "RfbStructureTraits.h"
#include "utils.h"

template <class Vehicle>
class CircularNaiveStreetDataStructure {
public:
  using vector_type = CircularVector<Vehicle>;

  // ------- Iterator & Iterable type defs -------
  using iterator               = typename vector_type::iterator;
  using const_iterator         = typename vector_type::const_iterator;
  using reverse_iterator       = typename vector_type::reverse_iterator;
  using const_reverse_iterator = typename vector_type::const_reverse_iterator;

  using reverse_category = rfbstructure_reversible_sorted_iterator_tag;

private:
  friend class _AllCarIterable;
  friend class _BeyondsCarIterable;

  /**
   * The number of lanes on the street (in the current direction).
   */
  const unsigned int laneCount;
  /**
   * The length of the street.
   */
  const double length;

  // ------- Data Storage -------
  /**
   * All cars that are currently on this street.
   * Sorted by their distance (regardless of their lane).
   */
  vector_type street;

  /**
   * rBeyondsIndex points to the first car beyond the street when reverse-iterating.
   * The car is accessible via street.end() - rBeyondsIndex which might resolve to rbegin().
   */
  typename vector_type::difference_type rBeyondsIndex;

public:
  // ------- Constructor -------
  /**
   * @brief      Default constructor. Valid, but results in unspecified behavior.
   */
  CircularNaiveStreetDataStructure() = default;

  /**
   * @brief      Proper constructor, initializes a new instance with specified parameters.
   *
   * @param[in]  laneCount  The number of lanes on the street (in the current direction).
   * @param[in]  length     The length of the street.
   */
  CircularNaiveStreetDataStructure(unsigned int laneCount, double length) : laneCount(laneCount), length(length) {}

  template <bool Const = false>
  class _AllCarIterable {
    using IteratorType        = std::conditional_t<Const, const_iterator, iterator>;
    using ReverseIteratorType = std::conditional_t<Const, const_reverse_iterator, reverse_iterator>;
    using StreetReference =
        std::conditional_t<Const, CircularNaiveStreetDataStructure const &, CircularNaiveStreetDataStructure &>;
    StreetReference dataStructure;

  public:
    _AllCarIterable(StreetReference dataStructure) : dataStructure(dataStructure) {}

    IteratorType begin() const { return dataStructure.street.begin(); }
    IteratorType end() const { return dataStructure.street.end(); }
    IteratorType cbegin() const { return dataStructure.street.cbegin(); }
    IteratorType cend() const { return dataStructure.street.cend(); }

    ReverseIteratorType rbegin() const { return dataStructure.street.rbegin(); }
    ReverseIteratorType rend() const { return dataStructure.street.rend(); }
    ReverseIteratorType crbegin() const { return dataStructure.street.crbegin(); }
    ReverseIteratorType crend() const { return dataStructure.street.crend(); }
  };

  template <bool Const = false>
  class _BeyondsCarIterable {
    using IteratorType =
        std::conditional_t<Const, typename vector_type::const_reverse_iterator, typename vector_type::reverse_iterator>;
    using StreetReference =
        std::conditional_t<Const, CircularNaiveStreetDataStructure const &, CircularNaiveStreetDataStructure &>;
    StreetReference dataStructure;

  public:
    _BeyondsCarIterable(StreetReference dataStructure) : dataStructure(dataStructure) {}

    IteratorType begin() const { return dataStructure.street.rbegin(); }
    IteratorType end() const { return dataStructure.street.rbegin() + dataStructure.rBeyondsIndex; }
  };

  using AllCarIterable          = _AllCarIterable<>;
  using ConstAllCarIterable     = _AllCarIterable<true>;
  using BeyondsCarIterable      = _BeyondsCarIterable<>;
  using ConstBeyondsCarIterable = _BeyondsCarIterable<true>;

  // ------- Getter -------
  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   *
   * @return     The number of lanes.
   */
  unsigned int getLaneCount() const { return laneCount; }

  /**
   * @brief      Gets the length of the street.
   *
   * @return     The street length.
   */
  double getLength() const { return length; }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   * Cars beyond the street and cars that are inserted but not incorporated are not considered.
   *
   * @return     The number cars on this street.
   */
  unsigned int getCarCount() const { return street.size(); }

  // ------- Access to Neighboring Cars -------

  /**
   * @brief      Find the next car in front of the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset. All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car in front represented by an iterator.
   */
  iterator getNextCarInFront(const iterator currentCarIt, const int laneOffset = 0) {
    const unsigned int lane = currentCarIt->getLane() + laneOffset;
    // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
    for (iterator i = currentCarIt + 1; i != street.end(); ++i) {
      if (i->getLane() == lane) { return i; }
    }
    return street.end(); // return end iterator if no prev car exists on the given lane
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    const unsigned int lane = currentCarIt->getLane() + laneOffset;
    // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
    for (const_iterator i = currentCarIt + 1; i != street.end(); ++i) {
      if (i->getLane() == lane) { return i; }
    }
    return street.end(); // return end iterator if no prev car exists on the given lane
  }

  /**
   * @brief      Find the next car behind the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset. All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car behind the current car represented by an iterator.
   */
  iterator getNextCarBehind(const iterator currentCarIt, const int laneOffset = 0) {
    const unsigned int lane = currentCarIt->getLane() + laneOffset;
    // iterate all cars behind the current car (on all lanes) return first car on the specified lane
    for (iterator i = currentCarIt; i != street.begin(); --i) { // reverse iteration
      if ((i - 1)->getLane() == lane) { return (i - 1); }
    }
    return street.end(); // return end iterator if no next car exists on the given lane
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    const unsigned int lane = currentCarIt->getLane() + laneOffset;
    // iterate all cars behind the current car (on all lanes) return first car on the specified lane
    for (const_iterator i = currentCarIt; i != street.begin(); --i) { // reverse iteration
      if ((i - 1)->getLane() == lane) { return (i - 1); }
    }
    return street.end(); // return end iterator if no next car exists on the given lane
  }

  /**
   * @brief      Add a new car to the street using move semantics.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(Vehicle &&car) {
    street.push_front(car);
    street.front().update();
  }

  /**
   * @brief      Add a new car to the street using copy semantics.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(const Vehicle &car) {
    street.push_front(car);
    street.front().update();
  }

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   */
  void incorporateInsertedCars() {
    std::sort(street.begin(), street.end(), compareLess<Vehicle>); // restore car order (sorted by distance)
    rBeyondsIndex = 0;
  }

  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency.
   * All cars in street are updated by calling their update() function. The restore the consistency street
   * is sorted. Cars that reached the end of this street are removed from street and moved to departedCars.
   */
  void updateCarsAndRestoreConsistency() {
    for (auto &car : street) { car.update(); }
    std::sort(street.begin(), street.end(), compareLess<Vehicle>); // restore car order (sorted by distance)

    auto itRBegin = street.rbegin();
    auto itREnd   = street.rend();
    auto it       = itRBegin;
    for (; it != itREnd; ++it) {
      if (it->getDistance() < length) break;
    }
    // The first car properly on the street is at itREnd - 1 - it.
    rBeyondsIndex = it - itRBegin;
  }

  /**
   * @brief      Iterable for iterating over all cars.
   * Cars are iterated in order of increasing distance from the start of the street, car with equal distance are
   * ordered by their id. The lanes are not considered for the sorting.
   * Cars which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() and cars that left this street and are accessible by the beyondsIterable are not
   * considered by the allIterable in this implementation.
   *
   * @return     An iterable object for all cars on this street.
   */
  AllCarIterable allIterable() { return AllCarIterable(*this); }
  ConstAllCarIterable allIterable() const { return ConstAllCarIterable(*this); }
  ConstAllCarIterable constAllIterable() const { return ConstAllCarIterable(*this); }

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  BeyondsCarIterable beyondsIterable() { return BeyondsCarIterable(*this); }
  ConstBeyondsCarIterable beyondsIterable() const { return ConstBeyondsCarIterable(*this); }
  ConstBeyondsCarIterable constBeyondsIterable() const { return ConstBeyondsCarIterable(*this); }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * Cars are removed by clearing the vector containing them.
   */
  void removeBeyonds() {
    street.erase(street.cend() - rBeyondsIndex, street.cend());
    rBeyondsIndex = 0;
  }
};

#endif
