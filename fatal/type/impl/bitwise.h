/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

namespace fatal {
namespace impl_bitwise {

template <typename...> struct b_and;

// TODO: do it in logarithmic time
template <typename T, typename... Args>
struct b_and<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value & b_and<Args...>::type::value)>::type,
    (T::value & b_and<Args...>::type::value)
  >;
};

template <typename T>
struct b_and<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

template <typename...> struct b_or;

// TODO: do it in logarithmic time
template <typename T, typename... Args>
struct b_or<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value | b_or<Args...>::type::value)>::type,
    (T::value | b_or<Args...>::type::value)
  >;
};

template <typename T>
struct b_or<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

template <typename...> struct b_xor;

// TODO: do it in logarithmic time
template <typename T, typename... Args>
struct b_xor<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value ^ b_xor<Args...>::type::value)>::type,
    (T::value ^ b_xor<Args...>::type::value)
  >;
};

template <typename T>
struct b_xor<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

} // namespace impl_bitwise {
} // namespace fatal {
