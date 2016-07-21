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

#include <fatal/type/impl/select.h>

namespace fatal {

template <typename LHS, typename RHS, template <typename...> class Less = less>
using min = typename impl_select::min<!Less<LHS, RHS>::value, RHS, LHS>::type;

template <typename LHS, typename RHS, template <typename...> class Less = less>
using max = typename impl_select::min<Less<LHS, RHS>::value, RHS, LHS>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_select_h
