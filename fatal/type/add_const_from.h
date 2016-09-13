/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_add_const_from_h
#define FATAL_INCLUDE_fatal_type_add_const_from_h

#include <type_traits>

namespace fatal {

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

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_add_const_from_h
