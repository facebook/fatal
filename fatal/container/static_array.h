/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_static_array_h
#define FATAL_INCLUDE_fatal_container_static_array_h

#include <fatal/container/impl/static_array.h>

namespace fatal {

template <typename Array, typename Factory, typename... T>
using static_array = impl_sa::sa<Array, Factory, T...>;

template <typename Array, typename... T>
using static_z_array = static_array<Array, impl_sa::zd, T...>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_static_array_h
