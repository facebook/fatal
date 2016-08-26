/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/list.h>

#ifndef FATAL_INCLUDE_fatal_type_impl_unique_h
#define FATAL_INCLUDE_fatal_type_impl_unique_h

namespace fatal {
namespace impl_uq {

// TODO: OPTIMIZE
// unique recursion //
template <template <typename...> class, typename...> struct a;

// unique //
template <
  template <typename...> class Variadic,
  typename... Result,
  typename T,
  typename U,
  typename... Args
>
struct a<Variadic, list<Result...>, T, U, Args...>:
  a<Variadic, list<Result..., T>, U, Args...>
{};

// duplicate //
template <
  template <typename...> class Variadic,
  typename... Result,
  typename T,
  typename... Args
>
struct a<Variadic, list<Result...>, T, T, Args...>:
  a<Variadic, list<Result...>, T, Args...>
{};

// base case //
template <
  template <typename...> class Variadic,
  typename... Result,
  typename T
>
struct a<Variadic, list<Result...>, T> {
  using type = Variadic<Result..., T>;
};

// unique entry point //
template <typename> struct au;

template <template <typename...> class Variadic, typename... Args>
struct au<Variadic<Args...>>: a<Variadic, list<>, Args...> {};

template <template <typename...> class Variadic>
struct au<Variadic<>> {
  using type = Variadic<>;
};

} // namespace impl_uq {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_unique_h
