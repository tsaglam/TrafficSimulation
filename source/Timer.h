#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

class Timer {
  using time_point  = std::chrono::time_point<std::chrono::high_resolution_clock>;
  using nanoseconds = std::chrono::nanoseconds;

  time_point lastTime;
  unsigned long totalNanoseconds = 0;
  unsigned long timeCount        = 0;
  bool on                        = false;

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
    totalNanoseconds += std::chrono::duration_cast<nanoseconds>(lastTime - now).count();
  }

  double getAvgTimeInNanoseconds() const { return (double)totalNanoseconds / (double)timeCount; }
  unsigned long getTotalTimeInNanoseconds() const { return totalNanoseconds; }
  unsigned long getMeasurementCount() const { return timeCount; }
};

void printTimerHeader() {
  std::cerr << std::setw(15) << "call_count" << std::setw(15) << "total_time_ns" << std::setw(15) << "avg_time_ns"
            << std::setw(30) << "description"
            << "\n";
}

void printTimer(const Timer &timer, const std::string &description) {
  std::cerr << std::setw(15) << timer.getMeasurementCount() << std::setw(15) << timer.getTotalTimeInNanoseconds()
            << std::setw(15) << std::fixed << std::setprecision(3) << timer.getAvgTimeInNanoseconds() << std::setw(30)
            << description << "\n";
}

#endif