/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_list_h
#define FATAL_INCLUDE_fatal_type_list_h

namespace fatal {

template <typename...> struct list {};

} // namespace fatal {

#include <fatal/type/deprecated/type_list.h>

#endif // FATAL_INCLUDE_fatal_type_list_h
