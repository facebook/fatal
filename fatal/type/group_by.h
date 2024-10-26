/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/list.h>
#include <fatal/type/pair.h>

#include <fatal/type/impl/group_by.h>

namespace fatal {

// O(n) on the size of the variadic template T
// `Group` is guaranteed to be instantiated exactly once per group
template <
  typename T,
  typename Key,
  template <typename...> class Group = list,
  template <typename...> class Outer = list
>
using group_by = typename i_g::G<T, Key, Group, Outer>::type;

// O(n) on the size of the variadic template T
// `Pair` is guaranteed to be instantiated exactly once
// `Group` is guaranteed to be instantiated exactly once per group
template <
  typename T,
  typename Key,
  typename Filter,
  template <typename...> class Pair = pair,
  template <typename...> class Group = list
>
using filtered_group_by = typename i_g::F<T, Key, Filter, Pair, Group>::type;

} // namespace fatal {
