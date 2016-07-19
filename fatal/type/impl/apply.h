/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_apply_h
#define FATAL_INCLUDE_fatal_type_impl_apply_h

namespace fatal {
namespace impl_apply {

template <typename> struct to;

template <template <typename...> class List, typename... Args>
struct to<List<Args...>> {
  template <template <typename...> class To, typename... Suffix>
  using apply = To<Args..., Suffix...>;

  template <template <typename...> class To, typename... Prefix>
  using front = To<Prefix..., Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values
>
struct to<Sequence<T, Values...>> {
  template<template <typename V, V...> class To, T... Suffix>
  using apply = To<T, Values..., Suffix...>;

  template<template <typename V, V...> class To, T... Prefix>
  using front = To<T, Prefix..., Values...>;
};

} // namespace impl_apply {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_apply_h
