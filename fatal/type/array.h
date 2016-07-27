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
using as_array = typename impl_arr::arr<Array, T...>::type;

template <typename Array, typename... T>
constexpr typename impl_arr::zdt<Array, T...>::type::value_type const *z_data()
{ return impl_arr::zdt<Array, T...>::type::get.data(); }

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_array_h
