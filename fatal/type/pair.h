/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_pair_h
#define FATAL_INCLUDE_fatal_type_pair_h

#include <fatal/type/debug.h>

namespace fatal {

template <typename, typename>
struct pair {};

template <typename First, typename Second>
struct to_pair {
  template <typename T>
  using apply = pair<
    typename First::template apply<T>,
    typename Second::template apply<T>
  >;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_pair_h
