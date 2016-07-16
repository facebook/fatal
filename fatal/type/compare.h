/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_compare_h
#define FATAL_INCLUDE_fatal_type_compare_h

#include <type_traits>

namespace fatal {

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the EQUAL TO
 * comparison, figuratively `LHS` == `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `false`
 *  comparison_transform::equal<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::equal<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::equal<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using equal = std::integral_constant<bool, LHS::value == RHS::value>;

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the NOT EQUAL TO
 * comparison, figuratively `LHS` != `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `true`
 *  comparison_transform::not_equal<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::not_equal<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::not_equal<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using not_equal = std::integral_constant<bool, LHS::value != RHS::value>;

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the LESS THAN
 * comparison, figuratively `LHS` < `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `true`
 *  comparison_transform::less_than<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::less_than<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::less_than<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using less_than = std::integral_constant<bool, (LHS::value < RHS::value)>;

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the LESS THAN or
 * EQUAL TO comparison, figuratively `LHS` <= `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `true`
 *  comparison_transform::less_than_equal<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::less_than_equal<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::less_than_equal<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using less_than_equal = std::integral_constant<
  bool,
  (LHS::value <= RHS::value)
>;

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the GREATER THAN
 * comparison, figuratively `LHS` > `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `false`
 *  comparison_transform::greater_than<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::greater_than<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::greater_than<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using greater_than = std::integral_constant<bool, (LHS::value > RHS::value)>;

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `LHS` and `RHS`. Its boolean value is the result of the GREATER THAN or
 * EQUAL TO comparison, figuratively `LHS` >= `RHS`.
 *
 * Example:
 *
 *  using A = std::integral_constant<int, 10>;
 *  using B = std::integral_constant<int, 20>;
 *
 *  // yields `false`
 *  comparison_transform::greater_than_equal<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::greater_than_equal<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::greater_than_equal<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename LHS, typename RHS>
using greater_than_equal = std::integral_constant<
  bool,
  (LHS::value >= RHS::value)
>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_compare_h
