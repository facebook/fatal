/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_sequence_h
#define FATAL_INCLUDE_fatal_type_impl_sequence_h

namespace fatal {
namespace impl_seq {

template <typename...> struct cat;

template <typename T, T... V0, T... V1, T... Tail>
struct cat<T, sequence<T, V0...>, sequence<T, V1...>, sequence<T, Tail...>> {
  using type = sequence<
    T,
    V0...,
    (sizeof...(V0) + V1)...,
    (sizeof...(V0) + sizeof...(V1) + Tail)...
  >;
};

template <typename T, std::size_t Size>
struct make {
  using type = typename cat<
    T,
    typename make<T, Size / 2>::type,
    typename make<T, Size / 2>::type,
    typename make<T, Size % 2>::type
  >::type;
};

template <typename T> struct make<T, 1> { using type = sequence<T, 0>; };
template <typename T> struct make<T, 0> { using type = sequence<T>; };

template <typename T, T Offset, typename> struct offset;

template <typename T, T Offset, T... Values>
struct offset<T, Offset, sequence<T, Values...>> {
  using type = sequence<T, (Offset + Values)...>;
};

template <typename T, std::size_t Size>
static constexpr std::size_t size(T const (&)[Size]) {
  static_assert(
    Size > 0,
    "expecting a string containing at least the null terminator"
  );
  return Size - 1;
}

#define FATAL_IMPL_BUILD_STRING(Id, Helper, Indexes, ...) \
  template <::std::size_t... Indexes> \
  static ::fatal::sequence< \
    typename ::std::decay<decltype(*(__VA_ARGS__))>::type, \
    (__VA_ARGS__)[Indexes]... \
  > Helper(::fatal::index_sequence<Indexes...>); \
  \
  using Id = decltype(Helper( \
    ::fatal::make_index_sequence<::fatal::impl_seq::size(__VA_ARGS__)>() \
  ))

} // namespace impl_seq {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_sequence_h
