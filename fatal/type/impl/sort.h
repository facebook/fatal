/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/slice.h>

#ifndef FATAL_INCLUDE_fatal_type_impl_sort_h
#define FATAL_INCLUDE_fatal_type_impl_sort_h

namespace fatal {
namespace impl_sort {

template <typename...> struct merge;

template <
  template <typename...> class List,
  typename L, typename... LHS,
  typename R, typename... RHS,
  typename... Args
>
struct merge<List<L, LHS...>, List<R, RHS...>, List<Args...>> {
  using type = typename std::conditional<
    (R::value < L::value),
    typename merge<List<L, LHS...>, List<RHS...>, List<Args..., R>>::type,
    typename merge<List<LHS...>, List<R, RHS...>, List<Args..., L>>::type
  >::type;
};

template <template <typename...> class List, typename... LHS, typename... Args>
struct merge<List<LHS...>, List<>, List<Args...>> {
  using type = List<Args..., LHS...>;
};

template <template <typename...> class List, typename... RHS, typename... Args>
struct merge<List<>, List<RHS...>, List<Args...>> {
  using type = List<Args..., RHS...>;
};

template <template <typename...> class List, typename... Args>
struct merge<List<>, List<>, List<Args...>> {
  using type = List<Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T L, T... LHS,
  T R, T... RHS,
  T... Values
>
struct merge<
  Sequence<T, L, LHS...>,
  Sequence<T, R, RHS...>,
  Sequence<T, Values...>
> {
  using type = typename std::conditional<
    (R < L),
    typename merge<
      Sequence<T, L, LHS...>,
      Sequence<T, RHS...>,
      Sequence<T, Values..., R>
    >::type,
    typename merge<
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
struct merge<Sequence<T, LHS...>, Sequence<T>, Sequence<T, Values...>> {
  using type = Sequence<T, Values..., LHS...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... RHS,
  T... Values
>
struct merge<Sequence<T>, Sequence<T, RHS...>, Sequence<T, Values...>> {
  using type = Sequence<T, Values..., RHS...>;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct merge<Sequence<T>, Sequence<T>, Sequence<T, Values...>> {
  using type = Sequence<T, Values...>;
};


template <typename...> struct mentry;

template <template <typename...> class List, typename... LHS, typename... RHS>
struct mentry<List<LHS...>, List<RHS...>> {
  using type = typename merge<List<LHS...>, List<RHS...>, List<>>::type;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... LHS,
  T... RHS
>
struct mentry<Sequence<T, LHS...>, Sequence<T, RHS...>> {
  using type = typename merge<
    Sequence<T, LHS...>,
    Sequence<T, RHS...>,
    Sequence<T>
  >::type;
};

template <typename...> struct msort;

template <template <typename...> class List>
struct msort<List<>> {
  using type = List<>;
};

template <template <typename...> class List, typename T>
struct msort<List<T>> {
  using type = List<T>;
};

template <template <typename...> class List, typename... Args>
struct msort<List<Args...>> {
  using type = typename merge<
    typename msort<head<List<Args...>, sizeof...(Args) / 2>>::type,
    typename msort<tail<List<Args...>, sizeof...(Args) / 2>>::type,
    List<>
  >::type;
};

template <template <typename V, V...> class Sequence, typename T>
struct msort<Sequence<T>> {
  using type = Sequence<T>;
};

template <template <typename V, V...> class Sequence, typename T, T Value>
struct msort<Sequence<T, Value>> {
  using type = Sequence<T, Value>;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct msort<Sequence<T, Values...>> {
  using type = typename merge<
    typename msort<head<Sequence<T, Values...>, sizeof...(Values) / 2>>::type,
    typename msort<tail<Sequence<T, Values...>, sizeof...(Values) / 2>>::type,
    Sequence<T>
  >::type;
};

} // namespace impl_sort {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_sort_h
