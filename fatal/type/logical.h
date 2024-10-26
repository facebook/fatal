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
#include <fatal/type/sequence.h>

#include <fatal/type/impl/logical.h>

namespace fatal {

/**
 * A metafunction that always evaluates to `std::true_type` regardless of the
 * arguments given.
 *
 * Example:
 *
 *  // yields `std::true_type`
 *  using result1 = tautology::apply<int>;
 *
 *  // yields `std::true_type`
 *  using result2 = tautology::apply<std::false_type>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct tautology {
  template <typename...>
  using apply = std::true_type;
};

/**
 * A metafunction that always evaluates to `std::false_type` regardless of the
 * arguments given.
 *
 * Example:
 *
 *  // yields `std::false_type`
 *  using result1 = contradiction::apply<int>;
 *
 *  // yields `std::false_type`
 *  using result2 = contradiction::apply<std::true_type>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct contradiction {
  template <typename...>
  using apply = std::false_type;
};

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
using logical_and = std::integral_constant<
  bool,
  impl_logical::l_and<Values...>()
>;

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_and_of = apply_to<T, logical_and>;

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
using logical_nor = std::integral_constant<
  bool,
  impl_logical::l_nor<Values...>()
>;

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_nor_of = apply_to<T, logical_nor>;

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

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_or_of = apply_to<T, logical_or>;

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
template <typename... Values>
using logical_xor = std::integral_constant<
  bool,
  impl_logical::l_xor<Values...>()
>;

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_xor_of = apply_to<T, logical_xor>;

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

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_nand_of = apply_to<T, logical_nand>;

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
using logical_xnor = negate<logical_xor<Args...>>;

// TODO: DOCUMENT AND TEST
template <typename T>
using logical_xnor_of = apply_to<T, logical_xnor>;

} // namespace fatal {
