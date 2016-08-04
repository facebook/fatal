/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_search_h
#define FATAL_INCLUDE_fatal_type_search_h

#include <fatal/functional/no_op.h>
#include <fatal/type/apply.h>
#include <fatal/type/identity.h>
#include <fatal/type/slice.h>

#include <utility>

#include <fatal/type/impl/search.h>

namespace fatal {

// TODO: MOVE IT SOMEWHERE ELSE
template <typename LHS>
struct value_comparer {
  template <typename RHS>
  static constexpr bool less(RHS &&rhs) {
    return LHS::value < rhs;
  }

  template <typename RHS>
  static constexpr bool equal(RHS &&rhs) {
    return LHS::value == rhs;
  }

  template <typename RHS>
  static constexpr bool greater(RHS &&rhs) {
    return LHS::value > rhs;
  }
};

// TODO: USE A CUSTOM COMPARER
// TODO: DOCUMENT THE NEED FOR A SORTED LIST
template <
  typename T,
  template <typename> class Filter = identity,
  template <typename> class Comparer = value_comparer,
  typename Needle,
  typename Visitor,
  typename... Args
>
constexpr bool sorted_search(
  Needle &&needle,
  Visitor &&visitor,
  Args &&...args
) {
  return impl_srch::srt<T>::template sorted_search<Filter, Comparer>(
    std::forward<Needle>(needle),
    std::forward<Visitor>(visitor),
    std::forward<Args>(args)...
  );
}

template <
  typename T,
  template <typename> class Filter = identity,
  template <typename> class Comparer = value_comparer,
  typename Needle
>
constexpr bool sorted_search(Needle &&needle) {
  return sorted_search<T, Filter, Comparer>(
    std::forward<Needle>(needle),
    fn::no_op()
  );
}

template <
  typename T,
  template <typename> class Comparer = value_comparer,
  typename... Args
>
constexpr bool sorted_map_search(Args &&...args) {
  return sorted_search<T, first, Comparer>(std::forward<Args>(args)...);
}

// TODO: MOVE SOMEWHERE ELSE??
template <typename T, typename Visitor, typename... Args>
void foreach(Visitor &&visitor, Args &&...args) {
  apply_to<T, impl_srch::fe>::foreach(
    std::forward<Visitor>(visitor),
    std::forward<Args>(args)...
  );
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_search_h
