/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/functional/no_op.h>
#include <fatal/type/identity.h>
#include <fatal/type/sort.h>

#include <fatal/type/impl/trie.h>

namespace fatal {

// TODO: INVERT COMPARER AND FILTER
template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = less,
  typename Begin,
  typename End,
  typename Visitor,
  typename... VArgs
>
static inline bool trie_find(
  Begin &&begin,
  End &&end,
  Visitor &&visitor,
  VArgs &&...args
) {
  assert(begin <= end);
  return i_t::e<Filter, sort<T, sequence_compare<Comparer>, Filter>>::type::f(
    static_cast<std::size_t>(std::distance(begin, end)),
    static_cast<Begin &&>(begin),
    static_cast<Visitor &&>(visitor),
    static_cast<VArgs &&>(args)...
  );
}

template <
  typename T,
  typename Filter = get_identity,
  typename Comparer = less,
  typename Begin,
  typename End
>
static inline bool trie_find(Begin &&begin, End &&end) {
  return trie_find<T, Filter, Comparer>(
    static_cast<Begin &&>(begin),
    static_cast<End &&>(end),
    fn::no_op()
  );
}

} // namespace fatal {
