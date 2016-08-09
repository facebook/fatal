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

#ifndef NDEBUG
#include <fatal/type/debug.h>
#endif // NDEBUG

#include <fatal/type/impl/cat.h>

namespace fatal {

// TODO: make it variadic
template <typename... Args>
using cat = typename impl_cat::cat<Args...>::type;

template <typename LHS, typename RHS>
using vcat = impl_cat::vcat<LHS, RHS>;

template <typename LHS, typename RHS, typename... Args>
using lcat = typename impl_cat::lcat<LHS, RHS, Args...>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_cat_h
