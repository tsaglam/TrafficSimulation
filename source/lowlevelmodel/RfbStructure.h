#ifdef RFB_STRUCTURE_INTERFACE_SPECIFICATION

/**
 * C++20 Vehicable concept, must be fulfilled in order for a class to be valid as the Vehicle template parameter for
 * RfbStructure.
 */
// template <typename T>
// concept Vehiclable = requires(T a, const T b) {
//   { b.getExternalId() }
//   ->unsigned int;
//   { b.getLane() }
//   ->unsigned int;
//   { b.getDistance() }
//   ->unsigned int;
//   { a.update() };
// };

/**
 * Low level data structure for storing vehicles on a one-directional street.
 * This type provides efficient access and modify operations.
 *
 * This is an interface specification only.
 *
 * The Vehicle template parameter must be a class implementing the following methods:
 *
 *     unsigned int getExternalId() const;
 *     unsigned int getLane() const;
 *     double getDistance() const;
 *     void update();
 *
 * Vehicles are modeled as points, there is no concept of length of a vehicle. The getPrevCar() and the getNextCar()
 * method only considers the distance of vehicles. This works in the expected way when all vehicles have the same
 * length.
 *
 * Some specification of the interface returns to "clean" and "un-clean" states. A clean "state" is reached when there
 * are no vehicles inserted but not yet incorporated and there are no cars beyond the street. In such a state, the
 * effect of all methods is specified. The effect of some methods may be unspecified in "un-clean" states.
 */
template <class Vehicle>
class RfbStructure {
public:
  /**
   * Type definition for iterators providing access to mutable, contained Vehicles retrieved via the "all" iterables
   * (AllCarIterable and AllConstCarIterable).
   */
  using iterator = typename std::vector<Vehicle>::iterator;
  /**
   * Type definition for iterators providing access to immutable, contained Vehicles retrieved via the "all" iterables
   * (AllCarIterable and AllConstCarIterable).
   */
  using const_iterator = typename std::vector<Vehicle>::const_iterator;

  /**
   * Type definition for iterators providing access to mutable, contained Vehicles retrieved via the
   * "beyonds" iterables (BeyondsCarIterable and BeyondsConstCarIterable).
   */
  using beyonds_iterator = typename std::vector<Vehicle>::iterator;
  /**
   * Type definition for iterators providing access to immutable, contained Vehicles retrieved via the
   * "beyonds" iterables (BeyondsCarIterable and BeyondsConstCarIterable).
   */
  using const_beyonds_iterator = typename std::vector<Vehicle>::const_iterator;

public:
  /**
   * Iterable type providing iterators for retrieving all vehicles on the street.
   *
   * In a "clean" state (see class comment), all vehicles tracked by the RfbStructure instance are iterated by the
   * iterators provided. In case the instance is in an "un-clean" state, the exact vehicles iterated are unspecified.
   * Vehicles beyond the end of the street and vehicles which have been added but not yet incorporated into the data
   * structure may be considered part of the iterable or not part of iterable.
   *
   * The number of vehicles retrievable through the iterable is always equal to the return value of the getCarCount()
   * method defined on RfbStructure.
   *
   * Usually, vehicles are iterated in order of increasing distance from the start of the street, but no specific order
   * is guaranteed.
   *
   * This is an interface specification only.
   *
   * May be used like this:
   *
   *     for (const auto& vehicle : rfb.allIterable()) { vehicle... }
   */
  class AllCarIterable {
  public:
    iterator begin() const;
    iterator end() const;
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    /* May be implemented like this: */

    // friend class RfbStructure;
    //
    // RfbStructure &rfb;
    //
    // AllCarIterable(RfbStructure &_rfb) : rfb(_rfb) {}
  };

  class ConstAllCarIterable {
  public:
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    /* May be implemented like this: */

    // friend class RfbStructure;
    //
    // const RfbStructure &rfb;
    //
    // ConstAllCarIterable(const RfbStructure &_rfb) : rfb(_rfb) {}
  };

  /**
   * Iterable type providing iterators for retrieving the vehicles "beyond the street" the street, i.e. vehicles with a
   * distance greater than the length of the street represented by this instance.
   *
   * This is an interface specification only.
   *
   * May be used like this:
   *
   *     for (const auto& vehicle : rfb.beyondsIterable()) { vehicle... }
   */
  class BeyondsCarIterable {
  public:
    beyonds_iterator begin() const;
    beyonds_iterator end() const;
    const_beyonds_iterator begin() const;
    const_beyonds_iterator end() const;

    const_beyonds_iterator cbegin() const;
    const_beyonds_iterator cend() const;

  private:
    /* May be implemented like this: */

    // friend class RfbStructure;
    //
    // RfbStructure &rfb;
    //
    // BeyondsCarIterable(RfbStructure &_rfb) rfb(_rfb) {}
  };

  class ConstBeyondsCarIterable {
  public:
    const_beyonds_iterator begin() const;
    const_beyonds_iterator end() const;

    const_beyonds_iterator cbegin() const;
    const_beyonds_iterator cend() const;

  private:
    /* May be implemented like this: */

    // friend class RfbStructure;
    //
    // const RfbStructure &rfb;
    //
    // ConstBeyondsCarIterable(const RfbStructure &_rfb) rfb(_rfb) {}
  };

protected:
  unsigned int lanes;
  double length;

public:
  /**
   * Returns the number of lanes of the street represented by the instance.
   */
  unsigned int getLaneCount() const;
  /**
   * Returns the length of the street represented by the instance.
   */
  double getLength() const;

  /**
   * Returns the number of vehicles tracked by the instance.
   *
   * In a "clean" state (see class comment), this method returns the number of vehicles tracked by the RfbStructure
   * instance. The return value is unspecified in case the instance is in an "un-clean" state.
   *
   * The value returned by this method always corresponds to the number of objects retrievable through allIterable().
   */
  unsigned int getCarCount() const;

  /**
   * Finds the next vehicle in front of the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * All vehicles are represented by iterators. Iterators of the "all" iterable are used, retrievable through
   * allIterable().
   *
   * If there is no vehicle in front of the vehicle passed in, the end() iterator is returned.
   */
  iterator getNextCarInFront(iterator originVehicleIt, const int laneOffset = 0);
  const_iterator getNextCarInFront(const_iterator originVehicleIt, const int laneOffset = 0) const;

  /**
   * Finds the next vehicle behind the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * All vehicles are represented by iterators. Iterators of the "all" iterable are used, retrievable through
   * allIterable().
   *
   * If there is no vehicle in front of the vehicle passed in, the end() iterator is returned.
   */
  iterator getNextCarBehind(iterator originVehicleIt, const int laneOffset = 0);
  const_iterator getNextCarBehind(const_iterator originVehicleIt, const int laneOffset = 0) const;

  /**
   * Add a new vehicle to the street using copy semantics.
   * The vehicle is inserted into the underlying data structure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(const Vehicle &vehicle);
  /**
   * Add a new vehicle to the street using move semantics.
   * The vehicle is inserted into the underlying data structure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(Vehicle &&vehicle);

  /**
   * Incorporates vehicles newly added to the street via insertCar() into the representation and ensures consistency of
   * the data structure.
   *
   * Before restoring consistency, update() will be called on the newly inserted vehicles.
   */
  void incorporateInsertedCars();

  /**
   * Applies updates on all vehicles and ensures continued consistency of the data structure.
   *
   * Updates are applied by calling update() on each vehicle. update() should never be called directly on vehicles.
   */
  void updateCarsAndRestoreConsistency();

  /**
   * Returns an iterable for iterating over all vehicles.
   */
  CarIterable allIterable();

  ConstCarIterable allIterable() const;

  ConstCarIterable constAllIterable() const;

  /**
   * Returns an iterable for iterating over vehicles which are currently "beyond the street", i.e. vehicles with a
   * distance greater than the length of the street represented by this instance.
   */
  BeyondsCarIterable beyondsIterable();

  ConstBeyondsCarIterable beyondsIterable() const;

  ConstBeyondsCarIterable constBeyondsIterable() const;

  /**
   * Removes all vehicles which are currently "beyond the street", i.e. vehicles with a distance greater than the length
   * of the street represented by this instance.
   */
  void removeBeyonds();

  /**
   * Default constructor. Valid, however any operations on the instance result in unspecified behaviour.
   */
  RfbStructure();
  /**
   * Proper constructor, initialises a new instance with the specified parameters.
   */
  RfbStructure(unsigned int lanes, double length);
};

#endif
