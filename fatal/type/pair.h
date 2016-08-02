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

#include <fatal/type/list.h>

namespace fatal {

template <typename, typename>
struct pair {};

} // namespace fatal {

#include <fatal/type/deprecated/type_pair.h>

#endif // FATAL_INCLUDE_fatal_type_pair_h
