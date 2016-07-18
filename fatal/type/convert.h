/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_convert_h
#define FATAL_INCLUDE_fatal_type_convert_h

#include <type_traits>

#include <fatal/type/impl/convert.h>

namespace fatal {

template <template <typename V, V...> class Sequence, typename... Args>
using sequence_from = typename impl_conv::seq<Sequence, Args...>::type;

template <template <typename...> class List, typename T>
using list_from = typename impl_conv::list<List, T>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_convert_h
