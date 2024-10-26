/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

#include <fatal/type/impl/compare.h>

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
 *  comparison_transform::equal::apply<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::equal::apply<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::equal::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct equal {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, LHS::value == RHS::value>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, LHS == RHS>;
};

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
 *  comparison_transform::not_equal::apply<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::not_equal::apply<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::not_equal::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct not_equal {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, LHS::value != RHS::value>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, LHS != RHS>;
};

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
 *  comparison_transform::less::apply<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::less::apply<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::less::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, (LHS::value < RHS::value)>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, (LHS < RHS)>;
};

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
 *  comparison_transform::less_equal::apply<A, B>::value
 *
 *  // yields `false`
 *  comparison_transform::less_equal::apply<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::less_equal::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less_equal {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, (LHS::value <= RHS::value)>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, (LHS <= RHS)>;
};

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
 *  comparison_transform::greater::apply<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::greater::apply<B, A>::value
 *
 *  // yields `false`
 *  comparison_transform::greater::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, (LHS::value > RHS::value)>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, (LHS > RHS)>;
};

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
 *  comparison_transform::greater_equal::apply<A, B>::value
 *
 *  // yields `true`
 *  comparison_transform::greater_equal::apply<B, A>::value
 *
 *  // yields `true`
 *  comparison_transform::greater_equal::apply<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater_equal {
  template <typename LHS, typename RHS>
  using apply = std::integral_constant<bool, (LHS::value >= RHS::value)>;

  template <typename T, T LHS, T RHS>
  using vapply = std::integral_constant<bool, (LHS >= RHS)>;
};

// TODO: DOCUMENT REQUIREMENTS OF PREDICATE
//         http://en.cppreference.com/w/cpp/concept/Compare
template <typename Less>
struct sequence_compare {
  template <typename LHS, typename RHS>
  using apply = i_c::sc<Less, LHS, RHS>;
};

struct value_comparer:
  public less
{
  template <typename LHS, typename RHS>
  static constexpr bool less(RHS &&rhs) {
    return LHS::value < static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr bool equal(RHS &&rhs) {
    return LHS::value == static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr bool greater(RHS &&rhs) {
    return LHS::value > static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }
};

struct value_reverse_comparer:
  public greater
{
  template <typename LHS, typename RHS>
  static constexpr bool less(RHS &&rhs) {
    return LHS::value > static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr bool equal(RHS &&rhs) {
    return LHS::value == static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr bool greater(RHS &&rhs) {
    return LHS::value < static_cast<
      typename std::decay<decltype(LHS::value)>::type
    >(rhs);
  }
};

} // namespace fatal {
