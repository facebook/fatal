/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/cartesian_product.h>

namespace fatal {

/**
 * Computes the cartesian product between two lists.
 *
 * Parameters:
 *
 * Example:
 *
 *  // yields `list<
 *  //   pair<int, double>,
 *  //   pair<int, bool>,
 *  //   pair<void, double>,
 *  //   pair<void, bool>
 *  // >
 *  using result = cartesian_product<
 *    list, pair,
 *    list<int, void>,
 *    list<double, bool>,
 *  >;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  typename LHS, typename RHS
>
using cartesian_product = typename impl_cp::cpe<Inner, Outer<>, LHS, RHS>::type;

namespace bound {

template <
  template <typename...> class Outer,
  template <typename...> class Inner
>
struct cartesian_product {
  template <typename LHS, typename RHS>
  using apply = fatal::cartesian_product<Outer, Inner, LHS, RHS>;
};

} // namespace bound {
} // namespace fatal {
