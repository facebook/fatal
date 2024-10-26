/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/apply.h>
#include <fatal/type/impl/reverse.h>

namespace fatal {
namespace i_rev {

template <typename> struct r;
template <typename...> struct R;
template <typename, typename T, T...>  struct S;

template <template <typename...> class T, typename... Args>
struct r<T<Args...>> {
  using type = typename R<Args...>::template apply<T>;
};

template <template <typename V, V...> class Sequence, typename T, T... Args>
struct r<Sequence<T, Args...>> {
  using type = typename S<Sequence<int>, T, Args...>::template apply<>;
};

template <>
struct R<> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, Suffix...>;
};

template <typename T0>
struct R<T0> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T0, Suffix...>;
};

template <typename T0, typename T1>
struct R<T0, T1> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T1, T0, Suffix...>;
};

template <typename T0, typename T1, typename T2>
struct R<T0, T1, T2> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T2, T1, T0, Suffix...>;
};

template <typename T0, typename T1, typename T2, typename T3>
struct R<T0, T1, T2, T3> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T3, T2, T1, T0, Suffix...>;
};

template <typename T0, typename T1, typename T2, typename T3, typename T4>
struct R<T0, T1, T2, T3, T4> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  typename T0, typename T1, typename T2, typename T3, typename T4, typename T5
>
struct R<T0, T1, T2, T3, T4, T5> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6
>
struct R<T0, T1, T2, T3, T4, T5, T6> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7, typename U0
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<T, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7, typename U0, typename U1
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2, typename U3
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2, typename U3, typename U4
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2, typename U3,
  typename U4, typename U5
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2, typename U3,
  typename U4, typename U5, typename U6
>
struct R<T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5, U6> {
  template <template <typename...> class T, typename... Suffix>
  using apply = apply_args<
    T, U6, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename U0, typename U1, typename U2, typename U3,
  typename U4, typename U5, typename U6, typename U7,
  typename... Tail
>
struct R<
  T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5, U6, U7, Tail...
> {
  template <template <typename...> class T, typename... Suffix>
  using apply = typename R<Tail...>::template apply<
    T, U7, U6, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <template <typename V, V...> class Sequence, typename T>
struct S<Sequence<int>, T> {
  template <T... Suffix>
  using apply = Sequence<T, Suffix...>;
};

template <template <typename V, V...> class Sequence, typename T, T T0>
struct S<Sequence<int>, T, T0> {
  template <T... Suffix>
  using apply = Sequence<T, T0, Suffix...>;
};

template <template <typename V, V...> class Sequence, typename T, T T0, T T1>
struct S<Sequence<int>, T, T0, T1> {
  template <T... Suffix>
  using apply = Sequence<T, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T, T T0, T T1, T T2
>
struct S<Sequence<int>, T, T0, T1, T2> {
  template <T... Suffix>
  using apply = Sequence<T, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T, T T0, T T1, T T2, T T3
>
struct S<Sequence<int>, T, T0, T1, T2, T3> {
  template <T... Suffix>
  using apply = Sequence<T, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T, T T0, T T1, T T2, T T3, T T4
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4> {
  template <T... Suffix>
  using apply = Sequence<T, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T, T T0, T T1, T T2, T T3, T T4, T T5
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5> {
  template <T... Suffix>
  using apply = Sequence<T, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6> {
  template <T... Suffix>
  using apply = Sequence<T, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7> {
  template <T... Suffix>
  using apply = Sequence<T, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T U0
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0> {
  template <T... Suffix>
  using apply = Sequence<T, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T U0, T U1
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1> {
  template <T... Suffix>
  using apply = Sequence<T, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T U0, T U1, T U2
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2> {
  template <T... Suffix>
  using apply = Sequence<
    T, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T U0, T U1, T U2, T U3
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3> {
  template <T... Suffix>
  using apply = Sequence<
    T, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T U0, T U1, T U2, T U3, T U4
>
struct S<Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4> {
  template <T... Suffix>
  using apply = Sequence<
    T, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7,
  T U0, T U1, T U2, T U3, T U4, T U5
>
struct S<
  Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5
> {
  template <T... Suffix>
  using apply = Sequence<
    T, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7,
  T U0, T U1, T U2, T U3, T U4, T U5, T U6
>
struct S<
  Sequence<int>, T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5, U6
> {
  template <T... Suffix>
  using apply = Sequence<
    T, U6, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7,
  T U0, T U1, T U2, T U3, T U4, T U5, T U6, T U7,
  T... Tail
>
struct S<
  Sequence<int>,
  T, T0, T1, T2, T3, T4, T5, T6, T7, U0, U1, U2, U3, U4, U5, U6, U7, Tail...
> {
  template <T... Suffix>
  using apply = typename S<Sequence<int>, T, Tail...>::template apply<
    U7, U6, U5, U4, U3, U2, U1, U0, T7, T6, T5, T4, T3, T2, T1, T0, Suffix...
  >;
};

} // namespace i_rev {
} // namespace fatal {
