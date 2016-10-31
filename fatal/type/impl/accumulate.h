/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_accumulate_h
#define FATAL_INCLUDE_fatal_type_impl_accumulate_h

namespace fatal {
namespace impl_acc {

// TODO: Implement a version for  sequences
template <typename...> struct accumulate;

template <typename Fn, typename Seed>
struct accumulate<Fn, Seed> {
  using type = Seed;
};

template <
  typename Fn,
  typename Seed,
  typename T,
  typename... Args
>
struct accumulate<Fn, Seed, T, Args...>:
  accumulate<Fn, typename Fn::template apply<Seed, T>, Args...>
{};

template <typename...> struct F;

template <
  typename Seed,
  typename Fn,
  template <typename...> class V,
  typename... Args
>
struct F<Seed, Fn, V<Args...>>:
  accumulate<Fn, Seed, Args...>
{};

} // namespace impl_acc {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_accumulate_h
