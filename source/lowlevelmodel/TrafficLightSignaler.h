#ifndef TRAFFIC_LIGHT_SIGNALER_H
#define TRAFFIC_LIGHT_SIGNALER_H

#include <vector>

#include "LowLevelCar.h"

enum Signal : bool { Red = true, Green = false };

template <typename RfbStructure>
class TrafficLightSignaler {
private:
  using ConcreteRfbStructure = typename RfbStructure<LowLevelCar>;
  using iterator             = typename ConcreteRfbStructure::iterator;
  using const_iterator       = typename ConcreteRfbStructure::const_iterator;

private:
  ConcreteRfbStructure &rfb;
  Signal signal;
  LowLevelCar trafficLightCar;

public:
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, double _streetLength, const LowLevelCar &_trafficLightCar,
      double _trafficLightOffset, unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = Green)
      : rfb(_rfb), trafficLightCar(_trafficLightCar), signal(_signal) {
    trafficLightCar.setPosition(_lane, _streetLength - _trafficLightOffset, _velocity);
  }
  TrafficLightSignaler(ConcreteRfbStructure &_rfb, const LowLevelCar &_trafficLightCar, double _trafficLightOffset,
      unsigned int _lane = 0, double _velocity = 0.0, Signal _signal = Green)
      : TrafficLightSignaler(
            _rfb, _rfb.getLength(), _trafficLightCar, _trafficLightOffset, _lane, _velocity, _signal){};

  inline Signal getSignal() const { return signalRed; }
  inline void setSignal(const Signal _signal) { signal = _signal; }
  inline void switchSignal() { signal = !signal; }

  /**
   * Finds the next car in front of the current car.
   * The method returns the next car on the current lane (if laneOffset == 0) or the next lane to the left (-1) /
   * right
   * (+1) of the current lane.
   *
   * All cars are represented by iterators.
   */
  LowLevelCar &getPrevCar(iterator currentCarIt, const int laneOffset = 0) {
    return *rfb.getPrevCar(currentCarIt, laneOffset);
  }
  const LowLevelCar &getPrevCar(const_iterator currentCarIt, const int laneOffset = 0) const {
    return *rfb.getPrevCar(currentCarIt, laneOffset);
  }

  /**
   * Finds the next car behind the current car.
   * The method returns the next car on the current lane (if laneOffset == 0) or the next lane to the left (-1) / right
   * (+1) of the current lane.
   *
   * All cars are represented by iterators.
   */
  LowLevelCar &getNextCar(iterator currentCarIt, const int laneOffset = 0) {
    iterator nextCarIt   = rfb.getNextCar(currentCarIt, laneOffset);
    LowLevelCar &nextCar = *nextCarIt;

    if (signal == Red) {
      // Logic logic...

      return trafficLightCar;
    } else {
      return nextCar;
    }
  }
  const LowLevelCar &getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const {
    const_iterator nextCarIt   = rfb.getNextCar(currentCarIt, laneOffset);
    const LowLevelCar &nextCar = *nextCarIt;

    if (signal == Red) {
      // Logic logic...

      return trafficLightCar;
    } else {
      return nextCar;
    }
  }
};

#endif
