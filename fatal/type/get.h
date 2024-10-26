/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

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
using get = typename impl_gt::g<T>::template apply<Key, KeyFilter, PostFilter>;

template <typename T, typename Key>
using pair_get = get<T, Key, get_first, get_second>;

template <typename T, typename Key>
using reverse_pair_get = get<T, Key, get_second, get_first>;

} // namespace fatal {
