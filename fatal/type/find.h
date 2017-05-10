/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_find_h
#define FATAL_INCLUDE_fatal_type_find_h

#include <fatal/type/identity.h>

#include <fatal/type/impl/find.h>

namespace fatal {

template <
  typename T,
  typename Key,
  typename Default,
  typename KeyFilter = get_identity,
  typename PostFilter = get_identity
>
using find = typename impl_fnd::f<T, Key, Default, KeyFilter, PostFilter>::type;

template <typename T, typename Key, typename KeyFilter = get_identity>
using contains = typename impl_fnd::c<T, Key, KeyFilter>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_find_h
