/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_push_h
#define FATAL_INCLUDE_fatal_type_impl_push_h

namespace fatal {
namespace impl_push {

template <typename> struct to;

template <template <typename...> class List, typename... Args>
struct to<List<Args...>> {
  template <typename... Suffix>
  using back = List<Args..., Suffix...>;

  template <typename... Prefix>
  using front = List<Prefix..., Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values
>
struct to<Sequence<T, Values...>> {
  template<T... Suffix>
  using back = Sequence<T, Values..., Suffix...>;

  template<T... Prefix>
  using front = Sequence<T, Prefix..., Values...>;
};

} // namespace impl_push {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_push_h
