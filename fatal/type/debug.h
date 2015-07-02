/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_debug_h
#define FATAL_INCLUDE_fatal_type_debug_h

#include <fatal/preprocessor.h>

namespace fatal {

template <typename...> struct debug_type;

// TODO: DOCUMENT
#define FATAL_DEBUG_TYPE(...) decltype(::fatal::debug_type<__VA_ARGS__>())

// TODO: DOCUMENT
#define FATAL_DEBUG_TYPE_OF(...) FATAL_DEBUG_TYPE(decltype(__VA_ARGS__))

// TODO: DOCUMENT
#define FATAL_DEBUG_TYPE_STMT(...) \
  ::fatal::debug_type<__VA_ARGS__> FATAL_UID(debug_type); \

// TODO: DOCUMENT
#define FATAL_DEBUG_TYPE_OF_STMT(...) \
  FATAL_DEBUG_TYPE_STMT(decltype(__VA_ARGS__))

///////////////////
// debug_type_if //
///////////////////

// TODO: DOCUMENT
template <bool TCondition, typename... T>
struct debug_type_if {
  static_assert(
    TCondition, "there's a bug in fatal, no debug should be generated"
  );

  using type = FATAL_DEBUG_TYPE(T...);
};

template <typename... T>
struct debug_type_if<false, T...> {};

///////////////////////
// debug_type_unless //
///////////////////////

// TODO: DOCUMENT
template <bool TCondition, typename... T>
using debug_type_unless = debug_type_if<!TCondition, T...>;

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_debug_h
