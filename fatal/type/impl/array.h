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

#include <fatal/type/size.h>

#include <type_traits>

namespace fatal {
namespace impl_a {

// constexpr statically allocated array //
template <std::size_t Excess, typename T, T... Values>
struct a {
  static_assert(Excess <= sizeof...(Values), "internal error");
  using size = std::integral_constant<std::size_t, sizeof...(Values) - Excess>;
  static constexpr T const data[sizeof...(Values)] = {Values...};
};

template <std::size_t Excess, typename T, T... Values>
constexpr T const a<Excess, T, Values...>::data[sizeof...(Values)];

template <typename...> struct ca;

// constexpr statically allocated array from a list or sequence //
template <template <typename...> class Variadics, typename... Args, typename T>
struct ca<Variadics<Args...>, T> {
  using type = a<0, T, Args::value...>;
};

template <template <typename...> class Variadics, typename T, typename... Args>
struct ca<Variadics<T, Args...>> {
  using type = a<
    0,
    typename std::decay<decltype(T::value)>::type,
    T::value,
    Args::value...
  >;
};

template <
  template <typename V, V...> class Variadics,
  typename V,
  V... Values,
  typename T
>
struct ca<Variadics<V, Values...>, T> {
  using type = a<0, T, Values...>;
};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct ca<Variadics<T, Values...>> {
  using type = a<0, T, Values...>;
};

// statically allocated array from a list or sequence with null terminator //

template <typename...> struct z;

template <template <typename...> class Variadics, typename T, typename... Args>
struct z<Variadics<T, Args...>>:
  a<
    1,
    typename std::decay<decltype(T::value)>::type,
    T::value,
    Args::value...,
    static_cast<typename std::decay<decltype(T::value)>::type>(0)
  >
{};

template <template <typename...> class Variadics, typename... Args, typename T>
struct z<Variadics<Args...>, T>:
  a<1, T, Args::value..., static_cast<T>(0)>
{};

template <template <typename V, V...> class Variadics, typename T, T... Args>
struct z<Variadics<T, Args...>>:
  a<1, T, Args..., static_cast<T>(0)>
{};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args,
  typename T
>
struct z<Variadics<Value, Args...>, T>:
  a<1, T, static_cast<T>(Args)..., static_cast<T>(0)>
{};

// statically allocated array from an element factory - entry point

template <template <typename...> class, typename...> struct sa;

template <
  template <typename...> class Array,
  template <typename...> class Variadics,
  typename... Args,
  typename Factory,
  typename T
>
struct sa<Array, Variadics<Args...>, Factory, T> {
  using type = Array<T, Factory, Args...>;
};

template <
  template <typename...> class Array,
  template <typename...> class Variadics,
  typename T,
  typename... Args,
  typename Factory
>
struct sa<Array, Variadics<T, Args...>, Factory> {
  using type = Array<
    typename std::decay<decltype(Factory::template get<T>())>::type,
    Factory,
    T, Args...
  >;
};

// z_array

template <typename T, typename... Args>
struct za {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = { z<Args>::data... };
};

template <typename T, typename... Args>
constexpr T const za<T, Args...>::data[sizeof...(Args)];

template <typename...> struct ZA;

template <template <typename...> class Variadics, typename... Args, typename T>
struct ZA<Variadics<Args...>, T> {
  using type = za<T, Args...>;
};

template <template <typename...> class Variadics, typename T, typename... Args>
struct ZA<Variadics<T, Args...>> {
  using type = za<typename std::decay<decltype(z<T>::data)>::type, T, Args...>;
};

// string_view_array

template <typename T, typename... Args>
struct s {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    T(z<Args, typename T::value_type>::data, fatal::size<Args>::value)...
  };
};

template <typename T, typename... Args>
constexpr T const s<T, Args...>::data[sizeof...(Args)];

template <typename...> struct S;

template <template <typename...> class Variadics, typename... Args, typename T>
struct S<Variadics<Args...>, T> {
  using type = s<T, Args...>;
};

// constexpr statically allocated array from element factory//

template <typename T, typename Factory, typename... Args>
struct c {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    Factory::template get<Args>()...
  };
};

template <typename T, typename Factory, typename... Args>
constexpr T const c<T, Factory, Args...>::data[sizeof...(Args)];

// non-constexpr statically allocated array from element factory//

template <typename T, typename Factory, typename... Args>
struct n {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static T const data[sizeof...(Args)];
};

template <typename T, typename Factory, typename... Args>
T const n<T, Factory, Args...>::data[sizeof...(Args)] = {
  Factory::template get<Args>()...
};

} // namespace impl_a {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_array_h
