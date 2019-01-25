#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "ModelSyncer.h"
#include "SimulationData.h"

/**
 * C++20 ComputionRoutine concept, must be fulfilled in order for a class to be valid as a SignalingRoutine, IDMRoutine
 * or ConsistencyRoutine template parameter for Simulator.
 *
 * TODO: The ComputationRoutine concept is invalid, fix or remove.
 */
// template <typename T>
// concept ComputationRoutine = requires(T a) {
//   { T() };
//   { T(SimulationData<...>()) };
//   { a.perform() }
// };

/**
 * Simulator controls the simulation process.
 * The template parameters SignalingRoutine, IDMRoutine and ConsistencyRoutine are used as the computation routines.
 *
 * The RfbStructure template parameter must adhere to the class interface described in RfbStructure.h.
 *
 * The computation routine template parameters must be classes implementing the following methods:
 *
 *     Constructor();
 *     Constructor(SimulationData<RfbStructure> &data);
 *     void perform();
 */
template <template <typename Vehicle> typename RfbStructure,
    template <template <typename Vehicle> typename _RfbStructure> typename SignalingRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename IDMRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename OptimizationRoutine,
    template <template <typename Vehicle> typename _RfbStructure> typename ConsistencyRoutine>
class Simulator {
private:
  SimulationData<RfbStructure> data;

  bool lowLevelInitialised;

  SignalingRoutine<RfbStructure> signalingRoutine;
  IDMRoutine<RfbStructure> idmRoutine;
  OptimizationRoutine<RfbStructure> optimizationRoutine;
  ConsistencyRoutine<RfbStructure> consistencyRoutine;

private:
  void initialiseLowLevel() {
    ModelSyncer<RfbStructure>(data).buildFreshLowLevel();

    lowLevelInitialised = true;
  }

  void writeChangesToDomainModel() { ModelSyncer<RfbStructure>(data).writeVehiclePositionToDomainModel(); }

  void computeStep() {
    signalingRoutine.perform();
    idmRoutine.perform();
    optimizationRoutine.perform();
    consistencyRoutine.perform();
  }

public:
  Simulator(DomainModel &_domainModel)
      : data(_domainModel), lowLevelInitialised(false), signalingRoutine(data), idmRoutine(data),
        optimizationRoutine(data), consistencyRoutine(data) {}

  void performStep() {
    if (!lowLevelInitialised) initialiseLowLevel();

    computeStep();

    writeChangesToDomainModel();
  }

  void performSteps(unsigned int n) {
    if (!lowLevelInitialised) initialiseLowLevel();

    for (unsigned int i = 0; i < n; ++i) { computeStep(); }

    writeChangesToDomainModel();
  }

  const SimulationData<RfbStructure> &getData() const { return data; }

  const SignalingRoutine<RfbStructure> &getSignalingRoutine() const { return signalingRoutine; }
  const IDMRoutine<RfbStructure> &getIDMRoutine() const { return idmRoutine; }
  const OptimizationRoutine<RfbStructure> &getOptimizationRoutine() const { return optimizationRoutine; }
  const ConsistencyRoutine<RfbStructure> &getConsistencyRoutine() const { return consistencyRoutine; }

  OptimizationRoutine<RfbStructure> &getOptimizationRoutine() { return optimizationRoutine; }
};

#endif
