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
namespace impl_cp {

template <template <typename...> class, typename...> struct cpe;

template <template <typename...> class, typename...> struct cp;

template <template <typename...> class Inner, typename Result, typename T>
struct cp<Inner, Result, T> {
  using type = Result;
};

template <
  template <typename...> class Inner,
  template <typename...> class Outer, typename... T,
  template <typename...> class Variadics, typename... RHS,
  typename L, typename... LHS
>
struct cp<Inner, Outer<T...>, Variadics<RHS...>, L, LHS...>:
  cp<Inner, Outer<T..., Inner<L, RHS>...>, Variadics<RHS...>, LHS...>
{};

template <
  template <typename...> class Inner, typename Outer,
  template <typename...> class L, typename... LHS,
  template <typename...> class R, typename... RHS
>
struct cpe<Inner, Outer, L<LHS...>, R<RHS...>>:
  cp<Inner, Outer, R<RHS...>, LHS...>
{};

} // namespace impl_cp {
} // namespace fatal {
