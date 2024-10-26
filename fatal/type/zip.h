/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/debug.h>

#include <fatal/type/impl/zip.h>

namespace fatal {

template <
  template <typename...> class Outer,
  template <typename...> class Inner,
  typename... L
>
using zip = typename impl_zp::zp<Outer, Inner, L...>::type;

} // namespace fatal {
