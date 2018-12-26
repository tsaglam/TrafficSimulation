#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

#include "TrafficLightSignaler.h"

/**
 * Class for the low level street, wraps the actual underlying data structure storing cars as well as a
 * TrafficLightSignaler.
 *
 * @tparam RfbStructure  Underlying data structure storing the cars on the street
 */
template <template <typename Vehicle> RfbStructure>
class LowLevelStreet {
private:
  using ConcreteRfbStructure = typename RfbStructure<LowLevelCar>;

private:
  /**
   * Internal id of the street.
   * This corresponds to the id of the corresponding street in the domain model.
   */
  unsigned int id;

private:
  /*
   * All this information is required for the position computation, i.e. needs to be copied to the GPU.
   */

  /**
   * Speed limit of the represented street.
   */
  double speedLimit;
  /**
   * Signaler used to switch the traffic light at the end of this street.
   * The getNextCarInFront() / getNextCarBehind() functions are forwarded to this signaler to allow returning the
   * traffic light car if the signal is red. All functions accessing the all iterable are forwarded as well, the
   * signaler performs iterator wrapping.
   */
  TrafficLightSignaler<RfbStructure> signaler;
  /**
   * Underlying storage of the cars on the street in the data structure specified by the template parameter
   * RfbStructure.
   */
  ConcreteRfbStructure rfb;

public:
  /**
   * Default constructor. Valid, but results in unspecified behavior.
   */
  LowLevelStreet() = default;
  /**
   * Proper constructor, initializes a new instance with specified parameters.
   * Constructs the low level street and initializes the underlying data storage and the traffic light signaler.
   *
   * @param[in]  _id            The internal street id
   * @param[in]  _lanes           The number of lanes on the street in the direction represented by this instance.
   * @param[in]  _length        The street length
   * @param[in]  _speedLimit          The speed limit on this street.
   * @param[in]  _trafficLightOffset  The position of the traffic light as distance from the end of the street.
   */

  LowLevelStreet(unsigned int _id, unsigned int _lanes, double _length, double _speedLimit, double _trafficLightOffset)
      : id(_id), speedLimit(_speedLimit), signaler(rfb, _length, TODO, _trafficLightOffset), rfb(_lanes, _length) {}

  /**
   * Gets the internal street id.
   *
   * @return     The internal street.
   */
  unsigned int getId() const { return id; }

  /**
   * Gets the speed limit of this street.
   *
   * @return     The speed limit.
   */
  double getSpeedLimit() const { return speedLimit; }

  /*
   * Signaling methods which forward to signaler.
   */

  bool isSignalRed() { return signaler.isSignalRed(); }
  void setSignalRed(bool signalRed) { signaler.setSignalRed(signalRed); }
  void switchSignal() { signaler.switchSignal(); }

  /*
   * RfbStructure accessors which forward to signaler.
   */

  /**
   * Find the next car in front of the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset.
   * If the traffic light is red, the traffic light car might be returned instead of an actual car.
   * All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car in front represented by an iterator.
   */
  LowLevelCar &getPrevCar(iterator currentCarIt, const int laneOffset = 0) {
    return signaler.getPrevCar(currentCarIt, laneOffset);
  }
  const LowLevelCar &getPrevCar(const_iterator currentCarIt, const int laneOffset = 0) const {
    return signaler.getPrevCar(currentCarIt, laneOffset);
  }

  /**
   * Find the next car behind the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset.
   * The return value is not affected by the traffic light.
   * All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car behind the current car represented by an iterator.
   */
  LowLevelCar &getNextCar(iterator currentCarIt, const int laneOffset = 0) {
    return signaler.getNextCar(currentCarIt, laneOffset);
  }
  const LowLevelCar &getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const {
    return signaler.getNextCar(currentCarIt, laneOffset);
  }

  /*
   * Data structure / representation operations which forward to rfb.
   */

  unsigned int getLanes() const { return rfb.getLanes(); }

  double getLength() const { return rfb.getLength(); }

  unsigned int getNumCars() const { return rfb.getNumCars(); }

  void insertCar(const Vehicle &car) { rfb.insertCar(car); }

  void insertCar(Vehicle &&car) { rfb.insertCar(car); }

  void incorporateInsertedCars() { rfb.incorporateInsertedCars(); }

  void applyUpdates() { rfb.applyUpdates(); }

  CarIterable allIterable() { return rfb.allIterable(); }

  ConstCarIterable allIterable() const { return rfb.allIterable(); }

  ConstCarIterable constAllIterable() const { return rfb.constAllIterable(); }

  CarIterable beyondsIterable() { return rfb.beyondsIterable(); }

  ConstCarIterable beyondsIterable() const { return rfb.beyondsIterable(); }

  ConstCarIterable constBeyondsIterable() const { return rfb.constBeyondsIterable(); }

  void removeBeyonds() { rfb.removeBeyonds(); }
};

#endif
