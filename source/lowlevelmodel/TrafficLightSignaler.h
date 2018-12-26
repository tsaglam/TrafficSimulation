#ifndef TRAFFIC_LIGHT_SIGNALER_H
#define TRAFFIC_LIGHT_SIGNALER_H

#include <vector>

#include "LowLevelCar.h"

template <typename RfbStructure>
class TrafficLightSignaler {
private:
  using concrete_rfb   = typename RfbStructure<LowLevelCar>;
  using iterator       = typename concrete_rfb::iterator;
  using const_iterator = typename concrete_rfb::const_iterator;

private:
  concrete_rfb &rfb;
  bool signalRed;
  LowLevelCar trafficLightCar;

public:
  TrafficLightSignaler(concrete_rfb &_rfb, double _streetLength, const LowLevelCar &_trafficLightCar,
      double _trafficLightOffset, unsigned int _lane = 0, double _velocity = 0.0, bool _signalRed = false)
      : rfb(_rfb), trafficLightCar(_trafficLightCar) signalRed(_signalRed) {
    trafficLightCar.setPosition(_lane, _streetLength - _trafficLightOffset, _velocity);
  }
  TrafficLightSignaler(concrete_rfb &_rfb, const LowLevelCar &_trafficLightCar, double _trafficLightOffset,
      unsigned int _lane = 0, double _velocity = 0.0, bool _signalRed = false)
      : TrafficLightSignaler(
            _rfb, _rfb.getLength(), _trafficLightCar, _trafficLightOffset, _lane, _velocity, _signalRed){};

  bool isSignalRed() { return signalRed; }
  void setSignalRed(bool _signalRed) { signalRed = _signalRed; }
  void switchSignal() { signalRed = !signalRed; }

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

    if (signalRed) {
      // Logic logic...

      return trafficLightCar;
    } else {
      return nextCar;
    }
  }
  const LowLevelCar &getNextCar(const_iterator currentCarIt, const int laneOffset = 0) const {
    const_iterator nextCarIt   = rfb.getNextCar(currentCarIt, laneOffset);
    const LowLevelCar &nextCar = *nextCarIt;

    if (signalRed) {
      // Logic logic...

      return trafficLightCar;
    } else {
      return nextCar;
    }
  }
};

#endif
