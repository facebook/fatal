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

#include <fatal/type/tag.h>

namespace fatal {
namespace impl_fe {

template <typename...>
struct fe {
  template <std::size_t = 0, typename... Args>
  static void f(Args &&...) {}
};

template <typename T, typename... Args>
struct fe<T, Args...> {
  template <std::size_t Index = 0, typename Visitor, typename... VArgs>
  static void f(Visitor &&visitor, VArgs &&...args) {
    visitor(indexed<T, Index>(), args...);
    fe<Args...>::template f<Index + 1>(
      std::forward<Visitor>(visitor),
      std::forward<VArgs>(args)...
    );
  }
};

} // namespace impl_fe {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_foreach_h
