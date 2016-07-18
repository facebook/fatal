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

#include <fatal/type/sequence.h>
#include <fatal/type/tag.h>

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

template <typename, std::size_t...> struct pick;

#ifndef FATAL_AT_USE_VOID_POINTER_TRICK

template <typename...> struct index;

template <std::size_t... Indexes, typename... Args>
struct index<index_sequence<Indexes...>, Args...>:
  indexed<Args, Indexes>...
{};

template <typename, typename T, T...> struct value_index;

template <std::size_t... Indexes, typename T, T... Values>
struct value_index<index_sequence<Indexes...>, T, Values...>:
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
      index<make_index_sequence<sizeof...(Args)>, Args...>()
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
      value_index<make_index_sequence<sizeof...(Values)>, T, Values...>()
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
        index<make_index_sequence<sizeof...(Args)>, Args...>()
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
        value_index<make_index_sequence<sizeof...(Values)>, T, Values...>()
      )
    )::value...
  >;
};

#else // FATAL_AT_USE_VOID_POINTER_TRICK

// TODO: RE-CHECK AND REMOVE DEFINES
template <std::size_t Index, typename = make_index_sequence<Index>>
struct index;

template <std::size_t Index, std::size_t... Indexes>
struct index<Index, index_sequence<Indexes...>> {
  template <typename T>
  static T find(decltype(reinterpret_cast<void *>(Indexes))..., tag<T> *, ...);
};

template <std::size_t Index, typename = make_index_sequence<Index>>
struct value_index;

template <std::size_t Index, std::size_t... Indexes>
struct value_index<Index, index_sequence<Indexes...>> {
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
    index<Index>::find(
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
    value_index<Index>::find(
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
      index<Indexes>::find(
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
      value_index<Indexes>::find(
        static_cast<std::integral_constant<T, Values> *>(nullptr)...
      )
    )::value...
  >;
};

#endif // FATAL_AT_USE_VOID_POINTER_TRICK

template <std::size_t...> struct tindex;

template <std::size_t Size, std::size_t Left, std::size_t... Indexes>
struct tindex<Size, Left, Indexes...> {
  template <template <typename...> class List, typename T, typename... Tail>
  static List<T, Tail...> list(
    decltype(reinterpret_cast<void *>(Indexes))...,
    tag<T> *,
    tag<Tail> *...
  );
};

template <std::size_t Size, std::size_t... Indexes>
struct tindex<Size, Size, Indexes...> {
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
    tindex<sizeof...(Args), sizeof...(Indexes), Indexes...>
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

template <
  typename,
  std::size_t,
  std::size_t Offset,
  typename = make_index_sequence<Offset>
>
struct head;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t Size,
  std::size_t Offset,
  std::size_t... Indexes
>
struct head<List<Args...>, Size, Offset, index_sequence<Indexes...>> {
  using type = typename pick<List<Args...>, Indexes...>::type;
};

template <template <typename...> class List, typename... Args, std::size_t Size>
struct head<List<Args...>, Size, 0, index_sequence<>> {
  using type = List<>;
};

template <template <typename...> class List, typename... Args>
struct head<List<Args...>, 0, 0, index_sequence<>> {
  using type = List<>;
};

template <
  template <typename...> class List,
  typename... Args,
  std::size_t Size,
  std::size_t... Indexes
>
struct head<List<Args...>, Size, Size, index_sequence<Indexes...>> {
  using type = List<Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t Size,
  std::size_t Offset,
  std::size_t... Indexes
>
struct head<Sequence<T, Values...>, Size, Offset, index_sequence<Indexes...>> {
  using type = typename pick<Sequence<T, Values...>, Indexes...>::type;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t Size
>
struct head<Sequence<T, Values...>, Size, 0, index_sequence<>> {
  using type = Sequence<T>;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct head<Sequence<T, Values...>, 0, 0, index_sequence<>> {
  using type = Sequence<T>;
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t Size,
  std::size_t... Indexes
>
struct head<Sequence<T, Values...>, Size, Size, index_sequence<Indexes...>> {
  using type = Sequence<T, Values...>;
};

} // namespace impl_at {
} // namespace fatal {

#undef FATAL_AT_USE_VOID_POINTER_TRICK

#endif // FATAL_INCLUDE_fatal_type_impl_slice_h
