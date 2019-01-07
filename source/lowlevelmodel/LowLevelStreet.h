#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

/**
 * @brief      Class for the low level street, handles access to the actual underlying data structure storing the cars.
 *
 * @tparam     Car                  class of a LowLevelCar stored in the underlying StreetDataStructure
 * @tparam     UnderlyingStreetDataStructure  underlying data structure storing the cars on the street
 */
template <typename Car, template <typename> class UnderlyingStreetDataStructure>
class LowLevelStreet {
  using StreetDataStructure = UnderlyingStreetDataStructure<Car>;

private:
  /**
   * Internal street id.
   * Corresponds to the id of the corresponding street in the domain model.
   */
  const unsigned int id;

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
  LowLevelStreet(
      unsigned int streetId, unsigned int laneCount, double streetLength, double speedLimit, double trafficLightOffset)
      : id(streetId), speedLimit(speedLimit), streetData(laneCount, streetLength) {}

  // ------- Getter -------

  /**
   * @brief      Gets the internal street id.
   *
   * @return     The internal street.
   */
  inline unsigned int getId() const { return id; }

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
  inline unsigned int getLaneCount() const { return streetData.getLaneCount(); }

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
  inline unsigned int getNumCars() const { return streetData.getNumCars(); }

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
  inline CarIterator getNextCarInFront(CarIterator currentCarIt, const int laneOffset = 0) {
    return streetData.getNextCarInFront(currentCarIt, laneOffset);
  }
  inline ConstCarIterator getNextCarInFront(ConstCarIterator currentCarIt, const int laneOffset = 0) const {
    return streetData.getNextCarInFront(currentCarIt, laneOffset);
  }

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
  inline CarIterator getNextCarBehind(CarIterator currentCarIt, const int laneOffset = 0) {
    return streetData.getNextCarBehind(currentCarIt, laneOffset);
  }
  inline ConstCarIterator getNextCarBehind(ConstCarIterator currentCarIt, const int laneOffset = 0) const {
    return streetData.getNextCarBehind(currentCarIt, laneOffset);
  }

  /**
   * @brief      Add a new car to the street using move semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(Car &&car) { return streetData.insertCar(car); }

  /**
   * @brief      Add a new car to the street using copy semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(const Car &car) { return streetData.insertCar(car); }

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   * Incorporates all cars that were added to the street via insertCar() after the last call to incorporateInsertedCars.
   * The consistency of the data structure after the function call is ensured. Calls update() on all incorporated
   * cars.
   */
  inline void incorporateInsertedCars() { return streetData.incorporateInsertedCars(); }

  //

  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency. Cars are moved to the correct new position in the underlying data structure. Updates are applied to
   * the cars by calling update() on each car. Cars that reached the end of this street are collected internally
   * and can accessed via the getDepartedCars function.
   */
  inline void updateCarsAndRestoreConsistency() { return streetData.updateCarsAndRestoreConsistency(); }

  /**
   * @brief      Iterable for iterating over all cars.
   * Usually, cars are iterated in order of increasing distance from the start of the street, but no specific order is
   * guaranteed.
   * Cars which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() may or may not be considered by the iterable.
   *
   * @return     An iterable object for all cars on this street.
   */
  inline AllCarIterable allIterable() { return streetData.allIterable(); }
  inline ConstAllCarIterable allIterable() const { return streetData.allIterable(); }
  inline ConstAllCarIterable constAllIterable() const { return streetData.constAllIterable(); }

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   *Cars are beyond the street if their distance is greater than the length of the street.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  inline BeyondsCarIterable beyondsIterable() { return streetData.beyondsCarIterable(); }
  inline ConstBeyondsCarIterable beyondsIterable() const { return streetData.bonstBeyondsCarIterable(); }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return streetData.constBeyondsCarIterable(); }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   */
  inline void removeBeyonds() { return streetData.removeBeyonds(); }
};

#endif