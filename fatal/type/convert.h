/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_convert_h
#define FATAL_INCLUDE_fatal_type_convert_h

#include <type_traits>

#include <fatal/type/impl/convert.h>

namespace fatal {

template <typename T>
using type_of = typename impl_conv::to<T>::type;

template <typename T>
using value_type_of = typename impl_conv::vto<T>::type;

template <
  template <typename V, V...> class Sequence,
  typename From,
  typename... T
>
using as_sequence = typename impl_conv::seq<Sequence, From, T...>::type;

template <template <typename...> class List, typename T>
using as_list = typename impl_conv::lst<List, T>::type;

namespace bound {

template <template <typename V, V...> class Sequence, typename... T>
struct as_sequence {
  template <typename From>
  using apply = fatal::as_sequence<Sequence, From, T...>;
};

template <template <typename...> class List>
struct as_list {
  template <typename T>
  using apply = fatal::as_list<List, T>;
};

} // namespace bound {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_convert_h
