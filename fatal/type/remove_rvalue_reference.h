/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {

/**
 * Removes any r-value references from a given type.
 * L-value references remain untouched.
 *
 * Example:
 *
 *  // yields `int`
 *  using result1 = remove_rvalue_reference<int &&>::type;
 *
 *  // yields `int const`
 *  using result2 = remove_rvalue_reference<int const &&>::type;
 *
 *  // yields `int *`
 *  using result3 = remove_rvalue_reference<int *>::type;
 *
 *  // yields `int *&`
 *  using result4 = remove_rvalue_reference<int *&>::type;
 *
 *  // yields `int const &`
 *  using result5 = remove_rvalue_reference<int const &>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct remove_rvalue_reference {
  using type = T;
};

template <typename T>
struct remove_rvalue_reference<T &&> {
  using type = T;
};

template <typename T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

} // namespace fatal
