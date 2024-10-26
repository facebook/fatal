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

#include <type_traits>

#include <fatal/type/impl/size.h>

namespace fatal {

template <typename T>
using size = typename impl_sz::s<T>::type;

template <typename T>
using empty = std::integral_constant<bool, size<T>::value == 0>;

} // namespace fatal {
