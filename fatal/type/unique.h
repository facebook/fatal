/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_unique_h
#define FATAL_INCLUDE_fatal_type_unique_h

#include <fatal/type/impl/unique.h>

namespace fatal {

template <typename T>
using adjacent_unique = typename impl_uq::au<T>::type;

template <typename T, typename Comparer>
using adjacent_unique_by = typename impl_uq::au<T, Comparer>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_unique_h
