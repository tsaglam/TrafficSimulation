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
  using bucket_iterator         = typename Bucket::iterator;
  using bucket_const_iterator   = typename Bucket::const_iterator;
  using BeyondsCarIterable      = bucket_iterator;
  using ConstBeyondsCarIterable = bucket_const_iterator;

  template <bool Const = false>
  class car_iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = Car;
    using difference_type   = std::ptrdiff_t;
    using reference         = typename std::conditional_t<Const, Car const &, Car &>;
    using pointer           = typename std::conditional_t<Const, Car const *, Car *>;
    using _bucket_iterator  = typename std::conditional_t<Const, bucket_const_iterator, bucket_iterator>;
    using buckets_iterator  = typename std::conditional_t<Const, typename std::vector<Bucket>::const_iterator,
        typename std::vector<Bucket>::iterator>;

  private:
    const buckets_iterator beginBucket, endBucket;
    buckets_iterator currentBucket;
    _bucket_iterator currentPositionInBucket;

    buckets_iterator findNextNonEmptyBucket(const bool includingCurrent = false) const {
      if ((includingCurrent && !currentBucket->empty()) || currentBucket == endBucket) { return currentBucket; }
      buckets_iterator nextBucket = currentBucket + 1;
      while (nextBucket != endBucket && nextBucket->empty()) { ++nextBucket; }
      return nextBucket;
    }

    buckets_iterator findPreviousNonEmptyBucket(const bool includingCurrent = false) const {
      if (includingCurrent && !currentBucket->empty()) { return currentBucket; }
      buckets_iterator previousBucket = currentBucket - 1;
      while (previousBucket->empty()) {
        if (previousBucket == beginBucket) { return endBucket; }
        --previousBucket;
      }
      return previousBucket;
    }

    friend class BucketList<Car>;

    unsigned getCurrentBucket() const { return currentBucket - beginBucket; }

    Car remove() {
      if (state == END) { return Car(); }     // TODO: calling remove on an end iterator is invalid
      Car removed = *currentPositionInBucket; // create copy of the current car TODO correct?
      // remove car and move currentPositionInBucket to next element
      currentPositionInBucket = currentBucket->erase(currentPositionInBucket);
      if (currentPositionInBucket == currentBucket.end()) {
        currentBucket = findNextNonEmptyBucket(); // find next bucket
        if (currentBucket == endBucket) {
          state = END;
        } else {
          currentPositionInBucket = currentBucket->begin();
        } // retrieve first element in new bucket
      }
      return removed;
    }

    enum { STANDARD, END } state;

    // STANDARD: currentPositionInBucket is a valid element (not end()) in the currentBucket (!= endBucket)
    // END: currentBucket == endBucket, currentPositionInBucket undefined

  public:
    car_iterator() = default;
    car_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket)
        : beginBucket(beginBucket), endBucket(endBucket), currentBucket(beginBucket),
          currentPositionInBucket(currentBucket->begin()), state(STANDARD) {
      currentBucket           = findNextNonEmptyBucket(true);
      currentPositionInBucket = currentBucket->begin();
    }
    car_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket, buckets_iterator initialBucket,
        bucket_iterator currentPositionInBucket)
        : beginBucket(beginBucket), endBucket(endBucket), currentBucket(initialBucket),
          currentPositionInBucket(currentPositionInBucket), state(STANDARD) {
      buckets_iterator it = findNextNonEmptyBucket(true);
      if (it != currentBucket) {
        currentBucket           = it;
        currentPositionInBucket = currentBucket->begin();
      }
    }
    // create end iterator
    car_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket, int)
        : beginBucket(beginBucket), endBucket(endBucket), currentBucket(endBucket),
          currentPositionInBucket((endBucket - 1)->end()), state(END) {}

    reference operator*() const { return currentPositionInBucket.operator*(); }
    pointer operator->() const { return currentPositionInBucket.operator->(); }

    car_iterator &operator++() {
      if (state == END) { return *this; }
      ++currentPositionInBucket; // get next car in current bucket
      // if this was the last car, find the next non empty bucket
      if (currentPositionInBucket == currentBucket->end()) {
        currentBucket = findNextNonEmptyBucket(); // find next bucket
        if (currentBucket == endBucket) {
          state = END;
        } else {
          currentPositionInBucket = currentBucket->begin();
        } // retrieve first element in new bucket
      }
      return *this;
    }
    car_iterator operator++(int) {
      car_iterator tmp(*this);
      ++*this;
      return tmp;
    }

    car_iterator &operator--() {
      // if this was the first car in the current bucket or this is the end iterator
      if (state == END || currentPositionInBucket == currentBucket->begin()) {
        buckets_iterator prevBucket = findPreviousNonEmptyBucket(); // find prev bucket
        if (prevBucket == endBucket) { return *this; }              // return if this is the begin iterator
        currentBucket           = prevBucket;
        currentPositionInBucket = currentBucket->end();
      }
      currentPositionInBucket--; // get previous car in current bucket
      return *this;
    }
    car_iterator operator--(int) {
      car_iterator tmp(*this);
      --*this;
      return tmp;
    }

    car_iterator operator+(const difference_type &n) const {
      car_iterator copy = this;
      copy += n;
      return copy;
    }
    car_iterator &operator+=(const difference_type &n) {
      // determine correct bucket
      buckets_iterator bucket                  = currentBucket;
      const unsigned remainingElementsInBucket = currentBucket->end() - currentPositionInBucket; // including current
      difference_type remainingDifference      = n - remainingElementsInBucket;                  // TODO check this
      while (remainingDifference > 0) {
        ++currentBucket;
        if (currentBucket == endBucket) { return car_iterator(beginBucket, endBucket, 0); } // end iterator
        remainingDifference -= currentBucket->size();
      }

      // determine position in currentBucket
      currentPositionInBucket = currentBucket->begin() + (remainingDifference + currentBucket->size());
      return *this;
    }

    car_iterator operator-(const difference_type &n) const {
      car_iterator copy = this;
      copy -= n;
      return copy;
    }
    car_iterator &operator-=(const difference_type &n) {
      // determine correct bucket
      const unsigned remainingElementsInBucket = currentBucket->begin() - currentPositionInBucket; // excluding current
      difference_type remainingDifference      = n - remainingElementsInBucket;                    // TODO check this
      while (remainingDifference < 0 && currentBucket != beginBucket) {
        --currentBucket;
        remainingDifference -= currentBucket->size();
      }

      // determine position in currentBucket
      currentPositionInBucket = currentBucket->begin();
      if (remainingDifference >= 0) { currentPositionInBucket += remainingDifference; }
      return *this;
    }

    reference operator[](const difference_type &n) const { return *(this + n); }

    bool operator==(const car_iterator<Const> &other) const {
      return currentBucket == other.currentBucket && currentPositionInBucket == other.currentPositionInBucket;
    }
    bool operator!=(const car_iterator &other) const { return !((*this) == other); }
    bool operator<(const car_iterator &other) {
      return currentBucket < other.currentBucket ||
             (currentBucket == other.currentBucket && currentPositionInBucket < other.currentPositionInBucket);
    }
    bool operator>(const car_iterator &other) {
      return currentBucket > other.currentBucket ||
             (currentBucket == other.currentBucket && currentPositionInBucket > other.currentPositionInBucket);
    }
    bool operator<=(const car_iterator &other) { return (*this) < other || (*this) == other; }
    bool operator>=(const car_iterator &other) { return (*this) > other || (*this) == other; }
  };

  template <bool Const = false>
  class _AllCarIterable {
    const car_iterator<Const> _begin;
    const car_iterator<Const> _end;

  public:
    _AllCarIterable(BucketList &list)
        : _begin(list.buckets.begin(), list.buckets.end()), _end(list.buckets.begin(), list.buckets.end(), 0) {}
    inline car_iterator<Const> begin() const { return _begin; }
    inline car_iterator<Const> end() const { return _end; }
  };

  using AllCarIterable      = _AllCarIterable<>;
  using ConstAllCarIterable = _AllCarIterable<true>;
  using iterator            = car_iterator<>;
  using const_iterator      = car_iterator<>;

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
  inline bucket_iterator findMinCarInBucket(const unsigned int bucketIndex, const double lowerLimit = -1) {
    bucket_iterator minIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() > lowerLimit && (minIt == buckets[bucketIndex].end() || it->lessThan(minIt.operator->()))) {
        minIt = it;
      }
    }
    return minIt;
  }
  inline bucket_const_iterator findMinCarInBucket(const unsigned int bucketIndex, const double lowerLimit = -1) const {
    bucket_const_iterator minIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
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
  inline bucket_iterator findMaxCarInBucket(
      const unsigned int bucketIndex, const double upperLimit = std::numeric_limits<double>::max()) {
    bucket_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (it->getDistance() < upperLimit &&
          (maxIt == buckets[bucketIndex].end() || it->greaterThan(maxIt.operator->()))) {
        maxIt = it;
      }
    }
    return maxIt;
  }
  inline bucket_const_iterator findMaxCarInBucket(
      const unsigned int bucketIndex, const double upperLimit = std::numeric_limits<double>::max()) const {
    bucket_const_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
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
  iterator getNextCarInFront(const iterator currentCarIt, const int laneOffset = 0) {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    bucket_iterator nextCar = findMinCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in next buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex += laneCount; // get next bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex >= buckets.size()) { return iterator(*this, true); }
      nextCar = findMinCarInBucket(currentBucketIndex);
    }
    return iterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    bucket_const_iterator nextCar = findMinCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in next buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex += laneCount; // get next bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex >= buckets.size()) { return const_iterator(*this, true); }
      nextCar = findMinCarInBucket(currentBucketIndex);
    }
    return const_iterator(*this, currentCarIt.getCurrentBucket(), nextCar);
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
  iterator getNextCarBehind(const iterator currentCarIt, const int laneOffset = 0) {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    bucket_iterator nextCar = findMaxCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in previous buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex -= laneCount; // get previous bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex < 0) { return iterator(*this, true); }
      nextCar = findMaxCarInBucket(currentBucketIndex);
    }
    return iterator(*this, currentCarIt.getCurrentBucket(), nextCar);
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    unsigned int currentBucketIndex =
        findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());

    double ownDistance = currentCarIt->getDistance();
    // search for next car in own bucket
    bucket_const_iterator nextCar = findMaxCarInBucket(currentBucketIndex, ownDistance);

    // search for next car in previous buckets until such a car is found
    while (nextCar == buckets[currentBucketIndex].end()) {
      currentBucketIndex -= laneCount; // get previous bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucketIndex < 0) { return const_iterator(*this, true); }
      nextCar = findMaxCarInBucket(currentBucketIndex);
    }
    return const_iterator(*this, currentCarIt.getCurrentBucket(), nextCar);
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
    Car copy = car;
    copy.update();
    buckets[findBucketIndex(copy.getLane(), copy.getDistance())].push_back(copy);
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
    AllCarIterable iterable(*this);
    iterator carIt = AllCarIterable(*this).end();
    for (int i = 0; i < getCarCount(); ++i) { // for all cars on the street
      carIt--;
      carIt->update(); // update car to new state and position

      // if car is beyond street, move it from bucket to departedCars
      if (carIt->getDistance() > streetLength) {
        departedCars.push_back(carIt.remove());
        --carCount;
        continue;
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
