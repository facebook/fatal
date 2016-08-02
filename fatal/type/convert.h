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

#include <fatal/type/identity.h>
#include <fatal/type/list.h>
#include <fatal/type/map.h>
#include <fatal/type/pair.h>
#include <fatal/type/sequence.h>

#include <type_traits>

#include <fatal/type/impl/convert.h>

namespace fatal {

template <typename T>
using type_of = typename impl_conv::to<T>::type;

template <typename T>
using value_type_of = typename impl_conv::vto<T>::type;

template <
  typename From,
  template <typename V, V...> class Sequence = sequence,
  typename... T
>
using as_sequence = typename impl_conv::seq<Sequence, From, T...>::type;

template <typename T, template <typename...> class List = list>
using as_list = typename impl_conv::lst<List, T>::type;

template <
  typename T,
  template <typename> class Key,
  template <typename> class Value = identity,
  template <typename...> class Map = map,
  template <typename...> class Pair = pair
>
using as_map = typename impl_conv::mp<T, Map, Pair, Key, Value>::type;

namespace bound {

template <template <typename V, V...> class Sequence, typename... T>
struct as_sequence {
  template <typename From>
  using apply = fatal::as_sequence<From, Sequence, T...>;
};

template <template <typename...> class List>
struct as_list {
  template <typename T>
  using apply = fatal::as_list<T, List>;
};

} // namespace bound {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_convert_h
