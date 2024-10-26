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
namespace impl_arithmetic {

template <typename...> struct add;

// TODO: do it in logarithmic time
template <typename T, typename... Args>
struct add<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value + add<Args...>::type::value)>::type,
    (T::value + add<Args...>::type::value)
  >;
};

template <typename T>
struct add<T> {
  using type = T;
};

template <typename...> struct sub;

template <typename LHS, typename RHS, typename... Args>
struct sub<LHS, RHS, Args...> {
  using type = typename sub<
    typename sub<LHS, RHS>::type,
    Args...
  >::type;
};

template <typename LHS, typename RHS>
struct sub<LHS, RHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(LHS::value - RHS::value)>::type,
    (LHS::value - RHS::value)
  >;
};

template <typename...> struct mul;

// TODO: do it in logarithmic time
template <typename T, typename... Args>
struct mul<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value * mul<Args...>::type::value)>::type,
    (T::value * mul<Args...>::type::value)
  >;
};

template <typename T>
struct mul<T> {
  using type = T;
};

template <typename...> struct div;

template <typename LHS, typename RHS, typename... Args>
struct div<LHS, RHS, Args...> {
  using type = typename div<typename div<LHS, RHS>::type, Args...>::type;
};

template <typename LHS, typename RHS>
struct div<LHS, RHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(LHS::value / RHS::value)>::type,
    (LHS::value / RHS::value)
  >;
};

template <typename...> struct mod;

template <typename LHS, typename RHS, typename... Args>
struct mod<LHS, RHS, Args...> {
  using type = typename mod<typename mod<LHS, RHS>::type, Args...>::type;
};

template <typename LHS, typename RHS>
struct mod<LHS, RHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(LHS::value % RHS::value)>::type,
    (LHS::value % RHS::value)
  >;
};

} // namespace impl_arithmetic {
} // namespace fatal {
