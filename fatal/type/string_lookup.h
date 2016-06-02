/*
 *  Copyright (c) 2015, Facebook, Inc.
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
template <template <typename...> class Comparer>
class custom_string_lookup {
  using impl = build_type_prefix_tree<Comparer>;

public:
  // TODO: DOCUMENT AND TEST
  template <typename... Strings>
  using apply = typename impl::template apply<Strings...>;
};

// TODO: DOCUMENT
template <typename... Args>
using string_lookup = fatal::build_type_prefix_tree<>::from<
  Args...
>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_string_lookup_h
