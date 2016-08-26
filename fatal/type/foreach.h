/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_foreach_h
#define FATAL_INCLUDE_fatal_type_foreach_h

#include <fatal/type/apply.h>

#include <utility>

#include <fatal/type/impl/foreach.h>

namespace fatal {

// TODO: MOVE SOMEWHERE ELSE??
template <typename T, typename Visitor, typename... Args>
static void foreach(Visitor &&visitor, Args &&...args) {
  apply_to<T, impl_fe::fe>::f(
    std::forward<Visitor>(visitor),
    std::forward<Args>(args)...
  );
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_foreach_h
