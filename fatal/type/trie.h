/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_trie_h
#define FATAL_INCLUDE_fatal_type_trie_h

#include <fatal/functional/no_op.h>
#include <fatal/type/sort.h>

#include <utility>

#include <fatal/type/impl/trie.h>

namespace fatal {

template <
  typename T,
  typename Comparer = less,
  typename Begin,
  typename End,
  typename Visitor,
  typename... VArgs
>
static bool trie_find(
  Begin &&begin,
  End &&end,
  Visitor &&visitor,
  VArgs &&...args
) {
  assert(begin <= end);
  return impl_tr::e<sort<T, sequence_compare<Comparer>>>::type::f(
    static_cast<std::size_t>(std::distance(begin, end)),
    std::forward<Begin>(begin),
    std::forward<Visitor>(visitor),
    std::forward<VArgs>(args)...
  );
}

template <typename T, typename Comparer = less, typename Begin, typename End>
static bool trie_find(Begin &&begin, End &&end) {
  assert(begin <= end);
  return impl_tr::e<sort<T, sequence_compare<Comparer>>>::type::f(
    static_cast<std::size_t>(std::distance(begin, end)),
    std::forward<Begin>(begin),
    fn::no_op()
  );
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_trie_h
