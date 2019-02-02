#ifndef CIRCULAR_VECTOR_H
#define CIRCULAR_VECTOR_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#define MOD(x, divisor) (((divisor) == 0) ? 0 : (((x) % (divisor)) + (divisor)) % (divisor))

/**
 * CircularVector behaves mostly like a vector, but is backed by a circular buffer.
 */
template <class T, class Allocator = std::allocator<T>>
class CircularVector {
public:
  template <bool Const>
  class BaseIterator {
  public:
    using vector_type = typename std::vector<T, Allocator>;
    using vector_iterator =
        typename std::conditional_t<Const, typename vector_type::const_iterator, typename vector_type::iterator>;
    using this_type = BaseIterator<Const>;

  public:
    /*
     * Iterator trait definitions.
     */

    using iterator_category = typename std::iterator_traits<vector_iterator>::iterator_category;
    using value_type        = typename std::iterator_traits<vector_iterator>::value_type;
    using difference_type   = typename std::iterator_traits<vector_iterator>::difference_type;
    using reference         = typename std::iterator_traits<vector_iterator>::reference;
    using pointer           = typename std::iterator_traits<vector_iterator>::pointer;

  private:
    /**
     * Iterator corresponding to the position of this iterator in the backing vector.
     */
    vector_iterator pos;
    /**
     * Iterator corresponding to "offset", that is the position of the logical first element in the circular vector.
     */
    vector_iterator offset;
    /**
     * Begin iterator of the backing vector.
     */
    vector_iterator begin;
    /**
     * End iterator of the backing vector.
     */
    vector_iterator end;

  public:
    /*
     * Definitions satisfying the LegacyIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/Iterator
     */

    reference operator*() const { return *pos; }

    this_type &operator++() {
      ++pos;
      if (pos == end) pos = begin;

      return *this;
    }

    /*
     * Definitions satisfying the EqualityComparable requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/EqualityComparable
     */

    friend bool operator==(const this_type &lhs, const this_type &rhs) { return lhs.pos == rhs.pos; }

    /*
     * Further definitions satisfying the LegacyInputIterator requirement
     *
     * https://en.cppreference.com/w/cpp/named_req/InputIterator
     *
     * and the LegacyOutputIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/OutputIterator
     */

    pointer operator->() const { return pos.operator->(); }

    this_type operator++(int) {
      this_type tmp;
      ++this;
      return tmp;
    }

    friend bool operator!=(const this_type &lhs, const this_type &rhs) { return lhs.pos != rhs.pos; }

    /*
     * Further definitions satisfying the LegacyBidirectionalIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
     */

    this_type &operator--() {
      if (pos == begin) pos = end;
      --pos;

      return *this;
    }

    this_type operator--(int) {
      this_type tmp;
      --this;
      return tmp;
    }

    /*
     * Further definitions satisfying the LegacyRandomAccessIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
     */

    this_type operator+(const difference_type &n) const {
      this_type tmp(*this);
      tmp += n;
      return tmp;
    }

    this_type &operator+=(const difference_type &n) {
      pos = begin + MOD((pos - begin) + n, end - begin);
      return *this;
    }

    this_type operator-(const difference_type &n) const {
      this_type tmp(*this);
      tmp -= n;
      return tmp;
    }

    this_type &operator-=(const difference_type &n) {
      pos = begin + MOD((pos - begin) - n, end - begin);
      return *this;
    }

    difference_type operator-(const this_type &other) const { return index() - other.index(); }

    reference operator[](const difference_type &n) const { return *(*this + n); }

    friend bool operator<(const this_type &lhs, const this_type &rhs) { return lhs.index() < rhs.index(); }

    friend bool operator>(const this_type &lhs, const this_type &rhs) { return lhs.index() > rhs.index(); }

    friend bool operator<=(const this_type &lhs, const this_type &rhs) { return lhs.index() <= rhs.index(); }

    friend bool operator>=(const this_type &lhs, const this_type &rhs) { return lhs.index() >= rhs.index(); }

    /*
     * Further definitions satisfying the LegacyForwardIterator requirement.
     *
     * https://en.cppreference.com/w/cpp/named_req/ForwardIterator
     */
    BaseIterator() {}

    BaseIterator(vector_iterator _pos, vector_iterator _offset, vector_iterator _begin, vector_iterator _end)
        : pos(_pos), offset(_offset), begin(_begin), end(_end) {}

    operator BaseIterator<true>() const { return BaseIterator<true>(pos, offset, begin, end); }

  private:
    difference_type index() const { return (pos >= offset) ? pos - offset : (end - offset) + (pos - begin); }
  };

public:
  using vector_type = std::vector<T, Allocator>;

  using value_type      = T;
  using allocator_type  = Allocator;
  using size_type       = typename vector_type::size_type;
  using difference_type = typename vector_type::difference_type;
  using reference       = typename vector_type::reference;
  using const_reference = typename vector_type::const_reference;
  using pointer         = typename vector_type::pointer;
  using const_pointer   = typename vector_type::const_pointer;

  using iterator               = BaseIterator<false>;
  using const_iterator         = BaseIterator<true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  vector_type vec;

  size_type length;
  difference_type offset;

public:
  CircularVector() : vec(), length(0), offset(0) {}

  explicit CircularVector(const Allocator &alloc) : vec(alloc), length(0), offset(0) {}

  CircularVector(size_type count, const T &value, const Allocator &alloc = Allocator())
      : vec(count + 1, value, alloc), length(count), offset(0) {}

  explicit CircularVector(size_type count, const Allocator &alloc = Allocator())
      : vec(count + 1, alloc), length(count), offset(0) {}

  // template <class InputIt>
  // CircularVector(InputIt first, InputIt last, const Allocator &alloc = Allocator());

  CircularVector(const CircularVector &other) : vec(other.vec.size()), length(other.length), offset(0) {
    other.copyToOther(vec);
  }

  CircularVector(const CircularVector &other, const Allocator &alloc)
      : vec(other.vec.size(), alloc), length(other.length), offset(0) {
    other.copyToOther(vec);
  }

  CircularVector(CircularVector &&other) noexcept
      : vec(std::move(other.vec)), length(other.length), offset(other.offset) {}

  CircularVector(CircularVector &&other, const Allocator &alloc)
      : vec(std::move(other.vec), alloc), length(other.length), offset(other.offset) {}

  // CircularVector( std::initializer_list<T> init,
  //         const Allocator& alloc = Allocator() ) : {}

  // void assign(size_type count, const T &value);

  // template <class InputIt>
  // void assign(InputIt first, InputIt last);

  // void assign(std::initializer_list<T> ilist);

  allocator_type get_allocator() const { return vec.get_allocator(); }

  reference operator[](size_type pos) { return vec[wrapOffset(pos)]; }
  const_reference operator[](size_type pos) const { return vec[wrapOffset(pos)]; }

  reference at(size_type pos) {
    if (pos < 0 || pos >= length) throw std::out_of_range("Index out of range");

    return vec[wrapOffset(pos)];
  }
  const_reference at(size_type pos) const {
    if (pos < 0 || pos >= length) throw std::out_of_range("Index out of range");

    return vec[wrapOffset(pos)];
  }

  reference front() { return vec[offset]; }
  const_reference front() const { return vec[offset]; }

  reference back() { return vec[wrapOffset(offset + static_cast<difference_type>(length) - 1)]; }
  const_reference back() const { return vec[wrapOffset(offset + static_cast<difference_type>(length) - 1)]; }

  iterator begin() noexcept { return iterator(vec.begin() + offset, vec.begin() + offset, vec.begin(), vec.end()); }

  const_iterator begin() const noexcept {
    return const_iterator(vec.begin() + offset, vec.begin() + offset, vec.begin(), vec.end());
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(vec.cbegin() + offset, vec.cbegin() + offset, vec.cbegin(), vec.cend());
  }

  iterator end() noexcept {
    return iterator(vec.begin() + wrapOffset(offset + length), vec.begin() + offset, vec.begin(), vec.end());
  }

  const_iterator end() const noexcept {
    return const_iterator(vec.begin() + wrapOffset(offset + length), vec.begin() + offset, vec.begin(), vec.end());
  }

  const_iterator cend() const noexcept {
    return const_iterator(vec.cbegin() + wrapOffset(offset + length), vec.cbegin() + offset, vec.cbegin(), vec.cend());
  }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

  bool empty() const noexcept { return length == 0; }

  size_type size() const noexcept { return length; }

  size_type max_size() const noexcept { return vec.max_size(); }

  size_type capacity() const noexcept { return (vec.size() == 0) ? 0 : vec.size() - 1; }

  void reserve(size_type new_cap) { autoEnlarge(new_cap); }

  /**
   * Shrinks the vector to reduce the allocated memory.
   *
   * Only re-allocates if the size is less than a quarter of the capacity.
   * In this case the new capacity will be double the current size.
   *
   * If a reallocation occurs, all iterators and references are invalidated.
   */
  void shrink_to_fit() {
    if (length < capacity() / 4) allocateAndCopy(length * 2);
  }

  void clear() noexcept {
    length = 0;
    offset = 0;
  }

  // iterator insert( const_iterator pos, const T& value );

  // iterator insert( const_iterator pos, T&& value );

  // iterator insert( const_iterator pos, size_type count, const T& value );

  // template< class InputIt >
  // iterator insert( const_iterator pos, InputIt first, InputIt last );

  // iterator insert( const_iterator pos, std::initializer_list<T> ilist );

  // template< class... Args >
  // iterator emplace( const_iterator pos, Args&&... args );

  iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

  iterator erase(const_iterator first, const_iterator last) {
    difference_type rangeLength = last - first;

    const_iterator cachedCEnd   = cend();
    const_iterator cachedCBegin = cbegin();
    iterator cachedBegin        = begin();

    if (last == cachedCEnd) {
      difference_type rangeLength = last - first;
      length -= rangeLength;
      return cachedBegin + (first - cachedCBegin);
    }
    if (first == cachedCBegin) {
      offset = wrapOffset(offset + rangeLength);
      length -= rangeLength;
      return cachedBegin + (last - cachedCBegin);
    }

    difference_type lowerDist  = first - cachedCBegin;
    difference_type uppperDist = cachedCEnd - last;

    if (uppperDist >= lowerDist) {
      // The range is located closer towards the end of CircularVector

      iterator nonConstFirst = cachedBegin + (first - cachedCBegin);

      // Copy to the left, overwriting the range
      std::copy(last, cachedCEnd, nonConstFirst);

      length -= rangeLength;
      return nonConstFirst;
    } else {
      // The range is located closer towards the begin of CircularVector

      iterator nonConstLast = cachedBegin + (last - cachedCBegin);

      // Copy to the right, overwriting the range
      std::copy_backward(cachedCBegin, first, cachedBegin + (last - cachedCBegin));

      offset = wrapOffset(offset + rangeLength);
      length -= rangeLength;
      return nonConstLast;
    }
  }

  void push_front(const T &value) {
    if (capacityExhausted(1)) autoEnlarge();
    offset = wrapOffset(offset - 1);
    ++length;
    vec[offset] = value;
  }

  void push_front(T &&value) {
    if (capacityExhausted(1)) autoEnlarge();
    offset = wrapOffset(offset - 1);
    ++length;
    vec[offset] = value;
  }

  void push_back(const T &value) {
    if (capacityExhausted(1)) autoEnlarge();
    vec[wrapOffset(offset + length)] = value;
    ++length;
  }

  void push_back(T &&value) {
    if (capacityExhausted(1)) autoEnlarge();
    vec[wrapOffset(offset + length)] = value;
    ++length;
  }

  // template< class... Args >
  // void emplace_back( Args&&... args );

  /**
   * Removes the first element of the vector.
   *
   * No iterators or references except begin() and front() are invalidated.
   */
  void pop_front() {
    offset = wrapOffset(offset + 1);
    --length;
  }

  /**
   * Removes the last element of the vector.
   *
   * No iterators or references except end() and back() are invalidated.
   */
  void pop_back() { --length; }

  void resize(size_type count) {
    if (count <= length)
      length = count;
    else {
      if (count > capacity()) autoEnlarge(count);

      for (size_type i = length; i < count; ++i)
        std::allocator_traits<Allocator>::construct(vec.get_allocator(), &this[i]);

      length = count;
    }
  }

  void resize(size_type count, const value_type &value) {
    if (count <= length)
      length = count;
    else {
      if (count > capacity()) autoEnlarge(count);

      auto fillBegin = vec.begin() + wrapOffset(offset + length);
      auto fillEnd   = vec.begin() + wrapOffset(offset) + count;
      if (fillEnd > vec.end()) fillEnd = vec.end();

      std::fill(fillBegin, fillEnd, value);

      if (fillEnd == vec.end()) std::fill(vec.begin(), vec.begin() + wrapOffset(offset + count), value);

      length = count;
    }
  }

  void swap(CircularVector &other) noexcept(
      noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
               std::allocator_traits<Allocator>::is_always_equal::value)) {
    vec.swap(other.vec);
    length = other.length;
    offset = other.offset;
  }

  friend void swap(CircularVector<T, Allocator> &lhs, CircularVector<T, Allocator> &rhs) noexcept(
      noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }

private:
  difference_type wrapOffset(difference_type offset) const {
    return MOD(offset, static_cast<difference_type>(vec.size()));
  }

  void autoEnlarge(size_type minCap) {
    size_type newCap = std::max(static_cast<size_type>(4), capacity());

    while (newCap < minCap) newCap *= 2;

    allocateAndCopy(newCap);
  }

  void autoEnlarge() { autoEnlarge(capacity() + 1); }

  bool capacityExhausted(size_type added) { return length + added + 1 > vec.size(); }

  void allocateAndCopy(size_type cap) {
    assert(cap >= length);

    vector_type newVec(cap + 1);

    moveToOther(newVec);

    vec.swap(newVec);
    offset = 0;
  }

  void moveToOther(vector_type &other) {
    assert(other.size() >= length + 1);

    auto contentBegin = vec.begin() + offset;
    auto contentEnd   = vec.begin() + offset + length;
    if (contentEnd > vec.end()) contentEnd = vec.end();

    auto lastUsed = std::move(contentBegin, contentEnd, other.begin());

    if (contentEnd == vec.end()) std::move(vec.begin(), vec.begin() + wrapOffset(offset + length), lastUsed);
  }

  void copyToOther(vector_type &other) const {
    assert(other.size() >= length + 1);

    auto contentBegin = vec.begin() + offset;
    auto contentEnd   = vec.begin() + offset + length;
    if (contentEnd > vec.end()) contentEnd = vec.end();

    auto lastUsed = std::copy(contentBegin, contentEnd, other.begin());

    if (contentEnd == vec.end()) std::copy(vec.begin(), vec.begin() + wrapOffset(offset + length), lastUsed);
  }
};

#endif
