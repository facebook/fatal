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
#include <fatal/type/array.h>
#include <fatal/type/cat.h>
#include <fatal/type/conditional.h>
#include <fatal/type/sequence.h>
#include <fatal/type/slice.h>
#include <fatal/type/tag.h>

// TODO: REMOVE THESE HEADERS WHEN A PROPER SORT FOR SEQUENCES IS IN
#include <fatal/type/convert.h>
#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {
namespace i_s {

template <bool>
struct Ptf;
template <>
struct Ptf<false> {
  template <template <typename...> class Variadic, typename>
  using apply = Variadic<>;
};
template <>
struct Ptf<true> {
  template <template <typename...> class Variadic, typename A>
  using apply = Variadic<A>;
};
template <bool>
struct Pvf;
template <>
struct Pvf<false> {
  template <template <typename V, V...> class Variadic, typename T, T>
  using apply = Variadic<T>;
};
template <>
struct Pvf<true> {
  template <template <typename V, V...> class Variadic, typename T, T A>
  using apply = Variadic<T, A>;
};

// partition entry point
template <typename> struct P;

template <
  template <typename...> class Variadic,
  typename... Args
>
struct P<Variadic<Args...>> {
  template <typename Filter, template <typename...> class Pair>
  using apply = Pair<
    cat<
      Variadic<>,
      typename Ptf<
        Filter::template apply<Args>::value
      >::template apply<Variadic, Args>...
    >,
    cat<
      Variadic<>,
      typename Ptf<
        !Filter::template apply<Args>::value
      >::template apply<Variadic, Args>...
    >
  >;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T... Args
>
struct P<Variadic<T, Args...>> {
  template <typename Filter, template <typename...> class Pair>
  using apply = Pair<
    cat<
      Variadic<T>,
      typename Pvf<
        Filter::template apply<T, Args>::value
      >::template apply<Variadic, T, Args>...
    >,
    cat<
      Variadic<T>,
      typename Pvf<
        !Filter::template apply<T, Args>::value
      >::template apply<Variadic, T, Args>...
    >
  >;
};

template <typename...> struct f;

template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Result
>
struct f<Predicate, Variadic<Result...>> {
  using type = Variadic<Result...>;
};

template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Result,
  typename T,
  typename... Args
>
struct f<Predicate, Variadic<Result...>, T, Args...>:
  f<
    Predicate,
    conditional<
      Predicate::template apply<T>::value,
      Variadic<Result..., T>,
      Variadic<Result...>
    >,
    Args...
  >
{};

template <typename...> struct F;

// TODO: OPTIMIZE
template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Args
>
struct F<Predicate, Variadic<Args...>>:
  f<Predicate, Variadic<>, Args...>
{};

// merge implementation
template <typename...> struct m;

template <
  template <typename...> class Variadic,
  typename L, typename... LHS,
  typename R, typename... RHS,
  typename... Args
>
struct m<Variadic<L, LHS...>, Variadic<R, RHS...>, Variadic<Args...>> {
  static constexpr bool c = R::value < L::value;
  using type = conditional<
    c,
    typename m<
      Variadic<L, LHS...>, Variadic<RHS...>, Variadic<Args..., R>
    >::type,
    typename m<
      Variadic<LHS...>, Variadic<R, RHS...>, Variadic<Args..., L>
    >::type
  >;
};

template <
  template <typename...> class Variadic,
  typename... LHS, typename... Args
>
struct m<Variadic<LHS...>, Variadic<>, Variadic<Args...>> {
  using type = Variadic<Args..., LHS...>;
};

template <
  template <typename...> class Variadic,
  typename... RHS, typename... Args
>
struct m<Variadic<>, Variadic<RHS...>, Variadic<Args...>> {
  using type = Variadic<Args..., RHS...>;
};

template <template <typename...> class Variadic, typename... Args>
struct m<Variadic<>, Variadic<>, Variadic<Args...>> {
  using type = Variadic<Args...>;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T L, T... LHS,
  T R, T... RHS,
  T... Values
>
struct m<
  Variadic<T, L, LHS...>,
  Variadic<T, R, RHS...>,
  Variadic<T, Values...>
> {
  using type = conditional<
    (R < L),
    typename m<
      Variadic<T, L, LHS...>,
      Variadic<T, RHS...>,
      Variadic<T, Values..., R>
    >::type,
    typename m<
      Variadic<T, LHS...>,
      Variadic<T, R, RHS...>,
      Variadic<T, Values..., L>
    >::type
  >;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... LHS,
  T... Values
>
struct m<Variadic<T, LHS...>, Variadic<T>, Variadic<T, Values...>> {
  using type = Variadic<T, Values..., LHS...>;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... RHS,
  T... Values
>
struct m<Variadic<T>, Variadic<T, RHS...>, Variadic<T, Values...>> {
  using type = Variadic<T, Values..., RHS...>;
};

template <template <typename V, V...> class Variadic, typename T, T... Values>
struct m<Variadic<T>, Variadic<T>, Variadic<T, Values...>> {
  using type = Variadic<T, Values...>;
};

// merge entry point
template <typename...> struct M;

template <
  template <typename...> class Variadic,
  typename... LHS, typename... RHS
>
struct M<Variadic<LHS...>, Variadic<RHS...>> {
  using type = typename m<Variadic<LHS...>, Variadic<RHS...>, Variadic<>>::type;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... LHS,
  T... RHS
>
struct M<Variadic<T, LHS...>, Variadic<T, RHS...>> {
  using type = typename m<
    Variadic<T, LHS...>,
    Variadic<T, RHS...>,
    Variadic<T>
  >::type;
};

// merge sort
template <typename...> struct s;

template <template <typename...> class Variadic>
struct s<Variadic<>> {
  using type = Variadic<>;
};

template <template <typename...> class Variadic, typename T>
struct s<Variadic<T>> {
  using type = Variadic<T>;
};

template <template <typename...> class Variadic, typename... Args>
struct s<Variadic<Args...>> {
  using type = typename m<
    typename s<head<Variadic<Args...>, sizeof...(Args) / 2>>::type,
    typename s<tail<Variadic<Args...>, sizeof...(Args) / 2>>::type,
    Variadic<>
  >::type;
};

template <template <typename V, V...> class Variadic, typename T>
struct s<Variadic<T>> {
  using type = Variadic<T>;
};

template <template <typename V, V...> class Variadic, typename T, T Value>
struct s<Variadic<T, Value>> {
  using type = Variadic<T, Value>;
};

template <template <typename V, V...> class Variadic, typename T, T... Values>
struct s<Variadic<T, Values...>> {
  using type = typename m<
    typename s<head<Variadic<T, Values...>, sizeof...(Values) / 2>>::type,
    typename s<tail<Variadic<T, Values...>, sizeof...(Values) / 2>>::type,
    Variadic<T>
  >::type;
};

// quicksort
template <typename...> struct q;

template <template <typename...> class Variadic, typename Less>
struct q<Variadic<>, Less> {
  using type = Variadic<>;
};

template <template <typename...> class Variadic, typename T, typename Less>
struct q<Variadic<T>, Less> {
  using type = Variadic<T>;
};

template <
  template <typename...> class Variadic,
  typename LHS,
  typename RHS,
  typename Less
>
struct q<Variadic<LHS, RHS>, Less> {
  using type = conditional<
    Less::template apply<RHS, LHS>::value,
    Variadic<RHS, LHS>,
    Variadic<LHS, RHS>
  >;
};

template <typename Type, bool Result>
struct q_tpair_ {
  using type = Type;
  static constexpr bool result = Result;
};
template <typename Less, typename Head, typename... Tail>
struct q_;
template <
  typename Less,
  template <typename...> class Variadic,
  typename Pivot,
  typename... Pairs
>
struct q_<Less, Variadic<Pivot>, Pairs...> {
  using type = lcat<
    typename q<
      cat<
        Variadic<>,
        typename Ptf<
          !Pairs::result
        >::template apply<Variadic, typename Pairs::type>...
      >,
      Less
    >::type,
    typename q<
      cat<
        Variadic<>,
        typename Ptf<
          Pairs::result
        >::template apply<Variadic, typename Pairs::type>...
      >,
      Less
    >::type,
    Pivot
  >;
};

template <
  template <typename...> class Variadic,
  typename Pivot, typename... Args, typename Less
>
struct q<Variadic<Pivot, Args...>, Less>:
  q_<
    Less,
    Variadic<Pivot>,
    q_tpair_<Args, Less::template apply<Pivot, Args>::value>...
  >
{};

template <template <typename V, V...> class Variadic, typename T, typename Less>
struct q<Variadic<T>, Less> {
  using type = Variadic<T>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T Value,
  typename Less
>
struct q<Variadic<T, Value>, Less> {
  using type = Variadic<T, Value>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T LHS, T RHS,
  typename Less
>
struct q<Variadic<T, LHS, RHS>, Less> {
  using type = conditional<
    Less::template vapply<T, RHS, LHS>::value,
    Variadic<T, RHS, LHS>,
    Variadic<T, LHS, RHS>
  >;
};

template <typename T, T Value, bool Result>
struct q_vpair_ {
  static constexpr T value = Value;
  static constexpr bool result = Result;
};
template <typename Less, typename Head, typename... Tail>
struct q_;
template <
  typename Less,
  template <typename V, V...> class Variadic,
  typename T, T Pivot,
  typename... Pairs
>
struct q_<Less, Variadic<T, Pivot>, Pairs...> {
  using type = typename vcat<
    typename q<
      cat<
        Variadic<T>,
        typename Pvf<
          !Pairs::result
        >::template apply<Variadic, T, Pairs::value>...
      >,
      Less
    >::type,
    typename q<
      cat<
        Variadic<T>,
        typename Pvf<
          Pairs::result
        >::template apply<Variadic, T, Pairs::value>...
      >,
      Less
    >::type
  >::template apply<Pivot>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T Pivot, T Arg0, T Arg1, T... Args,
  typename Less
>
struct q<Variadic<T, Pivot, Arg0, Arg1, Args...>, Less>:
  q_<
    Less,
    Variadic<T, Pivot>,
    q_vpair_<T, Arg0, Less::template vapply<T, Pivot, Arg0>::value>,
    q_vpair_<T, Arg1, Less::template vapply<T, Pivot, Arg1>::value>,
    q_vpair_<T, Args, Less::template vapply<T, Pivot, Args>::value>...
  >
{};

// TODO: OPTIMIZE BY EXPANDING THE q LIST INTO A PAIR(FILTERED, T)??
// filtered comparer for quicksort
template <typename Less, typename Filter>
struct C {
  template <typename LHS, typename RHS>
  using apply = typename Less::template apply<
    typename Filter::template apply<LHS>,
    typename Filter::template apply<RHS>
  >;
};

// biggest prime fitting in size_t
static constexpr std::size_t c_array_shuffle_prime =
  sizeof(std::size_t) == 8 ? std::size_t(-59) : std::size_t(-5);

template <typename T, std::size_t S>
constexpr c_array<T, S> c_array_iota() {
  c_array<T, S> a{};
  for (std::size_t i = 0; i < S; ++i) {
    a[i] = i;
  }
  return a;
}

template <typename T, std::size_t S>
constexpr c_array<T, S> c_array_shuffle(c_array<T, S> a) {
  std::size_t const p = c_array_shuffle_prime;
  for (std::size_t i = 0; i < S; ++i) {
    std::size_t const s = S - i; // unshuffled size remaining
    std::size_t const o = (p % (s + 1)) - 1; // because p % s != 0
    std::size_t const j = i + o; // index of swap partner

    std::size_t const t = a[i];
    a[i] = a[j];
    a[j] = t;
  }
  return a;
}

template <std::size_t S>
struct c_array_index_shuffle_storage {
  using array_t = c_array<std::size_t, S>;
  static constexpr array_t array = c_array_shuffle(
    c_array_iota<std::size_t, S>()
  );
  using seq = array_to_sequence<c_array, std::size_t, S, array>;
};

template <std::size_t S, typename X = c_array_index_shuffle_storage<S>>
using shuf_seq = array_to_sequence<c_array, std::size_t, S, X::array>;

template <typename List>
using shuf = pick_seq<List, shuf_seq<size<List>::value>>;

// quicksort entry-point
template <typename...> struct Q;

template <typename T, typename Less>
struct Q<T, Less>: q<shuf<T>, Less> {};

template <typename T, typename Less, typename By>
struct Q<T, Less, By>: q<shuf<T>, C<Less, By>> {};

template <typename> struct i;

template <template <typename, typename> class Pair, typename LHS, typename RHS>
struct i<Pair<LHS, RHS>> {
  using type = Pair<RHS, LHS>;
};

template <
  template <typename...> class Variadic,
  typename LHS,
  typename RHS,
  typename... Args
>
struct i<Variadic<LHS, RHS, Args...>> {
  using type = Variadic<RHS, LHS, Args...>;
};

} // namespace i_s {
} // namespace fatal {
