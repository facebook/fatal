/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_transform_h
#define FATAL_INCLUDE_fatal_type_transform_h

#include <fatal/type/debug.h>

#include <fatal/type/impl/transform.h>

namespace fatal {

template <typename T, typename... Transform>
using transform = typename i_t::t<T, Transform...>::type;

} // namespace fatal {

#include <fatal/type/deprecated/transform.h>

#endif // FATAL_INCLUDE_fatal_type_transform_h
