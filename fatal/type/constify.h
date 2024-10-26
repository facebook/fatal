/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/qualifier.h>

#include <type_traits>

namespace fatal {

/**
 * Adds the const qualifier to a type.
 *
 * If the type is a reference, to some other type `U`,
 * make it the same kind of reference to `U const`.
 *
 * Example:
 *
 *  // yields `int const`
 *  using result1 = constify<int>::type;
 *
 *  // yields `int const &`
 *  using result2 = constify<int &>::type;
 *
 *  // yields `int const &&`
 *  using result3 = constify<int &&>::type;
 *
 *  // yields `int const &`
 *  using result4 = constify<int const &>::type;
 *
 *  // yields `int *const`
 *  using result4 = constify<int *>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct constify {
  using type = add_reference_from_t<
    typename std::decay<T>::type const,
    T
  >;
};

} // namespace fatal
