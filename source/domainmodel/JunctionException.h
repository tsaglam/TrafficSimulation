#include "Junction.h"
#include <string>

/**
 * @brief      Exception for signaling junction errors.
 */
class JunctionException : public std::exception {

public:
  /**
   * @brief      Constructs the object.
   * @param[in]  junction  Is the junction in which this exception is caused.
   * @param[in]  message   Is the custom exception message which is passed along with the junction information.
   */
  JunctionException(Junction junction, const std::string message);

  /**
   * @brief      Returns the full exception message.
   * @return     the junction ID, the junction coordinates and the custom exception message.
   */
  virtual const char *what() const throw();

private:
  std::string fullMessage;
};