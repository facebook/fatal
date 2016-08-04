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

namespace fatal {
namespace impl_gt {

template <template <typename> class, typename...> struct gt;

template <typename Key, typename Value>
Value fnd(pair<Key, Value>);

template <
  template <typename...> class List,
  typename... Args,
  typename Key,
  template <typename> class Filter
>
struct gt<Filter, List<Args...>, Key> {
  using type = decltype(fnd<Key>(inherit<pair<Filter<Args>, Args>...>()));
};

} // namespace impl_gt {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_get_h
