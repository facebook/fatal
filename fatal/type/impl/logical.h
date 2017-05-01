/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_logical_h
#define FATAL_INCLUDE_fatal_type_impl_logical_h

#include <fatal/type/inherit.h>

#include <type_traits>

namespace fatal {
namespace impl_logical {

template <typename...> struct l_xor;

template <typename V0, typename V1, typename... Args>
struct l_xor<V0, V1, Args...> {
  using type = typename l_xor<
    std::integral_constant<
      bool,
      static_cast<bool>(V0::value ^ V1::value)
    >,
    Args...
  >::type;
};

template <typename V0, typename V1, typename V2, typename... Args>
struct l_xor<V0, V1, V2, Args...> {
  using type = typename l_xor<
    std::integral_constant<
      bool,
      static_cast<bool>(V0::value ^ V1::value ^ V2::value)
    >,
    Args...
  >::type;
};

template <typename V0, typename V1, typename V2, typename V3, typename... Args>
struct l_xor<V0, V1, V2, V3, Args...> {
  using type = typename l_xor<
    std::integral_constant<
      bool,
      static_cast<bool>(V0::value ^ V1::value ^ V2::value ^ V3::value)
    >,
    Args...
  >::type;
};

template <typename T>
struct l_xor<T> {
  using type = T;
};

} // namespace impl_logical {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_logical_h
