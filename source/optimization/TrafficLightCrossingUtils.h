#ifndef TRAFFIC_LIGHT_CROSSING_UTILS_H
#define TRAFFIC_LIGHT_CROSSING_UTILS_H

#include <cmath>
#include <vector>

struct TrafficLightCrossing {
  unsigned carId;
  unsigned streetId;
  unsigned timeStep;

  TrafficLightCrossing(unsigned carId, unsigned streetId, unsigned timeStep)
      : carId(carId), streetId(streetId), timeStep(timeStep) {}
};

double defaultCarPriority(const unsigned) { return 1.0; }

/**
 * Evaluates a given order of traffic light durations at a junction based on the additive priority of the car crossing a
 * traffic light while it is green.
 *
 * Expects three vectors of the same length. The first, 'crossingsPerStreet', contains for each street of the junction
 * when and which car passed this streets traffic light. The second, 'trafficLightDuration', contains how long each
 * street's traffic light is green. The street order must be the same for both vectors. The third vector,
 * 'trafficLightOrder', represents the order of the traffic signals by giving the index of the corresponding street in
 * the first two vectors. Takes an optional function to assign a car a priority (by its id). Default: all cars have
 * priority 1.
 *
 * After evaluation the traffic light signals, their rating regarding different metrics such as the throughput at green
 * lights, the total throughput and the wait time can be retrieved with getter functions. These metrics can be accessed
 * streetwise and as a total for all streets of the junction, they can be weighted with the car priority using the
 * priority function given in the constructor.
 */
struct RateTrafficLights {
private:
  const std::vector<std::vector<TrafficLightCrossing>> &crossingsPerStreet;
  const std::vector<unsigned> &trafficLightDuration;
  const std::vector<unsigned> &trafficLightOrder;
  double (*getCarPriority)(const unsigned);

  unsigned streetCount;
  unsigned totalDuration = 0;
  std::vector<unsigned> initialOffset;

  unsigned totalThroughputAtGreen;
  std::vector<unsigned> throughputAtGreen;

  double totalThroughputAtGreenWithPriority;
  std::vector<double> throughputAtGreenWithPriority;

  unsigned totalThroughput;
  std::vector<unsigned> throughput;

  double totalThroughputWithPriority;
  std::vector<double> throughputWithPriority;

  unsigned totalWaitTime;
  std::vector<unsigned> waitTime;

  double totalWaitTimeWithPriority;
  std::vector<double> waitTimeWithPriority;

  unsigned getTimeToNextGreen(const unsigned currentTime, const unsigned streetIndex) const {
    unsigned timeInFirstInterval = currentTime % totalDuration;
    unsigned offset              = initialOffset[streetIndex];
    if (timeInFirstInterval < offset) { // traffic light was not yet green in the current cycle
      return offset - timeInFirstInterval;
    } else if (timeInFirstInterval < offset + trafficLightDuration[streetIndex]) {
      return 0; // traffic light is currently green
    } else {    // traffic light was already green in the current cycle -> wait 'till the next cycle
      return totalDuration - timeInFirstInterval + offset;
    }
  }

public:
  RateTrafficLights(const std::vector<std::vector<TrafficLightCrossing>> &_crossingsPerStreet,
      const std::vector<unsigned> &_trafficLightDuration, const std::vector<unsigned> &_trafficLightOrder,
      double (*_getCarPriority)(const unsigned))
      : crossingsPerStreet(_crossingsPerStreet), trafficLightDuration(_trafficLightDuration),
        trafficLightOrder(_trafficLightOrder), getCarPriority(_getCarPriority), streetCount(_trafficLightOrder.size()),
        initialOffset(streetCount, 0) {
    unsigned totalDuration = 0;
    for (unsigned streetIndex : trafficLightOrder) {
      initialOffset[streetIndex] = totalDuration;
      totalDuration += trafficLightDuration[streetIndex];
    }
    evaluate();
  }

  void evaluate() {
    reset();

    // for each street and each car that crossed this streets traffic light
    for (unsigned streetIndex = 0; streetIndex < crossingsPerStreet.size(); ++streetIndex) {
      for (const TrafficLightCrossing crossing : crossingsPerStreet[streetIndex]) {
        double carPriority       = getCarPriority(crossing.carId);
        unsigned currentWaitTime = getTimeToNextGreen(crossing.timeStep, streetIndex);

        if (currentWaitTime == 0) {
          throughputAtGreen[streetIndex] += 1;
          throughputAtGreenWithPriority[streetIndex] += carPriority;
          throughput[streetIndex] += 1;
          throughputWithPriority[streetIndex] += carPriority;
        } else {
          throughput[streetIndex] += 1;
          throughputWithPriority[streetIndex] += carPriority;
          waitTime[streetIndex] += currentWaitTime;
          waitTimeWithPriority[streetIndex] += currentWaitTime * carPriority;
        }
      }
    }

    // compute totals
    for (unsigned streetIndex = 0; streetIndex < streetCount; ++streetIndex) {
      totalThroughputAtGreen += throughputAtGreen[streetIndex];
      totalThroughputAtGreenWithPriority += throughputAtGreenWithPriority[streetIndex];
      totalThroughput += throughput[streetIndex];
      totalThroughputWithPriority += throughputWithPriority[streetIndex];
      totalWaitTime += waitTime[streetIndex];
      totalWaitTimeWithPriority += waitTimeWithPriority[streetIndex];
    }
  }

  void reset() {
    totalThroughputAtGreen             = 0;
    totalThroughputAtGreenWithPriority = 0;
    totalThroughput                    = 0;
    totalThroughputWithPriority        = 0;
    totalWaitTime                      = 0;
    totalWaitTimeWithPriority          = 0;

    std::fill(throughputAtGreen.begin(), throughputAtGreen.end(), 0);
    std::fill(throughputAtGreenWithPriority.begin(), throughputAtGreenWithPriority.end(), 0);
    std::fill(throughput.begin(), throughput.end(), 0);
    std::fill(throughputWithPriority.begin(), throughputWithPriority.end(), 0);
    std::fill(waitTime.begin(), waitTime.end(), 0);
    std::fill(waitTimeWithPriority.begin(), waitTimeWithPriority.end(), 0);
  }

  unsigned getTotalThroughputAtGreen() const { return totalThroughputAtGreen; }
  unsigned getThroughputAtGreen(const unsigned streetIndex) const { return throughputAtGreen[streetIndex]; }

  double getTotalThroughputAtGreenWithPriority() const { return totalThroughputAtGreenWithPriority; }
  double getThroughputAtGreenWithPriority(const unsigned streetIndex) const {
    return throughputAtGreenWithPriority[streetIndex];
  }

  unsigned getTotalThroughput() const { return totalThroughput; }
  unsigned getThroughput(const unsigned streetIndex) const { return throughput[streetIndex]; }

  double getTotalThroughputWithPriority() const { return totalThroughputWithPriority; }
  double getThroughputWithPriority(const unsigned streetIndex) const { return throughputWithPriority[streetIndex]; }

  unsigned getTotalWaitTime() const { return totalWaitTime; }
  unsigned getWaitTime(const unsigned streetIndex) const { return waitTime[streetIndex]; }

  double getTotalWaitTimeWithPriority() const { return totalWaitTimeWithPriority; }
  double getWaitTimeWithPriority(const unsigned streetIndex) const { return waitTimeWithPriority[streetIndex]; }
};

#endif