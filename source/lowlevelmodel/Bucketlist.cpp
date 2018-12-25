#include "Bucketlist.h"


template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator Bucketlist<Car, Bucket>::_begin() {
  return iterator(&buckets[0]);
}
template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator Bucketlist<Car, Bucket>::_end() {
  return iterator(&lastBucket, lastBucket->end());
}
template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator Bucketlist<Car, Bucket>::_begin() const {
  return const_iterator(&buckets[0]);
}
template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator Bucketlist<Car, Bucket>::_end() const {
  return const_iterator(&lastBucket, lastBucket->end());
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator Bucketlist<Car, Bucket>::_cbegin() const {
  return const_iterator(&buckets[0]);
}
template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator Bucketlist<Car, Bucket>::_cend() const {
  return const_iterator(&lastBucket, lastBucket->end());
}

// ---- Iterators ----

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::iterator::iterator() {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::iterator::iterator(Bucket *initialBucket)
    : currentBucket(initialBucket), currentBucketIterator(initialBucket->begin()) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::iterator::iterator(Bucket *initialBucket, typename Bucket::iterator initialBucketIterator)
    : currentBucket(initialBucket), currentBucketIterator(initialBucketIterator) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::iterator::iterator(const iterator &it)
    : currentBucketIterator(it.currentBucketIterator), currentBucket(it.currentBucket) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::iterator::~iterator() {}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator &Bucketlist<Car, Bucket>::iterator::operator=(const iterator &it) {
  currentBucketIterator = it.currentBucketIterator;
  currentBucket         = it.currentBucket;
}

template <class Car, class Bucket>
bool Bucketlist<Car, Bucket>::iterator::operator==(const iterator &it) const {
  return currentBucket == it.currentBucket && currentBucketIterator == it.currentBucketIterator;
}

template <class Car, class Bucket>
bool Bucketlist<Car, Bucket>::iterator::operator!=(const iterator &it) const {
  return currentBucket != it.currentBucket || currentBucketIterator != it.currentBucketIterator;
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator &Bucketlist<Car, Bucket>::iterator::operator++() {
  ++currentBucketIterator;
  // if end of the current bucket is reached move on to the next bucket
  if (currentBucketIterator == currentBucket->end()) { currentBucketIterator = (++currentBucket)->begin(); }
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator::reference Bucketlist<Car, Bucket>::iterator::operator*() const {
  return currentBucketIterator.operator*();
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::iterator::pointer Bucketlist<Car, Bucket>::iterator::operator->() const {
  return currentBucketIterator.operator->();
}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::const_iterator() {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::const_iterator(const Bucket *initialBucket)
    : currentBucket(initialBucket), currentBucketIterator(initialBucket->cbegin()) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::const_iterator(
    Bucket *initialBucket, typename Bucket::const_iterator initialBucketIterator)
    : currentBucket(initialBucket), currentBucketIterator(initialBucketIterator) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::const_iterator(const const_iterator &it)
    : currentBucketIterator(it.currentBucketIterator), currentBucket(it.currentBucket) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::const_iterator(const iterator &it)
    : currentBucketIterator(it.currentBucketIterator), currentBucket(it.currentBucket) {}

template <class Car, class Bucket>
Bucketlist<Car, Bucket>::const_iterator::~const_iterator() {}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator &Bucketlist<Car, Bucket>::const_iterator::operator=(const const_iterator & it) {
  currentBucketIterator = it.currentBucketIterator;
  currentBucket         = it.currentBucket;
}

template <class Car, class Bucket>
bool Bucketlist<Car, Bucket>::const_iterator::operator==(const const_iterator &it) const {
  return currentBucket == it.currentBucket && currentBucketIterator == it.currentBucketIterator;
}

template <class Car, class Bucket>
bool Bucketlist<Car, Bucket>::const_iterator::operator!=(const const_iterator &it) const {
  return currentBucket != it.currentBucket || currentBucketIterator != it.currentBucketIterator;
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator &Bucketlist<Car, Bucket>::const_iterator::operator++() {
  ++currentBucketIterator;
  // if end of the current bucket is reached move on to the next bucket
  if (currentBucketIterator == currentBucket->end()) { currentBucketIterator = (++currentBucket)->begin(); }
}

template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator::reference Bucketlist<Car, Bucket>::const_iterator::operator*() const {
  return currentBucketIterator.operator*();
}
template <class Car, class Bucket>
typename Bucketlist<Car, Bucket>::const_iterator::pointer Bucketlist<Car, Bucket>::const_iterator::operator->() const {
  return currentBucketIterator.operator->();
}
