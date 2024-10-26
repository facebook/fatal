/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/inherit.h>
#include <fatal/type/pair.h>

namespace fatal {
namespace impl_gt {

template <typename Key, typename Value>
Value f(pair<Key, Value>);

template <typename> struct g;

template <template <typename...> class List, typename... Args>
struct g<List<Args...>> {
  template <typename Key, typename KeyFilter, typename PostFilter>
  using apply = typename PostFilter::template apply<decltype(f<Key>(
    inherit<pair<typename KeyFilter::template apply<Args>, Args>...>())
  )>;
};

} // namespace impl_gt {
} // namespace fatal {
