#include "DomainModel.h"
#include <iostream>

void simulateStep(DomainModel &model) {
  for (Junction &junction : model.getJunctions()) {
    bool lightChanged = junction.nextStep();

    // for (Junction::Signal &signal : junction.getSignals()) {
    //   unsigned int time = signal.getTime() - 1;
    //   if (time == 0) {
    //     // SET GREEN!!!
    //   } else if (time < 0) {
    //     // set to timer_max
    //     // SET RED!!!!
    //   } else {
    //     signal.setTime(time);
    //   }
    //   std::cout << signal.getDirection() << " " << signal.getTime() << std::endl; // TODO remove me
    // }
  }
}