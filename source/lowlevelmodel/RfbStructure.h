#ifdef RFB_STRUCTURE_INTERFACE_SPECIFICATION

/**
 * C++20 Vehicable concept, must be fulfilled in order for a class to be valid as the Vehicle template parameter for
 * RfbStructure.
 */
// template <typename T>
// concept Vehiclable = requires(T a, const T b) {
//   { b.getLane() }
//   ->unsigned int;
//   { b.getDistance() }
//   ->unsigned int;
//   {a.applyUpdates()};
// };

/**
 * Low level data structure for storing vehicles on a one-directional street.
 * This type provides efficient access and modify operations.
 *
 * This is an interface specification only.
 *
 * The Vehicle template parameter must be a class implementing the following methods:
 *
 *     unsigned int getLane() const;
 *     double getDistance() const;
 *     void applyUpdates();
 *
 * Vehicles are modeled as points, there is no concept of length of a vehicle. The getPrevCar() and the getNextCar()
 * method only considers the distance of vehicles. This works in the expected way when all vehicles have the same
 * length.
 */
template <class Vehicle>
class RfbStructure {
public:
  /**
   * Type definition for iterators providing access to mutable, contained Vehicles retrieved via the "all" iterables
   * (AllCarIterable and AllConstCarIterable).
   */
  typedef typename std::vector<Vehicle>::iterator iterator;
  /**
   * Type definition for iterators providing access to immutable, contained Vehicles retrieved via the "all" iterables
   * (AllCarIterable and AllConstCarIterable).
   */
  typedef typename std::vector<Vehicle>::const_iterator const_iterator;

  /**
   * Type definition for iterators providing access to mutable, contained Vehicles retrieved via the
   * "beyonds" iterables (BeyondsCarIterable and BeyondsConstCarIterable).
   */
  typedef typename std::vector<Vehicle>::iterator beyonds_iterator;
  /**
   * Type definition for iterators providing access to immutable, contained Vehicles retrieved via the
   * "beyonds" iterables (BeyondsCarIterable and BeyondsConstCarIterable).
   */
  typedef typename std::vector<Vehicle>::const_iterator const_beyonds_iterator;

public:
  /**
   * Utility type providing iterators for retrieving all vehicles on the street.
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
    // AllCarIterable(RfbStructure &_rfb) rfb(_rfb) {}
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
    // ConstAllCarIterable(const RfbStructure &_rfb) rfb(_rfb) {}
  };

  /**
   * Utility type providing iterators for retrieving the vehicles "beyond the street" the street, i.e. vehicles with a
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
  unsigned int getLanes() const;
  /**
   * Returns the length of the street represented by the instance.
   */
  double getLength() const;

  /**
   * Returns the number of vehicles tracked by the instance.
   * This includes vehicles on the street as well as beyond the street.
   */
  unsigned int getNumCars() const;

  /**
   * Finds the next vehicle in front of the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * All vehicles are represented by iterators.
   */
  iterator getPrevCar(iterator currentCarIt, const int laneOffset = 0);
  const_iterator getPrevCar(const_iterator currentCarIt, const int laneOffset = 0) const;

  /**
   * Finds the next vehicle behind the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * All vehicles are represented by iterators.
   */
  iterator getNextCar(iterator currentCarIt, const int laneOffset = 0);
  const_iterator getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const;

  /**
   * Add a new vehicle to the street using copy semantics.
   * The vehicle is inserted into the underlying data structure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(const Vehicle &car);
  /**
   * Add a new vehicle to the street using move semantics.
   * The vehicle is inserted into the underlying data structure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(Vehicle &&car);

  /**
   * Incorporates vehicles newly added to the street via insertCar() into the representation and ensures consistency of
   * the data structure.
   */
  void incorporateInsertedCars();

  /**
   * Applies updates on all vehicles and ensures continued consistency of the data structure.
   *
   * Updates are applied by calling applyUpdates() on each vehicle. applyUpdates() should never be called directly on
   * vehicles.
   */
  void applyUpdates();

  /**
   * Iterable for iterating over all vehicles.
   *
   * Usually, vehicles are iterated in order of increasing distance from the start of the street, but no specific order
   * is guaranteed.
   *
   * Vehicles which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() may or may not be considered by the iterable.
   */
  CarIterable allIterable();

  ConstCarIterable allIterable() const;

  ConstCarIterable constAllIterable() const;

  /**
   * Iterable for iterating over vehicles which are currently "beyond the street", i.e. vehicles with a distance greater
   * than the length of the street represented by this instance.
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
   * Proper constructor, initialises a new instance with specified parameters.
   */
  RfbStructure(unsigned int lanes, double length);
};

#endif
