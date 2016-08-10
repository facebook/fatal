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

#include <array>
#include <type_traits>

namespace fatal {
namespace impl_a {

template <typename T, T... Values>
struct a {
  static constexpr std::array<T, sizeof...(Values)> get{{Values...}};
};

template <typename T, T... Values>
constexpr std::array<T, sizeof...(Values)> a<T, Values...>::get;

template <typename...> struct ca;

template <template <typename...> class Variadics, typename... Args, typename T>
struct ca<Variadics<Args...>, T>:
  public a<T, Args::value...>
{};

template <template <typename...> class Variadics, typename T, typename... Args>
struct ca<Variadics<T, Args...>>:
  public a<
    typename std::decay<decltype(T::value)>::type,
    T::value,
    Args::value...
  >
{};

template <
  template <typename V, V...> class Variadics,
  typename V,
  V... Values,
  typename T
>
struct ca<Variadics<V, Values...>, T>:
  public a<T, Values...>
{};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct ca<Variadics<T, Values...>>:
  public a<T, Values...>
{};

template <typename...> struct z;

template <template <typename...> class Variadics, typename T, typename... Args>
struct z<Variadics<T, Args...>>:
  public a<
    typename std::decay<decltype(T::value)>::type,
    T::value,
    Args::value...,
    static_cast<typename std::decay<decltype(T::value)>::type>(0)
  >
{};

template <template <typename...> class Variadics, typename... Args, typename T>
struct z<Variadics<Args...>, T>:
  public a<T, Args::value..., static_cast<T>(0)>
{};

template <template <typename V, V...> class Variadics, typename T, T... Args>
struct z<Variadics<T, Args...>>:
  public a<T, Args..., static_cast<T>(0)>
{};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args,
  typename T
>
struct z<Variadics<Value, Args...>, T>:
  public a<T, static_cast<T>(Args)..., static_cast<T>(0)>
{};

template <typename T, typename Factory, typename... Args>
struct ar {
  static std::array<T, sizeof...(Args)> const get;
};

template <typename T, typename Factory, typename... Args>
std::array<T, sizeof...(Args)> const ar<T, Factory, Args...>::get{{
  Factory::template get<Args>()...
}};

template <typename...> struct sa;

template <
  template <typename...> class Variadics,
  typename... Args,
  typename Factory,
  typename T
>
struct sa<Variadics<Args...>, Factory, T>:
  public ar<T, Factory, Args...>
{};

template <
  template <typename...> class Variadics,
  typename T,
  typename... Args,
  typename Factory
>
struct sa<Variadics<T, Args...>, Factory>:
  public ar<
    typename std::decay<decltype(Factory::template get<T>())>::type,
    Factory,
    T, Args...
  >
{};

struct zd {
  template <typename T>
  static decltype(z<T>::get.data()) get() {
    return z<T>::get.data();
  }
};

} // namespace impl_a {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_array_h
