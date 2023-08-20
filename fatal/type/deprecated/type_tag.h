/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_deprecated_type_tag_h
#define FATAL_INCLUDE_fatal_type_deprecated_type_tag_h

#include <fatal/portability.h>
#include <type_traits>

namespace fatal {

template <typename T>
struct type_tag {
  using type = T;
};

template <typename T, std::size_t Index>
struct indexed_type_tag:
  public std::integral_constant<std::size_t, Index>
{
  using type = T;

  constexpr static std::size_t index = Index;
};

#if FATAL_CPLUSPLUS < 201703L
template <typename T, std::size_t Index>
constexpr std::size_t indexed_type_tag<T, Index>::index;
#endif

struct type_not_found_tag {};
struct no_metadata_tag {};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_deprecated_type_tag_h
