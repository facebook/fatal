/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/functional/no_op.h>
#include <fatal/type/compare.h>
#include <fatal/type/identity.h>
#include <fatal/type/slice.h>
#include <fatal/type/sort.h>

#include <cassert>

#include <fatal/type/impl/search.h>

namespace fatal {

// TODO: DOCUMENT THE NEED FOR A SORTED LIST
template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = value_comparer,
  typename Needle,
  typename Visitor,
  typename... Args
>
static inline constexpr bool sorted_search(
  Needle &&needle,
  Visitor &&visitor,
  Args &&...args
) {
  return i_S::s<0, size<T>::value>::template S<T, Comparer, Filter>(
    static_cast<Needle &&>(needle),
    static_cast<Visitor &&>(visitor),
    static_cast<Args &&>(args)...
  );
}

template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = value_comparer,
  typename Needle
>
static inline constexpr bool sorted_search(Needle &&needle) {
  return sorted_search<T, Filter, Comparer>(
    static_cast<Needle &&>(needle),
    fn::no_op()
  );
}

template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = value_comparer,
  typename Needle,
  typename Visitor,
  typename... Args
>
static inline constexpr bool scalar_search(
  Needle &&needle,
  Visitor &&visitor,
  Args &&...args
) {
  return sorted_search<sort<T, Comparer, Filter>, Filter, Comparer>(
    static_cast<Needle &&>(needle),
    static_cast<Visitor &&>(visitor),
    static_cast<Args &&>(args)...
  );
}

template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = value_comparer,
  typename Needle
>
static inline constexpr bool scalar_search(Needle &&needle) {
  return scalar_search<T, Filter, Comparer>(
    static_cast<Needle &&>(needle),
    fn::no_op()
  );
}

template <typename T, typename Visitor, typename... Args>
static inline constexpr bool index_search(
  std::size_t needle,
  Visitor &&visitor,
  Args &&...args
) {
  return sorted_search<T, index<T>>(
    needle,
    static_cast<Visitor &&>(visitor),
    static_cast<Args &&>(args)...
  );
}

} // namespace fatal {
