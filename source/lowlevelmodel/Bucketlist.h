#ifndef BUCKET_LIST_H
#define BUCKET_LIST_H

#include <vector>

template <class Car>
class BucketList {
private:
  /**
   * The number of lanes on the street (in the current direction).
   */
  const unsigned int laneCount;
  /**
   * The length of the street.
   */
  const double streetLength;
  /**
   * The distance represented by a single bucket.
   */
  const double sectionLength;

  // ------- Data Storage -------

  using Bucket = std::vector<Car>;
  /**
   * The street is partitioned into sections of equal length (sectionLength).
   * Each of these sections is represented by a bucket per lane containing all cars in that section and lane.
   * The cars within a bucket are not necessarily sorted.
   * For each car, the correct bucket is easily determined by its distance, its lane, and the section length.
   * Newly inserted cars are stored immediately in the correct bucket.
   */
  std::vector<Bucket> buckets;

  /**
   * All cars that left this street (i.e. their distance is greater than the street length).
   */
  Bucket departedCars;

  /**
   * The number of cars currently on the street. Includes cars inserted via insertCar even before
   * incorporateInsertedCars is called. Does not include cars beyond this street.
   */
  unsigned carCount = 0;

private:
  /**
   * @brief      Determine the index of the bucket representing the street at the given distance and lane.
   *
   * @param[in]  lane      The lane in [0, laneCount)
   * @param[in]  distance  The distance from the start of the street in [0, streetLength]
   *
   * @return     Returns the index of the correct bucket in the buckets data structure.
   */
  inline unsigned int findBucketIndex(const unsigned int lane, const double distance) const {
    const unsigned int sectionIndex = distance / sectionLength; // divide and cut off
    return sectionIndex * laneCount + lane;                     // 2D to 1D index conversion
  }

public:
  // ------- Constructor -------
  /**
   * @brief      Default constructor. Valid, but results in unspecified behavior.
   */
  BucketList() = default;

  /**
   * @brief      Proper constructor, initializes a new instance with specified parameters.
   *
   * @param[in]  laneCount     The number of lanes on the street (in the current direction).
   * @param[in]  length        The length of the street.
   * @param[in]  sectionLength The length of section represented by a bucket
   */
  BucketList(const unsigned int laneCount, const double length, const double sectionLength = 1.0)
      : laneCount(laneCount), streetLength(length), sectionLength(sectionLength),
        buckets(std::ceil(length / sectionLength)) {}

  // ------- Iterator & Iterable type defs -------
  using BucketIterator      = typename Bucket::iterator;
  using ConstBucketIterator = typename Bucket::const_iterator;

  friend class CarIterator;
  friend class ConstCarIterator;
  friend class ReverseCarIterator;

  // TODO: ensure that initial currentBucketIt != end()
  // TODO: reduce code duplication with
  // https://stackoverflow.com/questions/2150192/how-to-avoid-code-duplication-implementing-const-and-non-const-iterators

  class ReverseCarIterator {
    BucketIterator currentBucketIt;
    typename std::vector<Bucket>::reverse_iterator bucketsIt;
    const typename std::vector<Bucket>::reverse_iterator bucketsEnd;

    bool alreadyIncremented = false;

  public:
    ReverseCarIterator() = default;
    ReverseCarIterator(BucketList &list, const unsigned int initialBucket)
        : currentBucketIt((list.buckets.rbegin() + (list.buckets.size() - initialBucket))->begin()),
          bucketsIt(list.buckets.rbegin() + (list.buckets.size() - initialBucket)), bucketsEnd(list.buckets.rend()) {}
    ReverseCarIterator(BucketList &list, const unsigned int initialBucket, BucketIterator currentBucketIt)
        : currentBucketIt(currentBucketIt), bucketsIt(list.buckets.begin() + initialBucket),
          bucketsEnd(list.buckets.end()) {}
    ReverseCarIterator(BucketList &list)
        : currentBucketIt(list.buckets.rbegin()->begin()), bucketsIt(list.buckets.rbegin()),
          bucketsEnd(list.buckets.rend()) {}
    ReverseCarIterator(BucketList &list, bool) // end iterator
        : currentBucketIt(list.buckets.rend()->end()), bucketsIt(list.buckets.rend()), bucketsEnd(list.buckets.rend()) {
    }

    inline Car operator->() const { return currentBucketIt.operator->(); }
    inline ReverseCarIterator &operator++() {
      if (!alreadyIncremented) { ++currentBucketIt; } // get next car in current bucket
      // if this was the last car, find the next non empty bucket
      while (bucketsIt != bucketsEnd && currentBucketIt == bucketsIt->end()) {
        ++bucketsIt;                          // move to next bucket
        currentBucketIt = bucketsIt->begin(); // retrieve first element in new bucket
      }

      return *this;
    }

    inline ReverseCarIterator operator++(int) {
      ReverseCarIterator tmp(*this);
      ++*this;
      return tmp;
    }

    inline bool operator==(const ReverseCarIterator &it) const {
      return currentBucketIt == it.currentBucketIt && bucketsIt == it.bucketsIt;
    };
    inline bool operator!=(const ReverseCarIterator &it) const { return !((*this) == it); };

    inline unsigned int getCurrentBucket() const { return bucketsIt - bucketsEnd; }

    inline Car remove() {
      Car removed        = currentBucketIt.operator->();      // create copy of the current car TODO correct?
      currentBucketIt    = bucketsIt->erase(currentBucketIt); // remove car and move currentBucketIt to next element
      alreadyIncremented = true;                              // remember that erase already incremented the iterator
      return removed;
    }
  };

  class CarIterator {
    BucketIterator currentBucketIt;
    typename std::vector<Bucket>::iterator bucketsIt;
    const typename std::vector<Bucket>::iterator bucketsEnd;

  public:
    CarIterator() = default;
    CarIterator(BucketList &list, const unsigned int initialBucket)
        : currentBucketIt((list.buckets.begin() + initialBucket)->begin()),
          bucketsIt(list.buckets.begin() + initialBucket), bucketsEnd(list.buckets.end()) {}
    CarIterator(BucketList &list, const unsigned int initialBucket, BucketIterator currentBucketIt)
        : currentBucketIt(currentBucketIt), bucketsIt(list.buckets.begin() + initialBucket),
          bucketsEnd(list.buckets.end()) {}
    CarIterator(BucketList &list)
        : currentBucketIt(list.buckets.begin()->begin()), bucketsIt(list.buckets.begin()),
          bucketsEnd(list.buckets.end()) {}
    CarIterator(BucketList &list, bool) // end iterator
        : currentBucketIt(list.buckets.end()->end()), bucketsIt(list.buckets.end()), bucketsEnd(list.buckets.end()) {}

    inline Car operator->() const { return currentBucketIt.operator->(); }
    inline CarIterator &operator++() {
      ++currentBucketIt; // get next car in current bucket
      // if this was the last car, find the next non empty bucket
      while (bucketsIt != bucketsEnd && currentBucketIt == bucketsIt->end()) {
        ++bucketsIt;                          // move to next bucket
        currentBucketIt = bucketsIt->begin(); // retrieve first element in new bucket
      }

      return *this;
    }

    inline CarIterator operator++(int) {
      CarIterator tmp(*this);
      ++*this;
      return tmp;
    }

    inline bool operator==(const CarIterator &it) const {
      return currentBucketIt == it.currentBucketIt && bucketsIt == it.bucketsIt;
    };
    inline bool operator!=(const CarIterator &it) const { return !((*this) == it); };

    inline unsigned int getCurrentBucket() const { return bucketsIt - bucketsEnd; }
  };

  class ConstCarIterator {
    ConstBucketIterator currentBucketIt;
    typename std::vector<Bucket>::const_iterator bucketsIt;
    const typename std::vector<Bucket>::const_iterator bucketsEnd;

  public:
    ConstCarIterator() = default;
    ConstCarIterator(const BucketList &list, const unsigned int initialBucket)
        : currentBucketIt((list.buckets.begin() + initialBucket)->begin()),
          bucketsIt(list.buckets.begin() + initialBucket), bucketsEnd(list.buckets.end()) {}
    ConstCarIterator(BucketList &list, const unsigned int initialBucket, ConstBucketIterator currentBucketIt)
        : currentBucketIt(currentBucketIt), bucketsIt(list.buckets.begin() + initialBucket),
          bucketsEnd(list.buckets.end()) {}
    ConstCarIterator(const CarIterator &it)
        : currentBucketIt(it.currentBucketIt), bucketsIt(it.bucketsIt), bucketsEnd(it.bucketsEnd) {}
    ConstCarIterator(const BucketList &list)
        : currentBucketIt(list.buckets.begin()->begin()), bucketsIt(list.buckets.begin()),
          bucketsEnd(list.buckets.end()) {}
    ConstCarIterator(const BucketList &list, bool) // end iterator
        : currentBucketIt(list.buckets.end()->end()), bucketsIt(list.buckets.end()), bucketsEnd(list.buckets.end()) {}

    inline const Car operator->() const { return currentBucketIt.operator->(); }
    inline ConstCarIterator &operator++() {
      ++currentBucketIt; // get next car in current bucket
      // if this was the last car, find the next non empty bucket
      while (bucketsIt != bucketsEnd && currentBucketIt == bucketsIt->end()) {
        ++bucketsIt;                          // move to next bucket
        currentBucketIt = bucketsIt->begin(); // retrieve first element in new bucket
      }

      return *this;
    }

    inline ConstCarIterator operator++(int) {
      ConstCarIterator tmp(*this);
      ++*this;
      return tmp;
    }

    inline bool operator==(const ConstCarIterator &it) const {
      return currentBucketIt == it.currentBucketIt && bucketsIt == it.bucketsIt;
    };
    inline bool operator!=(const ConstCarIterator &it) const { return !((*this) == it); };

    inline unsigned int getCurrentBucket() const { return bucketsIt - bucketsEnd; }
  };

  template <class IteratorType>
  class _AllCarIterable {
    const IteratorType _begin;
    const IteratorType _end;

  public:
    _AllCarIterable(BucketList &list) : _begin(list), _end(list, true) {}

    inline IteratorType begin() const { return _begin; }
    inline IteratorType end() const { return _end; }
  };

  using AllCarIterable          = _AllCarIterable<CarIterator>;
  using ConstAllCarIterable     = _AllCarIterable<ConstCarIterator>;
  using ReverseAllCarIterable   = _AllCarIterable<ReverseCarIterator>;
  using BeyondsCarIterable      = BucketIterator;
  using ConstBeyondsCarIterable = ConstBucketIterator;

  // ------- Getter -------
  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   *
   * @return     The number of lanes.
   */
  inline unsigned int getLaneCount() const { return laneCount; }

  /**
   * @brief      Gets the length of the street.
   *
   * @return     The street length.
   */
  inline double getLength() const { return streetLength; }

  /**
   * @brief      Gets the length of a section represented by single buckets.
   *
   * @return     The section length.
   */
  inline double getSectionLength() const { return sectionLength; }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   *
   * Cars beyond the street and cars that are inserted but not incorporated are not considered.
   *
   * @return     The number cars on this street.
   */
  inline unsigned int getCarCount() const { return carCount; }

  // ------- Access to Neighboring Cars -------

private:
  /**
   * @brief      Determine the minimum car in the given bucket and return it via an iterator.
   *
   * @param[in]  bucketIndex  The bucket index of the bucket to search in.
   * @param[in]  lowerLimit   The lower limit of the distance, only cars with distance > lowerLimit are returned.
   *
   * @return     Iterator to the minimum car in the bucket with distance > lowerLimit, end() if no such car exists.
   */
  inline BucketIterator findMinCarInBucket(const unsigned int bucketIndex, const double lowerLimit = -1) {
    BucketIterator minIt = buckets[bucketIndex].end();
    for (BucketIterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() > lowerLimit && (minIt == buckets[bucketIndex].end() || it->lessThan(minIt.operator->()))) {
        minIt = it;
      }
    }
    return minIt;
  }
  inline ConstBucketIterator findMinCarInBucket(const unsigned int bucketIndex, const double lowerLimit = -1) const {
    ConstBucketIterator minIt = buckets[bucketIndex].end();
    for (ConstBucketIterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() > lowerLimit && (minIt == buckets[bucketIndex].end() || it->lessThan(minIt.operator->()))) {
        minIt = it;
      }
    }
    return minIt;
  }

  /**
   * @brief      Determine the maximum car in the given bucket and return it via an iterator.
   *
   * @param[in]  bucketIndex  The bucket index of the bucket to search in.
   * @param[in]  upperLimit   The upper limit of the distance, only cars with distance < upperLimit are returned.
   *
   * @return     Iterator to the maximum car in the bucket with distance < upperLimit, end() if no such car exists.
   */
  inline BucketIterator findMaxCarInBucket(
      const unsigned int bucketIndex, const double upperLimit = std::numeric_limits<double>::max()) {
    BucketIterator maxIt = buckets[bucketIndex].end();
    for (BucketIterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() < upperLimit &&
          (maxIt == buckets[bucketIndex].end() || it->greaterThan(maxIt.operator->()))) {
        maxIt = it;
      }
    }
    return maxIt;
  }
  inline ConstBucketIterator findMaxCarInBucket(
      const unsigned int bucketIndex, const double upperLimit = std::numeric_limits<double>::max()) const {
    ConstBucketIterator maxIt = buckets[bucketIndex].end();
    for (ConstBucketIterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() < upperLimit &&
          (maxIt == buckets[bucketIndex].end() || it->greaterThan(maxIt.operator->()))) {
        maxIt = it;
      }
    }
    return maxIt;
  }

public:
  /**
   * @brief      Find the next car in front of the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset. All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car in front represented by an iterator.
   */
  CarIterator getNextCarInFront(const CarIterator currentCarIt, const int laneOffset = 0) {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    BucketIterator nextCar = findMinCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in next buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex += laneCount; // get next bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex >= buckets.size()) { return CarIterator(*this, true); }
      nextCar = findMinCarInBucket(currentBucketIndex);
    }
    return CarIterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }
  ConstCarIterator getNextCarInFront(const ConstCarIterator currentCarIt, const int laneOffset = 0) const {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    ConstBucketIterator nextCar = findMinCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in next buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex += laneCount; // get next bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex >= buckets.size()) { return ConstCarIterator(*this, true); }
      nextCar = findMinCarInBucket(currentBucketIndex);
    }
    return ConstCarIterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }

  /**
   * @brief      Find the next car behind the current car on the current or neighboring lane.
   * The lane is determined by the laneOffset. All cars are represented by iterators.
   *
   * @param[in]  currentCarIt  The current car represented by an iterator.
   * @param[in]  laneOffset    The lane offset determining which lane to search on. Own lane: 0, Left: -1, Right: +1.
   *
   * @return     The car behind the current car represented by an iterator.
   */
  CarIterator getNextCarBehind(const CarIterator currentCarIt, const int laneOffset = 0) {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    BucketIterator nextCar = findMaxCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in previous buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex -= laneCount; // get previous bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex < 0) { return CarIterator(*this, true); }
      nextCar = findMaxCarInBucket(currentBucketIndex);
    }
    return CarIterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }
  ConstCarIterator getNextCarBehind(const ConstCarIterator currentCarIt, const int laneOffset = 0) const {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    ConstBucketIterator nextCar = findMaxCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in previous buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex -= laneCount; // get previous bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex < 0) { return ConstCarIterator(*this, true); }
      nextCar = findMaxCarInBucket(currentBucketIndex);
    }
    return ConstCarIterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }

  /**
   * @brief      Add a new car to the street using move semantics.
   * The car is inserted into the correct bucket and updated with a call to its update() function.
   * Increments 'carCount' for each inserted car.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(Car &&car) {
    car.update();
    buckets[findBucketIndex(car.getLane(), car.getDistance())].push_back(car);
    ++carCount;
  }

  /**
   * @brief      Add a new car to the street using copy semantics.
   * The car is inserted into the correct bucket and updated with a call to its update() function.
   * Increments 'carCount' for each inserted car.
   *
   * @param      car   The car to be inserted.
   */
  inline void insertCar(const Car &car) {
    car.update();
    buckets[findBucketIndex(car.getLane(), car.getDistance())].push_back(car);
    ++carCount;
  }

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   * This function has no effect on the bucket list since cars are incorporated immediately in the insert car function.
   * It exists merely for compatibility with other low level street data structures.
   */
  inline void incorporateInsertedCars() {} // this function does nothing

  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency.
   * The buckets are iterated in decreasing distance from the start, buckets at the same distance for different lanes
   * are are considered consecutively in decreasing lane number. Each car in the current bucket is updated by a call to
   * update() and move to its correct new bucket according to its new position. If the new position is beyond this
   * street, it is moved to the departedCars bucket. Decrements 'carCount' for each car moved to departedCars.
   * By iterating in decreasing distance, the number of cars considered twice is reduced to only those cars that changed
   * to a higher lane and stayed within the same section.
   */
  void updateCarsAndRestoreConsistency() {
    ReverseAllCarIterable iterable(*this);
    for (ReverseCarIterator carIt = iterable.begin(); carIt != iterable.end(); ++carIt) { // for all cars on the street
      carIt->update(); // update car to new state and position

      // if car is beyond street, move it from bucket to departedCars
      if (carIt->getDistance() > streetLength) {
        --carCount;
        departedCars.push_back(carIt.remove());
      }

      // if the car needs to be moved, move it from the old to the new bucket
      unsigned int newBucket = findBucketIndex(carIt->getLane(), carIt->getDistance());
      if (newBucket != carIt.getCurrentBucket()) { buckets[newBucket].push_back(carIt.remove()); }
    }
  }

  /**
   * @brief      Iterable for iterating over all cars.
   * All cars within the 'standard' buckets are iterated. This includes all cars inserted via insertCar independent of
   * calls to incorporateInsertedCars() since they are inserted immediately into the actual buckets. Cars beyond the
   * street (i.e. in the departedCars bucket) are not considered by the allIterable. They can be accessed with the
   * beyondsIterable. The buckets are iterated in increasing distance from the start, buckets at the same distance for
   * different lanes are are considered consecutively in increasing lane number. The cars within a bucket are iterated
   * in an arbitrary order.
   *
   * @return     An iterable object for all cars on this street.
   */
  inline AllCarIterable allIterable() { return AllCarIterable(*this); }
  inline ConstAllCarIterable allIterable() const { return ConstAllCarIterable(*this); }
  inline ConstAllCarIterable constAllIterable() const { return ConstAllCarIterable(*this); }

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * This iterates the cars in the departedCars bucket in an arbitrary order.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  inline BeyondsCarIterable beyondsIterable() { return BeyondsCarIterable(*this); }
  inline ConstBeyondsCarIterable beyondsIterable() const { return ConstBeyondsCarIterable(*this); }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return ConstBeyondsCarIterable(*this); }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * Cars are removed by clearing the Bucket containing them.
   */
  inline void removeBeyonds() { departedCars.clear(); }
};

#endif
