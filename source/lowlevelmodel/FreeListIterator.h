#ifndef FREE_LIST_ITERATOR
#define FREE_LIST_ITERATOR

template <class T>
class FreeList;

template <class T, bool Const = false>
class free_list_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = T;
  using difference_type   = int;
  using reference         = typename std::conditional_t<Const, T const &, T &>;
  using pointer           = typename std::conditional_t<Const, T const *, T *>;

  using list_pointer = typename std::conditional_t<Const, FreeList<T> const *, FreeList<T> *>;

private:
  list_pointer list = 0;
  int position      = 0; // is -1 to represent end on reverse iteration

  friend class FreeList<T>;
  friend class free_list_iterator<T, !Const>;

  int findNextElement(const int currentPosition) const {
    for (unsigned i = currentPosition + 1; i < list->isFree.size(); ++i) {
      if (!list->isFree[i]) { return i; }
    }
    return list->isFree.size();
  }

  int findPrevElement(const int currentPosition) const {
    for (int i = currentPosition - 1; i >= 0; --i) {
      if (!list->isFree[i]) { return i; }
    }
    return -1;
  }

  int positionAdd(const int currentPosition, const unsigned summand) const {
    unsigned visitedElements = 0;
    for (unsigned i = currentPosition + 1; i < list->isFree.size(); ++i) {
      if (!list->isFree[i]) {
        if (++visitedElements == summand) { return i; }
      }
    }
    return list->isFree.size();
  }

  int positionSubtract(const int currentPosition, const unsigned subtrahend) const {
    unsigned visitedElements = 0;
    for (int i = currentPosition - 1; i >= 0; --i) {
      if (!list->isFree[i]) {
        if (++visitedElements == subtrahend) { return i; }
      }
    }
    return -1;
  }

  free_list_iterator<T, false> setToFree(FreeList<T> *_list) const {
    if (position < 0 || position >= (int)_list->isFree.size()) { return free_list_iterator<T, false>(_list, position); }
    _list->isFree[position] = true;
    return free_list_iterator<T, false>(_list, findNextElement(position));
  }

  free_list_iterator(list_pointer list, int position) : list(list), position(position) {}

public:
  free_list_iterator() = default;
  free_list_iterator(list_pointer list) : list(list), position(findNextElement(-1)) {} // begin iterator
  free_list_iterator(list_pointer list, bool reverse)
      : list(list), position(reverse ? -1 : list->isFree.size()) {} // end iterator

  operator free_list_iterator<T, true>() const { return free_list_iterator<T, true>(list, position); }

  reference operator*() const { return list->data[position]; }
  pointer operator->() const { return &list->data[position]; }

  free_list_iterator &operator++() {
    if (position != (int)list->isFree.size()) { position = findNextElement(position); }
    return *this;
  }
  free_list_iterator operator++(int) {
    free_list_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  free_list_iterator &operator--() {
    if (position > 0) { position = findPrevElement(position); }
    return *this;
  }
  free_list_iterator operator--(int) {
    free_list_iterator tmp(*this);
    --*this;
    return tmp;
  }

  free_list_iterator operator+(const difference_type &n) const {
    free_list_iterator copy = *this;
    copy += n;
    return copy;
  }
  free_list_iterator &operator+=(const difference_type &n) {
    if (position != (int)list->isFree.size()) { position = positionAdd(position, n); }
    return *this;
  }

  free_list_iterator operator-(const difference_type &n) const {
    free_list_iterator copy = *this;
    copy -= n;
    return copy;
  }
  free_list_iterator &operator-=(const difference_type &n) {
    if (position > 0) { position = positionSubtract(position, n); }
    return *this;
  }

  reference operator[](const difference_type &n) const { return *(this + n); }

  bool operator==(const free_list_iterator &other) const { return list == other.list && position == other.position; }
  bool operator!=(const free_list_iterator &other) const { return !((*this) == other); }
  bool operator<(const free_list_iterator &other) { return position < other.position; }
  bool operator>(const free_list_iterator &other) { return position > other.position; }
  bool operator<=(const free_list_iterator &other) { return position <= other.position; }
  bool operator>=(const free_list_iterator &other) { return position >= other.position; }
};

#endif