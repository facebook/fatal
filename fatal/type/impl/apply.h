/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {
namespace i_ap {

template <template <typename...> class T, typename... Args>
struct a { using type = T<Args...>; };

template <template <typename> class T, typename U>
struct a<T, U> { using type = T<U>; };

template <template <typename, typename> class T, typename U0, typename U1>
struct a<T, U0, U1> { using type = T<U0, U1>; };

template <
  template <typename, typename, typename> class T,
  typename U0, typename U1, typename U2
>
struct a<T, U0, U1, U2> { using type = T<U0, U1, U2>; };

template <
  template <typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3
>
struct a<T, U0, U1, U2, U3> { using type = T<U0, U1, U2, U3>; };

template <
  template <typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4
>
struct a<T, U0, U1, U2, U3, U4> {
  using type = T<U0, U1, U2, U3, U4>;
};

template <
  template <typename, typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5
>
struct a<T, U0, U1, U2, U3, U4, U5> {
  using type = T<U0, U1, U2, U3, U4, U5>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0
>
struct a<T, U0, U1, U2, U3, U4, U5, V0> {
  using type = T<U0, U1, U2, U3, U4, U5, V0>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1
>
struct a<T, U0, U1, U2, U3, U4, U5, V0, V1> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2
>
struct a<T, U0, U1, U2, U3, U4, U5, V0, V1, V2> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3
>
struct a<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4
>
struct a<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4, typename V5
>
struct a<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5>;
};

template <template <typename...> class, typename...> struct LS;
template <
  template <typename...> class To,
  template <typename...> class List,
  typename... Args,
  typename... Suffix
>
struct LS<To, List<Args...>, Suffix...> {
  using type = typename a<To, Args..., Suffix...>::type;
};

template <template <typename...> class, typename...> struct lsf;

template <
  template <typename...> class To,
  template <typename...> class List,
  typename... Args,
  typename... Prefix
>
struct lsf<To, List<Args...>, Prefix...> {
  using type = typename a<To, Prefix..., Args...>::type;
};

template <typename> struct sq;

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values
>
struct sq<Sequence<T, Values...>> {
  template<template <typename V, V...> class To, T... Suffix>
  using apply = To<T, Values..., Suffix...>;

  template<template <typename V, V...> class To, T... Prefix>
  using front = To<T, Prefix..., Values...>;
};

template <template <typename...> class T, typename... Args>
struct app {
  using type = T<Args...>;
};

template <template <typename> class T, typename U>
struct app<T, U> {
  using type = T<U>;
};

template <template <typename, typename> class T, typename U0, typename U1>
struct app<T, U0, U1> {
  using type = T<U0, U1>;
};

template <
  template <typename, typename, typename> class T,
  typename U0, typename U1, typename U2
>
struct app<T, U0, U1, U2> {
  using type = T<U0, U1, U2>;
};

template <
  template <typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3
>
struct app<T, U0, U1, U2, U3> {
  using type = T<U0, U1, U2, U3>;
};

template <
  template <typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4
>
struct app<T, U0, U1, U2, U3, U4> {
  using type = T<U0, U1, U2, U3, U4>;
};

template <
  template <typename, typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5
>
struct app<T, U0, U1, U2, U3, U4, U5> {
  using type = T<U0, U1, U2, U3, U4, U5>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0
>
struct app<T, U0, U1, U2, U3, U4, U5, V0> {
  using type = T<U0, U1, U2, U3, U4, U5, V0>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1
>
struct app<T, U0, U1, U2, U3, U4, U5, V0, V1> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2
>
struct app<T, U0, U1, U2, U3, U4, U5, V0, V1, V2> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3
>
struct app<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4
>
struct app<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4, typename V5
>
struct app<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5>;
};

} // namespace i_ap {
} // namespace fatal {
