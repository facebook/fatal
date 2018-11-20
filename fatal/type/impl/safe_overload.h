/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_safe_overload_h
#define FATAL_INCLUDE_fatal_type_impl_safe_overload_h

#include <type_traits>

namespace fatal {
namespace i_sfo {

template <typename, typename...>
struct S { using type = std::true_type; };

template <typename Class, typename T>
struct S<Class, T> {
  using type = std::integral_constant<
    bool,
    !std::is_base_of<
      Class,
      typename std::remove_cv<
        typename std::remove_reference<T>::type
      >::type
    >::value
  >;
};

} // namespace i_sfo {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_safe_overload_h
