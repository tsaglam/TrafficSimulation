#ifndef JUNCTION_H
#define JUNCTION_H

#include <array>
#include <vector>

#include "DomainModelCommon.h"
#include "Street.h"

class Street;

class Junction {
private:
  friend class DomainModel;

public:
  class Signal {
  private:
    CardinalDirection direction;
    unsigned int time;

  public:
    Signal() = default;
    Signal(CardinalDirection direction, unsigned int time);
  };

private:
  class ConnectedStreet {
  private:
    friend class Junction;

    bool isConnected;
    Street* street;
    /**
     * @TODO: Could be omitted, depends on implementation of signaling.
     */
    CardinalDirection direction;

  public:
    ConnectedStreet() = default;
    ConnectedStreet(bool isConnected, Street* street,
                    CardinalDirection direction);
  };

private:
  id_type id;
  int externalId;
  int x;
  int y;
  std::vector<Signal> signals;
  std::array<ConnectedStreet, 4> incomingStreets = {{
      ConnectedStreet(false, nullptr, NORTH),
      ConnectedStreet(false, nullptr, EAST),
      ConnectedStreet(false, nullptr, SOUTH),
      ConnectedStreet(false, nullptr, WEST)
    }};
  std::array<ConnectedStreet, 4> outgoingStreets = {{
    ConnectedStreet(false, nullptr, NORTH),
    ConnectedStreet(false, nullptr, EAST),
    ConnectedStreet(false, nullptr, SOUTH),
    ConnectedStreet(false, nullptr, WEST)
  }};
  /**
   * @TODO: Track current signaling state here?
   */
  // unsigned int timer;

 public:
  Junction(id_type id, int externalId, int x, int y,
           const std::vector<Signal>& signals);
  Junction(id_type id, int externalId, int x, int y,
           std::vector<Signal>&& signals);
  void addIncomingStreet(Street& street, CardinalDirection direction);
  void addOutgoingStreet(Street& street, CardinalDirection direction);
};

#endif
