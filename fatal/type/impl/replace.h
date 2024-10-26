/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/cat.h>
#include <fatal/type/slice.h>

namespace fatal {
namespace impl_rp {

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
#if defined(__GNUC__) && !defined(__clang__)
  using seq = Sequence<T, Values...>;
  template <std::size_t Index, T... With>
  using at = typename vcat<
    head<seq, Index>,
    tail<seq, Index + 1>
  >::template apply<With...>;
#else // not GCC
  template <std::size_t Index, T... With>
  using at = typename vcat<
    head<Sequence<T, Values...>, Index>,
    tail<Sequence<T, Values...>, Index + 1>
  >::template apply<With...>;
#endif // not GCC
};

} // namespace impl_rp {
} // namespace fatal {
