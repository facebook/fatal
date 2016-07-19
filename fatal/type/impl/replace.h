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

namespace fatal {
namespace impl_replace {

template <typename...> struct in;

template <
  template <typename...> class List,
  typename... Prefix,
  typename... Suffix
>
struct in<List<Prefix...>, List<Suffix...>> {
  template <typename... Args>
  using with = List<Prefix..., Args..., Suffix...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Prefix,
  T... Suffix
>
struct in<Sequence<T, Prefix...>, Sequence<T, Suffix...>> {
  template<T... Values>
  using with = Sequence<T, Prefix..., Values..., Suffix...>;
};

} // namespace impl_replace {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_replace_h
