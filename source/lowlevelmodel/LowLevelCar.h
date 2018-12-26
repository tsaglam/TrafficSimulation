#ifndef LOW_LEVEL_CAR_H
#define LOW_LEVEL_CAR_H

class LowLevelCar {
public:
  unsigned int id;

  /**
   * Static properties.
   */

  double targetVelocity;
  double maxAcceleration;
  double targetDeceleration;
  double minDistance;
  double targetHeadway;
  double politeness;
  double length; // Could be omitted, depends on traffic light car.

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
  LowLevelCar(unsigned int id, double targetVelocity, double maxAcceleration, double targetDeceleration,
      double minDistance, double targetHeadway, double politeness, double length);
  LowLevelCar(unsigned int id, double targetVelocity, double maxAcceleration, double targetDeceleration,
      double minDistance, double targetHeadway, double politeness, double length, unsigned int lane, double distance,
      double velocity = 0.0);

  void setPosition(unsigned int lane, double distance, double velocity = 0.0);

public:
  /*
   * Interface for RfbStructure and computation routines.
   *
   * Implementation should probably be included in the header to allow for compiler optimisations.
   */

  unsigned int getLane() const;
  double getDistance() const;
  double getVelocity() const;
  void applyUpdates();

public:
  /*
   * Interface for computation routines.
   *
   * Implementation should probably be included in the header to allow for compiler optimisations.
   */

  unsigned int getId() const;
  double getTargetVelocity() const;
  double getMaxAcceleration() const;
  double getTargetDeceleration() const;
  double getMinDistance() const;
  double getTargetHeadway() const;
  double getPoliteness() const;
  double length() const;

  void setNextBaseAcceleration(double acceleration);
  double getNextBaseAcceleration() const;
  void setNext(unsigned int lane, double distance, double velocity);
};

#endif
