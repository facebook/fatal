/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/portability.h>
#include <fatal/type/sequence.h>
#include <fatal/type/size.h>

#include <fatal/type/impl/foreach.h>

namespace fatal {

template <typename List, typename Visitor, typename... Args>
FATAL_ATTR_ALWAYS_INLINE
constexpr static inline void foreach(Visitor&& visitor, Args&&... args) {
  impl_fe::f<List, make_index_sequence<size<List>::value>>::g(
    static_cast<Visitor &&>(visitor),
    static_cast<Args &&>(args)...
  );
}

} // namespace fatal {
