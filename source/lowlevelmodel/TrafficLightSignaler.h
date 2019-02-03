#ifndef TRAFFIC_LIGHT_SIGNALER_H
#define TRAFFIC_LIGHT_SIGNALER_H

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

#include "LowLevelCar.h"

enum Signal { RED, GREEN };

/**
 * TrafficLightSignaler wraps RfbStructure and contains functionality related to traffic lights.
 *
 * There is only one traffic car on each street. This car is returned as the traffic light car for all lanes of the
 * street. This car has its lane set to 0 by default.
 */
template <template <typename Vehicle> typename RfbStructure>
class TrafficLightSignaler {
private:
  using ConcreteRfbStructure = RfbStructure<LowLevelCar>;

public:
  /**
   * @brief      Exception for signaling errors.
   */
  class Exception : public std::exception {

  public:
    /**
     * @brief      Constructs the object.
     * @param[in]  message   Is the custom exception message which is passed along.
     */
    Exception(const std::string message) : fullMessage(message) {}

    /**
     * @brief      Returns the full exception message.
     * @return     the custom exception message.
     */
    virtual const char *what() const throw() { return fullMessage.c_str(); }

  private:
    std::string fullMessage;
  };

  template <typename RfbIterator, bool Const>
  class BaseIterator {
  public:
    /*
     * Iterator trait definitions.
     */

    using iterator_category = typename std::iterator_traits<RfbIterator>::iterator_category;
    using value_type        = LowLevelCar;
    using difference_type   = std::ptrdiff_t;
    using reference         = typename std::conditional_t<Const, LowLevelCar const &, LowLevelCar &>;
    using pointer           = typename std::conditional_t<Const, LowLevelCar const *, LowLevelCar *>;

  private:
    friend class TrafficLightSignaler;

    /**
     * Contains the state of the iterator, also referred to as tag.
     */
    enum { PROXY, SPECIAL } state;

    /**
     * This union contains either dest if the BaseIterator is in the PROXY state or the behindIt if the BaseIterator is
     * in the SPECIAL state.
     *
     * The union allows for proper naming while using the same memory.
     */
    union {
      /**
       * dest is the RfbIterator all method calls are forwarded to if the BaseIterator is in the PROXY state.
       */
      RfbIterator dest;

      /**
       * behindIt is the RfbIterator of the vehicle behind the special vehicle if the BaseIterator is in the SPECIAL
       * state.
       */
      RfbIterator behindIt;
    };
    /**
     * inFrontIt is the RfbIterator of the vehicle in front of the special vehicle if the BaseIterator is in the SPECIAL
     * state.
     */
    RfbIterator inFrontIt;

    /**
     * special contains the special (out-of-street) vehicle if the BaseIterator is in the SPECIAL state.
     */
    LowLevelCar *special;

  public:
    /*
     * Definitions satisfying the LegacyIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/Iterator
     */

    reference operator*() const {
      switch (state) {
      case PROXY: return *dest;
      case SPECIAL: return *special;
      default: return *special;
      }
    }

    BaseIterator<RfbIterator, Const> &operator++() {
      switch (state) {
      case PROXY: ++dest;
      case SPECIAL: break;
      default: break;
      }

      return *this;
    }

    /*
     * Definitions satisfying the EqualityComparable requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/EqualityComparable
     */

    friend bool operator==(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return false;

      switch (lhs.state) {
      case PROXY: return lhs.dest == rhs.dest;
      case SPECIAL: return lhs.special == rhs.special;
      default: return false;
      }
    }

    /*
     * Further definitions satisfying the LegacyInputIterator requirement
     *
     * https://en.cppreference.com/w/cpp/named_req/InputIterator
     *
     * and the LegacyOutputIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/OutputIterator
     */

    pointer operator->() const {
      switch (state) {
      case PROXY: return dest.operator->();
      case SPECIAL: return special;
      default: return nullptr;
      }
    }

    BaseIterator<RfbIterator, Const> operator++(int) {
      switch (state) {
      case PROXY: return BaseIterator<RfbIterator, Const>(dest++);
      case SPECIAL: return *this;
      default: return *this;
      }
    }

    friend bool operator!=(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return true;

      switch (lhs.state) {
      case PROXY: return lhs.dest != rhs.dest;
      case SPECIAL: return lhs.special != rhs.special;
      default: return true;
      }
    }

    /*
     * Further definitions satisfying the LegacyBidirectionalIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
     */

    BaseIterator<RfbIterator, Const> &operator--() {
      switch (state) {
      case PROXY: --dest;
      case SPECIAL: break;
      default: break;
      }

      return *this;
    }

    BaseIterator<RfbIterator, Const> operator--(int) {
      switch (state) {
      case PROXY: return BaseIterator<RfbIterator, Const>(dest--);
      case SPECIAL: return *this;
      default: return *this;
      }
    }

    /*
     * Further definitions satisfying the LegacyRandomAccessIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
     */

    BaseIterator<RfbIterator, Const> operator+(const difference_type &n) const {
      switch (state) {
      case PROXY: return BaseIterator<RfbIterator, Const>(dest + n);
      case SPECIAL: return *this;
      default: return *this;
      }
    }

    BaseIterator<RfbIterator, Const> &operator+=(const difference_type &n) {
      switch (state) {
      case PROXY: dest += n;
      case SPECIAL: break;
      default: break;
      }

      return *this;
    }

    BaseIterator<RfbIterator, Const> operator-(const difference_type &n) const {
      switch (state) {
      case PROXY: return BaseIterator<RfbIterator, Const>(dest - n);
      case SPECIAL: return *this;
      default: return *this;
      }
    }

    BaseIterator<RfbIterator, Const> &operator-=(const difference_type &n) {
      switch (state) {
      case PROXY: dest -= n;
      case SPECIAL: break;
      default: break;
      }

      return *this;
    }

    difference_type operator-(const BaseIterator<RfbIterator, Const> &other) const {
      if (state != other.state) return 0;

      switch (state) {
      case PROXY: return dest - other.dest;
      case SPECIAL: return 0;
      default: return 0;
      }
    }

    reference operator[](const difference_type &n) const {
      switch (state) {
      case PROXY: return dest[n];
      case SPECIAL: return *special;
      default: return *special;
      }
    }

    /**
     * Returns true if lhs is less than rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator<(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return lhs.state < rhs.state;

      switch (lhs.state) {
      case PROXY: return lhs.dest < rhs.dest;
      case SPECIAL: return lhs.special < rhs.special;
      default: return false;
      }
    }

    /**
     * Returns true if lhs is greater than rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator>(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return lhs.state > rhs.state;

      switch (lhs.state) {
      case PROXY: return lhs.dest > rhs.dest;
      case SPECIAL: return lhs.special > rhs.special;
      default: return false;
      }
    }

    /**
     * Returns true if lhs is less than or equal to rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator<=(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return lhs.state <= rhs.state;

      switch (lhs.state) {
      case PROXY: return lhs.dest <= rhs.dest;
      case SPECIAL: return lhs.special <= rhs.special;
      default: return false;
      }
    }

    /**
     * Returns true if lhs is greater than or equal to rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator>=(const BaseIterator<RfbIterator, Const> &lhs, const BaseIterator<RfbIterator, Const> &rhs) {
      if (lhs.state != rhs.state) return lhs.state >= rhs.state;

      switch (lhs.state) {
      case PROXY: return lhs.dest >= rhs.dest;
      case SPECIAL: return lhs.special >= rhs.special;
      default: return false;
      }
    }

    /*
     * Further definitions satisfying the LegacyForwardIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/ForwardIterator
     */
    BaseIterator() {}

    /**
     * Conversion operator, converts non-const into const iterator.
     */
    operator BaseIterator<RfbIterator, true>() const {
      switch (state) {
      case PROXY: return BaseIterator<RfbIterator, true>(dest);
      case SPECIAL: return BaseIterator<RfbIterator, true>(behindIt, inFrontIt, special);
      default: return BaseIterator<RfbIterator, true>();
      }
    }

  public:
    bool isSpecial() const { return state == SPECIAL; }

    BaseIterator<RfbIterator, Const> getThisOrNotSpecialCarInFront() const {
      switch (state) {
      case iterator::PROXY: return *this;
      case iterator::SPECIAL: return BaseIterator<RfbIterator, Const>(inFrontIt);
      default: return BaseIterator<RfbIterator, Const>();
      }
    }

    BaseIterator<RfbIterator, Const> getThisOrNotSpecialCarBehind() const {
      switch (state) {
      case iterator::PROXY: return *this;
      case iterator::SPECIAL: return BaseIterator<RfbIterator, Const>(behindIt);
      default: return BaseIterator<RfbIterator, Const>();
      }
    }

  private:
    BaseIterator(RfbIterator _dest) : state(PROXY), dest(_dest) {}
    BaseIterator(RfbIterator _behindIt, RfbIterator _inFrontIt, LowLevelCar &_special)
        : state(SPECIAL), behindIt(_behindIt), inFrontIt(_inFrontIt), special(&_special) {}
  };

  using iterator       = BaseIterator<typename ConcreteRfbStructure::iterator, false>;
  using const_iterator = BaseIterator<typename ConcreteRfbStructure::const_iterator, true>;

public:
  /**
   * Iterable type providing iterators for retrieving vehicles on the street.
   */
  class AllCarIterable {
  public:
    iterator begin() const { return iterator(signaler.rfb.allIterable().begin()); }
    iterator end() const { return iterator(signaler.rfb.allIterable().end()); }

    const_iterator cbegin() const { return iterator(signaler.rfb.allIterable().cbegin()); }
    const_iterator cend() const { return iterator(signaler.rfb.allIterable().cend()); }

  private:
    friend class TrafficLightSignaler;

    TrafficLightSignaler &signaler;

    AllCarIterable(TrafficLightSignaler &_signaler) : signaler(_signaler) {}
  };

  class ConstAllCarIterable {
  public:
    const_iterator begin() const { return const_iterator(signaler.rfb.constAllIterable().begin()); }
    const_iterator end() const { return const_iterator(signaler.rfb.constAllIterable().end()); }

    const_iterator cbegin() const { return const_iterator(signaler.rfb.constAllIterable().cbegin()); }
    const_iterator cend() const { return const_iterator(signaler.rfb.constAllIterable().cend()); }

  private:
    friend class TrafficLightSignaler;

    const TrafficLightSignaler &signaler;

    ConstAllCarIterable(const TrafficLightSignaler &_signaler) : signaler(_signaler) {}
  };

  friend class AllCarIterable;
  friend class ConstAllCarIterable;

private:
  ConcreteRfbStructure &rfb;
  Signal signal;
  LowLevelCar trafficLightCar;
  double trafficLightPosition;

public:
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, double _streetLength, const LowLevelCar &_trafficLightCar,
      double _trafficLightOffset, unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = GREEN)
      : rfb(_rfb), signal(_signal), trafficLightCar(_trafficLightCar),
        trafficLightPosition(_streetLength - _trafficLightOffset) {
    trafficLightCar.setPosition(_lane, trafficLightPosition, _velocity);
  }
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, const LowLevelCar &_trafficLightCar, double _trafficLightOffset,
      unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = GREEN)
      : TrafficLightSignaler(
            _rfb, _rfb.getLength(), _trafficLightCar, _trafficLightOffset, _lane, _velocity, _signal){};
  /**
   * Constructor copying values from another signaler but using a different RfbStructure.
   * Meant to be used in copy constructors of embedding types.
   */
  TrafficLightSignaler(const TrafficLightSignaler &other, ConcreteRfbStructure &_rfb)
      : rfb(_rfb), signal(other.signal), trafficLightCar(other.trafficLightCar) {}

  double getTrafficLightPosition() const { return trafficLightPosition; }

  /**
   * Retrieves the value of the signal.
   */
  Signal getSignal() const { return signal; }
  /**
   * Sets the value of the signal.
   */
  void setSignal(const Signal _signal) { signal = _signal; }
  /**
   * Switches the signal of the traffic light.
   *
   * If the signal is RED, it will be set to GREEN. If the signal is GREEN it will be set to RED.
   */
  void switchSignal() {
    if (signal == RED)
      signal = GREEN;
    else if (signal == GREEN)
      signal = RED;
  }

  /**
   * Finds the next vehicle in front of the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * This method wraps RfbStructure::getNextCarInFront() but will return a special iterator with a reference to the
   * traffic light car win the appropriate cases. I.e. when the traffic light signal is RED and the traffic light car is
   * located between the origin car and the next car in front. The method will also return the expected value when a
   * special iterator is passed in.
   *
   * All vehicles are represented by iterators. Iterators of the "all" iterable are used, retrievable through
   * allIterable().
   *
   * If there is no vehicle in front of the vehicle passed in, the end() iterator is returned.
   */
  iterator getNextCarInFront(iterator originVehicleIt, const int laneOffset = 0) {
    switch (originVehicleIt.state) {
    case iterator::PROXY: {
      // Create a new scope using brackets to be able to initialise variable only here
      typename ConcreteRfbStructure::iterator inFrontIt = rfb.getNextCarInFront(originVehicleIt.dest, laneOffset);

      if (signal == RED) {
        // Traffic light is RED, check if the traffic light is between the origin car and the next car in front.

        double inFrontDistance;
        if (inFrontIt == rfb.allIterable().end())
          inFrontDistance = rfb.getLength();
        else
          inFrontDistance = inFrontIt->getDistance();

        if (inFrontDistance > trafficLightCar.getDistance() &&
            originVehicleIt->getDistance() <= trafficLightCar.getDistance())
          return iterator(originVehicleIt.dest, inFrontIt, trafficLightCar);
      }

      return iterator(inFrontIt);
    }
    case iterator::SPECIAL: {
      if (laneOffset == 0) return iterator(originVehicleIt.inFrontIt);

      typename ConcreteRfbStructure::iterator endIt = rfb.allIterable().end();
      iterator wrappedEndIt                         = iterator(endIt);

      if (originVehicleIt.behindIt == endIt) {
        // The behindIt of special cars should always be != endIt.
        // If this turns out to not be the case, the same mechanism as in getNextCarBehind should be implemented here.
        throw TrafficLightSignaler::Exception("Encountered special car with behindIt == endIt.");
      } else {
        // There is a car behind the special car
        // Use this car to perform a forward search until a car in front of the special car is found.

        iterator wrappedForwardSearchIt;
        // Search for the next car in front on the correct lane
        // The getNextCarInFront method of TrafficLightSignaler has to be used, the car in front might be a traffic
        // light car.
        if (originVehicleIt.behindIt != endIt)
          wrappedForwardSearchIt = getNextCarInFront(iterator(originVehicleIt.behindIt), laneOffset);
        else
          return wrappedEndIt;

        while (wrappedForwardSearchIt != wrappedEndIt &&
               wrappedForwardSearchIt->getDistance() <= originVehicleIt->getDistance()) {
          // As long as wrappedForwardSearchIt is behind originVehicleIt, look for the next car in front
          wrappedForwardSearchIt = getNextCarInFront(wrappedForwardSearchIt, 0);
        }
        return wrappedForwardSearchIt;
      }
    }
    default: return iterator();
    }
  }
  const_iterator getNextCarInFront(const_iterator originVehicleIt, const int laneOffset = 0) const {
    switch (originVehicleIt.state) {
    case const_iterator::PROXY: {
      // Create a new scope using brackets to be able to initialise variable only here
      typename ConcreteRfbStructure::const_iterator inFrontIt = rfb.getNextCarInFront(originVehicleIt.dest, laneOffset);

      if (signal == RED) {
        // Traffic light is RED, check if the traffic light is between the origin car and the next car in front.

        double inFrontDistance;
        if (inFrontIt == rfb.allIterable().end())
          inFrontDistance = rfb.getLength();
        else
          inFrontDistance = inFrontIt->getDistance();

        if (inFrontDistance > trafficLightCar.getDistance() &&
            originVehicleIt->getDistance() <= trafficLightCar.getDistance())
          return const_iterator(originVehicleIt.dest, inFrontIt, trafficLightCar);
      }

      return const_iterator(inFrontIt);
    }
    case const_iterator::SPECIAL: {
      if (laneOffset == 0) return const_iterator(originVehicleIt.inFrontIt);

      typename ConcreteRfbStructure::const_iterator endIt = rfb.allIterable().end();
      const_iterator wrappedEndIt                         = const_iterator(endIt);

      if (originVehicleIt.behindIt == endIt) {
        // The behindIt of special cars should always be != endIt.
        // If this turns out to not be the case, the same mechanism as in getNextCarBehind should be implemented here.
        throw TrafficLightSignaler::Exception("Encountered special car with behindIt == endIt.");
      } else {
        // There is a car behind the special car
        // Use this car to perform a forward search until a car in front of the special car is found.

        const_iterator wrappedForwardSearchIt;
        // Search for the next car in front on the correct lane
        // The getNextCarInFront method of TrafficLightSignaler has to be used, the car in front might be a traffic
        // light car.
        if (originVehicleIt.behindIt != endIt)
          wrappedForwardSearchIt = getNextCarInFront(const_iterator(originVehicleIt.behindIt), laneOffset);
        else
          return wrappedEndIt;

        while (wrappedForwardSearchIt != wrappedEndIt &&
               wrappedForwardSearchIt->getDistance() <= originVehicleIt->getDistance()) {
          // As long as wrappedForwardSearchIt is behind originVehicleIt, look for the next car in front
          wrappedForwardSearchIt = getNextCarInFront(wrappedForwardSearchIt, 0);
        }
        return wrappedForwardSearchIt;
      }
    }
    default: return const_iterator();
    }
  }

  /**
   * Finds the next vehicle behind the current vehicle.
   * The method returns the next vehicle on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right (+1) of the current lane.
   *
   * This method wraps RfbStructure::getNextCarBehind() but will also return the expected values when a special iterator
   * is passed in to.
   *
   * All vehicles are represented by iterators. Iterators of the "all" iterable are used, retrievable through
   * allIterable().
   *
   * If there is no vehicle in front of the vehicle passed in, the end() iterator is returned.
   */
  iterator getNextCarBehind(iterator originVehicleIt, const int laneOffset = 0) {
    switch (originVehicleIt.state) {
    case iterator::PROXY: return iterator(rfb.getNextCarBehind(originVehicleIt.dest, laneOffset));
    case iterator::SPECIAL: {
      if (laneOffset == 0) return iterator(originVehicleIt.behindIt);

      typename ConcreteRfbStructure::iterator endIt = rfb.allIterable().end();

      if (originVehicleIt.inFrontIt == endIt) {
        // There is no car in front of the special car
        // Use the car behind (which is never == endIt) to perform a forward search until a car in front of the special
        // car is found. Then the car behind this car is returned.

        typename ConcreteRfbStructure::iterator candidate = endIt;
        typename ConcreteRfbStructure::iterator forwardSearchIt =
            rfb.getNextCarInFront(originVehicleIt.behindIt, laneOffset);

        while (forwardSearchIt != endIt && forwardSearchIt->getDistance() <= originVehicleIt->getDistance()) {
          // As long as forwardSearchIt is behind of originVehicleIt, look for the next car in front
          // forwardSearchIt is the new candidate for the return value
          candidate       = forwardSearchIt;
          forwardSearchIt = rfb.getNextCarInFront(candidate, 0);
        }
        return iterator(candidate);
      } else {
        // There is a car in front of the special car
        // Use this car to perform a backward search until a car behind the special car is found.

        typename ConcreteRfbStructure::iterator backwardSearchIt;
        // Search for the next car behind on the correct lane
        if (originVehicleIt.inFrontIt != endIt)
          backwardSearchIt = rfb.getNextCarBehind(originVehicleIt.inFrontIt, laneOffset);
        else
          return iterator(endIt);

        while (backwardSearchIt != endIt && backwardSearchIt->getDistance() >= originVehicleIt->getDistance()) {
          // As long as backwardSearchIt is in front of originVehicleIt, look for the next car behind
          backwardSearchIt = rfb.getNextCarBehind(backwardSearchIt, 0);
        }
        return iterator(backwardSearchIt);
      }
    }
    default: return iterator();
    }
  }
  const_iterator getNextCarBehind(const_iterator originVehicleIt, const int laneOffset = 0) const {
    switch (originVehicleIt.state) {
    case const_iterator::PROXY: return const_iterator(rfb.getNextCarBehind(originVehicleIt.dest, laneOffset));
    case const_iterator::SPECIAL: {
      if (laneOffset == 0) return const_iterator(originVehicleIt.behindIt);

      typename ConcreteRfbStructure::const_iterator endIt = rfb.allIterable().end();

      if (originVehicleIt.inFrontIt == endIt) {
        // There is no car in front of the special car
        // Use the car behind (which is never == endIt) to perform a forward search until a car in front of the special
        // car is found. Then the car behind this car is returned.

        typename ConcreteRfbStructure::const_iterator candidate = endIt;
        typename ConcreteRfbStructure::const_iterator forwardSearchIt =
            rfb.getNextCarInFront(originVehicleIt.behindIt, laneOffset);

        while (forwardSearchIt != endIt && forwardSearchIt->getDistance() <= originVehicleIt->getDistance()) {
          // As long as forwardSearchIt is behind of originVehicleIt, look for the next car in front
          // forwardSearchIt is the new candidate for the return value
          candidate       = forwardSearchIt;
          forwardSearchIt = rfb.getNextCarInFront(candidate, 0);
        }
        return const_iterator(candidate);
      } else {
        // There is a car in front of the special car
        // Use this car to perform a backward search until a car behind the special car is found.

        typename ConcreteRfbStructure::const_iterator backwardSearchIt;
        // Search for the next car behind on the correct lane
        if (originVehicleIt.inFrontIt != endIt)
          backwardSearchIt = rfb.getNextCarBehind(originVehicleIt.inFrontIt, laneOffset);
        else
          return const_iterator(endIt);

        while (backwardSearchIt != endIt && backwardSearchIt->getDistance() >= originVehicleIt->getDistance()) {
          // As long as backwardSearchIt is in front of originVehicleIt, look for the next car behind
          backwardSearchIt = rfb.getNextCarBehind(backwardSearchIt, 0);
        }
        return const_iterator(backwardSearchIt);
      }
    }
    default: return const_iterator();
    }
  }

  /**
   * Iterable for iterating over all vehicles.
   *
   * Usually, vehicles are iterated in order of increasing distance from the start of the street, but no specific order
   * is guaranteed.
   *
   * Vehicles which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() may or may not be considered by the iterable.
   */
  AllCarIterable allIterable() { return AllCarIterable(*this); }

  ConstAllCarIterable allIterable() const { return ConstAllCarIterable(*this); }

  ConstAllCarIterable constAllIterable() const { return ConstAllCarIterable(*this); }
};

#endif
