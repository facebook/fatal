/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

namespace fatal {

template <typename...> struct debug_type;

// TODO: DOCUMENT
#define FATAL_DEBUG_TYPE(...) decltype(::fatal::debug_type<__VA_ARGS__>())

} // namespace fatal
