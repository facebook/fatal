/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_cat_h
#define FATAL_INCLUDE_fatal_type_impl_cat_h

namespace fatal {
namespace impl_cat {

template <typename...> struct cat;

template <typename T>
struct cat<T> {
  using type = T;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1
>
struct cat<V0<T0...>, V1<T1...>> {
  using type = V0<T0..., T1...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2
>
struct cat<V0<T0...>, V1<T1...>, V2<T2...>> {
  using type = V0<T0..., T1..., T2...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3
>
struct cat<V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>> {
  using type = V0<T0..., T1..., T2..., T3...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4
>
struct cat<V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>> {
  using type = V0<T0..., T1..., T2..., T3..., T4...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5
>
struct cat<V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>> {
  using type = V0<T0..., T1..., T2..., T3..., T4..., T5...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>, V6<T6...>
> {
  using type = V0<T0..., T1..., T2..., T3..., T4..., T5..., T6...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>
> {
  using type = V0<T0..., T1..., T2..., T3..., T4..., T5..., T6..., T7...>;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5..., T6..., T7..., T8...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10,
  template <typename...> class V11, typename... T11
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>, V11<T11...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10,
  template <typename...> class V11, typename... T11,
  template <typename...> class V12, typename... T12
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>, V11<T11...>,
  V12<T12...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10,
  template <typename...> class V11, typename... T11,
  template <typename...> class V12, typename... T12,
  template <typename...> class V13, typename... T13
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>, V11<T11...>,
  V12<T12...>, V13<T13...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10,
  template <typename...> class V11, typename... T11,
  template <typename...> class V12, typename... T12,
  template <typename...> class V13, typename... T13,
  template <typename...> class V14, typename... T14
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>, V11<T11...>,
  V12<T12...>, V13<T13...>, V14<T14...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13..., T14...
  >;
};

template <
  template <typename...> class V0, typename... T0,
  template <typename...> class V1, typename... T1,
  template <typename...> class V2, typename... T2,
  template <typename...> class V3, typename... T3,
  template <typename...> class V4, typename... T4,
  template <typename...> class V5, typename... T5,
  template <typename...> class V6, typename... T6,
  template <typename...> class V7, typename... T7,
  template <typename...> class V8, typename... T8,
  template <typename...> class V9, typename... T9,
  template <typename...> class V10, typename... T10,
  template <typename...> class V11, typename... T11,
  template <typename...> class V12, typename... T12,
  template <typename...> class V13, typename... T13,
  template <typename...> class V14, typename... T14,
  template <typename...> class V15, typename... T15
>
struct cat<
  V0<T0...>, V1<T1...>, V2<T2...>, V3<T3...>, V4<T4...>, V5<T5...>,
  V6<T6...>, V7<T7...>, V8<T8...>, V9<T9...>, V10<T10...>, V11<T11...>,
  V12<T12...>, V13<T13...>, V14<T14...>, V15<T15...>
> {
  using type = V0<
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13..., T14..., T15...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1
>
struct cat<V0<T, T0...>, V1<T, T1...>> {
  using type = V0<T, T0..., T1...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2
>
struct cat<V0<T, T0...>, V1<T, T1...>, V2<T, T2...>> {
  using type = V0<T, T0..., T1..., T2...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3
>
struct cat<V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>> {
  using type = V0<T, T0..., T1..., T2..., T3...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>
> {
  using type = V0<T, T0..., T1..., T2..., T3..., T4...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>
> {
  using type = V0<T, T0..., T1..., T2..., T3..., T4..., T5...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>
> {
  using type = V0<T, T0..., T1..., T2..., T3..., T4..., T5..., T6...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>
> {
  using type = V0<T, T0..., T1..., T2..., T3..., T4..., T5..., T6..., T7...>;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>
> {
  using type = V0<
    T, T0..., T1..., T2..., T3..., T4..., T5..., T6..., T7..., T8...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>
> {
  using type = V0<
    T, T0..., T1..., T2..., T3..., T4..., T5..., T6..., T7..., T8..., T9...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10,
  template <typename V, V...> class V11, T... T11
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>, V11<T, T11...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10,
  template <typename V, V...> class V11, T... T11,
  template <typename V, V...> class V12, T... T12
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>, V11<T, T11...>, V12<T, T12...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10,
  template <typename V, V...> class V11, T... T11,
  template <typename V, V...> class V12, T... T12,
  template <typename V, V...> class V13, T... T13
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>, V11<T, T11...>, V12<T, T12...>, V13<T, T13...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10,
  template <typename V, V...> class V11, T... T11,
  template <typename V, V...> class V12, T... T12,
  template <typename V, V...> class V13, T... T13,
  template <typename V, V...> class V14, T... T14
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>, V11<T, T11...>, V12<T, T12...>, V13<T, T13...>, V14<T, T14...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13..., T14...
  >;
};

template <
  typename T,
  template <typename V, V...> class V0, T... T0,
  template <typename V, V...> class V1, T... T1,
  template <typename V, V...> class V2, T... T2,
  template <typename V, V...> class V3, T... T3,
  template <typename V, V...> class V4, T... T4,
  template <typename V, V...> class V5, T... T5,
  template <typename V, V...> class V6, T... T6,
  template <typename V, V...> class V7, T... T7,
  template <typename V, V...> class V8, T... T8,
  template <typename V, V...> class V9, T... T9,
  template <typename V, V...> class V10, T... T10,
  template <typename V, V...> class V11, T... T11,
  template <typename V, V...> class V12, T... T12,
  template <typename V, V...> class V13, T... T13,
  template <typename V, V...> class V14, T... T14,
  template <typename V, V...> class V15, T... T15
>
struct cat<
  V0<T, T0...>, V1<T, T1...>, V2<T, T2...>, V3<T, T3...>, V4<T, T4...>,
  V5<T, T5...>, V6<T, T6...>, V7<T, T7...>, V8<T, T8...>, V9<T, T9...>,
  V10<T, T10...>, V11<T, T11...>, V12<T, T12...>, V13<T, T13...>,
  V14<T, T14...>, V15<T, T15...>
> {
  using type = V0<
    T,
    T0..., T1..., T2..., T3..., T4..., T5...,
    T6..., T7..., T8..., T9..., T10..., T11...,
    T12..., T13..., T14..., T15...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3, typename T4,
  typename T5, typename T6, typename T7, typename T8, typename T9,
  typename T10, typename T11, typename T12, typename T13, typename T14,
  typename T15, typename... Args
>
struct cat<
  T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, Args...
> {
  using type = typename cat<
    typename cat<
      T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
    >::type,
    Args...
  >::type;
};

template <typename...> struct vcat;

template <
  template <typename...> class Variadic,
  typename... LHS,
  typename... RHS
>
struct vcat<Variadic<LHS...>, Variadic<RHS...>> {
  template <typename... Args>
  using apply = Variadic<LHS..., Args..., RHS...>;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... LHS,
  T... RHS
>
struct vcat<Variadic<T, LHS...>, Variadic<T, RHS...>> {
  template <T... Values>
  using apply = Variadic<T, LHS..., Values..., RHS...>;
};

template <typename...> struct lcat;

template <
  template <typename...> class Variadic,
  typename... LHS,
  typename... RHS,
  typename... Args
>
struct lcat<Variadic<LHS...>, Variadic<RHS...>, Args...> {
  using type = Variadic<LHS..., Args..., RHS...>;
};

} // namespace impl_cat {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_cat_h
