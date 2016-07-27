/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_array_h
#define FATAL_INCLUDE_fatal_type_impl_array_h

#include <fatal/container/constant_array.h>

namespace fatal {
namespace impl_arr {

template <typename...> struct arr;

template <template <typename...> class Variadics, typename T, typename... Args>
struct arr<Variadics<T, Args...>> {
  using type = constant_array<decltype(T::value), T::value, Args::value...>;
};

template <template <typename...> class Variadics, typename... Args, typename T>
struct arr<Variadics<Args...>, T> {
  using type = constant_array<T, Args::value...>;
};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args
>
struct arr<Variadics<Value, Args...>> {
  using type = constant_array<Value, Args...>;
};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args,
  typename T
>
struct arr<Variadics<Value, Args...>, T> {
  using type = constant_array<T, static_cast<T>(Args)...>;
};

template <typename...> struct zdt;

template <template <typename...> class Variadics, typename T, typename... Args>
struct zdt<Variadics<T, Args...>> {
  using type = constant_array<
    decltype(T::value),
    T::value,
    Args::value...,
    static_cast<decltype(T::value)>(0)
  >;
};

template <template <typename...> class Variadics, typename... Args, typename T>
struct zdt<Variadics<Args...>, T> {
  using type = constant_array<T, Args::value..., static_cast<T>(0)>;
};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args
>
struct zdt<Variadics<Value, Args...>> {
  using type = constant_array<Value, Args..., static_cast<Value>(0)>;
};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args,
  typename T
>
struct zdt<Variadics<Value, Args...>, T> {
  using type = constant_array<T, static_cast<T>(Args)..., static_cast<T>(0)>;
};

} // namespace impl_arr {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_array_h
