/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_qualifier_h
#define FATAL_INCLUDE_fatal_type_qualifier_h

#include <type_traits>

namespace fatal {

/**
 * Member function const/volatile qualifiers category.
 *
 * `none`:  neither const nor volatile
 * `c`:     const but not volatile
 * `v`:     volatile but not const
 * `cv`:    both const and volatile (guaranteed
 *          to be the bitwise or of `c` and `v`)
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
enum class cv_qualifier:
  unsigned char
{
  none = 0,
  c = 1,
  v = 2,
  cv = c | v
};

/**
 * Bitwise and (&) operator for `cv_qualifier` to make it easy to
 * check for the presence of either the const or volatile qualifier:
 *
 * Example:
 *
 *  void check_constness(cv_qualifier q) {
 *    if (q & cv_qualifier::c) {
 *      cout << "it is const, dunno about volatile";
 *    }
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
static constexpr bool operator &(cv_qualifier lhs, cv_qualifier rhs) {
  using type = std::underlying_type<cv_qualifier>::type;
  return static_cast<bool>(static_cast<type>(lhs) & static_cast<type>(rhs));
}

enum class ref_qualifier:
  unsigned char
{
  none = 0,
  lvalue = 1,
  rvalue = 2
};

/**
 * Applies `std::add_const` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_const_from<foo, int>::type;
 *
 *  // yields `foo const`
 *  using result2 = add_const_from<foo, int const>::type;
 *
 *  // yields `foo const`
 *  using result3 = add_const_from<foo const, int const>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_const_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_const_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, yield `std::add_lvalue_reference<T>::type`
 * - if `U` is an r-value reference, yield `std::add_rvalue_reference<T>::type`
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_reference_from<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = add_reference_from<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = add_reference_from<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = add_reference_from<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = add_reference_from<foo, int &&>::type;
 *
 *  // yields `foo &`
 *  using result6 = add_reference_from<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_reference_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &> {
  using type = typename std::add_lvalue_reference<T>::type;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<T>::type;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_qualifier_h
