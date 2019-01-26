#ifndef P_CONSISTENCY_LIGHT_ROUTINE_H
#define P_CONSISTENCY_LIGHT_ROUTINE_H

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "RfbStructure.h"
#include "SimulationData.h"
#include <algorithm>
#ifdef OMP
#include <omp.h>
#endif

template <template <typename Vehicle> typename RfbStructure>
class ParallelConsistencyRoutine {
public:
  /**
   * @brief      Ensures model consistency.
   * @param      data  Is the simulaton data for the routine.
   */
  ParallelConsistencyRoutine(SimulationData<RfbStructure> &data) : data(data) {}

  /**
   * @brief      Ensures model consistency, updates cars that change streets (and their correlating street).
   */
  void perform() {
#pragma omp parallel for
    for (std::size_t i = 0; i < data.getStreets().size(); i++) {
      auto &street = data.getStreets()[i];
      street.updateCarsAndRestoreConsistency();
    }

    DomainModel &model = data.getDomainModel();
    // create openmp locks:
    // #ifdef OMP
    //     omp_lock_t lock[4];
    //     for (int i = 0; i < 4; i++) { omp_init_lock(&(lock[i])); }
    // #endif
    for (auto &street : data.getStreets()) {
      // access domain model information for the low level street:
      Street &domStreet                 = model.getStreet(street.getId());
      Junction &domJunction             = domStreet.getTargetJunction();
      CardinalDirection originDirection = calculateOriginDirection(domJunction, domStreet);
      // for every low level car that changes streets:
      auto beyondsIterable        = street.beyondsIterable();
      const unsigned iteratorSize = beyondsIterable.end() - beyondsIterable.begin();
      //#pragma omp parallel for shared(lock, model, domJunction, originDirection, beyondsIterable, data)
      for (unsigned i = 0; i < iteratorSize; ++i) {
        auto vehicleIt = beyondsIterable.begin() + i;
        // get domain model car and desired/available domain model destination street:
        Vehicle &domVehicle                    = model.getVehicle(vehicleIt->getId());
        CardinalDirection destinationDirection = takeTurn(originDirection, domVehicle.getNextDirection());
        while (!domJunction.getOutgoingStreet(destinationDirection).isConnected()) {
          destinationDirection = CardinalDirection((destinationDirection + 1) % 4);
        }
        Street *domDestinationStreet = domJunction.getOutgoingStreet(destinationDirection).getStreet();
        // Copy the vehicle and adjust distance:
        LowLevelCar vehicle = *vehicleIt;
        int newLane         = std::min(vehicle.getLane(), domDestinationStreet->getLanes() - 1);
        vehicle.setNext(newLane, vehicle.getDistance() - street.getLength(), vehicle.getVelocity());
        // insert car on correlating low level destination street:
        LowLevelStreet<RfbStructure> &destinationStreet = data.getStreet(domDestinationStreet->getId());
        // #ifdef OMP
        //         omp_set_lock(&(lock[destinationDirection]));
        // #endif
        destinationStreet.insertCar(vehicle);
        // #ifdef OMP
        //         omp_unset_lock(&(lock[destinationDirection]));
        // #endif
      }
      // remove all leaving cars from current street:
      street.removeBeyonds();
    }
    // #ifdef OMP
    //     for (int i = 0; i < 4; i++) { omp_destroy_lock(&(lock[i])); }
    // #endif

// finally, incorperate every new car for every street:
#pragma omp parallel for
    for (std::size_t i = 0; i < data.getStreets().size(); i++) {
      auto &street = data.getStreets()[i];
      street.incorporateInsertedCars();
    }
  }

  /**
   * @brief      Calculates the resuting direction of a turn.
   * @param[in]  origin  The the direction where the car is coming from.
   * @param[in]  turn    The direction of the turn.
   * @return     The direction where the car is going.
   */
  CardinalDirection takeTurn(CardinalDirection origin, TurnDirection turn) {
    return (CardinalDirection)((origin + turn) % 4);
  }

  /**
   * @brief      Determines the cardinal direction from where a street is coming to a junction.
   * @param      junction  The junction, point of view for the direction.
   * @param      incomingStreet   The street connected to the junction.
   * @return     The origin cardinal direction related to the current junction.
   */
  CardinalDirection calculateOriginDirection(Junction &junction, Street &incomingStreet) {
    for (const auto &connectedStreet : junction.getIncomingStreets()) {
      if (connectedStreet.isConnected() && connectedStreet.getStreet()->getId() == incomingStreet.getId()) {
        return connectedStreet.getDirection();
      }
    }
    throw std::invalid_argument("Street is not connected to junction!");
  }

  SimulationData<RfbStructure> &data;
};

#endif
