#include "JunctionException.h"
#include <iostream>
#include <sstream>

JunctionException::JunctionException(Junction junction, const std::string message) {
  std::stringstream result;
  result << "Junction " << junction.getId() << " at (" << junction.getX() << "|" << junction.getY() << "): " << message;
  fullMessage = result.str();
}

const char *JunctionException::what() const noexcept { return fullMessage.c_str(); }