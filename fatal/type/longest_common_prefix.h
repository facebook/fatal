/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <cstdlib>

#include <fatal/type/impl/longest_common_prefix.h>

namespace fatal {

template <
  template <typename, std::size_t> class Key,
  std::size_t Begin,
  std::size_t End,
  typename... Args
>
using longest_common_prefix = typename impl_lcp::s<
  true, Key, Begin, End, Args...
>::type;

} // namespace fatal {
