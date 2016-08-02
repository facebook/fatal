/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_map_h
#define FATAL_INCLUDE_fatal_type_map_h

#include <fatal/type/apply.h>
#include <fatal/type/list.h>
#include <fatal/type/slice.h>
#include <fatal/type/transform.h>

namespace fatal {

template <typename... Pairs>
using map = list<Pairs...>;

template <typename T, template <typename...> class List = list>
using map_keys = transform<list_apply<T, List>, first>;

template <typename T, template <typename...> class List = list>
using map_values = transform<list_apply<T, List>, second>;

} // namespace fatal {

#include <fatal/type/deprecated/type_map.h>

#endif // FATAL_INCLUDE_fatal_type_map_h
