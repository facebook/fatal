/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <type_traits>

namespace fatal {

template <typename T> struct type_tag { typedef T type; };

template <typename T, std::size_t Index>
struct indexed_type_tag:
  public std::integral_constant<std::size_t, Index>
{
  typedef T type;
};

struct type_not_found_tag {};

} // namespace fatal
