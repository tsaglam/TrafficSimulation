#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <random>

// returns a non-deterministic random number which can be used as seed for pseudo-random number generation
unsigned int generateRandom() {
  std::random_device rd;
  return rd();
}

// returns a vector of the numeric type 'type' of 'size' random numbers in [min, max] with the given seed.
template <typename type>
std::vector<type> generateRandomVector(
    size_t size, unsigned int seed = 0, type min = 0, type max = std::numeric_limits<type>::max()) {
  std::default_random_engine randomEngine(seed);
  std::uniform_int_distribution<type> randomDist(min, max);
  std::vector<type> v(size);
  for (size_t i = 0; i < size; ++i) { v[i] = randomDist(randomEngine); }
  return v;
}

#endif