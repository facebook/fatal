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
namespace impl_zp {

template <
  template <typename...> class,
  template <typename...> class,
  typename...
>
struct zp;

template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  template <typename...> class L0, typename... T0
>
struct zp<Outer, Inner, L0<T0...>> {
  using type = Outer<Inner<T0>...>;
};

template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  template <typename...> class L0, typename... T0,
  template <typename...> class L1, typename... T1
>
struct zp<Outer, Inner, L0<T0...>, L1<T1...>> {
  using type = Outer<Inner<T0, T1>...>;
};

template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  template <typename...> class L0, typename... T0,
  template <typename...> class L1, typename... T1,
  template <typename...> class L2, typename... T2
>
struct zp<Outer, Inner, L0<T0...>, L1<T1...>, L2<T2...>> {
  using type = Outer<Inner<T0, T1, T2>...>;
};

template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  template <typename...> class L0, typename... T0,
  template <typename...> class L1, typename... T1,
  template <typename...> class L2, typename... T2,
  template <typename...> class L3, typename... T3
>
struct zp<Outer, Inner, L0<T0...>, L1<T1...>, L2<T2...>, L3<T3...>> {
  using type = Outer<Inner<T0, T1, T2, T3>...>;
};

} // namespace impl_zp {
} // namespace fatal {
