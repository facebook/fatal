/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_get_h
#define FATAL_INCLUDE_fatal_type_impl_get_h

#include <fatal/type/inherit.h>
#include <fatal/type/pair.h>
#include <fatal/type/slice.h>

namespace fatal {
namespace impl_gt {

template <typename, typename, template <typename...> class...> struct g;

template <typename Key, typename Value>
static Value fnd(pair<Key, Value>);

template <typename T, typename Key>
struct g<T, Key>: g<T, Key, first> {};

template <
  template <typename...> class List,
  typename... Args,
  typename Key,
  template <typename...> class KeyFilter
>
struct g<List<Args...>, Key, KeyFilter> {
  using type = decltype(fnd<Key>(inherit<pair<KeyFilter<Args>, Args>...>()));
};

template <
  typename T,
  typename Key,
  template <typename...> class KeyFilter,
  template <typename...> class PostFilter
>
struct g<T, Key, KeyFilter, PostFilter> {
  using type = PostFilter<typename g<T, Key, KeyFilter>::type>;
};

} // namespace impl_gt {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_get_h
