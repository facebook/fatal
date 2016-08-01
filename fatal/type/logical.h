/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_logical_h
#define FATAL_INCLUDE_fatal_type_logical_h

#include <fatal/type/sequence.h>

#include <fatal/type/impl/logical.h>

namespace fatal {

/**
 * Yields an std::integral whose value is the logical negation of T's value.
 *
 * Example:
 *
 *  template <int X>
 *  using is_negative = std::integral_constant<bool, (X < 0)>;
 *
 *  template <int X>
 *  using is_non_negative = negate<is_negative<X>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using negate = std::integral_constant<bool, !T::value>;

template <typename Predicate>
struct negation {
  template <typename T>
  using apply = std::integral_constant<
    bool,
    !Predicate::template apply<T>::value
  >;
};

/**
 * Yields an std::integral_constant whose value is the logical AND of the value
 * of each argument.
 *
 * Example:
 *
 *  template <typename A, typename B, typename C>
 *  using all_equal = logical_and<
 *    std::is_same<A, B>, std::is_same<B, C>
 *  >;
 *
 *  // yields `false`
 *  all_equal<int, bool, double>::value
 *
 *  // yields `false`
 *  all_equal<int, bool, int>::value
 *
 *  // yields `true`
 *  all_equal<int, int, int>::value
 *
 *  template <typename... Args>
 *  struct Foo {
 *    static_assert(
 *      logical_and<std::is_signed<Args>...)::value,
 *      "Args should be signed arithmetic types"
 *    );
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Values>
using logical_and = std::is_same<
  sequence<bool, Values::value...>,
  sequence<bool, impl_logical::b_true<Values>()...>
>;

/**
 * Yields an std::integral_constant whose value is the logical NOR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <bool... Args>
 *  using nor = logical_nor<std::integral_constant<bool, Args>...>;
 *
 *  // yields `false`
 *  nor<true, true, true>::value
 *
 *  // yields `false`
 *  nor<true, false, true>::value
 *
 *  // yields `true`
 *  nor<false, false, false>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Values>
using logical_nor = std::is_same<
  sequence<bool, Values::value...>,
  sequence<bool, impl_logical::b_false<Values>()...>
>;

/**
 * Yields an std::integral_constant whose value is the logical OR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <typename A, typename B, typename C>
 *  using has_duplicate = logical_or<
 *    std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
 *  >;
 *
 *  // yields `false`
 *  has_duplicate<int, bool, double>::value
 *
 *  // yields `true`
 *  has_duplicate<int, bool, int>::value
 *
 *  // yields `true`
 *  has_duplicate<int, int, int>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using logical_or = negate<logical_nor<Args...>>;

/**
 * Yields an std::integral_constant whose value is the logical XOR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <bool... Args>
 *  using xor = logical_xor<std::integral_constant<bool, Args>...>;
 *
 *  // yields `true`
 *  xor<true, true, true>::value
 *
 *  // yields `false`
 *  xor<true, false, true>::value
 *
 *  // yields `false`
 *  xor<false, false, false>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using logical_xor = typename impl_logical::l_xor<Args...>::type;

/**
 * Yields an std::integral_constant whose value is the logical NAND of the value
 * of each argument.
 *
 * Example:
 *
 *  template <bool... Args>
 *  using nand = logical_nand<std::integral_constant<bool, Args>...>;
 *
 *  // yields `false`
 *  nand<true, true, true>::value
 *
 *  // yields `true`
 *  nand<true, false, true>::value
 *
 *  // yields `true`
 *  nand<false, false, false>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using logical_nand = negate<logical_and<Args...>>;

/**
 * Yields an std::integral_constant whose value is the logical XNOR of the value
 * of each argument.
 *
 * Example:
 *
 *  template <bool... Args>
 *  using xnor = logical_xnor<std::integral_constant<bool, Args>...>;
 *
 *  // yields `false`
 *  xnor<true, true, true>::value
 *
 *  // yields `true`
 *  xnor<true, false, true>::value
 *
 *  // yields `true`
 *  xnor<false, false, false>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using logical_nnor = negate<logical_xor<Args...>>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_logical_h
