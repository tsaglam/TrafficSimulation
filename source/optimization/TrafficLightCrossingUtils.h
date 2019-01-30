#ifndef TRAFFIC_LIGHT_CROSSING_UTILS_H
#define TRAFFIC_LIGHT_CROSSING_UTILS_H

struct TrafficLightCrossing {
  unsigned carId;
  unsigned streetId;
  unsigned timeStep;

  TrafficLightCrossing(unsigned carId, unsigned streetId, unsigned timeStep)
      : carId(carId), streetId(streetId), timeStep(timeStep) {}
};

#endif