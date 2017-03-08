/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_portability_h
#define FATAL_INCLUDE_fatal_portability_h

//////////////////////////////
// FATAL_ATTR_ALWAYS_INLINE //
//////////////////////////////

/**
 * TODO: DOCUMENT
 */

#if _MSC_VER
# define FATAL_ATTR_ALWAYS_INLINE __forceinline
#elif __clang__ || __GNUC__
# define FATAL_ATTR_ALWAYS_INLINE inline __attribute__((always_inline))
#else
# define FATAL_ATTR_ALWAYS_INLINE inline
#endif

//////////////////////////////////
// FATAL_ATTR_VISIBILITY_HIDDEN //
//////////////////////////////////

/**
 * TODO: DOCUMENT
 */

#if _MSC_VER
# define FATAL_ATTR_VISIBILITY_HIDDEN
#elif __clang || __GNUC__
# define FATAL_ATTR_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))
#else
# define FATAL_ATTR_VISIBILITY_HIDDEN
#endif

#endif
