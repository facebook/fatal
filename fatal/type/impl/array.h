/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/portability.h>
#include <fatal/type/identity.h>
#include <fatal/type/list.h>
#include <fatal/type/slice.h>

#include <type_traits>

namespace fatal {

namespace c_array_ {

template <typename T, std::size_t S>
struct c_array;

} // namespace c_array_ {

namespace i_a {

template <
  template <typename, std::size_t> class A, typename T, std::size_t S,
  typename = make_index_sequence<S>
>
struct array_to_sequence_;
template <
  template <typename, std::size_t> class A, typename T, std::size_t S,
  std::size_t... I
>
struct array_to_sequence_<A, T, S, index_sequence<I...>> {
  template <A<T, S> const &V>
  using apply = sequence<T, V[I]...>;
};

// constexpr statically allocated array //

template <std::size_t Excess, typename T, T... Values>
struct a {
  static_assert(Excess <= sizeof...(Values), "internal error");
  using size = std::integral_constant<std::size_t, sizeof...(Values) - Excess>;
  static constexpr T const data[sizeof...(Values)] = {Values...};
};

#if FATAL_CPLUSPLUS < 201703L
template <std::size_t Excess, typename T, T... Values>
constexpr T const a<Excess, T, Values...>::data[sizeof...(Values)];
#endif

template <
  std::size_t Excess,
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
struct aF {
  static_assert(Excess <= sizeof...(Indexes), "internal error");
  using size = std::integral_constant<std::size_t, sizeof...(Indexes) - Excess>;
  static constexpr T const data[sizeof...(Indexes)] = {
    static_cast<T>(
      InnerFilter::template apply<
        at<typename OuterFilter::template apply<Array>, Indexes>
      >::value
    )...
  };
};

#if FATAL_CPLUSPLUS < 201703L
template <
  std::size_t Excess,
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
constexpr T const aF<
  Excess, T, OuterFilter, InnerFilter, Array, Indexes...
>::data[sizeof...(Indexes)];
#endif

template <typename...> struct C;

// constexpr statically allocated array from a list or sequence //
template <template <typename...> class Variadics, typename... Args, typename T>
struct C<Variadics<Args...>, T>: a<0, T, Args::value...> {};

template <template <typename...> class Variadics, typename T, typename... Args>
struct C<Variadics<T, Args...>>:
  a<
    0,
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
struct C<Variadics<V, Values...>, T>: a<0, T, Values...> {};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct C<Variadics<T, Values...>>: a<0, T, Values...> {};

template <typename...> struct CF;

// constexpr statically allocated array from a list or sequence //
template <
  std::size_t... Indexes,
  typename OuterFilter, typename InnerFilter,
  typename Array, typename T
>
struct CF<index_sequence<Indexes...>, OuterFilter, InnerFilter, Array, T>:
  aF<0, T, OuterFilter, InnerFilter, Array, Indexes...>
{};

template <
  std::size_t... Indexes,
  typename OuterFilter, typename InnerFilter,
  typename Array
>
struct CF<index_sequence<Indexes...>, OuterFilter, InnerFilter, Array>:
  aF<
    0,
    typename std::decay<
      decltype(first<typename OuterFilter::template apply<Array>>::value)
    >::type,
    OuterFilter, InnerFilter, Array, Indexes...
  >
{};

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
struct z<Variadics<Args...>, T>: a<1, T, Args::value..., static_cast<T>(0)> {};

template <template <typename V, V...> class Variadics, typename T, T... Args>
struct z<Variadics<T, Args...>>: a<1, T, Args..., static_cast<T>(0)> {};

template <
  template <typename V, V...> class Variadics,
  typename Value,
  Value... Args,
  typename T
>
struct z<Variadics<Value, Args...>, T>:
  a<1, T, static_cast<T>(Args)..., static_cast<T>(0)>
{};

template <typename...> struct zF;

template <
  std::size_t... Indexes,
  template <typename...> class Variadic,
  typename T, typename... Args
>
struct zF<index_sequence<Indexes...>, Variadic<T, Args...>>:
  aF<
    1, typename std::decay<decltype(T::value)>::type,
    get_identity, get_identity,
    list<
      T, Args...,
      std::integral_constant<
        typename std::decay<decltype(T::value)>::type,
        static_cast<typename std::decay<decltype(T::value)>::type>(0)
      >
    >,
    Indexes..., sizeof...(Indexes)
  >
{};

template <
  std::size_t... Indexes,
  template <typename...> class Variadic,
  typename... Args, typename T
>
struct zF<index_sequence<Indexes...>, Variadic<Args...>, T>:
  aF<
    1, T,
    get_identity, get_identity,
    list<Args..., std::integral_constant<T, static_cast<T>(0)>>,
    Indexes..., sizeof...(Indexes)
  >
{};

template <
  std::size_t... Indexes,
  template <typename V, V...> class Variadic,
  typename T, T... Args
>
struct zF<index_sequence<Indexes...>, Variadic<T, Args...>>:
  aF<
    1, T, get_identity, get_identity,
    sequence<T, Args..., static_cast<T>(0)>,
    Indexes..., sizeof...(Indexes)
  >
{};

template <
  std::size_t... Indexes,
  template <typename V, V...> class Variadic,
  typename Value,
  Value... Args,
  typename T
>
struct zF<index_sequence<Indexes...>, Variadic<Value, Args...>, T>:
  aF<
    1, T, get_identity, get_identity,
    sequence<Value, Args..., static_cast<Value>(0)>,
    Indexes..., sizeof...(Indexes)
  >
{};

// z_array

template <typename T, typename... Args>
struct Z {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = { z<Args>::data... };
};

#if FATAL_CPLUSPLUS < 201703L
template <typename T, typename... Args>
constexpr T const Z<T, Args...>::data[sizeof...(Args)];
#endif

template <typename...> struct ZA;

template <template <typename...> class Variadics, typename... Args, typename T>
struct ZA<Variadics<Args...>, T>: Z<T, Args...> {};

template <template <typename...> class Variadics, typename T, typename... Args>
struct ZA<Variadics<T, Args...>>:
  Z<typename std::decay<decltype(z<T>::data)>::type, T, Args...>
{};

template <typename T, typename Filter, typename... Args>
class ZF {
  template <typename Arg>
  using indexes = make_index_sequence<
    fatal::size<typename Filter::template apply<Arg>>::value
  >;

public:
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    zF<
      indexes<Args>,
      typename Filter::template apply<Args>
    >::data...
  };
};

#if FATAL_CPLUSPLUS < 201703L
template <typename T, typename Filter, typename... Args>
constexpr T const ZF<T, Filter, Args...>::data[sizeof...(Args)];
#endif

template <typename...> struct ZAF;

template <
  template <typename...> class Variadic,
  typename... Args,
  typename Filter,
  typename T
>
struct ZAF<Variadic<Args...>, Filter, T>: ZF<T, Filter, Args...> {};

template <
  template <typename...> class Variadic,
  typename T,
  typename... Args,
  typename Filter
>
struct ZAF<Variadic<T, Args...>, Filter>:
  ZF<
    typename std::decay<
      decltype(
        zF<
          make_index_sequence<size<typename Filter::template apply<T>>::value>,
          typename Filter::template apply<T>
        >::data
      )
    >::type,
    Filter,
    T, Args...
  >
{};

// string_view_array

template <typename T, typename... Args>
struct s {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    T(z<Args, typename T::value_type>::data, fatal::size<Args>::value)...
  };
};

#if FATAL_CPLUSPLUS < 201703L
template <typename T, typename... Args>
constexpr T const s<T, Args...>::data[sizeof...(Args)];
#endif

template <typename...> struct S;

template <template <typename...> class Variadics, typename... Args, typename T>
struct S<Variadics<Args...>, T>: s<T, Args...> {};

template <
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
class sF {
  template <typename Element>
  using str = zF<
    make_index_sequence<fatal::size<Element>::value>,
    Element,
    typename T::value_type
  >;

public:
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Indexes)>;
  static constexpr T const data[sizeof...(Indexes)] = {
    T(
      str<
        typename InnerFilter::template apply<
          at<typename OuterFilter::template apply<Array>, Indexes>
        >
      >::data,
      fatal::size<
        typename InnerFilter::template apply<
          at<typename OuterFilter::template apply<Array>, Indexes>
        >
      >::value
    )...
  };
};

#if FATAL_CPLUSPLUS < 201703L
template <
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
constexpr T const sF<T, OuterFilter, InnerFilter, Array, Indexes...>::data[
  sizeof...(Indexes)
];
#endif

template <typename...> struct SF;

template <
  std::size_t... Indexes,
  typename Array,
  typename OuterFilter,
  typename InnerFilter,
  typename T
>
struct SF<index_sequence<Indexes...>, Array, OuterFilter, InnerFilter, T>:
  sF<T, OuterFilter, InnerFilter, Array, Indexes...>
{};

// statically allocated array from an element factory - entry point

template <template <typename...> class, typename...> struct A;

template <
  template <typename...> class Array,
  template <typename...> class Variadic,
  typename... Args,
  typename Factory,
  typename T
>
struct A<Array, Variadic<Args...>, Factory, T>:
  Array<T, Factory, Args...>
{};

template <
  template <typename...> class Array,
  template <typename...> class Variadic,
  typename T,
  typename... Args,
  typename Factory
>
struct A<Array, Variadic<T, Args...>, Factory>:
  Array<
    typename std::decay<decltype(Factory::template get<T>())>::type,
    Factory,
    T, Args...
  >
{};

// constexpr statically allocated array from element factory//

template <typename T, typename Factory, typename... Args>
struct c {
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    Factory::template get<Args>()...
  };
};

#if FATAL_CPLUSPLUS < 201703L
template <typename T, typename Factory, typename... Args>
constexpr T const c<T, Factory, Args...>::data[sizeof...(Args)];
#endif

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

} // namespace i_a {
} // namespace fatal {
