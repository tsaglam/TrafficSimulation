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
class Parallel_SIMD_IDMRoutine : public SIMD_IDMRoutine<RfbStructure> {
  using car_iterator            = typename LowLevelStreet<RfbStructure>::iterator;
  using AccelerationComputerRfb = AccelerationComputer<RfbStructure>;
  using LaneChangeValues        = typename IDMRoutine<RfbStructure>::LaneChangeValues;

public:
  using SIMD_IDMRoutine<RfbStructure>::SIMD_IDMRoutine;

  void perform() {
// #ifdef _OPENMP
//     unsigned int customBlockSize = streetIds.size() / omp_get_max_threads();
// #endif
auto &dataRef = this->data;
#pragma omp parallel for shared(dataRef) schedule(static) //, customBlockSize)
    for (unsigned i = 0; i < dataRef.getStreets().size(); i++) { this->processStreet(dataRef.getStreet(i)); }
  }
};

#endif
