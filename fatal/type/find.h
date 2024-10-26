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
#include <fatal/type/constant.h>
#include <fatal/type/slice.h>

#include <fatal/type/impl/find.h>

namespace fatal {

template <
  typename T,
  typename Key,
  typename Default,
  typename KeyFilter = get_identity,
  typename PostFilter = get_identity
>
using find = typename impl_fnd::f<T>::template apply<
  Key, Default, KeyFilter, PostFilter
>;

template <typename T, typename Key, typename Default>
using pair_find = find<T, Key, Default, get_first, get_second>;

template <typename T, typename Key, typename Default>
using reverse_pair_find = find<T, Key, Default, get_second, get_first>;

template <typename T, typename Key, typename KeyFilter = get_identity>
using contains = find<
  T, Key, std::false_type, KeyFilter, get_constant<std::true_type>
>;

} // namespace fatal {
