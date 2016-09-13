/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_type_h
#define FATAL_INCLUDE_fatal_type_type_h

#include <fatal/type/debug.h>

#include <fatal/type/impl/type.h>

namespace fatal {

template <typename T>
using type_of = typename impl_t::to<T>::type;

template <typename T>
using value_type_of = typename impl_t::vto<T>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_type_h
