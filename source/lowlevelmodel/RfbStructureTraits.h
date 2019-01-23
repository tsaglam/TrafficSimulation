#ifndef RFB_STRUCTURE_TRAITS
#define RFB_STRUCTURE_TRAITS

template<class RfbStructure>
struct rfbstructure_traits {
  using reverse_category = typename RfbStructure::reverse_category;
};

struct rfbstructure_reversible_sorted_iterator_tag {};
struct rfbstructure_buckets_tag {};

#endif