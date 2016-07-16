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
namespace impl_get {

template <typename, template <typename> class> struct get;

template <
  template <typename...> class List,
  typename... Args,
  template <typename> class Filter
>
struct get<List<Args...>, Filter> {
  template <typename Key, typename Value>
  static Value find(pair<Key, Value>);

  template <typename Key>
  using type = decltype(find<Key>(inherit<pair<Filter<Args>, Args>...>()));
};

} // namespace impl_get {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_get_h
