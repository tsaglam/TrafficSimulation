#include "DomainModel.h"
#include "DomainModelTestFactory.h"
#include "Vehicle.h"
#include <../../snowhouse/snowhouse.h>

using namespace snowhouse;

void modelCreationTest() {
  DomainModel model = DomainModel();
  model.addVehicle(createTestVehicle());
  model.addJunction(createTestJunction());
  model.addStreet(createTestStreet());
  AssertThat(model.getVehicles().size(), Is().EqualTo((unsigned int)1));
  AssertThat(model.getStreets().size(), Is().EqualTo((unsigned int)1));
  AssertThat(model.getJunctions().size(), Is().EqualTo((unsigned int)1));
}