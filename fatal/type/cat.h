/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_cat_h
#define FATAL_INCLUDE_fatal_type_cat_h

#include <fatal/type/impl/cat.h>

namespace fatal {

// TODO: make it variadic
template <typename LHS, typename RHS>
using cat = typename impl_cat::cat<LHS, RHS>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_cat_h
