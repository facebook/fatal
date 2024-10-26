/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/inherit.h>

#include <type_traits>

namespace fatal {
namespace impl_logical {

// TODO: after C++17, switch to param-pack folds

template <typename... Values>
static constexpr bool l_and() {
  using _ = int[];
  bool ret = true;
  void(_{0, (void(ret = ret && Values::value), 0)...});
  return ret;
}

template <typename... Values>
static constexpr bool l_nor() {
  using _ = int[];
  bool ret = true;
  void(_{0, (void(ret = ret && !Values::value), 0)...});
  return ret;
}

template <typename... Values>
static constexpr bool l_xor() {
  using _ = int[];
  bool ret = false;
  void(_{0, (void(ret = ret ^ Values::value), 0)...});
  return ret;
}

} // namespace impl_logical {
} // namespace fatal {
