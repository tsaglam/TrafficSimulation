#ifndef RFB_STRUCTURE_TRAITS
#define RFB_STRUCTURE_TRAITS

#include "LowLevelCar.h"

template <template <typename Vehicle> typename RfbStructure>
struct rfbstructure_traits {
  using Vehicle          = LowLevelCar;
  using reverse_category = typename RfbStructure<Vehicle>::reverse_category;
};

struct rfbstructure_reversible_sorted_iterator_tag {};
struct rfbstructure_buckets_tag {};

#endif