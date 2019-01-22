#include "DomainModel.h"
#include "DomainModelTestFactory.h"
#include "Vehicle.h"
#include <../../snowhouse/snowhouse.h>

using namespace snowhouse;

void modelCreationTest() {
  DomainModel model = DomainModel();
  // create model elements, add them to model and remember their IDs:
  id_type vehicleId  = model.addVehicle(createTestVehicle()).getId();
  id_type junctionId = model.addJunction(createTestJunction()).getId();
  id_type streetId   = model.addStreet(createTestStreet()).getId();
  // check the ID specific access:
  AssertThat(model.getVehicle(vehicleId).getId(), Is().EqualTo(vehicleId));
  AssertThat(model.getJunction(junctionId).getId(), Is().EqualTo(junctionId));
  AssertThat(model.getStreet(streetId).getId(), Is().EqualTo(streetId));
}

void modelCreationTest2() {
  DomainModel model = DomainModel();
  // create 10 model elements each and add them to model:
  for (int i = 0; i < 10; ++i) {
    model.addVehicle(createTestVehicle());
    model.addJunction(createTestJunction());
    model.addStreet(createTestStreet());
  }
  // check the iterative access:
  AssertThat(model.getVehicles().size(), Is().EqualTo((unsigned int)10));
  AssertThat(model.getStreets().size(), Is().EqualTo((unsigned int)10));
  AssertThat(model.getJunctions().size(), Is().EqualTo((unsigned int)10));
}

void resetAllVehiclesTest() {
  DomainModel model = DomainModel();
  // create 10 model elements each and add them to model:
  for (int i = 0; i < 10; ++i) {
    model.addVehicle(createTestVehicle());
  }
  // change and verify position of every single one:
  for (auto const &vehicle : model.getVehicles()) {
    Vehicle::Position position = vehicle->getPosition();
    vehicle->setPosition(*position.getStreet(), 1, 44.4);
    AssertThat( vehicle->getPosition().getDistance(), Is().EqualTo(44.4));
  }
  // reset to starting point and verify:
  model.resetVehiclePositions();
  for (auto const &vehicle : model.getVehicles()) {
    AssertThat( vehicle->getPosition().getDistance(), Is().EqualTo(33.3));
  }
  
}