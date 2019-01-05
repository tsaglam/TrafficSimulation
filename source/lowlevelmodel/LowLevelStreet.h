#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

#include <vector>

/**
 * @brief      Class for the low level street, handles access to the actual underlying data structure storing the cars.
 *
 * @tparam     Car                  class of a LowLevelCar stored in the underlying StreetDataStructure
 * @tparam     StreetDataStructure  underlying data structure storing the cars on the street
 */
template <class Car, class UnderlyingStreetDataStructure>
class LowLevelStreet {
  using StreetDataStructure = typename UnderlyingStreetDataStructure<Car>;

private:
  /**
   * Internal street id.
   * Corresponds to the id of the corresponding street in the domain model.
   */
  const size_t id;

  /**
   * Speed limit of the represented street.
   */
  const double speedLimit;

  /**
   * Signaler used to switch the traffic light at the end of this street.
   * TODO: does the lowlevelstreet really need a signaler or should it rather have a bool denoting the current traffic
   * light status which is switched externally?
   */
  // TrafficLightSignaler<StreetDataStructure> signaler;

public:
  // use iterator types provided by the underlying StreetDataStructure
  using iterator       = typename StreetDataStructure::iterator;
  using const_iterator = typename StreetDataStructure::const_iterator;
  // ------- Constructor -------

  /**
   * @brief      Default constructor. Valid, but results in unspecified behavior.
   */
  LowLevelStreet() = default;
  /**
   * @brief      Proper constructor, initializes a new instance with specified parameters.
   * Constructs the low level street and initializes the underlying data storage. (TODO: and the traffic
   * light signaler?)
   * TODO: init StreetDataStructure
   * TODO: init signaler if used
   *
   * @param[in]  streetId            The internal street id
   * @param[in]  laneCount           The number of lanes on the street in the direction represented by this instance.
   * @param[in]  streetLength        The street length
   * @param[in]  speedLimit          The speed limit on this street.
   * @param[in]  trafficLightOffset  The position of the traffic light as distance from the end of the street.
   */
  LowLevelStreet(size_t streetId, size_t laneCount, double streetLength, double speedLimit, double trafficLightOffset)
      : id(streetId), speedLimit(speedLimit) {}

  // ------- Getter -------

  /**
   * @brief      Gets the internal street id.
   *
   * @return     The internal street.
   */
  inline size_t getId() const { return id; }

  /**
   * @brief      Gets the speed limit of this street.
   *
   * @return     The speed limit.
   */
  inline double getSpeedLimit() const { return speedLimit; }

  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   * Is forwarded to the underlying street data structure.
   *
   * @return     The number of lanes.
   */
  inline size_t getLaneCount() const { return streetData.getLaneCount(); }

  /**
   * @brief      Gets the length of the street.
   * Is forwarded to the underlying street data structure.
   *
   * @return     The street length.
   */
  inline double getLength() const { return streetData.getLength(); }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   * Is forwarded to the underlying street data structure.
   * TODO: what about cars that left the current street, what about those that were just inserted?
   *
   * @return     The number cars on this street.
   */
  inline size_t getNumCars() const { return streetData.getNumCars(); }

  // ------- Traffic Light Signaling -------

  /**
   * @brief      Determines if the traffic light at the end of the street is red.
   *
   * @return     True if signal red, False otherwise.
   */
  // bool isSignalRed() const { return signaler.isSignalRed(); }

  // // TODO: define this in TrafficLightSignaler?
  // enum Signal : bool {
  //   red = true,
  //   green = false,
  // };

  /**
   * @brief      Sets the signal of the traffic light at the end of the street to the given signal.
   *
   * @param[in]  signal  The traffic light signal, can be red or green.
   */
  // void setSignal(Signal signal) { signaler.setSignalRed(signal); }

  /**
   * @brief      Flip the signal of the traffic light at the end of the street from red to green or green to red.
   */
  // void switchSignal() { signaler.switchSignal(); }

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
  const_iterator getPrevCar(const_iterator currentCarIt, const int laneOffset = 0) const;

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
  const_iterator getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const;

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

};

#endif