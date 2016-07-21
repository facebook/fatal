/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_replace_h
#define FATAL_INCLUDE_fatal_type_impl_replace_h

#include <fatal/type/cat.h>
#include <fatal/type/slice.h>

namespace fatal {
namespace impl_replace {

template <typename> struct in;

template <template <typename...> class List, typename... Args>
struct in<List<Args...>> {
  template <std::size_t Index, typename... With>
  using at = lcat<
    head<List<Args...>, Index>,
    tail<List<Args...>, Index + 1>,
    With...
  >;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct in<Sequence<T, Values...>> {
  template <std::size_t Index, T... With>
  using at = typename vcat<
    head<Sequence<T, Values...>, Index>,
    tail<Sequence<T, Values...>, Index + 1>
  >::template apply<With...>;
};

} // namespace impl_replace {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_replace_h
