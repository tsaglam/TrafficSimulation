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
 * The Vehicle template parameter must be a class implementing the following methods:
 *
 *     unsigned int getLane() const;
 *     double getDistance() const;
 *     void applyUpdates();
 */
template <class Vehicle>
class RfbStructure {
public:
  /**
   * Type definition for iterators providing access to mutable, contained Vehicles.
   */
  typedef typename std::vector<Vehicle>::iterator iterator;
  /**
   * Type definition for iterators providing access to immutable, contained Vehicles.
   */
  typedef typename std::vector<Vehicle>::const_iterator const_iterator;

public:
  /*
   * Sensible Iterable types are hard to design.
   *
   * This should probably either be a virtual class or simply be implemented by a completely independent sub type of
   * DevisedRfbStructure.
   *
   * Alternatively, AllCarIterable and BeyondsCarIterable could be distinguished and be implemented here by calling
   * private methods of devised (allBegin, allEnd, allCbegin, allCend, beyondsBegin, beyondsEnd, beyondsCbegin,
   * beyondsCend).
   *
   * By using type erasure, the iterables could be exposed and stored as a single type at the cost of an additional
   * pointer.
   */

  /**
   * Utility type for iterating over Cars.
   *
   * Interface only class? Seems to be complicated to use this specific class for all implementations.
   *
   * May be used like this:
   *
   *     for (const auto& car : rfb.someIterable()) { car... }
   */
  class CarIterable {
  public:
    iterator begin() const;
    iterator end() const;
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    friend class DevisedRfbStructure;

    // &DevisedRfbStructure devised;
    //
    // CarIterable(&DevisedRfbStructure _devised) devised(_devised) {}
  };

  class ConstCarIterable {
  public:
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    friend class DevisedRfbStructure;

    // &DevisedRfbStructure devised;
    //
    // CarIterable(&DevisedRfbStructure _devised) devised(_devised) {}
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
   * Returns the number of cars on the street represented by the instance.
   */
  unsigned int getNumCars() const;

  /**
   * Finds the next car in front of the current car.
   * The method returns the next car on the current lane (if laneOffset == 0) or the next lane to the left (-1) / right
   * (+1) of the current lane.
   *
   * All cars are represented by iterators.
   */
  iterator getPrevCar(iterator currentCarIt, const int laneOffset = 0);
  const_iterator getPrevCar(const_iterator currentCarIt, const int laneOffset = 0) const;

  /**
   * Finds the next car behind the current car.
   * The method returns the next car on the current lane (if laneOffset == 0) or the next lane to the left (-1) / right
   * (+1) of the current lane.
   *
   * All cars are represented by iterators.
   */
  iterator getNextCar(iterator currentCarIt, const int laneOffset = 0);
  const_iterator getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const;

  /**
   * Add a new car to the street using copy semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(const Vehicle &car);
  /**
   * Add a new car to the street using move semantics.
   * The car is inserted into the underlying DevisedRfbStructure, however, the data structure may be inconsistent until
   * incorporateInsertedCars() has been called.
   */
  void insertCar(Vehicle &&car);

  /**
   * Incorporates cars newly added to the street via insertCar() into the representation and ensures consistency of the
   * data structure.
   *
   * Does this method call applyUpdates() on the cars? This would result in a a more consistent API, all writers only
   * need to update the next* fields of cars. There seems to be no particular advantages for either alternative.
   */
  void incorporateInsertedCars();

  /**
   * Applies updates on all cars and ensures continued consistency of the data structure.
   *
   * Updates are applied by calling applyUpdates() on each car. applyUpdates() should never be called directly on cars.
   */
  void applyUpdates();

  /**
   * Iterable for iterating over all cars.
   *
   * Usually, cars are iterated in order of increasing distance from the start of the street, but no specific order is
   * guaranteed.
   *
   * Cars which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() may or may not be considered by the iterable.
   */
  CarIterable allIterable();

  ConstCarIterable allIterable() const;

  ConstCarIterable constAllIterable() const;

  /**
   * Iterable for iterating over cars which are currently "beyond the street", i.e. cars with a distance greater than
   * the length of the street represented by this instance.
   */
  CarIterable beyondsIterable();

  ConstCarIterable beyondsIterable() const;

  ConstCarIterable constBeyondsIterable() const;

  /**
   * Removes all cars which are currently "beyond the street", i.e. cars with a distance greater than the length of the
   * street represented by this instance.
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
