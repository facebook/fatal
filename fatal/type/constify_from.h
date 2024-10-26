/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/constify.h>

namespace fatal {

///////////////////
// constify_from //
///////////////////

/**
 * Applies `constify` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo const`
 *  using result1 = constify_from<foo, int const>::type;
 *
 *  // yields `foo const &`
 *  using result2 = constify_from<foo &, int const>::type;
 *
 *  // yields `foo &&`
 *  using result3 = constify_from<foo &&, int>::type;
 *
 *  // yields `foo const &`
 *  using result4 = constify_from<foo const &, int const>::type;
 *
 *  // yields `foo const &`
 *  using result5 = constify_from<foo const &, int>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct constify_from {
  using type = T;
};

template <typename T, typename TFrom>
struct constify_from<T, TFrom const> {
  using type = typename constify<T>::type;
};

} // namespace fatal
