#ifndef MODEL_SYNCER_H
#define MODEL_SYNCER_H

#include <vector>

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "LowLevelStreet.h"
#include "Simulator.h"

template <typename RfbStructure>
class Simulator;

#define VEHICLE_LENGTH 5.0
#define TRAFFIC_LIGHT_OFFSET 35.0 / 2.0

template <typename RfbStructure>
class ModelSyncer {
private:
  using Accessor = Simulator<RfbStructure>::Accessor;
  using Car      = Simulator<RfbStructure>::Car; // TODO: sensible?

private:
  Accessor accessor;

public:
  ModelSyncer(Accessor _accessor) : accessor(_accessor) {}

  void buildFreshLowLevel() {
    auto streets     = accessor.getStreets();
    auto domainModel = accessor.getDomainModel();

    // Clear streets, start fresh
    streets.clear();

    for (const auto &domainStreet : domainModel.getStreets()) {
      streets.emplace_back(domainStreet.getId(), domainStreet.getLanes(), domainStreet.getLength(),
          domainStreet.getSpeedLimit() TRAFFIC_LIGHT_OFFSET);
    }

    for (const auto &domainVehicle : domainModel.getVehicles()) {
      // TODO: could use Simulator<RfbStructure>::Car
      // Car car(...)
      LowLevelCar car(domainVehicle->getId(), domainVehicle->getTargetVelocity(), domainVehicle->getMaxAcceleration(),
          domainVehicle->getTargetDeceleration(), domainVehicle->getMinDistance(), domainVehicle->getTargetHeadway(),
          domainVehicle->getPoliteness(), VEHICLE_LENGTH, domainVehicle->getPosition().getLane(),
          domainVehicle->getPosition().getDistance());

      auto &street = streets.at(domainVehicle->getPosition().getStreet()->getId());

      street.insertCar(car);
    }

    for (const auto &street : streets) { street.incorporateInsertedCars(); }

    lowLevelInitialised = true;
  }

  void writeVehiclePositionToDomainModel() {
    auto domainModel = accessor.getDomainModel();

    for (const auto &street : accessor.getStreets()) {
      auto &domainStreet = domainModel.getStreet(street.getId());

      for (const auto &car : street.allIterable()) {
        auto &domainVehicle = domainModel.getVehicle(car.getId());
        domainVehicle.setPosition(domainStreet, car.getLane(), car.getDistance());
      }
    }
  }
};

#endif
