/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/debug.h>

#include <fatal/type/impl/cat.h>

namespace fatal {

// TODO: make it variadic
template <typename... Args>
using cat = typename impl_cat::c<Args...>::type;

template <typename LHS, typename RHS>
using vcat = impl_cat::v<LHS, RHS>;

template <typename LHS, typename RHS, typename... Args>
using lcat = typename impl_cat::l<LHS, RHS, Args...>::type;

} // namespace fatal {
