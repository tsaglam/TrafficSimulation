#ifndef LOW_LEVEL_STREET_H
#define LOW_LEVEL_STREET_H

#include <utility>

#include "LowLevelCar.h"
#include "TrafficLightSignaler.h"

/**
 * Class for the low level street, wraps the actual underlying data structure storing cars as well as a
 * TrafficLightSignaler.
 *
 * @tparam RfbStructure  Underlying data structure storing the cars on the street
 */
template <template <typename Vehicle> typename RfbStructure>
class LowLevelStreet {
public:
  using Vehicle                      = LowLevelCar;
  using ConcreteRfbStructure         = RfbStructure<Vehicle>;
  using ConcreteTrafficLightSignaler = TrafficLightSignaler<RfbStructure>;
  using iterator                     = typename ConcreteTrafficLightSignaler::iterator;
  using const_iterator               = typename ConcreteTrafficLightSignaler::const_iterator;

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
   * @param[in]  _trafficLightCar  A properly initialised car which is used as the basis of the traffic light car of the
   * signaler.
   * @param[in]  _trafficLightOffset  The position of the traffic light as distance from the end of the street.
   */
  LowLevelStreet(unsigned int _id, unsigned int _lanes, double _length, double _speedLimit,
      const LowLevelCar &_trafficLightCar, double _trafficLightOffset)
      : id(_id), speedLimit(_speedLimit), signaler(rfb, _length, _trafficLightCar, _trafficLightOffset),
        rfb(_lanes, _length) {}

  /* Rule of Five */

  /**
   * Destructor.
   */
  ~LowLevelStreet<RfbStructure>() = default;
  /**
   * Copy constructor.
   */
  LowLevelStreet<RfbStructure>(const LowLevelStreet<RfbStructure> &other)
      : id(other.id), speedLimit(other.speedLimit), signaler(other.signaler, rfb), rfb(other.rfb) {}
  /**
   * Move constructor.
   */
  LowLevelStreet<RfbStructure>(LowLevelStreet<RfbStructure> &&other) noexcept
      : id(other.id), speedLimit(other.speedLimit), signaler(other.signaler, rfb), rfb(std::move(other.rfb)) {}
  /**
   * Copy assignment operator.
   */
  LowLevelStreet<RfbStructure> &operator=(const LowLevelStreet<RfbStructure> &other) = delete;
  /**
   * Move assignment operator.
   */
  LowLevelStreet<RfbStructure> &operator=(LowLevelStreet<RfbStructure> &&other) = delete;

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

  /**
   * Retrieves the value of the signal.
   */
  Signal getSignal() const { return signaler.getSignal(); }
  /**
   * Sets the value of the signal.
   */
  void setSignal(const Signal signal) { signaler.setSignal(signal); }
  /**
   * Switches the signal of the traffic light.
   *
   * If the signal is RED, it will be set to GREEN. If the signal is GREEN it will be set to RED.
   */
  void switchSignal() { signaler.switchSignal(); }

  /*
   * RfbStructure accessors which forward to signaler.
   */

  double getTrafficLightPosition() const { return signaler.getTrafficLightPosition(); }

  typename TrafficLightSignaler<RfbStructure>::AllCarIterable allIterable() { return signaler.allIterable(); }

  typename TrafficLightSignaler<RfbStructure>::ConstAllCarIterable allIterable() const {
    return signaler.allIterable();
  }

  typename TrafficLightSignaler<RfbStructure>::ConstAllCarIterable constAllIterable() const {
    return signaler.constAllIterable();
  }

  /**
   * Find the next car in front of an origin car on the same or a neighbouring lane.
   * The lane is determined by the laneOffset.
   * If the traffic light is red, the traffic light car might be returned instead of an actual car.
   * All cars are represented by iterators.
   *
   * @param[in]  originVehicleIt The origin car represented by an iterator.
   * @param[in]  laneOffset      The lane offset determining which lane to search on. Same lane: 0, Left: -1, Right: +1.
   *
   * @return     The car in front of the origin car represented by an iterator.
   */
  iterator getNextCarInFront(iterator originVehicleIt, const int laneOffset = 0) {
    return signaler.getNextCarInFront(originVehicleIt, laneOffset);
  }
  const_iterator getNextCarInFront(const_iterator originVehicleIt, const int laneOffset = 0) const {
    return signaler.getNextCarInFront(originVehicleIt, laneOffset);
  }

  /**
   * Find the next car in front of an origin car on the same or a neighbouring lane.
   * The lane is determined by the laneOffset.
   * The return value is not affected by the traffic light.
   * All cars are represented by iterators.
   *
   * @param[in]  originVehicleIt The origin car represented by an iterator.
   * @param[in]  laneOffset      The lane offset determining which lane to search on. Same lane: 0, Left: -1, Right: +1.
   *
   * @return     The car behind the origin car represented by an iterator.
   */
  iterator getNextCarBehind(iterator originVehicleIt, const int laneOffset = 0) {
    return signaler.getNextCarBehind(originVehicleIt, laneOffset);
  }
  const_iterator getNextCarBehind(const_iterator originVehicleIt, const int laneOffset = 0) const {
    return signaler.getNextCarBehind(originVehicleIt, laneOffset);
  }

  /*
   * Data structure / representation operations which forward to rfb.
   */

  unsigned int getLaneCount() const { return rfb.getLaneCount(); }

  double getLength() const { return rfb.getLength(); }

  unsigned int getCarCount() const { return rfb.getCarCount(); }

  void insertCar(const Vehicle &vehicle) { rfb.insertCar(vehicle); }

  void insertCar(Vehicle &&vehicle) { rfb.insertCar(vehicle); }

  void incorporateInsertedCars() { rfb.incorporateInsertedCars(); }

  void updateCarsAndRestoreConsistency() { rfb.updateCarsAndRestoreConsistency(); }

  typename ConcreteRfbStructure::BeyondsCarIterable beyondsIterable() { return rfb.beyondsIterable(); }

  typename ConcreteRfbStructure::ConstBeyondsCarIterable beyondsIterable() const { return rfb.beyondsIterable(); }

  typename ConcreteRfbStructure::ConstBeyondsCarIterable constBeyondsIterable() const {
    return rfb.constBeyondsIterable();
  }

  void removeBeyonds() { rfb.removeBeyonds(); }

  const ConcreteRfbStructure &getUnderlyingDataStructure() const { return rfb; }
};

#endif
