#ifndef MODEL_SYNCER_H
#define MODEL_SYNCER_H

#include <vector>

#include "DomainModel.h"
#include "LowLevelCar.h"
#include "SimulationData.h"

#define VEHICLE_LENGTH 5.0
#define TRAFFIC_LIGHT_OFFSET 35.0 / 2.0

template <typename RfbStructure>
class ModelSyncer {
private:
  using Data = SimulationData<RfbStructure>;
  using Car  = Data::Car; // TODO: sensible?

private:
  Data &data;

public:
  ModelSyncer(Data _data) : data(_data) {}

  void buildFreshLowLevel() {
    auto streets     = data.getStreets();
    auto domainModel = data.getDomainModel();

    // Clear streets, start fresh
    streets.clear();

    for (const auto &domainStreet : domainModel.getStreets()) {
      streets.emplace_back(domainStreet.getId(), domainStreet.getLanes(), domainStreet.getLength(),
          domainStreet.getSpeedLimit(), TRAFFIC_LIGHT_OFFSET);
    }

    for (const auto &domainVehicle : domainModel.getVehicles()) {
      // TODO: could use SimulationData<RfbStructure>::Car (mapped to Car), will no longer require LowLevelCar.h
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
    auto domainModel = data.getDomainModel();

    for (const auto &street : data.getStreets()) {
      auto &domainStreet = domainModel.getStreet(street.getId());

      for (const auto &car : street.allIterable()) {
        auto &domainVehicle = domainModel.getVehicle(car.getId());
        domainVehicle.setPosition(domainStreet, car.getLane(), car.getDistance());
      }
    }
  }
};

#endif
