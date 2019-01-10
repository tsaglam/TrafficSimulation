#ifndef NULL_ROUTINE_H
#define NULL_ROUTINE_H

#include "LowLevelCar.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class NullRoutine {
public:
  NullRoutine(SimulationData<RfbStructure> &) {}
  void perform() {}
};

#endif
