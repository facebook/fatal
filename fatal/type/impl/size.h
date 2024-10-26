/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

namespace fatal {
namespace impl_sz {

// size //
template <typename> struct s;

template <template <typename...> class T, typename... Args>
struct s<T<Args...>> {
  using type = std::integral_constant<std::size_t, sizeof...(Args)>;
};

template <typename T, template <typename V, V...> class Sequence, T... Values>
struct s<Sequence<T, Values...>> {
  using type = std::integral_constant<std::size_t, sizeof...(Values)>;
};

} // namespace impl_sz {
} // namespace fatal {
