/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/cat.h>
#include <fatal/type/slice.h>

// TODO: REMOVE THESE HEADERS WHEN A PROPER SORT FOR SEQUENCES IS IN
#include <fatal/type/convert.h>
#include <fatal/type/list.h>

#include <type_traits>

#ifndef FATAL_INCLUDE_fatal_type_impl_sort_h
#define FATAL_INCLUDE_fatal_type_impl_sort_h

namespace fatal {
namespace impl_srt {

template <template <typename...> class, typename...> struct p;

template <
  template <typename...> class Pair,
  template <typename...> class List,
  typename... Left,
  typename... Right,
  typename Less,
  typename T,
  typename... Args
>
struct p<Pair, List<Left...>, List<Right...>, Less, T, Args...>:
  p<
    Pair,
    typename std::conditional<
      Less::template apply<T>::value,
      List<Left..., T>,
      List<Left...>
    >::type,
    typename std::conditional<
      Less::template apply<T>::value,
      List<Right...>,
      List<Right..., T>
    >::type,
    Less, Args...
  >
{};

template <
  template <typename...> class Pair,
  template <typename...> class List,
  typename... Left,
  typename... Right,
  typename Less
>
struct p<Pair, List<Left...>, List<Right...>, Less> {
  using type = Pair<List<Left...>, List<Right...>>;
};

template <template <typename...> class, typename...> struct part;

template <
  template <typename...> class Pair,
  typename Less,
  template <typename...> class List,
  typename... Args
>
struct part<Pair, List<Args...>, Less>:
  p<Pair, List<>, List<>, Less, Args...>
{};

template <typename...> struct fl;

template <
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result
>
struct fl<Predicate, Variadics<Result...>> {
  using type = Variadics<Result...>;
};

template <
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result,
  typename T,
  typename... Args
>
struct fl<Predicate, Variadics<Result...>, T, Args...>:
  fl<
    Predicate,
    typename std::conditional<
      Predicate::template apply<T>::value,
      Variadics<Result..., T>,
      Variadics<Result...>
    >::type,
    Args...
  >
{};

template <typename...> struct flt;

// TODO: OPTIMIZE
template <
  typename Predicate,
  template <typename...> class Variadics,
  typename... Args
>
struct flt<Predicate, Variadics<Args...>>:
  fl<Predicate, Variadics<>, Args...>
{};

template <typename...> struct mrg;

template <
  template <typename...> class List,
  typename L, typename... LHS,
  typename R, typename... RHS,
  typename... Args
>
struct mrg<List<L, LHS...>, List<R, RHS...>, List<Args...>> {
  using type = typename std::conditional<
    (R::value < L::value),
    typename mrg<List<L, LHS...>, List<RHS...>, List<Args..., R>>::type,
    typename mrg<List<LHS...>, List<R, RHS...>, List<Args..., L>>::type
  >::type;
};

template <template <typename...> class List, typename... LHS, typename... Args>
struct mrg<List<LHS...>, List<>, List<Args...>> {
  using type = List<Args..., LHS...>;
};

template <template <typename...> class List, typename... RHS, typename... Args>
struct mrg<List<>, List<RHS...>, List<Args...>> {
  using type = List<Args..., RHS...>;
};

template <template <typename...> class List, typename... Args>
struct mrg<List<>, List<>, List<Args...>> {
  using type = List<Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T L, T... LHS,
  T R, T... RHS,
  T... Values
>
struct mrg<
  Sequence<T, L, LHS...>,
  Sequence<T, R, RHS...>,
  Sequence<T, Values...>
> {
  using type = typename std::conditional<
    (R < L),
    typename mrg<
      Sequence<T, L, LHS...>,
      Sequence<T, RHS...>,
      Sequence<T, Values..., R>
    >::type,
    typename mrg<
      Sequence<T, LHS...>,
      Sequence<T, R, RHS...>,
      Sequence<T, Values..., L>
    >::type
  >::type;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... LHS,
  T... Values
>
struct mrg<Sequence<T, LHS...>, Sequence<T>, Sequence<T, Values...>> {
  using type = Sequence<T, Values..., LHS...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... RHS,
  T... Values
>
struct mrg<Sequence<T>, Sequence<T, RHS...>, Sequence<T, Values...>> {
  using type = Sequence<T, Values..., RHS...>;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct mrg<Sequence<T>, Sequence<T>, Sequence<T, Values...>> {
  using type = Sequence<T, Values...>;
};


template <typename...> struct merge;

template <template <typename...> class List, typename... LHS, typename... RHS>
struct merge<List<LHS...>, List<RHS...>> {
  using type = typename mrg<List<LHS...>, List<RHS...>, List<>>::type;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... LHS,
  T... RHS
>
struct merge<Sequence<T, LHS...>, Sequence<T, RHS...>> {
  using type = typename mrg<
    Sequence<T, LHS...>,
    Sequence<T, RHS...>,
    Sequence<T>
  >::type;
};

template <typename...> struct ms;

template <template <typename...> class List>
struct ms<List<>> {
  using type = List<>;
};

template <template <typename...> class List, typename T>
struct ms<List<T>> {
  using type = List<T>;
};

template <template <typename...> class List, typename... Args>
struct ms<List<Args...>> {
  using type = typename mrg<
    typename ms<head<List<Args...>, sizeof...(Args) / 2>>::type,
    typename ms<tail<List<Args...>, sizeof...(Args) / 2>>::type,
    List<>
  >::type;
};

template <template <typename V, V...> class Sequence, typename T>
struct ms<Sequence<T>> {
  using type = Sequence<T>;
};

template <template <typename V, V...> class Sequence, typename T, T Value>
struct ms<Sequence<T, Value>> {
  using type = Sequence<T, Value>;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct ms<Sequence<T, Values...>> {
  using type = typename mrg<
    typename ms<head<Sequence<T, Values...>, sizeof...(Values) / 2>>::type,
    typename ms<tail<Sequence<T, Values...>, sizeof...(Values) / 2>>::type,
    Sequence<T>
  >::type;
};

template <typename...> struct qs;

template <template <typename...> class List, typename Less>
struct qs<List<>, Less> {
  using type = List<>;
};

template <template <typename...> class List, typename T, typename Less>
struct qs<List<T>, Less> {
  using type = List<T>;
};

template <
  template <typename...> class List,
  typename LHS,
  typename RHS,
  typename Less
>
struct qs<List<LHS, RHS>, Less>:
  std::conditional<
    Less::template apply<RHS, LHS>::value,
    List<RHS, LHS>,
    List<LHS, RHS>
  >
{};

// TODO: MAKE IT FOUR WAY QUICK SORT??
template <
  template <typename...> class List,
  typename T,
  typename... Args,
  typename Less
>
// TODO: USE A BETTER PIVOT
struct qs<List<T, Args...>, Less> {
  using type = lcat<
    typename qs<
      second<typename part<pair, List<Args...>, curry<Less, T>>::type>,
      Less
    >::type,
    typename qs<
      first<typename part<pair, List<Args...>, curry<Less, T>>::type>,
      Less
    >::type,
    T
  >;
};

// TODO: PROVIDE A FIRST CLASS IMPLEMENTATION FOR SEQUENCES
template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  typename Less
>
struct qs<Sequence<T, Values...>, Less> {
  using type = as_sequence<
    typename qs<list<std::integral_constant<T, Values>...>, Less>::type,
    Sequence,
    T
  >;
};

template <typename> struct inv;

template <template <typename, typename> class Pair, typename LHS, typename RHS>
struct inv<Pair<LHS, RHS>> {
  using type = Pair<RHS, LHS>;
};

template <
  template <typename...> class Variadics,
  typename LHS,
  typename RHS,
  typename... Args
>
struct inv<Variadics<LHS, RHS, Args...>> {
  using type = Variadics<RHS, LHS, Args...>;
};

} // namespace impl_srt {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_sort_h
