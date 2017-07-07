/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_sequence_h
#define FATAL_INCLUDE_fatal_type_sequence_h

#include <utility>

#include <fatal/type/debug.h>

namespace fatal {

#if __cpp_lib_integer_sequence || _MSC_VER

template <typename T, T... Ts>
using sequence = std::integer_sequence<T, Ts...>;

#else

template <typename T, T...>
struct sequence;

#endif

} // namespace fatal {

#include <cstdlib>

#include <fatal/type/impl/sequence.h>

namespace fatal {

#if __cpp_lib_integer_sequence || _MSC_VER

#if __GNUC__ && __GNUC__ < 6 && !__clang__

// GCC < 6 has inefficient, linearly recursive std::make_integer_sequence; avoid
template <typename T, std::size_t Size>
using make_sequence = typename impl_seq::make<T, Size>::type;

#else

template <typename T, std::size_t Size>
using make_sequence = std::make_integer_sequence<T, Size>;

#endif

#else

template <typename T, T... Values>
struct sequence {
  using value_type = T;
};

template <typename T, std::size_t Size>
using make_sequence = typename impl_seq::make<T, Size>::type;

#endif

template <typename T, T Begin, T End>
using make_interval = typename impl_seq::offset<
  T, Begin, make_sequence<T, End - Begin>
>::type;

template <std::size_t... Values>
using index_sequence = sequence<std::size_t, Values...>;

template <std::size_t Size>
using make_index_sequence = make_sequence<std::size_t, Size>;

template <std::size_t Begin, std::size_t End>
using make_index_interval = make_interval<std::size_t, Begin, End>;

template <bool... Values>
using bool_sequence = sequence<bool, Values...>;

template <char... Values>
using char_sequence = sequence<char, Values...>;

template <int... Values>
using int_sequence = sequence<int, Values...>;

#define FATAL_S(Id, String) \
  FATAL_IMPL_BUILD_STRING( \
    Id, \
    FATAL_UID(FATAL_CAT(fatal_str_, Id)), \
    FATAL_UID(Indexes), \
    String \
  )

} // namespace fatal {

#include <fatal/type/deprecated/constant_sequence.h>

#endif // FATAL_INCLUDE_fatal_type_sequence_h
