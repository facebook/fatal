/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/list.h>

namespace fatal {
namespace impl_uq {

/////////////////////
// adjacent_unique //
/////////////////////

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

////////////////////////
// adjacent_unique_by //
////////////////////////

// unique recursion //
template <bool, template <typename...> class, typename...> struct A;

// unique //
template <
  template <typename...> class Variadic,
  typename Comparer,
  typename... Result,
  typename LHS,
  typename RHS,
  typename... Args
>
struct A<
  false,
  Variadic,
  Comparer,
  list<Result...>,
  LHS, RHS, Args...
>:
  A<
    Comparer::template apply<LHS, RHS>::value,
    Variadic, Comparer, list<Result..., LHS>, RHS, Args...
  >
{};

// duplicate //
template <
  template <typename...> class Variadic,
  typename Comparer,
  typename... Result,
  typename LHS,
  typename RHS,
  typename... Args
>
struct A<true, Variadic, Comparer, list<Result...>, LHS, RHS, Args...>:
  A<
    Comparer::template apply<LHS, RHS>::value,
    Variadic, Comparer, list<Result...>, RHS, Args...
  >
{};

// base case //
template <
  template <typename...> class Variadic,
  typename Comparer,
  typename... Result,
  typename T
>
struct A<false, Variadic, Comparer, list<Result...>, T> {
  using type = Variadic<Result..., T>;
};

// base case //
template <
  template <typename...> class Variadic,
  typename Comparer,
  typename... Result,
  typename T
>
struct A<true, Variadic, Comparer, list<Result...>, T> {
  using type = Variadic<Result...>;
};

/////////////////
// entry_point //
/////////////////

// unique entry point //
template <typename...> struct au;

template <template <typename...> class Variadic, typename... Args>
struct au<Variadic<Args...>>: a<Variadic, list<>, Args...> {};

template <template <typename...> class Variadic>
struct au<Variadic<>> {
  using type = Variadic<>;
};

template <
  template <typename...> class Variadic,
  typename... Args,
  typename Comparer
>
struct au<Variadic<Args...>, Comparer>:
  A<false, Variadic, Comparer, list<>, Args...>
{};

template <template <typename...> class Variadic, typename Comparer>
struct au<Variadic<>, Comparer> {
  using type = Variadic<>;
};

} // namespace impl_uq {
} // namespace fatal {
