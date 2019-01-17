#ifndef CONSISTENCY_LIGHT_ROUTINE_H
#define CONSISTENCY_LIGHT_ROUTINE_H

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "RfbStructure.h"
#include "SimulationData.h"

template <template <typename Vehicle> typename RfbStructure>
class ConsistencyRoutine {
public:
  /**
   * @brief      Ensures model consistency.
   * @param      data  Is the simulaton data for the routine.
   */
  ConsistencyRoutine(SimulationData<RfbStructure> &data) : data(data) {}

  /**
   * @brief      Ensures model consistency, updates cars that change streets (and their correlating street).
   */
  void perform() {
    DomainModel &model = data.getDomainModel();
    for (auto &street : data.getStreets()) {
      // access domain model information for the low level street:
      Street &domStreet                 = model.getStreet(street.getId());
      Junction &domJunction             = domStreet.getTargetJunction();
      CardinalDirection originDirection = calculateOriginDirection(domJunction, domStreet.getSourceJunction());
      // update low level  cars and restore consistency:
      street.updateCarsAndRestoreConsistency();
      // for every low level car that changes streets:
      auto beyondsIterable = street.beyondsIterable();
      for (auto vehicle = beyondsIterable.begin(); vehicle != beyondsIterable.end(); ++vehicle) {
        // get domain model car and desired domain model destination street:
        Vehicle &domVehicle                    = model.getVehicle(vehicle->getId());
        CardinalDirection destinationDirection = takeTurn(originDirection, domVehicle.getNextDirection());
        Street *domDestinationStreet           = domJunction.getOutgoingStreet(destinationDirection).getStreet();
        // insert car on correlating low level destination street:
        LowLevelStreet<RfbStructure> &destinationStreet = data.getStreet(domDestinationStreet->getId());
        destinationStreet.insertCar(*vehicle);
      }
      // remove all leaving cars from current street:
      street.removeBeyonds();
    }
    // finally, incorperate every new car for every street:
    for (auto &street : data.getStreets()) { street.incorporateInsertedCars(); }
  }

  /**
   * @brief      Calculates the resuting direction of a turn.
   * @param[in]  origin  The the direction where the car is coming from.
   * @param[in]  turn    The direction of the turn.
   * @return     The direction where the car is going.
   */
  CardinalDirection takeTurn(CardinalDirection origin, TurnDirection turn) {
    return (CardinalDirection)(origin + turn) % 4;
  }

  /**
   * @brief      Determines the cardinal direction where a street is coming from related to the current junction.
   * @param      current  The current junction.
   * @param      origin   The origin junction where the street is also connected to.
   * @return     The origin cardinal direction related to the current junction.
   */
  CardinalDirection calculateOriginDirection(Junction &current, Junction &origin) {
    if (current.getX() < origin.getX()) {
      return CardinalDirection::EAST;
    } else if (current.getX() > origin.getX()) {
      return CardinalDirection::WEST;
    } else if (current.getY() < origin.getY()) {
      return CardinalDirection::SOUTH;
    } else if (current.getY() > origin.getY()) {
      return CardinalDirection::NORTH;
    } 
    throw std::invalid_argument("Junctions cannot be placed at the same spot!");
  }

  SimulationData<RfbStructure> &data;
};

#endif
