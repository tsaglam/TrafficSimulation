#ifndef MERGE_N_SKIP_H
#define MERGE_N_SKIP_H

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <vector>

#include "CircularVector.h"
#include "RfbStructureTraits.h"
#include "utils.h"

#define MERGE_N_SKIP_CHECKPOINT_INTERVAL 50.0
#define MAX_LANES 3

/**
 * Traits for the Container types used as the underlying data store of the MergeNSkip data structure.
 */
template <template <typename T> typename Container, typename ElementType>
struct mergenskip_container_traits {
  using insert_category = typename Container<ElementType>::insert_category;
};

struct insert_category_push_front {};
struct insert_category_collect_insert {};

template <typename Vehicle, template <typename T> typename Container = CircularVector>
class MergeNSkip {
private:
  class VehicleEntry;

  using vector_type = Container<VehicleEntry>;

  class VehicleEntry {
  public:
    typename vector_type::size_type nextBehind;
    typename vector_type::size_type nextInFront;
    Vehicle vehicle;

    VehicleEntry() = default;

    VehicleEntry(const Vehicle &vehicle) : vehicle(vehicle) {}
    VehicleEntry(Vehicle &&vehicle) : vehicle(vehicle) {}

    friend bool operator<(const VehicleEntry &lhs, const VehicleEntry &rhs) {
      return compareLess(lhs.vehicle, rhs.vehicle);
    }

    friend bool operator>(const VehicleEntry &lhs, const VehicleEntry &rhs) {
      return compareGreater(lhs.vehicle, rhs.vehicle);
    }
  };

  class CheckpointLane {
  public:
    typename vector_type::size_type nextBehind;
    typename vector_type::size_type nextInFront;
  };

  class Checkpoint {
  public:
    std::array<CheckpointLane, MAX_LANES> lanes;
  };

public:
  template <typename VectorIterator, typename ConstVectorIterator, bool Const>
  class UnwrappingIterator {
  public:
    /*
     * Iterator trait definitions.
     */

    using iterator_category = typename std::iterator_traits<VectorIterator>::iterator_category;
    using value_type        = Vehicle;
    using difference_type   = typename std::iterator_traits<VectorIterator>::difference_type;
    using reference         = typename std::conditional_t<Const, Vehicle const &, Vehicle &>;
    using pointer           = typename std::conditional_t<Const, Vehicle const *, Vehicle *>;

  private:
    friend class MergeNSkip;

    /**
     * Contains the state of the iterator, also referred to as tag.
     */
    VectorIterator it;

  public:
    /*
     * Definitions satisfying the LegacyIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/Iterator
     */

    reference operator*() const { return (*it).vehicle; }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &operator++() {
      ++it;
      return *this;
    }

    /*
     * Definitions satisfying the EqualityComparable requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/EqualityComparable
     */

    friend bool operator==(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it == rhs.it;
    }

    /*
     * Further definitions satisfying the LegacyInputIterator requirement
     *
     * https://en.cppreference.com/w/cpp/named_req/InputIterator
     *
     * and the LegacyOutputIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/OutputIterator
     */

    pointer operator->() const { return &(it->vehicle); }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> operator++(int) {
      return UnwrappingIterator<VectorIterator, ConstVectorIterator, Const>(it++);
    }

    friend bool operator!=(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it != rhs.it;
    }

    /*
     * Further definitions satisfying the LegacyBidirectionalIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
     */

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &operator--() {
      --it;
      return *this;
    }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> operator--(int) {
      return UnwrappingIterator<VectorIterator, ConstVectorIterator, Const>(it--);
    }

    /*
     * Further definitions satisfying the LegacyRandomAccessIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
     */

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> operator+(const difference_type &n) const {
      return UnwrappingIterator<VectorIterator, ConstVectorIterator, Const>(it + n);
    }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &operator+=(const difference_type &n) {
      it += n;
      return *this;
    }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> operator-(const difference_type &n) const {
      return UnwrappingIterator<VectorIterator, ConstVectorIterator, Const>(it - n);
    }

    UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &operator-=(const difference_type &n) {
      it -= n;
      return *this;
    }

    difference_type operator-(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &other) const {
      return it - other.it;
    }

    reference operator[](const difference_type &n) const { return it[n].vehicle; }

    /**
     * Returns true if lhs is less than rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator<(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it < rhs.it;
    }

    /**
     * Returns true if lhs is greater than rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator>(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it > rhs.it;
    }

    /**
     * Returns true if lhs is less than or equal to rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator<=(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it <= rhs.it;
    }

    /**
     * Returns true if lhs is greater than or equal to rhs, false otherwise.
     *
     * The operation is only results in sensible results if both iterators are in the PROXY state. In this case the
     * operation is forwarded to the RfbIterator instances. The operation complies with a total ordering in all cases.
     */
    friend bool operator>=(const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &lhs,
        const UnwrappingIterator<VectorIterator, ConstVectorIterator, Const> &rhs) {
      return lhs.it >= rhs.it;
    }

    /*
     * Further definitions satisfying the LegacyForwardIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/ForwardIterator
     */
    UnwrappingIterator() {}

    UnwrappingIterator(VectorIterator _it) : it(_it) {}

    /**
     * Conversion operator, converts non-const into const iterator.
     */
    operator UnwrappingIterator<ConstVectorIterator, ConstVectorIterator, true>() const {
      return UnwrappingIterator<ConstVectorIterator, ConstVectorIterator, true>(it);
    }
  };

public:
  // ------- Iterator & Iterable type defs -------
  using iterator = UnwrappingIterator<typename vector_type::iterator, typename vector_type::const_iterator, false>;
  using const_iterator =
      UnwrappingIterator<typename vector_type::const_iterator, typename vector_type::const_iterator, true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using reverse_category = rfbstructure_reversible_sorted_iterator_tag;

private:
  friend class _AllCarIterable;
  friend class _BeyondsCarIterable;

  /**
   * The number of lanes on the street (in the current direction).
   */
  const unsigned int laneCount;
  /**
   * The length of the street.
   */
  const double length;
  /**
   * The interval at which checkpoints are inserted.
   */
  const double checkpointInterval;

  // ------- Data Storage -------
  /**
   * All cars that are currently on this street.
   * Sorted by their distance (regardless of their lane).
   */
  vector_type street;

  /**
   * rBeyondsIndex points to the first car beyond the street when reverse-iterating.
   * The car is accessible via street.end() - rBeyondsIndex which might resolve to rbegin().
   */
  typename vector_type::difference_type rBeyondsIndex;

  /**
   * Checkpoints at checkpointInterval.
   */
  std::vector<Checkpoint> checkpoints;

  /**
   * Vector holding newly inserted cars.
   * This field is only used when the insert_category field of the mergenskip_container_traits of the Container is
   * insert_category_collect_insert.
   */
  std::vector<Vehicle> insertedCars;

public:
  // ------- Constructor -------
  /**
   * @brief      Default constructor. Valid, but results in unspecified behavior.
   */
  MergeNSkip() = default;

  /**
   * @brief      Proper constructor, initializes a new instance with specified parameters.
   *
   * @param[in]  laneCount  The number of lanes on the street (in the current direction).
   * @param[in]  length     The length of the street.
   */
  MergeNSkip(unsigned int laneCount, double length, double checkpointInterval = MERGE_N_SKIP_CHECKPOINT_INTERVAL)
      : laneCount(laneCount), length(length), checkpointInterval(checkpointInterval) {}

  template <bool Const = false>
  class _AllCarIterable {
    using IteratorType        = std::conditional_t<Const, const_iterator, iterator>;
    using ReverseIteratorType = std::conditional_t<Const, const_reverse_iterator, reverse_iterator>;
    using StreetReference     = std::conditional_t<Const, MergeNSkip const &, MergeNSkip &>;
    StreetReference dataStructure;

  public:
    _AllCarIterable(StreetReference dataStructure) : dataStructure(dataStructure) {}

    IteratorType begin() const { return IteratorType(dataStructure.street.begin()); }
    IteratorType end() const { return IteratorType(dataStructure.street.end()); }
    IteratorType cbegin() const { return IteratorType(dataStructure.street.cbegin()); }
    IteratorType cend() const { return IteratorType(dataStructure.street.cend()); }

    ReverseIteratorType rbegin() const { return ReverseIteratorType(dataStructure.street.rbegin()); }
    ReverseIteratorType rend() const { return ReverseIteratorType(dataStructure.street.rend()); }
    ReverseIteratorType crbegin() const { return ReverseIteratorType(dataStructure.street.crbegin()); }
    ReverseIteratorType crend() const { return ReverseIteratorType(dataStructure.street.crend()); }
  };

  template <bool Const = false>
  class _BeyondsCarIterable {
    using IteratorType    = std::conditional_t<Const, const_reverse_iterator, reverse_iterator>;
    using StreetReference = std::conditional_t<Const, MergeNSkip const &, MergeNSkip &>;
    StreetReference dataStructure;

  public:
    _BeyondsCarIterable(StreetReference dataStructure) : dataStructure(dataStructure) {}

    IteratorType begin() const { return IteratorType(dataStructure.street.rbegin()); }
    IteratorType end() const { return IteratorType(dataStructure.street.rbegin() + dataStructure.rBeyondsIndex); }
  };

  using AllCarIterable          = _AllCarIterable<>;
  using ConstAllCarIterable     = _AllCarIterable<true>;
  using BeyondsCarIterable      = _BeyondsCarIterable<>;
  using ConstBeyondsCarIterable = _BeyondsCarIterable<true>;

  // ------- Getter -------
  /**
   * @brief      Gets the number of lanes on the street (in the current direction).
   *
   * @return     The number of lanes.
   */
  unsigned int getLaneCount() const { return laneCount; }

  /**
   * @brief      Gets the length of the street.
   *
   * @return     The street length.
   */
  double getLength() const { return length; }

  /**
   * @brief      Gets the number cars on this street (in the current direction).
   * Cars beyond the street and cars that are inserted but not incorporated are not considered.
   *
   * @return     The number cars on this street.
   */
  unsigned int getCarCount() const { return street.size(); }

  // ------- Access to Neighboring Cars -------

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
    if (laneOffset == 0) {
      return iterator(street.begin() + currentCarIt.it->nextInFront);
    } else {
      const unsigned int lane = currentCarIt->getLane() + laneOffset;
      // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
      for (iterator it = currentCarIt + 1; it != street.end(); ++it) {
        if (it->getLane() == lane) { return iterator(it); }
      }
      return iterator(street.end()); // return end iterator if no prev car exists on the given lane
    }
  }
  const_iterator getNextCarInFront(const const_iterator currentCarIt, const int laneOffset = 0) const {
    if (laneOffset == 0) {
      return const_iterator(street.begin() + currentCarIt.it->nextInFront);
    } else {
      const unsigned int lane = currentCarIt->getLane() + laneOffset;
      // iterate all cars in front of the current car (on all lanes) return first car on the specified lane
      for (const_iterator it = currentCarIt + 1; it != street.end(); ++it) {
        if (it->getLane() == lane) { return const_iterator(it); }
      }
      return const_iterator(street.end()); // return end iterator if no prev car exists on the given lane
    }
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
    if (laneOffset == 0) {
      return iterator(street.begin() + currentCarIt.it->nextBehind);
    } else {
      const unsigned int lane = currentCarIt->getLane() + laneOffset;
      // iterate all cars behind the current car (on all lanes) return first car on the specified lane
      for (iterator it = currentCarIt; it != street.begin(); --it) { // reverse iteration
        if ((it - 1)->getLane() == lane) { return iterator(it - 1); }
      }
      return iterator(street.end()); // return end iterator if no next car exists on the given lane
    }
  }
  const_iterator getNextCarBehind(const const_iterator currentCarIt, const int laneOffset = 0) const {
    if (laneOffset == 0) {
      return const_iterator(street.begin() + currentCarIt.it->nextBehind);
    } else {
      const unsigned int lane = currentCarIt->getLane() + laneOffset;
      // iterate all cars behind the current car (on all lanes) return first car on the specified lane
      for (const_iterator it = currentCarIt; it != street.begin(); --it) { // reverse iteration
        if ((it - 1)->getLane() == lane) { return const_iterator(it - 1); }
      }
      return const_iterator(street.end()); // return end iterator if no next car exists on the given lane
    }
  }

  /**
   * @brief      Add a new car to the street using move semantics.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(Vehicle &&car) {
    _insertCar(car, typename mergenskip_container_traits<Container, VehicleEntry>::insert_category());
  }

  /**
   * @brief      Add a new car to the street using copy semantics.
   *
   * @param      car   The car to be inserted.
   */
  void insertCar(const Vehicle &car) {
    _insertCar(car, typename mergenskip_container_traits<Container, VehicleEntry>::insert_category());
  }

  /**
   * @brief      Incorporates all new cars into the underlying data structure while retaining its consistency.
   */
  void incorporateInsertedCars() {
    _incorporateInsertedCars(typename mergenskip_container_traits<Container, VehicleEntry>::insert_category());
  }

  /**
   * @brief      Update the position of all cars on this street in the underlying data structure while retaining its
   * consistency.
   * All cars in street are updated by calling their update() function. The restore the consistency street
   * is sorted. Cars that reached the end of this street are removed from street and moved to departedCars.
   */
  void updateCarsAndRestoreConsistency() {
    for (auto &entry : street) { entry.vehicle.update(); }
    std::sort(street.begin(), street.end()); // restore car order (sorted by distance)

    auto itRBegin = street.rbegin();
    auto itREnd   = street.rend();
    auto it       = itRBegin;
    for (; it != itREnd; ++it) {
      if (it->vehicle.getDistance() < length) break;
    }
    // The first car properly on the street is at itREnd - 1 - it.
    rBeyondsIndex = it - itRBegin;
  }

  /**
   * @brief      Iterable for iterating over all cars.
   * Cars are iterated in order of increasing distance from the start of the street, car with equal distance are
   * ordered by their id. The lanes are not considered for the sorting.
   * Cars which were added by insertCar() but not yet integrated into the data structure by a call to
   * incorporateInsertedCars() and cars that left this street and are accessible by the beyondsIterable are not
   * considered by the allIterable in this implementation.
   *
   * @return     An iterable object for all cars on this street.
   */
  AllCarIterable allIterable() { return AllCarIterable(*this); }
  ConstAllCarIterable allIterable() const { return ConstAllCarIterable(*this); }
  ConstAllCarIterable constAllIterable() const { return ConstAllCarIterable(*this); }

  /**
   * @brief      Iterable for iterating over cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   *
   * @return     An iterable object for all cars beyond this street.
   */
  BeyondsCarIterable beyondsIterable() { return BeyondsCarIterable(*this); }
  ConstBeyondsCarIterable beyondsIterable() const { return ConstBeyondsCarIterable(*this); }
  ConstBeyondsCarIterable constBeyondsIterable() const { return ConstBeyondsCarIterable(*this); }

  /**
   * @brief      Removes all cars which are currently "beyond the street".
   * Cars are beyond the street if their distance is greater than the length of the street.
   * Cars are removed by clearing the vector containing them.
   */
  void removeBeyonds() {
    street.erase(street.cend() - rBeyondsIndex, street.cend());
    rBeyondsIndex = 0;
  }

private:
  void _insertCar(Vehicle &&car, insert_category_push_front) {
    VehicleEntry entry = VehicleEntry(car);
    street.push_front(std::move(entry));
    street.front().vehicle.update();
  }

  void _insertCar(const Vehicle &car, insert_category_push_front) {
    VehicleEntry entry = VehicleEntry(car);
    street.push_front(entry);
    street.front().vehicle.update();
  }

  void _insertCar(Vehicle &&car, insert_category_collect_insert) {
    insertedCars.push_back(car);
    insertedCars.back().update();
  }

  void _insertCar(const Vehicle &car, insert_category_collect_insert) {
    insertedCars.push_back(car);
    insertedCars.back().update();
  }

  void _incorporateInsertedCars(insert_category_push_front) {
    std::sort(street.begin(), street.end()); // restore car order (sorted by distance)
    rBeyondsIndex = 0;
    buildIndex();
  }

  void _incorporateInsertedCars(insert_category_collect_insert) {
    street.insert(street.begin(), insertedCars.begin(), insertedCars.end());
    std::sort(street.begin(), street.end()); // restore car order (sorted by distance)
    rBeyondsIndex = 0;
    buildIndex();
    insertedCars.clear();
  }

  void buildIndex() {
    typename vector_type::iterator endIt = street.end();

    // Prepare the lastCarIts array
    std::array<typename vector_type::iterator, MAX_LANES> lastCarIts;
    std::fill_n(lastCarIts.begin(), MAX_LANES, endIt);

    // std::vector<Checkpoint>::iterator checkpointIt = checkpoints.begin();

    typename vector_type::iterator beginIt = street.begin();
    for (typename vector_type::iterator it = beginIt; it != endIt; ++it) {
      unsigned int lane = it->vehicle.getLane();
      if (lastCarIts[lane] != endIt) lastCarIts[lane]->nextInFront = it - beginIt;
      it->nextBehind   = lastCarIts[lane] - beginIt;
      lastCarIts[lane] = it;
    }

    for (typename vector_type::iterator &lastCarIt : lastCarIts) {
      if (lastCarIt != endIt) lastCarIt->nextInFront = endIt - beginIt;
    }
  }
};

template <template <typename T> typename Container>
struct ContainerisedMergeNSkip {
  template <typename ElementType>
  using MergeNSkip = MergeNSkip<ElementType, Container>;
};

template <typename T>
using StdVectorDefaultAllocator = std::vector<T, std::allocator<T>>;
template <typename T>
using CircularVectorDefaultAllocator = CircularVector<T, std::allocator<T>>;

/**
 * Traits specialisation for all std::vector container types with the default allocator.
 */
template <typename ElementType>
struct mergenskip_container_traits<StdVectorDefaultAllocator, ElementType> {
  using insert_category = insert_category_collect_insert;
};

/**
 * Traits specialisation for all CircularVector container types with the default allocator.
 */
template <typename ElementType>
struct mergenskip_container_traits<CircularVectorDefaultAllocator, ElementType> {
  using insert_category = insert_category_push_front;
};

/**
 * Typedefinition for MergeNSkipLinear, a MergeNSkip data structure backed by an std::vector.
 */
template <typename ElementType>
using MergeNSkipLinear = ContainerisedMergeNSkip<StdVectorDefaultAllocator>::MergeNSkip<ElementType>;
/**
 * Typedefinition for MergeNSkipLinear, a MergeNSkip data structure backed by a CircularVector.
 */
template <typename ElementType>
using MergeNSkipCircular = ContainerisedMergeNSkip<CircularVectorDefaultAllocator>::MergeNSkip<ElementType>;

#endif
