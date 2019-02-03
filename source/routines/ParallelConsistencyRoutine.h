#ifndef P_CONSISTENCY_LIGHT_ROUTINE_H
#define P_CONSISTENCY_LIGHT_ROUTINE_H

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "RfbStructure.h"
#include "SimulationData.h"
#include "Timer.h"
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
#ifdef TIMER
    consistencyRoutine_restoreConsistency_timer.start();
    restoreConsistency(); // street-wise parallel
    consistencyRoutine_restoreConsistency_timer.stop();
    consistencyRoutine_relocateCars_timer.start();
    relocateCars(); // sequential
    consistencyRoutine_relocateCars_timer.stop();
    consistencyRoutine_incorporateCars_timer.start();
    incorporateCars(); // street-wise parallel
    consistencyRoutine_incorporateCars_timer.stop();
#else
    restoreConsistency(); // street-wise parallel
    relocateCars();       // sequential
    incorporateCars();    // street-wise parallel
#endif
  }

  /**
   * @brief      1. Updates cars and restores consistency for every street.
   */
  void restoreConsistency() {
#pragma omp parallel for shared(data)
    for (std::size_t i = 0; i < data.getStreets().size(); i++) {
      auto &street = data.getStreets()[i];
      street.updateCarsAndRestoreConsistency();
    }
  }

  /**
   * @brief      2. Relocate the cars that change streets to the right streets and lanes.
   */
  void relocateCars() {
    DomainModel &model = data.getDomainModel();
    for (auto &street : data.getStreets()) {
      // access domain model information for the low level street:
      Street &domStreet                 = model.getStreet(street.getId());
      Junction &domJunction             = domStreet.getTargetJunction();
      CardinalDirection originDirection = calculateOriginDirection(domJunction, domStreet);
      // for every low level car that changes streets:
      auto beyondsIterable = street.beyondsIterable();
      for (auto vehicleIt = beyondsIterable.begin(); vehicleIt != beyondsIterable.end(); ++vehicleIt) {
        LowLevelCar &vehicle = *vehicleIt;
        relocateCar(vehicle, street, domJunction, originDirection);
      }
      // remove all leaving cars from current street:
      street.removeBeyonds();
    }
  }

  /**
   * @brief      3. Incorperate every new car of every street into its data structure.
   */
  void incorporateCars() {
#pragma omp parallel for shared(data)
    for (std::size_t i = 0; i < data.getStreets().size(); i++) {
      auto &street = data.getStreets()[i];
      street.incorporateInsertedCars();
    }
  }

  /**
   * @brief      Relocates a single car that change streets.
   * @param      vehicle          The vehicle iterator, which points to the car.
   * @param      street           The street where the car is coming from.
   * @param      domJunction      The junction where the car changes streets.
   * @param[in]  originDirection  The origin direction from where the car is coming from.
   */
  void relocateCar(LowLevelCar &vehicle, LowLevelStreet<RfbStructure> &street, Junction &domJunction,
      CardinalDirection originDirection) {
    DomainModel &model = data.getDomainModel();
    // get domain model car and desired/available domain model destination street:
    Vehicle &domVehicle                    = model.getVehicle(vehicle.getId());
    CardinalDirection destinationDirection = takeTurn(originDirection, domVehicle.getNextDirection());
    while (!domJunction.getOutgoingStreet(destinationDirection).isConnected()) {
      destinationDirection = CardinalDirection((destinationDirection + 1) % 4);
    }
    Street *domDestinationStreet = domJunction.getOutgoingStreet(destinationDirection).getStreet();
    // Copy the vehicle and adjust distance:
    int newLane = std::min(vehicle.getLane(), domDestinationStreet->getLanes() - 1);
    vehicle.setNext(newLane, vehicle.getDistance() - street.getLength(), vehicle.getVelocity());
    // insert car on correlating low level destination street:
    LowLevelStreet<RfbStructure> &destinationStreet = data.getStreet(domDestinationStreet->getId());
    destinationStreet.insertCar(vehicle);
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
