#ifndef UTILS_H
#define UTILS_H

/**
 * @brief      Compare two LowLevelCar by their distance from the start of their current street.
 * LowLevelCar a < LowLevelCar b if a is closer to the start of the street.
 * If the cars have the same distance, a < b if the (external) id of a > id of b.
 * The cars are compared regardless of their current street. The caller has to ensure that the cars are to the same
 * street to allow appropriate comparision.
 *
 * @param[in]  a     A LowLevelCar which is compared to b.
 * @param[in]  b     A LowLevelCar which is compared to a.
 *
 * @tparam     LowLevelCar   The low level data structure for car objects.
 *
 * @return     Whether LowLevelCar a < LowLevelCar b, i.e. closer to the start of their street.
 */
template <class LowLevelCar>
inline bool compareLess(const LowLevelCar &a, const LowLevelCar &b) {
  return (a.getDistance() < b.getDistance()) ||
         (a.getDistance() == b.getDistance() && a.getExternalId() > b.getExternalId());
}

template <class LowLevelCar>
inline bool compareGreater(const LowLevelCar &a, const LowLevelCar &b) {
  return (a.getDistance() > b.getDistance()) ||
         (a.getDistance() == b.getDistance() && a.getExternalId() < b.getExternalId());
}

#endif