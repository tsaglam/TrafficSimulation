#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

#include <vector>

template <class Car, class StreetDataStructure>
/**
 * @brief      Class for the low level street, handles access to the actual underlying data structure storing the cars.
 *
 * @tparam     Car                  class of a LowLevelCar stored in the underlying StreetDataStructure
 * @tparam     StreetDataStructure  underlying data structure storing the cars on the street
 */
class LowLevelStreet {
public:
  // use iterator types provided by the underlying StreetDataStructure
  using iterator       = typename StreetDataStructure::iterator;
  using const_iterator = typename StreetDataStructure::const_iterator;

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
  iterator getPrevCar(iterator currentCarIt, const int laneOffset = 0);
  iterator getPrevCar(const_iterator currentCarIt, const int laneOffset = 0);

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
  iterator getNextCar(iterator currentCarIt, const int laneOffset = 0);
  iterator getNextCar(const_iterator currentCarIt, const int laneOffset = 0);

  /**
   * @brief      Add a new car to the street using move semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(Car &&car);

  void incorporateAddedCars();

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   * Incorporates all cars that were added to the street via insertCar() after the last call to incorporateInsertedCars.
   * The consistency of the data structure after the function call is ensured. Calls applyUpdates() on all incorporated
   * cars.
   */

  // Access the cars which left the current street (as determined by the applyUpdates function).
  std::vector<Car>& getDepartedCars();

  // Begin and end iterators to iterate over all cars on this street. Usually in order of
  // increasing distance from the start of the street, but no specific order is guaranteed.
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency. Cars are moved to the correct new position in the underlying data structure. Updates are applied to
   * the cars by calling applyUpdates() on each car. Cars that reached the end of this street are collected internally
   * and can accessed via the getDepartedCars function.
   */
  void applyUpdates();

  const_iterator cbegin() const;
  const_iterator cend() const;

private:
  LowLevelStreet(){};
  friend StreetDataStructure;
};

#endif