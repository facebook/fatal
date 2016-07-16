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

template <typename...> struct slice;

template <typename T, std::size_t... Indexes>
struct slice<T, index_sequence<Indexes...>> {
  using type = typename pick<T, Indexes...>::type;
};

} // namespace impl_at {
} // namespace fatal {

#undef FATAL_AT_USE_VOID_POINTER_TRICK

#endif // FATAL_INCLUDE_fatal_type_impl_slice_h
