/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/apply.h>
#include <fatal/type/impl/bitwise.h>

namespace fatal {

/**
 * Yields an std::integral_constant whose value is the bitwise AND of the value
 * of each argument.
 *
 * Example:
 *
 *  template <int... Args>
 *  using all = bitwise_and<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `0`
 *  all<1, 2, 4>::value
 *
 *  // yields `3`
 *  all<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_and = typename impl_bitwise::b_and<Args...>::type;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_and_of = apply_to<T, bitwise_and>;

/**
 * Yields an std::integral_constant whose value is the bitwise OR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <int... Args>
 *  using any = bitwise_or<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `7`
 *  any<1, 2, 4>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_or = typename impl_bitwise::b_or<Args...>::type;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_or_of = apply_to<T, bitwise_or>;

/**
 * Yields an std::integral_constant whose value is the bitwise XOR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <int... Args>
 *  using diff = bitwise_xor<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `3`
 *  diff<1, 2>::value
 *
 *  // yields `12`
 *  diff<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_xor = typename impl_bitwise::b_xor<Args...>::type;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_xor_of = apply_to<T, bitwise_xor>;

/**
 * Yields an std::integral_constant whose value is the bitwise complement of T's
 * value.
 *
 * Example:
 *
 *  // yields `0xf0`
 *  complement<std::integral_constant<std::uint8_t, 0xf>>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
// TODO: REVIEW THIS IMPLEMENTATION
/*
using complement = std::integral_constant<
  typename std::conditional<
    std::is_integral<typename std::decay<decltype(T::value)>::type>::value,
    typename std::decay<decltype(T::value)>::type,
    typename std::decay<decltype(~T::value)>::type
  >::type,
  static_cast<
    typename std::conditional<
      std::is_integral<typename std::decay<decltype(T::value)>::type>::value,
      typename std::decay<decltype(T::value)>::type,
      typename std::decay<decltype(~T::value)>::type
    >::type
  >(~T::value)
>;
*/
using complement = std::integral_constant<decltype(~T::value), ~T::value>;

/**
 * Yields an std::integral_constant whose value is the bitwise AND of the value
 * of each argument.
 *
 * Example:
 *
 *  template <std::uint8_t... Args>
 *  using all = bitwise_nand<
 *    std::integral_constant<std::uint8_t, Args>...
 *  >;
 *
 *  // yields `0b11111100`
 *  all<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_nand = complement<bitwise_and<Args...>>;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_nand_of = apply_to<T, bitwise_nand>;

/**
 * Yields an std::integral_constant whose value is the bitwise OR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <std::uint8_t... Args>
 *  using any = bitwise_nor<
 *    std::integral_constant<std::uint8_t, Args>...
 *  >;
 *
 *  // yields `0b11111000`
 *  any<1, 2, 4>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_nor = complement<bitwise_or<Args...>>;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_nor_of = apply_to<T, bitwise_nor>;

/**
 * Yields an std::integral_constant whose value is the bitwise XOR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <std::uint8_t... Args>
 *  using diff = bitwise_xnor<
 *    std::integral_constant<std::uint8_t, Args>...
 *  >;
 *
 *  // yields `0b11110011`
 *  diff<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using bitwise_xnor = complement<bitwise_xor<Args...>>;

// TODO: DOCUMENT AND TEST
template <typename T>
using bitwise_xnor_of = apply_to<T, bitwise_xnor>;

} // namespace fatal {
