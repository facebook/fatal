/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_select_h
#define FATAL_INCLUDE_fatal_type_select_h

#include <fatal/type/compare.h>

#include <type_traits>

namespace fatal {

template <typename LHS, typename RHS, typename Less = less>
using min = typename std::conditional<
  Less::template apply<RHS, LHS>::value,
  RHS,
  LHS
>::type;

template <typename LHS, typename RHS, typename Less = less>
using max = typename std::conditional<
  Less::template apply<LHS, RHS>::value,
  RHS,
  LHS
>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_select_h
