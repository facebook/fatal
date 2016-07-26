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

#include <fatal/type/impl/prefix_tree.h>

namespace fatal {

template <typename T>
using prefix_tree = T;

template <typename T>
using build_prefix_tree = typename impl_trie::rc<T, 0>::type;

} // namespace fatal {

#include <fatal/type/deprecated/type_prefix_tree.h>

#endif // FATAL_INCLUDE_fatal_type_prefix_tree_h
