/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_sort_h
#define FATAL_INCLUDE_fatal_type_impl_sort_h

#include <fatal/type/apply.h>
#include <fatal/type/cat.h>
#include <fatal/type/conditional.h>
#include <fatal/type/slice.h>
#include <fatal/type/tag.h>

// TODO: REMOVE THESE HEADERS WHEN A PROPER SORT FOR SEQUENCES IS IN
#include <fatal/type/convert.h>
#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {
namespace i_s {

// partition recursion
template <template <typename...> class, typename...> struct p;

template <template <typename...> class Pair, typename L, typename R>
struct p<Pair, L, R> {
  using type = Pair<L, R>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0
>
struct p<Pair, Variadic<L...>, Variadic<R...>, indexed<T0, true>> {
  using type = Pair<Variadic<L..., T0>, Variadic<R...>>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0
>
struct p<Pair, Variadic<L...>, Variadic<R...>, indexed<T0, false>> {
  using type = Pair<Variadic<L...>, Variadic<R..., T0>>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, false>
> {
  using type = Pair<Variadic<L...>, Variadic<R..., T0, T1>>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, false>, indexed<T1, true>
> {
  using type = Pair<Variadic<L..., T1>, Variadic<R..., T0>>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, false>
> {
  using type = Pair<Variadic<L..., T0>, Variadic<R..., T1>>;
};

template <
  template <typename...> class Pair, template <typename...> class Variadic,
  typename... L, typename... R, typename T0, typename T1
>
struct p<
  Pair, Variadic<L...>, Variadic<R...>,
  indexed<T0, true>, indexed<T1, true>
> {
  using type = Pair<Variadic<L..., T0, T1>, Variadic<R...>>;
};

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
template <typename T, T, bool> struct h;

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic, typename T,
  T... L, T... R, T T0
>
struct p<Pair, Variadic<T, L...>, Variadic<T, R...>, h<T, T0, true>> {
  using type = Pair<Variadic<T, L..., T0>, Variadic<T, R...>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>, h<T, T0, false>
> {
  using type = Pair<Variadic<T, L...>, Variadic<T, R..., T0>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  h<T, T0, false>, h<T, T1, false>
> {
  using type = Pair<Variadic<T, L...>, Variadic<T, R..., T0, T1>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  h<T, T0, false>, h<T, T1, true>
> {
  using type = Pair<Variadic<T, L..., T1>, Variadic<T, R..., T0>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  h<T, T0, true>, h<T, T1, false>
> {
  using type = Pair<Variadic<T, L..., T0>, Variadic<T, R..., T1>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  h<T, T0, true>, h<T, T1, true>
> {
  using type = Pair<Variadic<T, L..., T0, T1>, Variadic<T, R...>>;
};

template <
  template <typename...> class Pair,
  template <typename V, V...> class Variadic,
  typename T, T... L, T... R, T T0, T T1, T T2, typename... Args
>
struct p<
  Pair, Variadic<T, L...>, Variadic<T, R...>,
  h<T, T0, false>, h<T, T1, false>, h<T, T2, false>,
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
  h<T, T0, false>, h<T, T1, false>, h<T, T2, true>,
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
  h<T, T0, false>, h<T, T1, true>, h<T, T2, false>,
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
  h<T, T0, false>, h<T, T1, true>, h<T, T2, true>,
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
  h<T, T0, true>, h<T, T1, false>, h<T, T2, false>,
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
  h<T, T0, true>, h<T, T1, false>, h<T, T2, true>,
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
  h<T, T0, true>, h<T, T1, true>, h<T, T2, false>,
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
  h<T, T0, true>, h<T, T1, true>, h<T, T2, true>,
  Args...
>:
  p<Pair, Variadic<T, L..., T0, T1, T2>, Variadic<T, R...>, Args...>
{};

// partition entry point
template <template <typename...> class, typename...> struct P;

template <
  template <typename...> class Pair,
  template <typename...> class Variadic,
  typename... Args,
  typename Filter
>
struct P<Pair, Variadic<Args...>, Filter>:
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
struct P<Pair, Variadic<T, Args...>, Filter>:
  p<
    Pair, Variadic<T>, Variadic<T>,
    h<T, Args, Filter::template apply<T, Args>::value>...
  >
{};

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
  using type = conditional<
    (R::value < L::value),
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

// TODO: MAKE IT FOUR WAY QUICK SORT??
template <
  template <typename...> class Variadic,
  typename Pivot, typename... Args, typename Less
>
// TODO: USE A BETTER PIVOT
struct q<Variadic<Pivot, Args...>, Less> {
  using type = lcat<
    typename q<
      second<typename P<pair, Variadic<Args...>, curry<Less, Pivot>>::type>,
      Less
    >::type,
    typename q<
      first<typename P<pair, Variadic<Args...>, curry<Less, Pivot>>::type>,
      Less
    >::type,
    Pivot
  >;
};

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

// curried comparer for sequences
template <typename Less, typename T, T LHS>
struct c {
  template <typename U, U RHS>
  using apply = typename Less::template vapply<T, LHS, RHS>;
};

template <
  template <typename V, V...> class Variadic,
  typename T, T Pivot, T Arg0, T Arg1, T... Args,
  typename Less
>
// TODO: USE A BETTER PIVOT
struct q<Variadic<T, Pivot, Arg0, Arg1, Args...>, Less> {
  using type = typename vcat<
    typename q<
      second<
        typename P<
          pair,
          Variadic<T, Arg0, Arg1, Args...>,
          c<Less, T, Pivot>
        >::type
      >,
      Less
    >::type,
    typename q<
      first<
        typename P<
          pair,
          Variadic<T, Arg0, Arg1, Args...>,
          c<Less, T, Pivot>
        >::type
      >,
      Less
    >::type
  >::template apply<Pivot>;
};

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

// quicksort entry-point
template <typename...> struct Q;

template <typename T, typename Less>
struct Q<T, Less>: q<T, Less> {};

template <typename T, typename Less, typename By>
struct Q<T, Less, By>: q<T, C<Less, By>> {};

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

#endif // FATAL_INCLUDE_fatal_type_impl_sort_h
