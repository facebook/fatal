/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_arithmetic_h
#define FATAL_INCLUDE_fatal_type_arithmetic_h

#include <fatal/type/impl/arithmetic.h>

namespace fatal {

template <typename... Args>
using add = typename impl_arithmetic::add<Args...>::type;

template <typename... Args>
using subtract = typename impl_arithmetic::sub<Args...>::type;

template <typename... Args>
using multiply = typename impl_arithmetic::mul<Args...>::type;

template <typename... Args>
using divide = typename impl_arithmetic::div<Args...>::type;

template <typename... Args>
using modulo = typename impl_arithmetic::mod<Args...>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_arithmetic_h
