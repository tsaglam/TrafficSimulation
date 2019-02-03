#ifndef PARALLEL_SMDI_IDM_ROUTINE_H
#define PARALLEL_SMDI_IDM_ROUTINE_H

#include <immintrin.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "AccelerationComputer.h"
#include "SIMD_IDMRoutine.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class Parallel_SIMD_IDMRoutine : public SMDI_IDMRoutine<RfbStructure> {
  using car_iterator            = typename LowLevelStreet<RfbStructure>::iterator;
  using AccelerationComputerRfb = AccelerationComputer<RfbStructure>;
  using LaneChangeValues        = typename IDMRoutine<RfbStructure>::LaneChangeValues;

public:
  using SMDI_IDMRoutine<RfbStructure>::SMDI_IDMRoutine;

  void perform() {
// #ifdef _OPENMP
//     unsigned int customBlockSize = streetIds.size() / omp_get_max_threads();
// #endif
#pragma omp parallel for shared(this->data) schedule(static) //, customBlockSize)
    for (unsigned i = 0; i < this->data.getStreets().size(); i++) { this->processStreet(this->data.getStreet(i)); }
  }
};

#endif
