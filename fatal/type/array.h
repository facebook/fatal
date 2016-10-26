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
using as_array = typename impl_a::ca<Array, T...>::type;

template <typename Array, typename... T>
static constexpr typename std::decay<
  decltype(impl_a::z<Array, T...>::data)
>::type z_data() {
  return impl_a::z<Array, T...>::data;
}

template <typename Array, typename Factory, typename... T>
using as_array_from = typename impl_a::sa<
  impl_a::c, Array, Factory, T...
>::type;

template <typename Array, typename Factory, typename... T>
using as_runtime_array_from = typename impl_a::sa<
  impl_a::n, Array, Factory, T...
>::type;

template <typename Array, typename... T>
using z_array = as_array_from<Array, impl_a::zd, T...>;

template <typename Array, typename StringView>
using string_view_array = as_array_from<
  Array,
  impl_a::s<StringView>,
  StringView
>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_array_h
