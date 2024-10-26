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

namespace fatal {

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, turn `T` into an l-value reference
 * - if `U` is an r-value reference, turn `T` into an r-value reference
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = same_reference_as<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = same_reference_as<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = same_reference_as<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = same_reference_as<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = same_reference_as<foo, int &&>::type;
 *
 *  // yields `foo &&`
 *  using result6 = same_reference_as<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct same_reference_as {
  using type = T;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &> {
  using type = typename std::add_lvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

} // namespace fatal
