/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_slice_h
#define FATAL_INCLUDE_fatal_type_slice_h

#include <fatal/type/size.h>

#include <cstdint>

#include <fatal/type/impl/slice.h>

namespace fatal {

template <typename T, std::size_t Index>
using at = typename impl_at::at<T, Index>::type;

template <typename T>
using first = at<T, 0>;

template <typename T>
using second = at<T, 1>;

template <typename T>
using third = at<T, 2>;

template <typename T, std::size_t... Indexes>
using pick = typename impl_at::pick<T, Indexes...>::type;

template <typename T, std::size_t Begin, std::size_t End = size<T>::value>
using slice = typename impl_at::slice<T, make_index_interval<Begin, End>>::type;

template <typename T, std::size_t Offset>
using split = slice<T, 0, Offset>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_slice_h
