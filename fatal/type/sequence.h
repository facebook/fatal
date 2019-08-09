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

#include <fatal/portability.h>
#include <fatal/preprocessor.h>
#include <fatal/type/debug.h>

#include <cstdlib>

#include <fatal/type/impl/sequence.h>

namespace fatal {

template <typename T, T... Values>
struct sequence {
  using value_type = T;
};

#if FATAL_HAS_BUILTIN(__make_integer_seq) || _MSC_FULL_VER >= 190023918

template <typename T, std::size_t Size>
using make_sequence = __make_integer_seq<sequence, T, Size>;

#else

template <typename T, std::size_t Size>
using make_sequence =
  typename impl_seq::make<Size>::template apply<sequence<T>, sequence<T, 0>>;

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
    ::fatal::sequence, \
    Id, \
    FATAL_UID(FATAL_CAT(fatal_str_, Id)), \
    FATAL_UID(ArgT), \
    FATAL_UID(ArgV), \
    String \
  )

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_sequence_h
