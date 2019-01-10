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
      case SPECIAL: *this;
      default: *this;
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
      case SPECIAL: *this;
      default: *this;
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
    BaseIterator() = default;

  public:
    bool isSpecial() const { return state == SPECIAL; }

  private:
    BaseIterator(RfbIterator _dest) : state(PROXY), dest(_dest) {}
    BaseIterator(RfbIterator _behindIt, RfbIterator _inFrontIt, LowLevelCar &_special)
        : state(SPECIAL), behindIt(_behindIt), inFrontIt(_inFrontIt), special(&_special) {}
  };

  using iterator       = BaseIterator<typename RfbStructure<LowLevelCar>::iterator, false>;
  using const_iterator = BaseIterator<typename RfbStructure<LowLevelCar>::const_iterator, true>;

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
    const_iterator begin() const { return const_iterator(signaler.rfb.allIterable().begin()); }
    const_iterator end() const { return const_iterator(signaler.rfb.allIterable().end()); }

    const_iterator cbegin() const { return const_iterator(signaler.rfb.allIterable().cbegin()); }
    const_iterator cend() const { return const_iterator(signaler.rfb.allIterable().cend()); }

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

public:
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, double _streetLength, const LowLevelCar &_trafficLightCar,
      double _trafficLightOffset, unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = GREEN)
      : rfb(_rfb), signal(_signal), trafficLightCar(_trafficLightCar) {
    trafficLightCar.setPosition(_lane, _streetLength - _trafficLightOffset, _velocity);
  }
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, const LowLevelCar &_trafficLightCar, double _trafficLightOffset,
      unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = GREEN)
      : TrafficLightSignaler(
            _rfb, _rfb.getLength(), _trafficLightCar, _trafficLightOffset, _lane, _velocity, _signal){};

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
      iterator inFrontIt = rfb.getNextCarInFront(originVehicleIt.dest, laneOffset);

      if (signal == RED) {
        // Traffic light is RED, check if the traffic light is between the origin car and the next car in front.
        if (inFrontIt->getDistance() > trafficLightCar.getDistance() &&
            originVehicleIt->getDistance() <= trafficLightCar.getDistance())
          return iterator(originVehicleIt, inFrontIt, trafficLightCar);
      }

      return iterator(inFrontIt);
    }
    case iterator::SPECIAL: return iterator(originVehicleIt.inFrontIt);
    default: return iterator();
    }
  }
  const_iterator getNextCarInFront(const_iterator originVehicleIt, const int laneOffset = 0) const {
    switch (originVehicleIt.state) {
    case const_iterator::PROXY: {
      // Create a new scope using brackets to be able to initialise variable only here
      const_iterator inFrontIt = rfb.getNextCarInFront(originVehicleIt.dest, laneOffset);

      if (signal == RED) {
        // Traffic light is RED, check if the traffic light is between the origin car and the next car in front.
        if (inFrontIt->getDistance() > trafficLightCar.getDistance() &&
            originVehicleIt->getDistance() <= trafficLightCar.getDistance())
          return const_iterator(originVehicleIt, inFrontIt, trafficLightCar);
      }

      return const_iterator(inFrontIt);
    }
    case const_iterator::SPECIAL: return const_iterator(originVehicleIt.inFrontIt);
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
    case iterator::SPECIAL: return iterator(originVehicleIt.behindIt);
    default: return iterator();
    }
  }
  const_iterator getNextCarBehind(const_iterator originVehicleIt, const int laneOffset = 0) const {
    switch (originVehicleIt.state) {
    case const_iterator::PROXY: return const_iterator(rfb.getNextCarBehind(originVehicleIt.dest, laneOffset));
    case const_iterator::SPECIAL: return const_iterator(originVehicleIt.behindIt);
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
