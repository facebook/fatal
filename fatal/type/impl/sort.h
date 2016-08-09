/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/cat.h>
#include <fatal/type/tag.h>

// TODO: REMOVE THESE HEADERS WHEN A PROPER SORT FOR SEQUENCES IS IN
#include <fatal/type/convert.h>
#include <fatal/type/list.h>

#include <type_traits>

#ifndef FATAL_INCLUDE_fatal_type_impl_sort_h
#define FATAL_INCLUDE_fatal_type_impl_sort_h

namespace fatal {
namespace impl_srt {

// partition recursion
template <template <typename...> class, typename...> struct p;

template <template <typename...> class Pair, typename L, typename R>
struct p<Pair, L, R> {
  using type = Pair<L, R>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename... Args
>
struct p<Pair, Variadic<L...>, Variadic<R...>, indexed<T0, true>, Args...>:
  p<Pair, Variadic<L..., T0>, Variadic<R...>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename... Args
>
struct p<Pair, Variadic<L...>, Variadic<R...>, indexed<T0, false>, Args...>:
  p<Pair, Variadic<L...>, Variadic<R..., T0>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, false>,
  Args...
>:
  p<Pair, Variadic<L...>, Variadic<R..., T0, T1>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, true>,
  Args...
>:
  p<Pair, Variadic<L..., T1>, Variadic<R..., T0>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, false>,
  Args...
>:
  p<Pair, Variadic<L..., T0>, Variadic<R..., T1>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, true>,
  Args...
>:
  p<Pair, Variadic<L..., T0, T1>, Variadic<R...>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, false>, indexed<T2, false>,
  Args...
>:
  p<Pair, Variadic<L...>, Variadic<R..., T0, T1, T2>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, false>, indexed<T2, true>,
  Args...
>:
  p<Pair, Variadic<L..., T2>, Variadic<R..., T0, T1>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, true>, indexed<T2, false>,
  Args...
>:
  p<Pair, Variadic<L..., T1>, Variadic<R..., T0, T2>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, true>, indexed<T2, true>,
  Args...
>:
  p<Pair, Variadic<L..., T1, T2>, Variadic<R..., T0>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, false>, indexed<T2, false>,
  Args...
>:
  p<Pair, Variadic<L..., T0>, Variadic<R..., T1, T2>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, false>, indexed<T2, true>,
  Args...
>:
  p<Pair, Variadic<L..., T0, T2>, Variadic<R..., T1>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, true>, indexed<T2, false>,
  Args...
>:
  p<Pair, Variadic<L..., T0, T1>, Variadic<R..., T2>, Args...>
{};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R,
  typename T0, typename T1, typename T2, typename... Args
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, true>, indexed<T2, true>,
  Args...
>:
  p<Pair, Variadic<L..., T0, T1, T2>, Variadic<R...>, Args...>
{};

// placeholder for partition's filtered sequence elements
template <typename T, T, bool> struct fv;

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic, typename T,
  T... L, T... R, T T0, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>, fv<T, T0, true>, Args...
>:
  p<Pair, Variadic<T, L..., T0>, Variadic<T, R...>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>, fv<T, T0, false>, Args...
>:
  p<Pair, Variadic<T, L...>, Variadic<T, R..., T0>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, false>,
  Args...
>:
  p<Pair, Variadic<T, L...>, Variadic<T, R..., T0, T1>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T1>, Variadic<T, R..., T0>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, false>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0>, Variadic<T, R..., T1>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0, T1>, Variadic<T, R...>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, false>, fv<T, T2, false>,
  Args...
>:
  p<Pair, Variadic<T, L...>, Variadic<T, R..., T0, T1, T2>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, false>, fv<T, T2, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T2>, Variadic<T, R..., T0, T1>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, true>, fv<T, T2, false>,
  Args...
>:
  p<Pair, Variadic<T, L..., T1>, Variadic<T, R..., T0, T2>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, false>, fv<T, T1, true>, fv<T, T2, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T1, T2>, Variadic<T, R..., T0>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, false>, fv<T, T2, false>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0>, Variadic<T, R..., T1, T2>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, false>, fv<T, T2, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0, T2>, Variadic<T, R..., T1>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, true>, fv<T, T2, false>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0, T1>, Variadic<T, R..., T2>, Args...>
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  fv<T, T0, true>, fv<T, T1, true>, fv<T, T2, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0, T1, T2>, Variadic<T, R...>, Args...>
{};

// partition entry point
template <template <typename...> class, typename...> struct pr;

template <
  template <typename...> class Pair,
  template <typename...> class Variadic,
  typename... Args,
  typename Filter
>
struct pr<Pair, Variadic<Args...>, Filter>:
  p<
    Pair, Variadic<>, Variadic<>,
    indexed<Args, Filter::template apply<Args>::value>...
  >
{};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... Args,
  typename Filter
>
struct pr<Pair, Variadic<T, Args...>, Filter>:
  p<
    Pair, Variadic<T>, Variadic<T>,
    fv<T, Args, Filter::template apply<T, Args>::value>...
  >
{};

template <typename...> struct fl;

template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Result
>
struct fl<Predicate, Variadic<Result...>> {
  using type = Variadic<Result...>;
};

template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Result,
  typename T,
  typename... Args
>
struct fl<Predicate, Variadic<Result...>, T, Args...>:
  fl<
    Predicate,
    typename std::conditional<
      Predicate::template apply<T>::value,
      Variadic<Result..., T>,
      Variadic<Result...>
    >::type,
    Args...
  >
{};

template <typename...> struct flt;

// TODO: OPTIMIZE
template <
  typename Predicate,
  template <typename...> class Variadic,
  typename... Args
>
struct flt<Predicate, Variadic<Args...>>:
  fl<Predicate, Variadic<>, Args...>
{};

template <typename...> struct mrg;

template <
  template <typename...> class Variadic,
  typename L, typename... LHS,
  typename R, typename... RHS,
  typename... Args
>
struct mrg<Variadic<L, LHS...>, Variadic<R, RHS...>, Variadic<Args...>> {
  using type = typename std::conditional<
    (R::value < L::value),
    typename mrg<Variadic<L, LHS...>, Variadic<RHS...>, Variadic<Args..., R>>::type,
    typename mrg<Variadic<LHS...>, Variadic<R, RHS...>, Variadic<Args..., L>>::type
  >::type;
};

template <template <typename...> class Variadic, typename... LHS, typename... Args>
struct mrg<Variadic<LHS...>, Variadic<>, Variadic<Args...>> {
  using type = Variadic<Args..., LHS...>;
};

template <template <typename...> class Variadic, typename... RHS, typename... Args>
struct mrg<Variadic<>, Variadic<RHS...>, Variadic<Args...>> {
  using type = Variadic<Args..., RHS...>;
};

template <template <typename...> class Variadic, typename... Args>
struct mrg<Variadic<>, Variadic<>, Variadic<Args...>> {
  using type = Variadic<Args...>;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T L, T... LHS,
  T R, T... RHS,
  T... Values
>
struct mrg<
  Variadic<T, L, LHS...>,
  Variadic<T, R, RHS...>,
  Variadic<T, Values...>
> {
  using type = typename std::conditional<
    (R < L),
    typename mrg<
      Variadic<T, L, LHS...>,
      Variadic<T, RHS...>,
      Variadic<T, Values..., R>
    >::type,
    typename mrg<
      Variadic<T, LHS...>,
      Variadic<T, R, RHS...>,
      Variadic<T, Values..., L>
    >::type
  >::type;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... LHS,
  T... Values
>
struct mrg<Variadic<T, LHS...>, Variadic<T>, Variadic<T, Values...>> {
  using type = Variadic<T, Values..., LHS...>;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... RHS,
  T... Values
>
struct mrg<Variadic<T>, Variadic<T, RHS...>, Variadic<T, Values...>> {
  using type = Variadic<T, Values..., RHS...>;
};

template <template <typename V, V...> class Variadic, typename T, T... Values>
struct mrg<Variadic<T>, Variadic<T>, Variadic<T, Values...>> {
  using type = Variadic<T, Values...>;
};


template <typename...> struct merge;

template <template <typename...> class Variadic, typename... LHS, typename... RHS>
struct merge<Variadic<LHS...>, Variadic<RHS...>> {
  using type = typename mrg<Variadic<LHS...>, Variadic<RHS...>, Variadic<>>::type;
};

template <
  template <typename V, V...> class Variadic,
  typename T,
  T... LHS,
  T... RHS
>
struct merge<Variadic<T, LHS...>, Variadic<T, RHS...>> {
  using type = typename mrg<
    Variadic<T, LHS...>,
    Variadic<T, RHS...>,
    Variadic<T>
  >::type;
};

template <typename...> struct ms;

template <template <typename...> class Variadic>
struct ms<Variadic<>> {
  using type = Variadic<>;
};

template <template <typename...> class Variadic, typename T>
struct ms<Variadic<T>> {
  using type = Variadic<T>;
};

template <template <typename...> class Variadic, typename... Args>
struct ms<Variadic<Args...>> {
  using type = typename mrg<
    typename ms<head<Variadic<Args...>, sizeof...(Args) / 2>>::type,
    typename ms<tail<Variadic<Args...>, sizeof...(Args) / 2>>::type,
    Variadic<>
  >::type;
};

template <template <typename V, V...> class Variadic, typename T>
struct ms<Variadic<T>> {
  using type = Variadic<T>;
};

template <template <typename V, V...> class Variadic, typename T, T Value>
struct ms<Variadic<T, Value>> {
  using type = Variadic<T, Value>;
};

template <template <typename V, V...> class Variadic, typename T, T... Values>
struct ms<Variadic<T, Values...>> {
  using type = typename mrg<
    typename ms<head<Variadic<T, Values...>, sizeof...(Values) / 2>>::type,
    typename ms<tail<Variadic<T, Values...>, sizeof...(Values) / 2>>::type,
    Variadic<T>
  >::type;
};

// quick-sort
template <typename...> struct qs;

template <template <typename...> class Variadic, typename Less>
struct qs<Variadic<>, Less> {
  using type = Variadic<>;
};

template <template <typename...> class Variadic, typename T, typename Less>
struct qs<Variadic<T>, Less> {
  using type = Variadic<T>;
};

template <
  template <typename...> class Variadic,
  typename LHS,
  typename RHS,
  typename Less
>
struct qs<Variadic<LHS, RHS>, Less>:
  std::conditional<
    Less::template apply<RHS, LHS>::value,
    Variadic<RHS, LHS>,
    Variadic<LHS, RHS>
  >
{};

// TODO: MAKE IT FOUR WAY QUICK SORT??
template <
  template <typename...> class Variadic,
  typename Pivot, typename... Args, typename Less
>
// TODO: USE A BETTER PIVOT
struct qs<Variadic<Pivot, Args...>, Less> {
  using type = lcat<
    typename qs<
      second<typename pr<pair, Variadic<Args...>, curry<Less, Pivot>>::type>,
      Less
    >::type,
    typename qs<
      first<typename pr<pair, Variadic<Args...>, curry<Less, Pivot>>::type>,
      Less
    >::type,
    Pivot
  >;
};

template <template <typename V, V...> class Variadic, typename T, typename Less>
struct qs<Variadic<T>, Less> {
  using type = Variadic<T>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T Value,
  typename Less
>
struct qs<Variadic<T, Value>, Less> {
  using type = Variadic<T, Value>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T LHS, T RHS,
  typename Less
>
struct qs<Variadic<T, LHS, RHS>, Less>:
  std::conditional<
    Less::template vapply<T, RHS, LHS>::value,
    Variadic<T, RHS, LHS>,
    Variadic<T, LHS, RHS>
  >
{};

// curried comparer for sequences
template <typename Less, typename T, T LHS>
struct sl {
  template <typename U, U RHS>
  using apply = typename Less::template vapply<T, LHS, RHS>;
};

// TODO: MAKE IT FOUR WAY QUICK SORT??
template <
  template <typename V, V...> class Variadic,
  typename T, T Pivot, T... Args,
  typename Less
>
// TODO: USE A BETTER PIVOT
struct qs<Variadic<T, Pivot, Args...>, Less> {
  using type = typename vcat<
    typename qs<
      second<typename pr<pair, Variadic<T, Args...>, sl<Less, T, Pivot>>::type>,
      Less
    >::type,
    typename qs<
      first<typename pr<pair, Variadic<T, Args...>, sl<Less, T, Pivot>>::type>,
      Less
    >::type
  >::template apply<Pivot>;
};

template <typename> struct inv;

template <template <typename, typename> class Pair, typename LHS, typename RHS>
struct inv<Pair<LHS, RHS>> {
  using type = Pair<RHS, LHS>;
};

template <
  template <typename...> class Variadic,
  typename LHS,
  typename RHS,
  typename... Args
>
struct inv<Variadic<LHS, RHS, Args...>> {
  using type = Variadic<RHS, LHS, Args...>;
};

} // namespace impl_srt {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_sort_h
