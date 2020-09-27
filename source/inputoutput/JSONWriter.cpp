#include "JSONWriter.h"

#include <exception>
#include <iostream>

#include "../../json/single_include/nlohmann/json.hpp"

#include "DomainModel.h"

using json = nlohmann::json;

JSONWriter::Exception::Exception(const char *_what) : whatStr(_what) {}

const char *JSONWriter::Exception::what() const throw() { return whatStr.c_str(); }

JSONWriter::JSONWriter(std::ostream &_out) : out(_out) {}

void JSONWriter::writeVehicles(DomainModel &domainModel) {
  json output;
  output["cars"] = json(json::value_t::array);

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

void JSONWriter::writeSignals(DomainModel &domainModel) {
  json output;
  output["junctions"] = json(json::value_t::array);

  for (const auto &junction : domainModel.getJunctions()) {
    json outputJunction;

    outputJunction["id"] = junction->getExternalId();
    // signals
    outputJunction["signals"] = json(json::value_t::array);
    json &outputSignals       = outputJunction["signals"];
    for (const auto &signal : junction->getSignals()) {
      json outputSignal;

      outputSignal["dir"]  = static_cast<int>(signal.getDirection());
      outputSignal["time"] = signal.getDuration();

      outputSignals.push_back(outputSignal);
    }

    output["junctions"].push_back(outputJunction);
  }
  out << output.dump() << "\n";
}
