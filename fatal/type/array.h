/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_array_h
#define FATAL_INCLUDE_fatal_type_array_h

#include <fatal/type/impl/array.h>

namespace fatal {

template <typename Array, typename... T>
using as_array = impl_a::ca<Array, T...>;

template <typename Array, typename... T>
constexpr decltype(impl_a::z<Array, T...>::get.data()) z_data() {
  return impl_a::z<Array, T...>::get.data();
}

template <typename Array, typename Factory, typename... T>
using as_array_from = impl_a::sa<Array, Factory, T...>;

template <typename Array, typename... T>
using z_array = as_array_from<Array, impl_a::zd, T...>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_array_h
