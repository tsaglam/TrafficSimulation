#include "LowLevelStreet.h"
#include <vector>

template <class Car, class Bucket>
class Bucketlist {
private:
  size_t bucketLength; // the distance covered by a single bucket
  // total street length / bucketLength = number of buckets
  size_t laneCount; // total number of lanes

  // a vector of buckets containing all the buckets of all lanes
  std::vector<Bucket> buckets;
  Bucket *lastBucket; // utility pointer to the last bucket

  // retrieve the bucket index from car position (given as the distance traveled on the current street)
  inline size_t distanceToBucket(const size_t distanceOnStreet) const { return distanceOnStreet / bucketLength; }

public:
  class iterator {
  private:
    typename Bucket::iterator currentBucketIterator;
    Bucket *currentBucket;

  public:
    using reference         = Car &;
    using pointer           = Car *;
    using iterator_category = std::random_access_iterator_tag;

    iterator();
    iterator(Bucket *);
    iterator(Bucket *, typename Bucket::iterator);
    iterator(const iterator &);
    ~iterator();

    iterator &operator=(const iterator &);
    bool operator==(const iterator &) const;
    bool operator!=(const iterator &) const;

    iterator &operator++();

    reference operator*() const;
    pointer operator->() const;

    // Access the bucket in which the iterator is currently located
    Bucket* getBucket() const;
  };

  class const_iterator {
  private:
    typename Bucket::const_iterator currentBucketIterator;
    Bucket *currentBucket;

  public:
    using reference         = const Car &;
    using pointer           = const Car *;
    using iterator_category = std::random_access_iterator_tag;

    const_iterator();
    const_iterator(const Bucket *);
    const_iterator(Bucket *, typename Bucket::const_iterator);
    const_iterator(const const_iterator &);
    const_iterator(const iterator &);
    ~const_iterator();

    const_iterator &operator=(const const_iterator &);
    bool operator==(const const_iterator &) const;
    bool operator!=(const const_iterator &) const;

    const_iterator &operator++();

    reference operator*() const;
    pointer operator->() const;

    // Access the bucket in which the iterator is currently located
    Bucket* getBucket() const;
  };

  friend iterator;
  friend const_iterator;

  Bucketlist();
  ~Bucketlist();

  // Find the next car in front of the current car on the given lane (see LowLevelStreet::getPrevCar).
  // Search the current bucket for a predecessor of the current car (i.e. driving behind the current car).
  // If there is no predecessor in the current bucket, search for the closest non-empty previous bucket
  // and return the first car in this bucket (i.e. the car with the highest distance).
  iterator _getPrevCar(iterator currentCarIt, const int laneOffset = 0);
  iterator _getPrevCar(const_iterator currentCarIt, const int laneOffset = 0);

  // Find the next car in front of the current car on the given lane (see LowLevelStreet::getNextCar).
  // Search the current bucket for a successor of the current car (i.e. driving in front of the current car).
  // If there is no successor in the current bucket, search for the next non-empty bucket and return the last
  // car in this bucket (i.e. the car with the smallest distance).
  iterator _getNextCar(iterator currentCarIt, const int laneOffset = 0);
  iterator _getNextCar(const_iterator currentCarIt, const int laneOffset = 0);

  // Add a new car to the street using move semantics.
  // Inserted cars are initially stored in a special bucket and only incorporated into the actual
  // bucket data structure once _incorporateAddedCars is called.
  void _insertCar(Car &&car);

  // Inserts all cars stored in the temporary vector for inserted cars and moves them to the correct bucket.
  // Invalidates all iterators, data structure consistency is only ensured before and after but not necessarily
  // during the function execution.
  void _incorporateAddedCars();

  // Update the position of **all** cars on this street while retaining the consistency of the
  // underlying StreetDataStructure.
  // If a car left its current bucket (i.e. its distance is larger than the maximum distance covered by the
  // current bucket), move it to the appropriate next bucket.
  // Cars that reached the end of this street are collected
  // internally in a vector which can be accessed via the _getDepartedCars function.
  // Invalidates all iterators, data structure consistency is only ensured before and after but not necessarily
  // during the function execution.
  void _applyUpdates();

  // Access the cars which left the current street (as determined by the _applyUpdates function).
  std::vector<Car> &_getDepartedCars();

  // Iterators to iterate all cars in the street bucket-wise (i.e. first bucket of each lane, followed by the second
  // bucket of each lane etc.). The cars in the buckets are not necessarily sorted.
  iterator _begin();
  iterator _end();
  const_iterator _begin() const;
  const_iterator _end() const;

  const_iterator _cbegin() const;
  const_iterator _cend() const;
};