/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <limits.h>

#include <fatal/type/scalar.h>
#include <fatal/type/sort.h>
#include <fatal/type/unique.h>

namespace fatal {

struct get_data_bits;
struct data_bits_eq;

namespace i_num {

template <typename T>
using data_bits = std::integral_constant<
  std::size_t,
  std::is_same<bool, typename std::decay<T>::type>::value
    ? 1
    : sizeof(typename std::decay<T>::type) * CHAR_BIT
>;

constexpr std::size_t msb_mp_impl(std::uintmax_t Value) noexcept {
  return Value
    ? 1 + msb_mp_impl(Value >> 1)
    : 0;
}

template <std::uintmax_t Value>
using most_significant_bit = size_constant<msb_mp_impl(Value)>;

constexpr std::size_t pop_count_impl(std::uintmax_t Value) noexcept {
  return Value
    ? pop_count_impl(Value & (Value - 1)) + 1
    : 0;
}

template <typename List>
using uniquify_list_by_bit_size_impl = adjacent_unique_by<
  sort_by<List, get_data_bits>,
  data_bits_eq
>;

template <typename T, std::size_t Size>
inline constexpr std::size_t slcu() noexcept {
  static_assert(
    Size + std::is_signed<T>::value <= data_bits<T>::value,
    "value already uses up all the bits available"
  );

  return data_bits<T>::value - Size - std::is_signed<T>::value;
}

template <typename T>
inline constexpr T integral_reverse(
  T value,
  std::size_t end_phase,
  std::size_t phase,
  T mask
) noexcept {
  return phase == end_phase
    ? value
    : integral_reverse<T>(
      ((value >> phase) & mask) | ((value << phase) & ~mask),
      end_phase,
      phase >> 1,
      mask ^ (T(mask << (phase >> 1)))
    );
}

template <
  typename T,
  typename U = typename std::make_unsigned<T>::type,
  std::size_t Phase = ((sizeof(T) * CHAR_BIT) >> 1)
>
inline constexpr T integral_reverse(T value, std::size_t end_phase) noexcept {
  static_assert(sizeof(T) == sizeof(U), "internal error");
  static_assert(!(Phase & (Phase - 1)), "phase must be a power of two");
  static_assert(std::is_integral<T>::value, "only integrals can be reversed");

  return static_cast<T>(
    integral_reverse<U>(
      U(value), end_phase, Phase, U(~U(0)) ^ U(U(~U(0)) << Phase)
    )
  );
}

template <std::size_t BitSizeUpperBound>
struct data_bits_filter {
  template <typename T>
  using apply = bool_constant<
    (most_significant_bit<T::value>::value <= BitSizeUpperBound)
  >;
};

template <std::size_t BitCount>
struct sff {
  template <typename T>
  using apply = bool_constant<
    BitCount <= data_bits<T>::value
  >;
};

template <typename List, std::size_t BitCount>
struct smallest_for_impl {
  using type = first<
    filter<i_num::uniquify_list_by_bit_size_impl<List>, sff<BitCount>>
  >;

  static_assert(
    BitCount <= data_bits<type>::value,
    "there's no known type to hold that many bits"
  );
};

} // namespace i_num {
} // namespace fatal {
