/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_get_h
#define FATAL_INCLUDE_fatal_type_get_h

#include <fatal/type/identity.h>
#include <fatal/type/slice.h>

#include <fatal/type/impl/get.h>

namespace fatal {

template <
  typename T,
  typename Key,
  typename KeyFilter = get_first,
  typename PostFilter = get_identity
>
using get = typename impl_gt::g<T, Key, KeyFilter, PostFilter>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_get_h
