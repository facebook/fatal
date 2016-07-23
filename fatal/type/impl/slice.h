/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_slice_h
#define FATAL_INCLUDE_fatal_type_impl_slice_h

#include <fatal/type/inherit.h>
#include <fatal/type/sequence.h>

#include <type_traits>

namespace fatal {
namespace impl_at {

template <typename, std::size_t> struct at;

template <template <typename> class Single, typename T>
struct at<Single<T>, 0> {
  using type = T;
};

template <
  template <typename, typename> class Pair,
  typename First,
  typename Second
>
struct at<Pair<First, Second>, 0> {
  using type = First;
};

template <
  template <typename, typename> class Pair,
  typename First,
  typename Second
>
struct at<Pair<First, Second>, 1> {
  using type = Second;
};

template <template <typename...> class List, typename T, typename... Args>
struct at<List<T, Args...>, 0> {
  using type = T;
};

template <
  template <typename...> class List,
  typename First,
  typename Second,
  typename... Args
>
struct at<List<First, Second, Args...>, 1> {
  using type = Second;
};

template <
  template <typename...> class List,
  typename First,
  typename Second,
  typename Third,
  typename... Args
>
struct at<List<First, Second, Third, Args...>, 2> {
  using type = Third;
};

template <
  typename T,
  template <typename, T...> class Sequence,
  T Value,
  T... Values
>
struct at<Sequence<T, Value, Values...>, 0> {
  using type = std::integral_constant<T, Value>;
};

template <
  typename T,
  template <typename, T...> class Sequence,
  T First,
  T Second,
  T... Values
>
struct at<Sequence<T, First, Second, Values...>, 1> {
  using type = std::integral_constant<T, Second>;
};

template <
  typename T,
  template <typename, T...> class Sequence,
  T First,
  T Second,
  T Third,
  T... Values
>
struct at<Sequence<T, First, Second, Third, Values...>, 2> {
  using type = std::integral_constant<T, Third>;
};

template <bool, typename, std::size_t, typename Default>
struct tat {
  using type = Default;
};

template <typename T, std::size_t Index, typename Default>
struct tat<true, T, Index, Default>: at<T, Index> {};

template <typename, std::size_t...> struct pick;

#ifndef FATAL_AT_USE_VOID_POINTER_TRICK

template <typename...> struct tindex;

template <std::size_t... Indexes, typename... Args>
struct tindex<index_sequence<Indexes...>, Args...>:
  indexed<Args, Indexes>...
{};

template <typename, typename T, T...> struct vindex;

template <std::size_t... Indexes, typename T, T... Values>
struct vindex<index_sequence<Indexes...>, T, Values...>:
  indexed<std::integral_constant<T, Values>, Indexes>...
{};

template <std::size_t Index, typename T>
static T find(indexed<T, Index>);

template <
  template <typename...> class List,
  typename... Args,
  std::size_t Index
>
struct at<List<Args...>, Index> {
  using type = decltype(
    find<Index>(
      tindex<make_index_sequence<sizeof...(Args)>, Args...>()
    )
  );
};

template <
  typename T,
  template <typename, T...> class Sequence,
  T... Values,
  std::size_t Index
>
struct at<Sequence<T, Values...>, Index> {
  using type = decltype(
    find<Index>(
      vindex<make_index_sequence<sizeof...(Values)>, T, Values...>()
    )
  );
};

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct pick<List<Args...>, Indexes...> {
  using type = List<
    decltype(
      find<Indexes>(
        tindex<make_index_sequence<sizeof...(Args)>, Args...>()
      )
    )...
  >;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct pick<Sequence<T, Values...>, Indexes...> {
  using type = Sequence<
    T,
    decltype(
      find<Indexes>(
        vindex<make_index_sequence<sizeof...(Values)>, T, Values...>()
      )
    )::value...
  >;
};

#else // FATAL_AT_USE_VOID_POINTER_TRICK

// TODO: RE-CHECK AND REMOVE DEFINES
template <typename> struct tindex;

template <std::size_t... Indexes>
struct tindex<index_sequence<Indexes...>> {
  template <typename T>
  static T find(decltype(reinterpret_cast<void *>(Indexes))..., tag<T> *, ...);
};

template <typename> struct vindex;

template <std::size_t... Indexes>
struct vindex<index_sequence<Indexes...>> {
  template <typename T, T Value>
  static std::integral_constant<T, Value> find(
    decltype(reinterpret_cast<void *>(Indexes))...,
    std::integral_constant<T, Value> *,
    ...
  );
};

template <
  template <typename...> class List,
  typename... Args,
  std::size_t Index
>
struct at<List<Args...>, Index> {
  using type = decltype(
    tindex<make_index_sequence<Index>>::find(
      static_cast<tag<Args> *>(nullptr)...
    )
  );
};

template <
  typename T,
  template <typename, T...> class Sequence,
  T... Values,
  std::size_t Index
>
struct at<Sequence<T, Values...>, Index> {
  using type = decltype(
    vindex<make_index_sequence<Index>>::find(
      static_cast<std::integral_constant<T, Values> *>(nullptr)...
    )
  );
};

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct pick<List<Args...>, Indexes...> {
  using type = List<
    decltype(
      tindex<make_index_sequence<Indexes>>::find(
        static_cast<tag<Args> *>(nullptr)...
      )
    )...
  >;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct pick<Sequence<T, Values...>, Indexes...> {
  using type = Sequence<
    T,
    decltype(
      vindex<make_index_sequence<Indexes>>::find(
        static_cast<std::integral_constant<T, Values> *>(nullptr)...
      )
    )::value...
  >;
};

#endif // FATAL_AT_USE_VOID_POINTER_TRICK

template <std::size_t...> struct ttindex;

template <std::size_t Size, std::size_t Left, std::size_t... Indexes>
struct ttindex<Size, Left, Indexes...> {
  template <template <typename...> class List, typename T, typename... Tail>
  static List<T, Tail...> list(
    decltype(reinterpret_cast<void *>(Indexes))...,
    tag<T> *,
    tag<Tail> *...
  );
};

template <std::size_t Size, std::size_t... Indexes>
struct ttindex<Size, Size, Indexes...> {
  template <template <typename...> class List>
  static List<> list(...);
};

template <std::size_t...> struct tvindex;

template <std::size_t Size, std::size_t Left, std::size_t... Indexes>
struct tvindex<Size, Left, Indexes...> {
  template <
    template <typename V, V...> class Sequence,
    typename T,
    T Value,
    T... Values
  >
  static Sequence<T, Value, Values...> seq(
    decltype(reinterpret_cast<void *>(Indexes))...,
    std::integral_constant<T, Value> *,
    std::integral_constant<T, Values> *...
  );
};

template <std::size_t Size, std::size_t... Indexes>
struct tvindex<Size, Size, Indexes...> {
  template <template <typename V, V...> class Sequence, typename T>
  static Sequence<T> seq(...);
};

template <typename...> struct tail;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct tail<List<Args...>, index_sequence<Indexes...>> {
  using type = decltype(
    ttindex<sizeof...(Args), sizeof...(Indexes), Indexes...>
      ::template list<List>(static_cast<tag<Args> *>(nullptr)...)
  );
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct tail<Sequence<T, Values...>, index_sequence<Indexes...>> {
  using type = decltype(
    tvindex<sizeof...(Values), sizeof...(Indexes), Indexes...>
      ::template seq<Sequence, T>(
        static_cast<std::integral_constant<T, Values> *>(nullptr)...
      )
  );
};

template <bool, std::size_t, template <typename...> class, typename...>
struct thead;

template <template <typename...> class List, typename... Tail>
struct thead<false, 0, List, Tail...> {
  template <typename... Head>
  using type = List<Head...>;
};

template <template <typename...> class List, typename T01, typename... Tail>
struct thead<false, 1, List, T01, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01>;
};

template <
  template <typename...> class List,
  typename T01, typename T02,
  typename... Tail
>
struct thead<false, 2, List, T01, T02, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03,
  typename... Tail
>
struct thead<false, 3, List, T01, T02, T03, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04,
  typename... Tail
>
struct thead<false, 4, List, T01, T02, T03, T04, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename... Tail
>
struct thead<false, 5, List, T01, T02, T03, T04, T05, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06,
  typename... Tail
>
struct thead<false, 6, List, T01, T02, T03, T04, T05, T06, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05, T06>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07,
  typename... Tail
>
struct thead<false, 7, List, T01, T02, T03, T04, T05, T06, T07, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05, T06, T07>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08,
  typename... Tail
>
struct thead<false, 8, List, T01, T02, T03, T04, T05, T06, T07, T08, Tail...> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05, T06, T07, T08>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09,
  typename... Tail
>
struct thead<
  false, 9, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09,
  Tail...
> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05, T06, T07, T08, T09>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename... Tail
>
struct thead<
  false, 10, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10,
  Tail...
> {
  template <typename... Head>
  using type = List<Head..., T01, T02, T03, T04, T05, T06, T07, T08, T09, T10>;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11,
  typename... Tail
>
struct thead<
  false, 11, List, T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12,
  typename... Tail
>
struct thead<
  false, 12, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13,
  typename... Tail
>
struct thead<
  false, 13, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14,
  typename... Tail
>
struct thead<
  false, 14, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename... Tail
>
struct thead<
  false, 15, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16,
  typename... Tail
>
struct thead<
  false, 16, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17,
  typename... Tail
>
struct thead<
  false, 17, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18,
  typename... Tail
>
struct thead<
  false, 18, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19,
  typename... Tail
>
struct thead<
  false, 19, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename... Tail
>
struct thead<
  false, 20, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21,
  typename... Tail
>
struct thead<
  false, 21, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22,
  typename... Tail
>
struct thead<
  false, 22, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23,
  typename... Tail
>
struct thead<
  false, 23, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24,
  typename... Tail
>
struct thead<
  false, 24, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename... Tail
>
struct thead<
  false, 25, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26,
  typename... Tail
>
struct thead<
  false, 26, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27,
  typename... Tail
>
struct thead<
  false, 27, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28,
  typename... Tail
>
struct thead<
  false, 28, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28, typename T29,
  typename... Tail
>
struct thead<
  false, 29, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28, typename T29, typename T30,
  typename... Tail
>
struct thead<
  false, 30, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28, typename T29, typename T30,
  typename T31,
  typename... Tail
>
struct thead<
  false, 31, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
  T31,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
    T31
  >;
};

template <
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28, typename T29, typename T30,
  typename T31, typename T32,
  typename... Tail
>
struct thead<
  false, 32, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
  T31, T32,
  Tail...
> {
  template <typename... Head>
  using type = List<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
    T31, T32
  >;
};

template <
  std::size_t Size,
  template <typename...> class List,
  typename T01, typename T02, typename T03, typename T04, typename T05,
  typename T06, typename T07, typename T08, typename T09, typename T10,
  typename T11, typename T12, typename T13, typename T14, typename T15,
  typename T16, typename T17, typename T18, typename T19, typename T20,
  typename T21, typename T22, typename T23, typename T24, typename T25,
  typename T26, typename T27, typename T28, typename T29, typename T30,
  typename T31, typename T32,
  typename... Tail
>
struct thead<
  true, Size, List,
  T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
  T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
  T31, T32,
  Tail...
> {
  template <typename... Head>
  using type = typename thead<
    ((Size - 32) > 32), Size - 32, List, Tail...
  >::template type<
    Head...,
    T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15,
    T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
    T31, T32
  >;
};

template <
  bool, std::size_t, template <typename V, V...> class, typename T, T...
>
struct vhead;

template <template <typename V, V...> class Sequence, typename T, T... Tail>
struct vhead<false, 0, Sequence, T, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head...>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01,
  T... Tail
>
struct vhead<false, 1, Sequence, T, V01, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02,
  T... Tail
>
struct vhead<false, 2, Sequence, T, V01, V02, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03,
  T... Tail
>
struct vhead<false, 3, Sequence, T, V01, V02, V03, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04,
  T... Tail
>
struct vhead<false, 4, Sequence, T, V01, V02, V03, V04, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03, V04>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05,
  T... Tail
>
struct vhead<false, 5, Sequence, T, V01, V02, V03, V04, V05, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03, V04, V05>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06,
  T... Tail
>
struct vhead<false, 6, Sequence, T, V01, V02, V03, V04, V05, V06, Tail...> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03, V04, V05, V06>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07,
  T... Tail
>
struct vhead<
  false, 7, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07,
  Tail...
> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03, V04, V05, V06, V07>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08,
  T... Tail
>
struct vhead<
  false, 8, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08,
  Tail...
> {
  template <T... Head>
  using type = Sequence<T, Head..., V01, V02, V03, V04, V05, V06, V07, V08>;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09,
  T... Tail
>
struct vhead<
  false, 9, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10,
  T... Tail
>
struct vhead<
  false, 10, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T... Tail
>
struct vhead<
  false, 11, Sequence, T, V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12,
  T... Tail
>
struct vhead<
  false, 12, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13,
  T... Tail
>
struct vhead<
  false, 13, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14,
  T... Tail
>
struct vhead<
  false, 14, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15,
  T... Tail
>
struct vhead<
  false, 15, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16,
  T... Tail
>
struct vhead<
  false, 16, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17,
  T... Tail
>
struct vhead<
  false, 17, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18,
  T... Tail
>
struct vhead<
  false, 18, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19,
  T... Tail
>
struct vhead<
  false, 19, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20,
  T... Tail
>
struct vhead<
  false, 20, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21,
  T... Tail
>
struct vhead<
  false, 21, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T... Tail
>
struct vhead<
  false, 22, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23,
  T... Tail
>
struct vhead<
  false, 23, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24,
  T... Tail
>
struct vhead<
  false, 24, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25,
  T... Tail
>
struct vhead<
  false, 25, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26,
  T... Tail
>
struct vhead<
  false, 26, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27,
  T... Tail
>
struct vhead<
  false, 27, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28,
  T... Tail
>
struct vhead<
  false, 28, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28, T V29,
  T... Tail
>
struct vhead<
  false, 29, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28, T V29, T V30,
  T... Tail
>
struct vhead<
  false, 30, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28, T V29, T V30, T V31,
  T... Tail
>
struct vhead<
  false, 31, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
  V31,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
    V31
  >;
};

template <
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28, T V29, T V30, T V31, T V32,
  T... Tail
>
struct vhead<
  false, 32, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
  V31, V32,
  Tail...
> {
  template <T... Head>
  using type = Sequence<
    T, Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
    V31, V32
  >;
};

template <
  std::size_t Size,
  template <typename V, V...> class Sequence, typename T,
  T V01, T V02, T V03, T V04, T V05, T V06, T V07, T V08, T V09, T V10, T V11,
  T V12, T V13, T V14, T V15, T V16, T V17, T V18, T V19, T V20, T V21, T V22,
  T V23, T V24, T V25, T V26, T V27, T V28, T V29, T V30, T V31, T V32,
  T... Tail
>
struct vhead<
  true, Size, Sequence, T,
  V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
  V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
  V31, V32,
  Tail...
> {
  template <T... Head>
  using type = typename vhead<
    ((Size - 32) > 32), Size - 32, Sequence, T, Tail...
  >::template type<
    Head...,
    V01, V02, V03, V04, V05, V06, V07, V08, V09, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30,
    V31, V32
  >;
};

template <typename, std::size_t> struct head;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t Offset
>
struct head<List<Args...>, Offset>:
  thead<(Offset > 32), Offset, List, Args...>
{
  static_assert(Offset <= sizeof...(Args), "index out of bounds");
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t Offset
>
struct head<Sequence<T, Values...>, Offset>:
  vhead<(Offset > 32), Offset, Sequence, T, Values...>
{
  static_assert(Offset <= sizeof...(Values), "index out of bounds");
};
//*/

template <typename T, std::size_t Index>
static std::integral_constant<std::size_t, Index> itfind(indexed<T, Index>);

template <typename T, T, std::size_t> struct vindexed {};

template <typename T, T Value, std::size_t Index>
static std::integral_constant<std::size_t, Index> ivfind(
  vindexed<T, Value, Index>
);

template <typename...> struct idx;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct idx<List<Args...>, index_sequence<Indexes...>> {
  template <typename T>
  using of = decltype(itfind<T>(inherit<indexed<Args, Indexes>...>()));
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct idx<Sequence<T, Values...>, index_sequence<Indexes...>> {
  template <T Value>
  using of = decltype(
    ivfind<T, Value>(inherit<vindexed<T, Values, Indexes>...>())
  );
};

} // namespace impl_at {
} // namespace fatal {

#undef FATAL_AT_USE_VOID_POINTER_TRICK

#endif // FATAL_INCLUDE_fatal_type_impl_slice_h
