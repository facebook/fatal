/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/pair.h>
#include <fatal/type/slice.h>

namespace fatal {

template <typename T, std::size_t Offset>
using split = pair<head<T, Offset>, tail<T, Offset>>;

} // namespace fatal {
