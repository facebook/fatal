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

#if __cpp_lib_integer_sequence >= 201304 || _MSC_VER
#define FATAL_SEQUENCE_HAS_STD_INTEGER_SEQUENCE 1
#else
#define FATAL_SEQUENCE_HAS_STD_INTEGER_SEQUENCE 0
#endif

#if FATAL_SEQUENCE_HAS_STD_INTEGER_SEQUENCE && ( \
      (__clang_major__ >= 3 && __clang_minor__ >= 8) || \
      _MSC_FULL_VER >= 190023918 || \
      0 \
    )
#define FATAL_SEQUENCE_HAS_INTRINSIC_MAKE_INTEGER_SEQ 1
#else
#define FATAL_SEQUENCE_HAS_INTRINSIC_MAKE_INTEGER_SEQ 0
#endif

namespace fatal {

#if FATAL_SEQUENCE_HAS_STD_INTEGER_SEQUENCE

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

#if FATAL_SEQUENCE_HAS_STD_INTEGER_SEQUENCE

#if FATAL_SEQUENCE_HAS_INTRINSIC_MAKE_INTEGER_SEQ

template <typename T, std::size_t Size>
using make_sequence = __make_integer_seq<sequence, T, Size>;

#else

template <typename T, std::size_t Size>
using make_sequence = typename impl_seq::make<T, Size>::type;

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
