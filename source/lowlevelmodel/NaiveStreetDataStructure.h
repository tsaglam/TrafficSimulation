#ifndef NAIVE_STREET_DATA_STRUCTURE_H
#define NAIVE_STREET_DATA_STRUCTURE_H

#include <vector>

template <class Car>
class NaiveStreetDataStructure {
private:
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
  std::vector<Car> carsOnStreet;
  /**
   * All cars that are inserted but not yet incorporated.
   * Sorted by their distance (regardless of their lane).
   */
  std::vector<Car> newCars;
  /**
   * All cars that left this street (i.e. their distance is greater than the street length).
   * Not necessarily sorted.
   */
  std::vector<Car> departedCars;

  /**
   * @brief      Compare two cars by their distance from the start of this street.
   *
   * @param[in]  a     The first car to compare.
   * @param[in]  b     The second car to compare.
   *
   * @return     True, if Car a is closer to street start than Car b.
   */
  static inline bool carComperator(const Car a, const Car b) { return a.getDistance() < b.getDistance(); }

public:
  // ------- Constructor -------
  /**
   * @brief      Default constructor. Valid, but results in unspecified behavior.
   */
  NaiveStreetDataStructure();

  /**
   * @brief      Proper constructor, initialises a new instance with specified parameters.
   *
   * @param[in]  laneCount  The number of lanes on the street (in the current direction).
   * @param[in]  length     The length of the street.
   */
  NaiveStreetDataStructure(unsigned int laneCount, double length);

  // ------- Iterator & Iterable type defs -------
  using CarIterator      = typename std::vector<Car>::iterator;
  using ConstCarIterator = typename std::vector<Car>::const_iterator;

  friend class _AllCarIterable;
  friend class _BeyondsCarIterable;

  template <class IteratorType>
  class _AllCarIterable {
    const IteratorType _begin;
    const IteratorType _end;

  public:
    _AllCarIterable(NaiveStreetDataStructure &dataStructure)
        : _begin(dataStructure.carsOnStreet.begin()), _end(dataStructure.carsOnStreet.end()) {}

    inline IteratorType begin() const { return _begin; }
    inline IteratorType end() const { return _end; }
  };

  template <class IteratorType>
  class _BeyondsCarIterable {
    const IteratorType _begin;
    const IteratorType _end;

  public:
    _BeyondsCarIterable(NaiveStreetDataStructure &dataStructure)
        : _begin(dataStructure.departedCars.begin()), _end(dataStructure.departedCars.end()) {}

    inline IteratorType begin() const { return _begin; }
    inline IteratorType end() const { return _end; }
  };

  using AllCarIterable          = _AllCarIterable<CarIterator>;
  using ConstAllCarIterable     = _AllCarIterable<ConstCarIterator>;
  using BeyondsCarIterable      = _BeyondsCarIterable<CarIterator>;
  using ConstBeyondsCarIterable = _BeyondsCarIterable<ConstCarIterator>;

  // ------- Getter -------
  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   *
   * @return     The number of lanes.
   */
  inline unsigned int getLaneCount() const { return laneCount; }

  /**
   * @brief      Gets the length of the street.
   *
   * @return     The street length.
   */
  inline double getLength() const { return length; }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   * Cars beyond the street and cars that are inserted but not incorporated are not considered.
   *
   * @return     The number cars on this street.
   */
  inline unsigned int getNumCars() const { return carsOnStreet.size(); }

  // ------- Access to Neighboring Cars -------
  // TODO: naming of prev and next might be confusing

  /**
   * @brief      Find the next car in front of the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset.
   * If the traffic light is red, the traffic light car might be returned instead of an actual car.
   * All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car in front represented by an iterator.
   */
  CarIterator getPrevCar(const CarIterator currentCarIt, const int laneOffset = 0);
  ConstCarIterator getPrevCar(const ConstCarIterator currentCarIt, const int laneOffset = 0) const;

  /**
   * @brief      Find the next car behind the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset.
   * The return value is not affected by the traffic light.
   * All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car behind the current car represented by an iterator.
   */
  CarIterator getNextCar(const CarIterator currentCarIt, const int laneOffset = 0);
  ConstCarIterator getNextCar(const ConstCarIterator currentCarIt, const int laneOffset = 0) const;

  /**
   * @brief      Add a new car to the street using move semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(Car &&car) { newCars.push_back(car); }

  /**
   * @brief      Add a new car to the street using copy semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(const Car &car) { newCars.push_back(car); }

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   * Incorporates all cars that were added to the street via insertCar() after the last call to incorporateInsertedCars.
   * The consistency of the data structure after the function call is ensured. Calls applyUpdates() on all incorporated
   * cars.
   */
  void incorporateInsertedCars();

  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency. Cars are moved to the correct new position in the underlying data structure. Updates are applied to
   * the cars by calling applyUpdates() on each car. Cars that reached the end of this street are collected internally
   * and can accessed via the getDepartedCars function.
   */
  void applyUpdates();

  /**
   * @brief      Iterable for iterating over all cars.
   * Usually, cars are iterated in order of increasing distance from the start of the street, but no specific order is
   * guaranteed.
   * Cars which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() may or may not be considered by the iterable.
   *
   * @return     An iterable object for all cars on this street.
   */
  inline AllCarIterable allIterable() { return AllCarIterable(*this); }
  inline ConstAllCarIterable allIterable() const { return ConstAllCarIterable(*this); }
  inline ConstAllCarIterable constAllIterable() const { return ConstAllCarIterable(*this); }

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   *Cars are beyond the street if their distance is greater than the length of the street.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  inline BeyondsCarIterable beyondsIterable() { return BeyondsCarIterable(*this); }
  inline ConstBeyondsCarIterable beyondsIterable() const { return ConstBeyondsCarIterable(*this); }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return ConstBeyondsCarIterable(*this); }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * Cars are removed by clearing the vector containing them.
   */
  inline void removeBeyonds() { departedCars.clear(); }
};

#endif
