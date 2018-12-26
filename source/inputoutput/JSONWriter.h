#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <exception>
#include <ostream>
#include <string>

#include "DomainModel.h"

class JSONWriter {
public:
  class Exception : public std::exception {
  protected:
    std::string whatStr;

  public:
    Exception(const char *what);
    virtual const char *what() const throw();
  };

private:
  std::ostream &out;

public:
  JSONWriter(std::ostream &out);
  void writeVehicles(DomainModel &domainModel);
};

#endif
