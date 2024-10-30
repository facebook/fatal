/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <memory>

#include <cstdint>

namespace fatal {
namespace detail {

std::uintptr_t prevent_optimization = 0;

} // namespace detail {

template <typename T>
inline void prevent_optimization(T const &what) {
  detail::prevent_optimization |= reinterpret_cast<
    decltype(detail::prevent_optimization)
  >(std::addressof(what));
}

} // namespace fatal {
