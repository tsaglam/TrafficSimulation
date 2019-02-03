#ifndef MODEL_SYNCER_H
#define MODEL_SYNCER_H

#include <vector>

#include "BucketList.h"
#include "DomainModel.h"
#include "LowLevelCar.h"
#include "SimulationData.h"

#define VEHICLE_LENGTH 5.0
#define TRAFFIC_LIGHT_OFFSET 35.0 / 2.0

template <template <typename Vehicle> typename RfbStructure>
class ModelSyncer {
private:
  using Data = SimulationData<RfbStructure>;

private:
  Data &data;

  /**
   * @brief      Sets a signal of the low level street correlating to a specific domain model street.
   */
  void setLowLevelSignal(Signal signal, Street *domainModelStreet) {
    LowLevelStreet<RfbStructure> &street = data.getStreet(domainModelStreet->getId());
    street.setSignal(signal);
  }

public:
  ModelSyncer(Data &_data) : data(_data) {}

  void buildFreshLowLevel() {
    auto &streets     = data.getStreets();
    auto &domainModel = data.getDomainModel();

    LowLevelCar trafficLightCar(0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // Clear streets, start fresh
    streets.clear();

    // Compute optimal bucket section length as '2 * total street length / total car count'
    if (domainModel.getVehicles().size() > 0) {
      unsigned long totalStreetLength = 0;
      for (const auto &street : domainModel.getStreets()) { totalStreetLength += street->getLength(); }
      BucketListSectionLength = 2 * totalStreetLength / domainModel.getVehicles().size();
    }

    for (const auto &domainStreet : domainModel.getStreets()) {
      streets.emplace_back(domainStreet->getId(), domainStreet->getLanes(), domainStreet->getLength(),
          domainStreet->getSpeedLimit(), trafficLightCar, TRAFFIC_LIGHT_OFFSET);
    }

    for (const auto &domainVehicle : domainModel.getVehicles()) {
      const double accelerationDivisor =
          2.0 * std::sqrt(domainVehicle->getMaxAcceleration() * domainVehicle->getTargetDeceleration());

      LowLevelCar car(domainVehicle->getId(), domainVehicle->getExternalId(), domainVehicle->getTargetVelocity(),
          domainVehicle->getMaxAcceleration(), accelerationDivisor, domainVehicle->getMinDistance(),
          domainVehicle->getTargetHeadway(), domainVehicle->getPoliteness(), VEHICLE_LENGTH,
          domainVehicle->getPosition().getLane(), domainVehicle->getPosition().getDistance());

      auto &street = streets.at(domainVehicle->getPosition().getStreet()->getId());

      street.insertCar(car);
    }

    for (auto &street : streets) { street.incorporateInsertedCars(); }

    // Init signals on low level streets:
    for (const auto &domainJunction : domainModel.getJunctions()) {
      Junction::Signal currentSignal = domainJunction->getCurrentSignal();
      for (const auto &connectedStreet : domainJunction->getIncomingStreets()) {
        if (connectedStreet.isConnected()) {
          if (connectedStreet.getDirection() == currentSignal.getDirection()) {
            setLowLevelSignal(Signal::GREEN, connectedStreet.getStreet());
          } else {
            setLowLevelSignal(Signal::RED, connectedStreet.getStreet());
          }
        }
      }
    }
  }

  void writeVehiclePositionToDomainModel() {
    auto &domainModel = data.getDomainModel();

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
