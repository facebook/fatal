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
namespace i_A {

// TODO: Implement a version for  sequences
template <typename...> struct a;

template <typename Fn, typename Seed>
struct a<Fn, Seed> {
  using type = Seed;
};

template <
  typename Fn,
  typename Seed,
  typename T,
  typename... Args
>
struct a<Fn, Seed, T, Args...>:
  a<Fn, typename Fn::template apply<Seed, T>, Args...>
{};

template <typename...> struct F;

template <
  typename Seed,
  typename Fn,
  template <typename...> class V,
  typename... Args
>
struct F<Seed, Fn, V<Args...>>:
  a<Fn, Seed, Args...>
{};

// default metafunction //
// TODO: TEST DEFAULT METAFUNCTION
struct f {
  template <typename Seed, typename T>
  using apply = typename Seed::template apply<T>;
};

} // namespace i_A {
} // namespace fatal {
