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

#include <fatal/type/list.h>
#include <fatal/type/slice.h>

#include <type_traits>

namespace fatal {
namespace i_a {

// constexpr statically allocated array //
template <
  std::size_t Excess,
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
struct a {
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

template <
  std::size_t Excess,
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
constexpr T const a<
  Excess, T, OuterFilter, InnerFilter, Array, Indexes...
>::data[sizeof...(Indexes)];

template <typename...> struct C;

// constexpr statically allocated array from a list or sequence //
template <
  std::size_t... Indexes,
  typename OuterFilter, typename InnerFilter,
  typename Array, typename T
>
struct C<index_sequence<Indexes...>, OuterFilter, InnerFilter, Array, T>:
  a<0, T, OuterFilter, InnerFilter, Array, Indexes...>
{};

template <
  std::size_t... Indexes,
  typename OuterFilter, typename InnerFilter,
  typename Array
>
struct C<index_sequence<Indexes...>, OuterFilter, InnerFilter, Array>:
  a<
    0,
    typename std::decay<
      decltype(first<typename OuterFilter::template apply<Array>>::value)
    >::type,
    OuterFilter, InnerFilter, Array, Indexes...
  >
{};

// statically allocated array from a list or sequence with null terminator //

template <typename...> struct z;

template <
  std::size_t... Indexes,
  template <typename...> class Variadic,
  typename T, typename... Args
>
struct z<index_sequence<Indexes...>, Variadic<T, Args...>>:
  a<
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
struct z<index_sequence<Indexes...>, Variadic<Args...>, T>:
  a<
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
struct z<index_sequence<Indexes...>, Variadic<T, Args...>>:
  a<
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
struct z<index_sequence<Indexes...>, Variadic<Value, Args...>, T>:
  a<
    1, T, get_identity, get_identity,
    sequence<Value, Args..., static_cast<Value>(0)>,
    Indexes..., sizeof...(Indexes)
  >
{};

// z_array

template <typename T, typename Filter, typename... Args>
class Z {
  template <typename Arg>
  using indexes = make_index_sequence<
    size<typename Filter::template apply<Arg>>::value
  >;

public:
  using value_type = T;
  using size = std::integral_constant<std::size_t, sizeof...(Args)>;
  static constexpr T const data[sizeof...(Args)] = {
    z<
      indexes<Args>,
      typename Filter::template apply<Args>
    >::data...
  };
};

template <typename T, typename Filter, typename... Args>
constexpr T const Z<T, Filter, Args...>::data[sizeof...(Args)];

template <typename...> struct ZA;

template <
  template <typename...> class Variadic,
  typename... Args,
  typename Filter,
  typename T
>
struct ZA<Variadic<Args...>, Filter, T>:
  Z<T, Filter, Args...>
{};

template <
  template <typename...> class Variadic,
  typename T,
  typename... Args,
  typename Filter
>
struct ZA<Variadic<T, Args...>, Filter>:
  Z<
    typename std::decay<
      decltype(
        z<
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

template <
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
class s {
  template <typename Element>
  using str = z<
    make_index_sequence<size<Element>::value>,
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

template <
  typename T,
  typename OuterFilter,
  typename InnerFilter,
  typename Array,
  std::size_t... Indexes
>
constexpr T const s<T, OuterFilter, InnerFilter, Array, Indexes...>::data[
  sizeof...(Indexes)
];

template <typename...> struct S;

template <
  std::size_t... Indexes,
  typename Array,
  typename OuterFilter,
  typename InnerFilter,
  typename T
>
struct S<index_sequence<Indexes...>, Array, OuterFilter, InnerFilter, T>:
  s<T, OuterFilter, InnerFilter, Array, Indexes...>
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

} // namespace i_a {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_array_h
