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

#include <fatal/type/deprecated/type_list.h>

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

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_qualifier_h
