#ifndef BUCKET_LIST_ITERATOR
#define BUCKET_LIST_ITERATOR

template <class Car>
class BucketList;

template <class Bucket, class Car, bool Const = false>
class bucket_list_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = Car;
  using difference_type   = std::ptrdiff_t;
  using reference         = typename std::conditional_t<Const, Car const &, Car &>;
  using pointer           = typename std::conditional_t<Const, Car const *, Car *>;
  using bucket_iterator =
      typename std::conditional_t<Const, typename Bucket::const_iterator, typename Bucket::iterator>;
  using buckets_iterator = typename std::conditional_t<Const, typename std::vector<Bucket>::const_iterator,
      typename std::vector<Bucket>::iterator>;

private:
  buckets_iterator beginBucket, endBucket;
  buckets_iterator currentBucket;
  bucket_iterator currentPositionInBucket;

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
  friend class bucket_list_iterator<Bucket, Car, !Const>;

  unsigned getCurrentBucket() const { return currentBucket - beginBucket; }

  Car remove() {
    if (state == END) { return Car(); }     // TODO: calling remove on an end iterator is invalid
    Car removed = *currentPositionInBucket; // create copy of the current car TODO correct?
    // remove car and move currentPositionInBucket to next element
    currentPositionInBucket = currentBucket->erase(currentPositionInBucket);
    if (currentPositionInBucket == currentBucket->end()) {
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

  void setToEnd() {
    state                   = END;
    currentBucket           = endBucket;
    currentPositionInBucket = (endBucket - 1)->end();
  }

public:
  bucket_list_iterator() = default;
  bucket_list_iterator(const bucket_list_iterator<Bucket, Car, false> &it)
      : beginBucket(it.beginBucket), endBucket(it.endBucket), currentBucket(it.currentBucket),
        currentPositionInBucket(it.currentPositionInBucket), state(STANDARD) {
    if (it.state == END) { state = END; }
  }
  bucket_list_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket)
      : beginBucket(beginBucket), endBucket(endBucket), currentBucket(beginBucket),
        currentPositionInBucket(currentBucket->begin()), state(STANDARD) {
    currentBucket = findNextNonEmptyBucket(true);
    if (currentBucket == endBucket) {
      currentPositionInBucket = (endBucket - 1)->end();
      state                   = END;
    } else {
      currentPositionInBucket = currentBucket->begin();
    }
  }
  bucket_list_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket,
      buckets_iterator initialBucket, bucket_iterator currentPositionInBucket)
      : beginBucket(beginBucket), endBucket(endBucket), currentBucket(initialBucket),
        currentPositionInBucket(currentPositionInBucket), state(STANDARD) {
    buckets_iterator it = findNextNonEmptyBucket(true);
    if (it == endBucket) {
      setToEnd();
    } else if (it != currentBucket) {
      currentBucket           = it;
      currentPositionInBucket = currentBucket->begin();
    }
  }
  // create end iterator
  bucket_list_iterator(const buckets_iterator beginBucket, const buckets_iterator endBucket, int)
      : beginBucket(beginBucket), endBucket(endBucket), currentBucket(endBucket),
        currentPositionInBucket((endBucket - 1)->end()), state(END) {}

  reference operator*() const { return currentPositionInBucket.operator*(); }
  pointer operator->() const { return currentPositionInBucket.operator->(); }

  bucket_list_iterator &operator++() {
    if (state == END) { return *this; }
    ++currentPositionInBucket; // get next car in current bucket
    // if this was the last car, find the next non empty bucket
    if (currentPositionInBucket == currentBucket->end()) {
      currentBucket = findNextNonEmptyBucket(); // find next bucket
      if (currentBucket == endBucket) {
        setToEnd();
      } else {
        currentPositionInBucket = currentBucket->begin();
      } // retrieve first element in new bucket
    }
    return *this;
  }
  bucket_list_iterator operator++(int) {
    bucket_list_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  bucket_list_iterator &operator--() {
    // if this was the first car in the current bucket or this is the end iterator
    if (state == END || currentPositionInBucket == currentBucket->begin()) {
      buckets_iterator prevBucket = findPreviousNonEmptyBucket(); // find prev bucket
      if (prevBucket == endBucket) { return *this; }              // return if this is the begin iterator
      currentBucket           = prevBucket;
      currentPositionInBucket = currentBucket->end();
      state                   = STANDARD;
    }
    currentPositionInBucket--; // get previous car in current bucket
    return *this;
  }
  bucket_list_iterator operator--(int) {
    bucket_list_iterator tmp(*this);
    --*this;
    return tmp;
  }

  bucket_list_iterator operator+(const difference_type &n) const {
    bucket_list_iterator copy = this;
    copy += n;
    return copy;
  }
  bucket_list_iterator &operator+=(const difference_type &n) {
    if (n < 0) { return operator-=(-n); }
    // determine correct bucket
    buckets_iterator bucket                  = currentBucket;
    const unsigned remainingElementsInBucket = currentBucket->end() - currentPositionInBucket; // including current
    difference_type remainingDifference      = n - remainingElementsInBucket;                  // TODO check this
    while (remainingDifference > 0) {
      ++currentBucket;
      if (currentBucket == endBucket) { return bucket_list_iterator(beginBucket, endBucket, 0); } // end iterator
      remainingDifference -= currentBucket->size();
    }

    // determine position in currentBucket
    currentPositionInBucket = currentBucket->begin() + (remainingDifference + currentBucket->size());
    return *this;
  }

  bucket_list_iterator operator-(const difference_type &n) const {
    bucket_list_iterator copy = this;
    copy -= n;
    return copy;
  }
  bucket_list_iterator &operator-=(const difference_type &n) {
    if (n < 0) { return operator+=(-n); }
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

  bool operator==(const bucket_list_iterator &other) const {
    return state == other.state && currentBucket == other.currentBucket &&
           currentPositionInBucket == other.currentPositionInBucket;
  }
  bool operator!=(const bucket_list_iterator &other) const { return !((*this) == other); }
  bool operator<(const bucket_list_iterator &other) {
    return currentBucket < other.currentBucket ||
           (currentBucket == other.currentBucket && currentPositionInBucket < other.currentPositionInBucket);
  }
  bool operator>(const bucket_list_iterator &other) {
    return currentBucket > other.currentBucket ||
           (currentBucket == other.currentBucket && currentPositionInBucket > other.currentPositionInBucket);
  }
  bool operator<=(const bucket_list_iterator &other) { return (*this) < other || (*this) == other; }
  bool operator>=(const bucket_list_iterator &other) { return (*this) > other || (*this) == other; }
};

#endif