/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_get_h
#define FATAL_INCLUDE_fatal_type_get_h

#include <fatal/type/slice.h>

#include <fatal/type/impl/get.h>

namespace fatal {

template <typename T, typename Key, template <typename> class KeyFilter = first>
using get = typename impl_gt::gt<KeyFilter, T, Key>::type;

template <typename T, typename Key>
using map_get = second<get<T, Key>>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_get_h
