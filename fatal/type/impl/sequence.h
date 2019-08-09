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

#include <cstdlib>

namespace fatal {
namespace impl_seq {

template <typename...> struct cat;

template <
  template <typename T, T...> class Sequence,
  typename T, T... V0, T... V1, T... Tail
>
struct cat<Sequence<T, V0...>, Sequence<T, V1...>, Sequence<T, Tail...>> {
  using type = Sequence<
    T,
    V0...,
    (sizeof...(V0) + V1)...,
    (sizeof...(V0) + sizeof...(V1) + Tail)...
  >;
};

template <std::size_t Size>
struct make {
  template <typename S0, typename S1>
  using apply = typename cat<
    typename make<Size / 2>::template apply<S0, S1>,
    typename make<Size / 2>::template apply<S0, S1>,
    typename make<Size % 2>::template apply<S0, S1>
  >::type;
};

template <> struct make<1> {
  template <typename S0, typename S1>
  using apply = S1;
};
template <> struct make<0> {
  template <typename S0, typename S1>
  using apply = S0;
};

template <
  template <typename T, T...> class Sequence, typename T, std::size_t Size
>
using make_sequence = typename make<Size>::template apply<
  Sequence<T>, Sequence<T, 0>
>;

template <typename T, T Offset, typename> struct offset;

template <
  template <typename T, T...> class Sequence,
  typename T, T Offset, T... Values
>
struct offset<T, Offset, Sequence<T, Values...>> {
  using type = Sequence<T, (Offset + Values)...>;
};

template <typename T, std::size_t Size>
static constexpr std::size_t size(T const (&)[Size]) {
  static_assert(
    Size > 0,
    "expecting a string containing at least the null terminator"
  );
  return Size - 1;
}

#define FATAL_IMPL_BUILD_STRING(Sequence, Id, Helper, ArgT, ArgV, ...) \
  template <typename...> struct Helper; \
  template <typename ArgT, ArgT... ArgV> \
  struct Helper<Sequence<ArgT, ArgV...>> { \
    using char_type = typename ::std::decay<decltype(*(__VA_ARGS__))>::type; \
    using type = Sequence<char_type, (__VA_ARGS__)[ArgV]...>; \
  }; \
  using Id = typename Helper< \
    ::fatal::impl_seq::make_sequence< \
      Sequence, ::std::size_t, ::fatal::impl_seq::size(__VA_ARGS__) \
    > \
  >::type

} // namespace impl_seq {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_sequence_h
