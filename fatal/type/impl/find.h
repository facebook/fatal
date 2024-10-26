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
namespace impl_fnd {

// find sfinae //
template <typename, typename Key, typename Value>
static Value sfinae(pair<Key, Value>);

template <typename Default, typename...>
static Default sfinae(...);

// find //
template <typename> struct f;

template <template <typename...> class List, typename... Args>
struct f<List<Args...>> {
  template <
    typename Key,
    typename Default,
    typename KeyFilter,
    typename PostFilter
  >
  using apply = decltype(
    sfinae<Default, Key>(
      inherit<
        pair<
          typename KeyFilter::template apply<Args>,
          typename PostFilter::template apply<Args>
        >...
      >()
    )
  );
};

} // namespace impl_fnd {
} // namespace fatal {
