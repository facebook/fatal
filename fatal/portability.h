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

///////////////////////
// FATAL_HAS_BUILTIN //
///////////////////////

#if defined(_MSC_VER)
#define FATAL_CPLUSPLUS _MSVC_LANG
#else
#define FATAL_CPLUSPLUS __cplusplus
#endif

/**
 * TODO: DOCUMENT
 */

#if defined(__has_builtin)
# define FATAL_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
# define FATAL_HAS_BUILTIN(...) 0
#endif

// Generalize warning push/pop.
#if defined(__clang__) || defined(__GNUC__)
# define FATAL_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
# define FATAL_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
# define FATAL_GCC_DIAGNOSTIC_IGNORED_INTERNAL2(warningName) #warningName
# define FATAL_GCC_DIAGNOSTIC_IGNORED(warningName) \
  _Pragma(                                      \
  FATAL_GCC_DIAGNOSTIC_IGNORED_INTERNAL2(GCC diagnostic ignored warningName))
#else
# define FATAL_DIAGNOSTIC_PUSH
# define FATAL_DIAGNOSTIC_POP
# define FATAL_GCC_DIAGNOSTIC_IGNORED(warningName)
#endif

#if __GNUC__ && __GNUC__ < 5
# define FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN \
  FATAL_GCC_DIAGNOSTIC_IGNORED("-Wshadow")
#else
# define FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN
#endif

//////////////////////////////
// FATAL_ATTR_ALWAYS_INLINE //
//////////////////////////////

/**
 * TODO: DOCUMENT
 */

#ifdef _MSC_VER
# define FATAL_ATTR_ALWAYS_INLINE __forceinline
#elif __clang__ || __GNUC__
# define FATAL_ATTR_ALWAYS_INLINE __attribute__((always_inline))
#else
# define FATAL_ATTR_ALWAYS_INLINE
#endif

//////////////////////////////////
// FATAL_ATTR_VISIBILITY_HIDDEN //
//////////////////////////////////

/**
 * TODO: DOCUMENT
 */

#ifdef _MSC_VER
# define FATAL_ATTR_VISIBILITY_HIDDEN
#elif __clang__ || __GNUC__
# define FATAL_ATTR_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))
#else
# define FATAL_ATTR_VISIBILITY_HIDDEN
#endif

#endif
