#ifndef BUCKET_TRAITS
#define BUCKET_TRAITS

/**
 * Traits for Bucket types.
 */

template <typename Bucket>
struct bucket_traits {
  using erase_category = typename Bucket::erase_category;
};

struct bucket_single_erasable {};
struct bucket_multi_erasable {};

#endif
