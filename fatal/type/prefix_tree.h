/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_prefix_tree_h
#define FATAL_INCLUDE_fatal_type_prefix_tree_h

#include <fatal/type/list.h>
#include <fatal/type/sort.h>

#include <utility>

#include <fatal/type/impl/prefix_tree.h>

namespace fatal {

template <typename T /* TODO: CUSTOM COMPARER*/>
struct prefix_tree {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static bool find(
    Begin &&begin,
    End &&end,
    Visitor &&visitor,
    Args &&...args
  ) {
    bool found = false;
    impl_trie::frc<0, impl_trie::build<sort<T, sequence_compare<less>>>>::f(
      found,
      std::forward<Begin>(begin),
      std::forward<End>(end),
      std::forward<Visitor>(visitor),
      std::forward<Args>(args)...
    );
    return found;
  }

  template <typename Begin, typename End>
  static bool find(Begin &&begin, End &&end) {
    return find(
      std::forward<Begin>(begin),
      std::forward<End>(end),
      fn::no_op()
    );
  }
};

} // namespace fatal {

#include <fatal/type/deprecated/type_prefix_tree.h>

#endif // FATAL_INCLUDE_fatal_type_prefix_tree_h
