/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/conditional.h>
#include <fatal/type/compare.h>

#include <type_traits>

namespace fatal {
namespace impl_sl {

template <typename LHS, typename RHS, typename Less = less>
using min = conditional<
  Less::template apply<RHS, LHS>::value,
  RHS,
  LHS
>;

template <typename LHS, typename RHS, typename Less = less>
using max = conditional<
  Less::template apply<LHS, RHS>::value,
  RHS,
  LHS
>;

// variadic min //
template <typename...> struct vn;

template <typename Less, typename T>
struct vn<Less, T> {
  using type = T;
};

template <typename Less, typename T0, typename T1>
struct vn<Less, T0, T1> {
  using type = min<T0, T1, Less>;
};

template <typename Less, typename T0, typename T1, typename T2>
struct vn<Less, T0, T1, T2> {
  using type = min<min<T0, T1, Less>, T2, Less>;
};

template <
  typename Less, typename T0, typename T1, typename T2, typename T3,
  typename... T
>
struct vn<Less, T0, T1, T2, T3, T...>:
  vn<Less, min<T0, T1, Less>, min<T2, T3, Less>, T...>
{};

template <
  typename Less,
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename... T
>
struct vn<Less, T0, T1, T2, T3, T4, T5, T6, T7, T...>:
  vn<
    Less,
    min<T0, T1, Less>, min<T2, T3, Less>,
    min<T4, T5, Less>, min<T6, T7, Less>,
    T...
  >
{};

template <
  typename Less,
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename T10, typename T11,
  typename T12, typename T13, typename T14, typename T15,
  typename... T
>
struct vn<
  Less,
  T0, T1, T2, T3, T4, T5, T6, T7,
  T8, T9, T10, T11, T12, T13, T14, T15,
  T...
>:
  vn<
    Less,
    min<T0, T1, Less>, min<T2, T3, Less>,
    min<T4, T5, Less>, min<T6, T7, Less>,
    min<T8, T9, Less>, min<T10, T11, Less>,
    min<T12, T13, Less>, min<T14, T15, Less>,
    T...
  >
{};

// variadic max //
template <typename...> struct vx;

template <typename Less, typename T>
struct vx<Less, T> {
  using type = T;
};

template <typename Less, typename T0, typename T1>
struct vx<Less, T0, T1> {
  using type = max<T0, T1, Less>;
};

template <typename Less, typename T0, typename T1, typename T2>
struct vx<Less, T0, T1, T2> {
  using type = max<max<T0, T1, Less>, T2, Less>;
};

template <
  typename Less, typename T0, typename T1, typename T2, typename T3,
  typename... T
>
struct vx<Less, T0, T1, T2, T3, T...>:
  vx<Less, max<T0, T1, Less>, max<T2, T3, Less>, T...>
{};

template <
  typename Less,
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename... T
>
struct vx<Less, T0, T1, T2, T3, T4, T5, T6, T7, T...>:
  vx<
    Less,
    max<T0, T1, Less>, max<T2, T3, Less>,
    max<T4, T5, Less>, max<T6, T7, Less>,
    T...
  >
{};

template <
  typename Less,
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename T10, typename T11,
  typename T12, typename T13, typename T14, typename T15,
  typename... T
>
struct vx<
  Less,
  T0, T1, T2, T3, T4, T5, T6, T7,
  T8, T9, T10, T11, T12, T13, T14, T15,
  T...
>:
  vx<
    Less,
    max<T0, T1, Less>, max<T2, T3, Less>,
    max<T4, T5, Less>, max<T6, T7, Less>,
    max<T8, T9, Less>, max<T10, T11, Less>,
    max<T12, T13, Less>, max<T14, T15, Less>,
    T...
  >
{};

// list min //

template <typename, typename, template <typename...> class...> struct ln;

template <typename Less, template <typename...> class List, typename... Args>
struct ln<Less, List<Args...>>: vn<Less, Args...> {};

template <
  typename Less,
  template <typename...> class List, typename... Args,
  template <typename...> class Filter
>
struct ln<Less, List<Args...>, Filter>: vn<Less, Filter<Args>...> {};

// list max //

template <typename, typename, template <typename...> class...> struct lx;

template <typename Less, template <typename...> class List, typename... Args>
struct lx<Less, List<Args...>>: vx<Less, Args...> {};

template <
  typename Less,
  template <typename...> class List, typename... Args,
  template <typename...> class Filter
>
struct lx<Less, List<Args...>, Filter>: vx<Less, Filter<Args>...> {};

} // namespace impl_sl {
} // namespace fatal {
