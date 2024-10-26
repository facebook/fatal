/*
 *  Copyright (c) 2017, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/void.h>

namespace fatal {

////////////
// void_t //
////////////

/**
 * A type alias for `void`. Useful in sfinae-decided template specialization.
 *
 * Example:
 *
 *  template <typename T, typename = void>
 *  struct has_type : std::false_type {};
 *
 *  template <typename T>
 *  struct has_type<T, void_t<typename T::type>> : std::true_type {};
 *
 *  static_assert(!has_type<int>::value);
 *  static_assert(has_type<std::decay<int>>::value);
 */
template <typename... Ts>
using void_t = typename detail::void_impl::void_t_<Ts...>::type;

} // namespace fatal {
