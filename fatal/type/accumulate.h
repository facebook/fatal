/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/accumulate.h>

namespace fatal {

template <typename T, typename Seed, typename Fn = i_A::f>
using accumulate = typename i_A::F<Seed, Fn, T>::type;

} // namespace fatal {
