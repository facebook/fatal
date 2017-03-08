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

#include <fatal/portability.h>
#include <fatal/type/list.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace impl_fe {

template <typename, typename>
struct f {
  template <typename... Args>
  FATAL_ATTR_ALWAYS_INLINE FATAL_ATTR_VISIBILITY_HIDDEN
  static void g(Args &&...) {}
};

template <typename... T, std::size_t... Indexes>
struct f<list<T...>, index_sequence<Indexes...>> {
  static_assert(sizeof...(T) == sizeof...(Indexes), "size mismatch");

  template <typename Visitor, typename... Args>
  FATAL_ATTR_ALWAYS_INLINE FATAL_ATTR_VISIBILITY_HIDDEN
  static void g(Visitor &&visitor, Args &&...args) {
    bool _[sizeof...(T)] = {
      (visitor(indexed<T, Indexes>{}, args...), false)...
    };
    (void)_;
  }
};

} // namespace impl_fe {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_foreach_h
