/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_foreach_h
#define FATAL_INCLUDE_fatal_type_impl_foreach_h

#include <fatal/type/list.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace impl_fe {

template <typename, typename>
struct fe {
  template <typename Visitor, typename... Args>
  static void go(Visitor&&, Args&&...) {}
};

template <typename... Els, std::size_t... Indexes>
struct fe<list<Els...>, index_sequence<Indexes...>> {
  static_assert(sizeof...(Els) == sizeof...(Indexes), "size mismatch");
  static constexpr std::size_t size = sizeof...(Els);

  template <typename Visitor, typename... Args>
  static void f(Visitor&& visitor, Args&&... args) {
    bool _[size] = {(visitor(indexed<Els, Indexes>{}, args...), false)...};
    (void) _;
  }
};

} // namespace impl_fe {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_foreach_h
