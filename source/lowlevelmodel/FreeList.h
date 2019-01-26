#ifndef FREE_LIST_H
#define FREE_LIST_H

#include <iostream>
#include <vector>

#include "FreeListIterator.h"

template <typename T>
class FreeList {
  friend class free_list_iterator<T, false>;
  friend class free_list_iterator<T, true>;

  std::vector<bool> isFree;
  std::vector<T> data;

  size_t elementCount = 0;

  size_t findFreeCell() const {
    for (size_t index = 0; index < isFree.size(); ++index) {
      if (isFree[index]) { return index; }
    }
    return isFree.size();
  }

  size_t resizeAndReturnNextFreeCell(const size_t newSize) {
    // Alloc new vectors
    std::vector<T> newData(newSize);
    std::vector<bool> newIsFree(newSize, true);
    // Move data from old to new vector
    size_t index = 0;
    for (size_t i = 0; i < data.size(); ++i) {
      if (isFree[i]) { continue; } // skip empty cells
      newData[index]   = std::move(data[i]);
      newIsFree[index] = false;
      ++index;
    }
    // Assign new vectors to class members
    data   = newData;
    isFree = newIsFree;
    return index;
  }

public:
  using iterator       = free_list_iterator<T, false>;
  using const_iterator = free_list_iterator<T, true>;

  FreeList() = default;
  FreeList(size_t n) : isFree(n, true), data(n) {}

  bool empty() const { return elementCount == 0; }
  size_t size() const { return elementCount; }

  void push_back(const T &val) {
    size_t index = findFreeCell();
    if (index >= data.size()) { index = resizeAndReturnNextFreeCell(data.size() == 0 ? 2 : data.size() * 2); }
    data[index]   = val;
    isFree[index] = false;
    ++elementCount;
  }
  void push_back(T &&val) {
    size_t index = findFreeCell();
    if (index >= data.size()) { index = resizeAndReturnNextFreeCell(data.size() == 0 ? 2 : data.size() * 2); }
    data[index]   = std::move(val);
    isFree[index] = false;
    ++elementCount;
  }

  iterator erase(const_iterator position) {
    --elementCount;
    iterator it = position.setToFree(this);
    if (elementCount < data.size() / 4) { resizeAndReturnNextFreeCell(data.size() / 2); }
    return it;
  }

  iterator begin() { return iterator(this); }
  const_iterator begin() const { return const_iterator(this); }
  const_iterator cbegin() const { return const_iterator(this); }

  iterator end() { return iterator(this, false); }
  const_iterator end() const { return const_iterator(this, false); }
  const_iterator cend() const { return const_iterator(this, false); }
};

#endif