#include "JSONWriter.h"

#include <exception>
#include <iostream>

#include "json.hpp"

#include "DomainModel.h"

using json = nlohmann::json;

JSONWriter::Exception::Exception(const char *_what) : whatStr(_what) {}

const char *JSONWriter::Exception::what() const throw() { return whatStr.c_str(); }

JSONWriter::JSONWriter(std::ostream &_out) : out(_out) {}

void JSONWriter::writeVehicles(DomainModel &domainModel) {
  json output;
  for (const auto &vehicle : domainModel.getVehicles()) {
    json outputCar;

    outputCar["id"] = vehicle->getExternalId();
    // junction IDs
    outputCar["from"] = vehicle->getPosition().getStreet()->getSourceJunction().getExternalId();
    outputCar["to"]   = vehicle->getPosition().getStreet()->getTargetJunction().getExternalId();
    outputCar["lane"] = vehicle->getPosition().getLane();
    // Output in m
    outputCar["position"] = vehicle->getPosition().getDistance();

    output["cars"].push_back(outputCar);
  }
  out << output.dump() << "\n";
}
