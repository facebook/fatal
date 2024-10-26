/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <cstdlib>

namespace fatal {
namespace impl_seq {

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
    ::fatal::make_integer_seq< \
      Sequence, ::std::size_t, ::fatal::impl_seq::size(__VA_ARGS__) \
    > \
  >::type

} // namespace impl_seq {
} // namespace fatal {
