#ifndef LOW_LEVEL_CAR_H
#define LOW_LEVEL_CAR_H

class LowLevelCar {
private:
  unsigned int id;
  unsigned int externalId;

  /**
   * Static properties.
   */

  double targetVelocity;
  double maxAcceleration;
  double targetDeceleration;
  double minDistance;
  double targetHeadway;
  double politeness;
  double length;

  /**
   * Dynamic properties for access to current values by readers-only and to retrieve the current values during
   * computation of the values for the next step.
   */

  unsigned int currentLane;
  double currentDistance;
  double currentVelocity;

  // Move parts of the static block here to prevent false sharing?
  // On CUDA: The cache line size is generally 128 byte (L1) and 32 byte (L2).

  /**
   * Dynamic properties used by computation routines to store intermittent results.
   */

  double nextBaseAcceleration; // Re-used by other cars.
  unsigned int nextLane;
  double nextDistance;
  double nextVelocity;

public:
  LowLevelCar() = default;
  LowLevelCar(unsigned int _id, unsigned int _externalId, double _targetVelocity, double _maxAcceleration,
      double _targetDeceleration, double _minDistance, double _targetHeadway, double _politeness, double _length)
      : id(_id), externalId(_externalId), targetVelocity(_targetVelocity), maxAcceleration(_maxAcceleration),
        targetDeceleration(_targetDeceleration), minDistance(_minDistance), targetHeadway(_targetHeadway),
        politeness(_politeness), length(_length) {}
  LowLevelCar(unsigned int _id, unsigned int _externalId, double _targetVelocity, double _maxAcceleration,
      double _targetDeceleration, double _minDistance, double _targetHeadway, double _politeness, double _length,
      unsigned int _lane, double _distance, double _velocity = 0.0)
      : LowLevelCar(_id, _externalId, _targetVelocity, _maxAcceleration, _targetDeceleration, _minDistance,
            _targetHeadway, _politeness, _length) {
    setPosition(_lane, _distance, _velocity);
  }

  void setPosition(unsigned int lane, double distance, double velocity = 0.0) {
    currentLane     = lane;
    currentDistance = distance;
    currentVelocity = velocity;
  }

public:
  /*
   * Interface for RfbStructure and computation routines.
   */

  unsigned int getExternalId() const { return externalId; }
  unsigned int getLane() const { return currentLane; }
  double getDistance() const { return currentDistance; }
  double getVelocity() const { return currentVelocity; }

  /**
   * Update sets the fields containing the instance's dynamic properties respecting time to the values of the next time
   * step. This means all values of dynamic property fields for the current time step (current*) are re-written to the
   * values of the next time step (next*).
   *
   * The dynamic property fields are: {current,next}Lane, {current,next}Distance, {current,next}Velocity.
   *
   * Semantically, update() corresponds to the car progressing to the next time step; the values computed for the next
   * time step are then available through getDistance(), ...
   */
  void update() {
    currentLane     = nextLane;
    currentDistance = nextDistance;
    currentVelocity = nextVelocity;
  }

public:
  /*
   * Interface for computation routines.
   */

  unsigned int getId() const { return id; }
  double getTargetVelocity() const { return targetVelocity; }
  double getMaxAcceleration() const { return maxAcceleration; }
  double getTargetDeceleration() const { return targetDeceleration; }
  double getMinDistance() const { return minDistance; }
  double getTargetHeadway() const { return targetHeadway; }
  double getPoliteness() const { return politeness; }
  double getLength() const { return length; }

  void setNextBaseAcceleration(double acceleration) { nextBaseAcceleration = acceleration; }
  double getNextBaseAcceleration() const { return nextBaseAcceleration; }
  void setNext(unsigned int lane, double distance, double velocity) {
    nextLane     = lane;
    nextDistance = distance;
    nextVelocity = velocity;
  }
};

#endif
