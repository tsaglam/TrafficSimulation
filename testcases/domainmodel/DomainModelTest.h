#include "DomainModel.h"
#include "DomainModelTestFactory.h"
#include "Vehicle.h"

bool modelCreationTest() {
  DomainModel model = DomainModel();
  model.addVehicle(createTestVehicle());
  model.addJunction(createTestJunction());
  model.addStreet(createTestStreet());
  return model.getVehicles().size() == 1 && model.getStreets().size() == 1 && model.getJunctions().size() == 1;
}