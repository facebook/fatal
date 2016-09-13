/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_add_reference_from_h
#define FATAL_INCLUDE_fatal_type_add_reference_from_h

#include <type_traits>

namespace fatal {

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

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_add_reference_from_h
