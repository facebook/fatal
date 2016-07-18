/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_sort_h
#define FATAL_INCLUDE_fatal_type_sort_h

//#include <fatal/type/compare.h>

#include <fatal/type/impl/sort.h>

namespace fatal {

// TODO: ACCEPT CUSTOM COMPARER
template  <typename LHS, typename RHS>
using merge = typename impl_sort::mentry<LHS, RHS>::type;

// TODO: ACCEPT CUSTOM COMPARER
template <typename T>
using sort = typename impl_sort::msort<T>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_sort_h
