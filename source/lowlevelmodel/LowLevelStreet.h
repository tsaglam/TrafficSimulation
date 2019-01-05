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
   * Underlying storage of the cars on the street in the data structure specified by the template parameter
   * StreetDataStructure.
   */
  StreetDataStructure streetData;

  /**
   * Signaler used to switch the traffic light at the end of this street.
   * TODO: does the lowlevelstreet really need a signaler or should it rather have a bool denoting the current traffic
   * light status which is switched externally?
   */
  // TrafficLightSignaler<StreetDataStructure> signaler;

public:
  // ------- Iterator & Iterable type defs -------
  using CarIterator      = typename StreetDataStructure::CarIterator;
  using ConstCarIterator = typename StreetDataStructure::ConstCarIterator;

  using AllCarIterable          = typename StreetDataStructure::AllCarIterable;
  using ConstAllCarIterable     = typename StreetDataStructure::ConstAllCarIterable;
  using BeyondsCarIterable      = typename StreetDataStructure::BeyondsCarIterable;
  using ConstBeyondsCarIterable = typename StreetDataStructure::ConstBeyondsCarIterable;

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
  CarIterator getPrevCar(CarIterator currentCarIt, const int laneOffset = 0);
  CarIterator getPrevCar(ConstCarIterator currentCarIt, const int laneOffset = 0);
  ConstCarIterator getPrevCar(ConstCarIterator currentCarIt, const int laneOffset = 0) const;

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
  CarIterator getNextCar(CarIterator currentCarIt, const int laneOffset = 0);
  CarIterator getNextCar(ConstCarIterator currentCarIt, const int laneOffset = 0);
  ConstCarIterator getNextCar(ConstCarIterator currentCarIt, const int laneOffset = 0) const;

  /**
   * @brief      Add a new car to the street using move semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(Car &&car);

  /**
   * @brief      Add a new car to the street using copy semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(const Car &car);

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   * Incorporates all cars that were added to the street via insertCar() after the last call to incorporateInsertedCars.
   * The consistency of the data structure after the function call is ensured. Calls applyUpdates() on all incorporated
   * cars.
   */
  void incorporateInsertedCars();

  //

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
  AllCarIterable allIterable();
  ConstAllCarIterable allIterable() const;
  ConstAllCarIterable constAllIterable() const;

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   *Cars are beyond the street if their distance is greater than the length of the street.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  BeyondsCarIterable beyondsIterable();
  ConstBeyondsCarIterable beyondsIterable() const;
  ConstBeyondsCarIterable constBeyondsIterable() const;

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   */
  void removeBeyonds();
};

#endif