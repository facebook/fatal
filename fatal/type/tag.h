/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_tag_h
#define FATAL_INCLUDE_fatal_type_tag_h

#include <fatal/type/pair.h>

#include <type_traits>

namespace fatal {

template <typename T> struct tag {};

template <typename T, std::size_t Index>
struct indexed:
  std::integral_constant<std::size_t, Index>
{
  constexpr inline operator tag<T>() const { return {}; };
};

template <typename First, typename Second, std::size_t Index>
using indexed_pair = indexed<pair<First, Second>, Index>;

struct not_found {};

} // namespace fatal {

#include <fatal/type/deprecated/type_tag.h>

#endif // FATAL_INCLUDE_fatal_type_tag_h
