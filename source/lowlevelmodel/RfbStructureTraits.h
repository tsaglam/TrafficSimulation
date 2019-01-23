#ifndef RFB_STRUCTURE_TRAITS
#define RFB_STRUCTURE_TRAITS

template<template <typename Vehicle> typename RfbStructure>
struct rfbstructure_traits {
  using reverse_category = typename RfbStructure<Vehicle>::reverse_category;
};

struct rfbstructure_reversible_sorted_iterator_tag {};
struct rfbstructure_buckets_tag {};

#endif