#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

template <typename timeUnit>
class Timer {
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
  time_point lastTime;
  unsigned long totalTime = 0;
  unsigned long timeCount = 0;
  bool on                 = false;

public:
  Timer() = default;

  void start() {
    lastTime = std::chrono::high_resolution_clock::now();
    on       = true;
  }
  void stop() {
    if (!on) { return; }
    on             = false;
    time_point now = std::chrono::high_resolution_clock::now();
    auto duration  = std::chrono::duration_cast<timeUnit>(now - lastTime).count();
    totalTime += duration;
    ++timeCount;
  }

  double getAvgTime() const { return (double)totalTime / (double)timeCount; }
  unsigned long getTotalTime() const { return totalTime; }
  unsigned long getMeasurementCount() const { return timeCount; }
};

void printTimerHeader() {
  std::cerr << std::setw(15) << "call_count" << std::setw(25) << "total_time" << std::setw(25) << "avg_time"
            << std::setw(50) << "description"
            << "\n";
}

template <typename timeUnit>
void printTimer(const Timer<timeUnit> &timer, const std::string &description) {
  std::cerr << std::setw(15) << timer.getMeasurementCount() << std::setw(25) << timer.getTotalTime() << std::setw(25)
            << std::fixed << std::setprecision(3) << timer.getAvgTime() << std::setw(50) << description << "\n";
}

#endif