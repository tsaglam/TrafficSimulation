#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "ModelSyncer.h"

template <typename RfbStructure>
class ModelSyncer;

/**
 * C++20 ComputionRoutine concept, must be fulfilled in order for a class to be valid as a SignalingRoutine, IDMRoutine
 * or ConsistencyRoutine template parameter for Simulator.
 *
 * TODO: The ComputationRoutine concept is invalid, fix or remove.
 */
// template <typename T>
// concept ComputationRoutine = requires(T a) {
//   { T() };
//   { T(Simulator<...>::Accessor()) };
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
 *     Constructor(Simulator<RfbStructure>::Accessor accessor);
 *     void perform();
 */
template <typename RfbStructure, typename SignalingRoutine, typename IDMRoutine, typename ConsistencyRoutine>
class Simulator {
public:
  typedef LowLevelCar Car;
  typedef typename LowLevelStreet<RfbStructure> Street;
  typedef typename RfbStructure<Car> ConcreteRfbStructure;

public:
  /**
   * The Accessor class serves as the downward-facing API of the Simulator.
   * Computation routines should exclusively use Accessor to operate on domain model as well as low level
   * representation.
   */
  class Accessor {
  private:
    Simulator &simulator;
    Accessor(Simulator &_simulator) : simulator(_simulator) {}

  public:
    Street &getStreet(unsigned int id) const { return simulator.streets.at(id); }
    std::vector<Street> &getStreets() const { return simulator.streets; }
    DomainModel &getDomainModel() const { return simulator.domainModel; }
  };

private:
  friend class Accessor;

  DomainModel &domainModel;
  bool lowLevelInitialised;
  SignalingRoutine signalingRoutine;
  IDMRoutine idmRoutine;
  ConsistencyRoutine consistencyRoutine;
  std::vector<Street> streets;

private:
  void initialiseLowLevel() {
    ModelSyncer(Accessor(*this)).buildFreshLowLevel();

    lowLevelInitialised = true;
  }

  void writeChangesToDomainModel() { ModelSyncer(Accessor(*this)).writeVehiclePositionToDomainModel(); }

  void computeStep() {
    signalingRoutine.perform();
    idmRoutine.perform();
    consistencyRoutine.perform();
  }

public:
  Simulator(DomainModel &_domainModel)
      : domainModel(_domainModel), lowLevelInitialised(false), signalingRoutine(Accessor(*this)),
        idmRoutine(Accessor(*this)), consistencyRoutine(Accessor(*this)) {}

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
};

#endif
