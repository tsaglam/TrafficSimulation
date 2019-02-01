#ifndef BUCKET_LIST_H
#define BUCKET_LIST_H

#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

#include "BucketListIterator.h"
#include "BucketTraits.h"
#include "FreeList.h"
#include "RfbStructureTraits.h"

template <class Car, class Bucket>
class BucketList {
private:
  /** The number of lanes on the street (in the current direction). */
  const unsigned int laneCount;
  /** The length of the street. */
  const double streetLength;
  /** The distance represented by a single bucket. */
  const double sectionLength;

  // ------- Data Storage -------
  /**
   * The street is partitioned into sections of equal length (sectionLength).
   * Each of these sections is represented by a bucket per lane containing all cars in that section and lane.
   * The cars within a bucket are not necessarily sorted.
   * For each car, the correct bucket is easily determined by its distance, its lane, and the section length.
   * Newly inserted cars are stored immediately in the correct bucket.
   */
  std::vector<Bucket> buckets;

  /**  All cars that left this street (i.e. their distance is greater than the street length). */
  std::vector<Car> departedCars;

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
    assert(lane >= 0 && lane < laneCount);
    assert(distance >= 0 && distance < streetLength);
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
  BucketList(const unsigned int laneCount, const double length, const double sectionLength = 25)
      : laneCount(laneCount), streetLength(length), sectionLength(sectionLength),
        buckets(std::ceil(length / sectionLength) * laneCount) {}

  // ------- Iterator & Iterable type defs -------
  using bucket_iterator       = typename Bucket::iterator;
  using bucket_const_iterator = typename Bucket::const_iterator;

  using reverse_category = rfbstructure_buckets_tag;

  using BeyondsCarIterable      = std::vector<Car> &;
  using ConstBeyondsCarIterable = const std::vector<Car> &;
  using iterator                = bucket_list_iterator<Bucket, Car>;
  using const_iterator          = bucket_list_iterator<Bucket, Car, true>;

  template <bool Const = false>
  class _AllCarIterable {
    using ListReference = typename std::conditional_t<Const, BucketList const &, BucketList &>;
    using iterator_type = typename std::conditional_t<Const, BucketList::const_iterator, BucketList::iterator>;
    const iterator_type _begin;
    const iterator_type _end;

  public:
    _AllCarIterable(ListReference list)
        : _begin(list.buckets.begin(), list.buckets.end()), _end(list.buckets.begin(), list.buckets.end(), 0) {}
    inline iterator_type begin() const { return _begin; }
    inline iterator_type end() const { return _end; }
  };

  template <bool Const>
  friend class _AllCarIterable;
  using AllCarIterable      = _AllCarIterable<>;
  using ConstAllCarIterable = _AllCarIterable<true>;

  // ------- Getter -------
  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   * @return     The number of lanes.
   */
  inline unsigned int getLaneCount() const { return laneCount; }

  /**
   * @brief      Gets the length of the street.
   * @return     The street length.
   */
  inline double getLength() const { return streetLength; }

  /**
   * @brief      Gets the length of a section represented by single buckets.
   * @return     The section length.
   */
  inline double getSectionLength() const { return sectionLength; }

  /**
   * @brief      Gets the number of sections (i.e. the number of buckets per lane).
   * @return     The section count.
   */
  inline unsigned getSectionCount() const {
    assert(buckets.size() > 0);
    assert(buckets.size() / laneCount);
    return buckets.size() / laneCount;
  }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   * Cars beyond the street and cars that are inserted but not incorporated are not considered.
   * @return     The number cars on this street.
   */
  inline unsigned int getCarCount() const { return carCount; }

  /**
   * Returns a reference to the bucket representing the given section on the given lane.
   * @param[in]  sectionIndex  The section index
   * @param[in]  lane          The lane
   * @return     The bucket as const reference.
   */
  const Bucket &getBucket(const unsigned sectionIndex, const unsigned lane) const {
    assert(sectionIndex < getSectionCount());
    assert(lane < laneCount);
    return buckets[sectionIndex * laneCount + lane];
  }

  // ------- Access to Neighboring Cars -------

private:
  /**
   * @brief      Determine the minimum car in the given bucket and return it via an iterator. Return only cars > the
   * given limitCar.
   *
   * @param[in]  bucketIndex  The bucket index of the bucket to search in.
   * @param[in]  limitCar     The limit car,  only cars > limitCar are returned.
   *
   * @return     Iterator to the minimum car in the bucket > limitCar, end() if no such car exists.
   */
  inline bucket_iterator findMinCarInBucket(const unsigned int bucketIndex, const Car &limitCar) {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_iterator minIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (!compareGreater(*it, limitCar)) { continue; } // skip if it < limit
      if (minIt == buckets[bucketIndex].end() || compareLess(*it, *minIt)) { minIt = it; }
    }
    return minIt;
  }
  inline bucket_const_iterator findMinCarInBucket(const unsigned int bucketIndex, const Car &limitCar) const {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_const_iterator minIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (!compareGreater(*it, limitCar)) { continue; } // skip if it < limit
      if (minIt == buckets[bucketIndex].end() || compareLess(*it, *minIt)) { minIt = it; }
    }
    return minIt;
  }

  /**
   * @brief      Variant of findMinCarInBucket without limitCar. Returns the actual minimum car in the bucket.
   */
  inline bucket_iterator findMinCarInBucket(const unsigned int bucketIndex) {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_iterator minIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (minIt == buckets[bucketIndex].end() || compareLess(*it, *minIt)) { minIt = it; }
    }
    return minIt;
  }
  inline bucket_const_iterator findMinCarInBucket(const unsigned int bucketIndex) const {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_const_iterator minIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (minIt == buckets[bucketIndex].end() || compareLess(*it, *minIt)) { minIt = it; }
    }
    return minIt;
  }

  /**
   * @brief      Determine the maximum car in the given bucket and return it via an iterator. Return only cars > the
   * given limitCar.
   *
   * @param[in]  bucketIndex  The bucket index of the bucket to search in.
   * @param[in]  limitCar     The limit car,  only cars < limitCar are returned.
   *
   * @return     Iterator to the maximum car in the bucket < limitCar, end() if no such car exists.
   */
  inline bucket_iterator findMaxCarInBucket(const unsigned int bucketIndex, const Car &limitCar) {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (!compareLess(*it, limitCar)) { continue; } // skip if it > limit
      if (maxIt == buckets[bucketIndex].end() || compareGreater(*it, *maxIt)) { maxIt = it; }
    }
    return maxIt;
  }
  inline bucket_const_iterator findMaxCarInBucket(const unsigned int bucketIndex, const Car &limitCar) const {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_const_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (!compareLess(*it, limitCar)) { continue; } // skip if it > limit
      if (maxIt == buckets[bucketIndex].end() || compareGreater(*it, *maxIt)) { maxIt = it; }
    }
    return maxIt;
  }

  /**
   * @brief      Variant of findMaxCarInBucket without limitCar. Returns the actual maximum car in the bucket.
   */
  inline bucket_iterator findMaxCarInBucket(const unsigned int bucketIndex) {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (maxIt == buckets[bucketIndex].end() || compareGreater(*it, *maxIt)) { maxIt = it; }
    }
    return maxIt;
  }
  inline bucket_const_iterator findMaxCarInBucket(const unsigned int bucketIndex) const {
    assert(bucketIndex >= 0 && bucketIndex < buckets.size());
    bucket_const_iterator maxIt = buckets[bucketIndex].end();
    for (bucket_const_iterator it = buckets[bucketIndex].begin(); it != buckets[bucketIndex].end(); ++it) {
      if (maxIt == buckets[bucketIndex].end() || compareGreater(*it, *maxIt)) { maxIt = it; }
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
    // search for next car in own bucket
    unsigned int currentBucket = findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());
    bucket_iterator nextCar    = findMinCarInBucket(currentBucket, *currentCarIt);

    // search for next car in next buckets until such a car is found
    while (nextCar == buckets[currentBucket].end()) {
      currentBucket += laneCount; // get next bucket in the current lane
      // if end of street is reached return end iterator to represent no next car
      if (currentBucket >= buckets.size()) { return iterator(buckets.begin(), buckets.end(), 0); }
      nextCar = findMinCarInBucket(currentBucket);
    }
    return iterator(buckets.begin(), buckets.end(), buckets.begin() + currentBucket, nextCar);
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    // search for next car in own bucket
    unsigned int currentBucket    = findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());
    bucket_const_iterator nextCar = findMinCarInBucket(currentBucket, *currentCarIt);

    // if this is the first car in the bucket find the next non-empty in front of the current bucket in this lane
    if (nextCar == buckets[currentBucket].end()) {
      currentBucket += laneCount;
      while (currentBucket < buckets.size() && buckets[currentBucket].empty()) { currentBucket += laneCount; }
      // if end of street is reached return end iterator to represent no next car
      if (currentBucket >= buckets.size()) { return const_iterator(buckets.begin(), buckets.end(), 0); }
      nextCar = findMinCarInBucket(currentBucket); // otherwise return the first car of the next non-empty bucket
    }
    return const_iterator(buckets.begin(), buckets.end(), buckets.begin() + currentBucket, nextCar);
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
    // search for next car in own bucket
    int currentBucket       = findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());
    bucket_iterator nextCar = findMaxCarInBucket(currentBucket, *currentCarIt);

    // if this is the last car in the bucket find the next non-empty behind the current bucket in this lane
    if (nextCar == buckets[currentBucket].end()) {
      currentBucket -= laneCount;
      while (currentBucket >= 0 && buckets[currentBucket].empty()) { currentBucket -= laneCount; }
      // if start of street is reached return end iterator to represent no next car
      if (currentBucket < 0) { return iterator(buckets.begin(), buckets.end(), 0); }
      nextCar = findMaxCarInBucket(currentBucket); // otherwise return the last car of the next non-empty bucket
    }
    return iterator(buckets.begin(), buckets.end(), buckets.begin() + currentBucket, nextCar);
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    // search for next car in own bucket
    int currentBucket             = findBucketIndex(currentCarIt->getLane() + laneOffset, currentCarIt->getDistance());
    bucket_const_iterator nextCar = findMaxCarInBucket(currentBucket, *currentCarIt);

    // if this is the last car in the bucket find the next non-empty behind the current bucket in this lane
    if (nextCar == buckets[currentBucket].end()) {
      currentBucket -= laneCount;
      while (currentBucket >= 0 && buckets[currentBucket].empty()) { currentBucket -= laneCount; }
      // if start of street is reached return end iterator to represent no next car
      if (currentBucket < 0) { return const_iterator(buckets.begin(), buckets.end(), 0); }
      nextCar = findMaxCarInBucket(currentBucket); // otherwise return the last car of the next non-empty bucket
    }
    return const_iterator(buckets.begin(), buckets.end(), buckets.begin() + currentBucket, nextCar);
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
    buckets[findBucketIndex(car.getLane(), car.getDistance())].push_back(std::move(car));
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
    // for each bucket in reverse order
    std::vector<bucket_iterator> eraseIterators;
    for (int bucketIndex = buckets.size() - 1; bucketIndex >= 0; --bucketIndex) {
      Bucket &currentBucket = buckets[bucketIndex];
      for (auto carIt = currentBucket.begin(); carIt != currentBucket.end(); ++carIt) {
        carIt->update(); // update car to new state and position

        // if car is beyond street, move it from bucket to departedCars
        if (carIt->getDistance() >= streetLength) {
          departedCars.push_back(std::move(*carIt));
          eraseIterators.push_back(carIt);
          --carCount;
          continue;
        }

        // if the car needs to be moved, move it from the old to the new bucket
        int newBucket = findBucketIndex(carIt->getLane(), carIt->getDistance());
        if (newBucket != bucketIndex) {
          buckets[newBucket].push_back(std::move(*carIt));
          eraseIterators.push_back(carIt);
        }
      }

      eraseFromBucket(currentBucket, eraseIterators, typename bucket_traits<Bucket>::erase_category());
      eraseIterators.clear();
    }
  }

  void eraseFromBucket(
      Bucket &currentBucket, const std::vector<bucket_iterator> &eraseIterators, bucket_single_erasable) {
    for (auto eraseIt = eraseIterators.rbegin(); eraseIt != eraseIterators.rend(); ++eraseIt) {
      currentBucket.erase(*eraseIt);
    }
  }

  void eraseFromBucket(
      Bucket &currentBucket, const std::vector<bucket_iterator> &eraseIterators, bucket_multi_erasable) {
    currentBucket.erase(eraseIterators);
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
  inline BeyondsCarIterable beyondsIterable() { return departedCars; }
  inline ConstBeyondsCarIterable beyondsIterable() const { return departedCars; }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return departedCars; }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * Cars are removed by clearing the Bucket containing them.
   */
  inline void removeBeyonds() { departedCars.clear(); }
};

/**
 * Traits specialisation for all std::vector bucket types.
 */
template <class Car>
struct bucket_traits<typename std::vector<Car>> {
  using erase_category = bucket_single_erasable;
};

template <class Car>
class VectorBucketList {
private:
  using Bucket = std::vector<Car>;
  BucketList<Car, Bucket> list;

public:
  VectorBucketList() = default;
  VectorBucketList(const unsigned int laneCount, const double length, const double sectionLength = 25)
      : list(laneCount, length, sectionLength) {}

  // ------- Iterator & Iterable type defs -------
  using iterator                = typename BucketList<Car, Bucket>::iterator;
  using const_iterator          = typename BucketList<Car, Bucket>::const_iterator;
  using AllCarIterable          = typename BucketList<Car, Bucket>::AllCarIterable;
  using ConstAllCarIterable     = typename BucketList<Car, Bucket>::ConstAllCarIterable;
  using BeyondsCarIterable      = typename BucketList<Car, Bucket>::BeyondsCarIterable;
  using ConstBeyondsCarIterable = typename BucketList<Car, Bucket>::ConstBeyondsCarIterable;
  using reverse_category        = typename BucketList<Car, Bucket>::reverse_category;

  unsigned int getLaneCount() const { return list.getLaneCount(); }
  double getLength() const { return list.getLength(); }
  double getSectionLength() const { return list.getSectionLength(); }
  unsigned getSectionCount() const { return list.getSectionCount(); }
  unsigned int getCarCount() const { return list.getCarCount(); }
  const Bucket &getBucket(const unsigned sectionIndex, const unsigned lane) const {
    return list.getBucket(sectionIndex, lane);
  }

public:
  iterator getNextCarInFront(const iterator currentCarIt, const int laneOffset = 0) {
    return list.getNextCarInFront(currentCarIt, laneOffset);
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    return list.getNextCarInFront(currentCarIt, laneOffset);
  }
  iterator getNextCarBehind(const iterator currentCarIt, const int laneOffset = 0) {
    return list.getNextCarBehind(currentCarIt, laneOffset);
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    return list.getNextCarBehind(currentCarIt, laneOffset);
  }

  inline void insertCar(Car &&car) { return list.insertCar(std::move(car)); }
  inline void insertCar(const Car &car) { return list.insertCar(car); }
  inline void incorporateInsertedCars() { return list.incorporateInsertedCars(); }
  void updateCarsAndRestoreConsistency() { return list.updateCarsAndRestoreConsistency(); }
  void removeBeyonds() { return list.removeBeyonds(); }

  inline AllCarIterable allIterable() { return list.allIterable(); }
  inline ConstAllCarIterable allIterable() const { return list.allIterable(); }
  inline ConstAllCarIterable constAllIterable() const { return list.constAllIterable(); }
  inline BeyondsCarIterable beyondsIterable() { return list.beyondsIterable(); }
  inline ConstBeyondsCarIterable beyondsIterable() const { return list.beyondsIterable(); }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return list.constBeyondsIterable(); }
};

template <class Car>
class FreeListBucketList {
private:
  using Bucket = FreeList<Car>;
  BucketList<Car, Bucket> list;

public:
  FreeListBucketList() = default;
  FreeListBucketList(const unsigned int laneCount, const double length, const double sectionLength = 25)
      : list(laneCount, length, sectionLength) {}

  // ------- Iterator & Iterable type defs -------
  using iterator                = typename BucketList<Car, Bucket>::iterator;
  using const_iterator          = typename BucketList<Car, Bucket>::const_iterator;
  using AllCarIterable          = typename BucketList<Car, Bucket>::AllCarIterable;
  using ConstAllCarIterable     = typename BucketList<Car, Bucket>::ConstAllCarIterable;
  using BeyondsCarIterable      = typename BucketList<Car, Bucket>::BeyondsCarIterable;
  using ConstBeyondsCarIterable = typename BucketList<Car, Bucket>::ConstBeyondsCarIterable;
  using reverse_category        = typename BucketList<Car, Bucket>::reverse_category;

  unsigned int getLaneCount() const { return list.getLaneCount(); }
  double getLength() const { return list.getLength(); }
  double getSectionLength() const { return list.getSectionLength(); }
  unsigned getSectionCount() const { return list.getSectionCount(); }
  unsigned int getCarCount() const { return list.getCarCount(); }
  const Bucket &getBucket(const unsigned sectionIndex, const unsigned lane) const {
    return list.getBucket(sectionIndex, lane);
  }

public:
  iterator getNextCarInFront(const iterator currentCarIt, const int laneOffset = 0) {
    return list.getNextCarInFront(currentCarIt, laneOffset);
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    return list.getNextCarInFront(currentCarIt, laneOffset);
  }
  iterator getNextCarBehind(const iterator currentCarIt, const int laneOffset = 0) {
    return list.getNextCarBehind(currentCarIt, laneOffset);
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    return list.getNextCarBehind(currentCarIt, laneOffset);
  }

  inline void insertCar(Car &&car) { return list.insertCar(std::move(car)); }
  inline void insertCar(const Car &car) { return list.insertCar(car); }
  inline void incorporateInsertedCars() { return list.incorporateInsertedCars(); }
  void updateCarsAndRestoreConsistency() { return list.updateCarsAndRestoreConsistency(); }
  void removeBeyonds() { return list.removeBeyonds(); }

  inline AllCarIterable allIterable() { return list.allIterable(); }
  inline ConstAllCarIterable allIterable() const { return list.allIterable(); }
  inline ConstAllCarIterable constAllIterable() const { return list.constAllIterable(); }
  inline BeyondsCarIterable beyondsIterable() { return list.beyondsIterable(); }
  inline ConstBeyondsCarIterable beyondsIterable() const { return list.beyondsIterable(); }
  inline ConstBeyondsCarIterable constBeyondsIterable() const { return list.constBeyondsIterable(); }
};

#endif
