/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_string_lookup_h
#define FATAL_INCLUDE_fatal_type_string_lookup_h

#include <fatal/type/prefix_tree.h>

namespace fatal {

// TODO: DOCUMENT AND TEST
template <template <typename...> class Comparer, typename... Strings>
using custom_string_lookup = typename build_type_prefix_tree<Comparer>
  ::template apply<Strings...>;

// TODO: DOCUMENT
template <typename... Strings>
using string_lookup = custom_string_lookup<Strings...>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_string_lookup_h
