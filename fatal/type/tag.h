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

#include <cstddef>

namespace fatal {

template <typename T> struct tag {
  using type = T;
};

template <typename First, typename Second>
using tag_pair = tag<pair<First, Second>>;

template <typename T, std::size_t Index>
struct indexed: tag<T> {
  static constexpr std::size_t value = Index;
};

template <typename T, std::size_t Index>
constexpr std::size_t indexed<T, Index>::value;

template <typename First, typename Second, std::size_t Index>
using indexed_pair = indexed<pair<First, Second>, Index>;

template <typename T>
T tag_type(tag<T> const &);

template <typename T, std::size_t Index>
constexpr std::size_t inline tag_index(indexed<T, Index>) { return Index; }

template <typename First, typename Second>
First tag_first(tag_pair<First, Second> const &);

template <typename First, typename Second>
Second tag_second(tag_pair<First, Second> const &);

struct not_found {};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_tag_h
