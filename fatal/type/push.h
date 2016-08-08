/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_push_h
#define FATAL_INCLUDE_fatal_type_push_h

#include <fatal/type/impl/push.h>

namespace fatal {

template <typename T>
using push = typename impl_psh::to<T>;

// TODO: REDUCE TEMPLATE INSTANTIATIONS
template <typename T, typename... Args>
using push_back = typename push<T>::template back<Args...>;

// TODO: REDUCE TEMPLATE INSTANTIATIONS
template <typename T, typename... Args>
using push_front = typename push<T>::template front<Args...>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_push_h
