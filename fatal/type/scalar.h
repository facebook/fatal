/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_scalar_h
#define FATAL_INCLUDE_fatal_type_scalar_h

#include <type_traits>

namespace fatal {

// TODO: DOCUMENT AND TEST
template <bool Value>
using bool_constant = std::integral_constant<bool, Value>;

// TODO: DOCUMENT AND TEST
template <std::size_t Value>
using size_constant = std::integral_constant<std::size_t, Value>;

template <typename T>
using size_of_constant = std::integral_constant<std::size_t, sizeof(T)>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_scalar_h
