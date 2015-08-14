/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_tag_h
#define FATAL_INCLUDE_fatal_type_tag_h

#include <fatal/type/debug.h>

#include <type_traits>

namespace fatal {

template <typename T> struct type_tag { typedef T type; };

template <typename T, std::size_t Index>
struct indexed_tag:
  public std::integral_constant<std::size_t, Index>
{
  typedef T type;

  constexpr static std::size_t index = Index;
};

template <typename T, std::size_t Index>
constexpr std::size_t indexed_tag<T, Index>::index;

// TODO: Deprecate indexed_type_tag
template <typename T, std::size_t Index>
using indexed_type_tag = indexed_tag<T, Index>;

struct type_not_found_tag {};

struct no_metadata_tag {};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_tag_h
