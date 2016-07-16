/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_convert_h
#define FATAL_INCLUDE_fatal_type_impl_convert_h

namespace fatal {
namespace impl_conv {

template <template <typename V, V...> class, typename...> struct to_seq;

template <
  template <typename V, V...> class Sequence,
  template <typename...> class List,
  typename... Values,
  typename T
>
struct to_seq<Sequence, List<Values...>, T> {
  using type = Sequence<T, Values::value...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename...> class List,
  typename Head,
  typename... Tail
>
struct to_seq<Sequence, List<Head, Tail...>> {
  using type = Sequence<decltype(Head::value), Head::value, Tail::value...>;
};

} // namespace impl_conv {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_convert_h
