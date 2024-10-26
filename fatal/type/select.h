/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/compare.h>

#include <type_traits>

#include <fatal/type/impl/select.h>

namespace fatal {

template <typename LHS, typename RHS, typename Less = less>
using min = impl_sl::min<LHS, RHS, Less>;

template <typename LHS, typename RHS, typename Less = less>
using max = impl_sl::max<LHS, RHS, Less>;

template <typename Less, typename... Args>
using vmin = typename impl_sl::vn<Less, Args...>::type;

template <typename Less, typename... Args>
using vmax = typename impl_sl::vx<Less, Args...>::type;

template <typename Less, typename T, template <typename...> class... Filter>
using lmin = typename impl_sl::ln<Less, T, Filter...>::type;

template <typename Less, typename T, template <typename...> class... Filter>
using lmax = typename impl_sl::lx<Less, T, Filter...>::type;

} // namespace fatal {
