/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/conditional.h>

namespace fatal {

/**
 * Equivalent to `std::conditional_t`.
 *
 * Implemented with fewer template instantiations, can often be used to improve
 * build times compared to using `std::conditional` or `std::conditional_t`.
 */
template <bool C, typename T, typename F>
using conditional = typename impl_cnd::cond<C>::template apply<T, F>;

} // namespace fatal {
