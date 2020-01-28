/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_is_complete_h
#define FATAL_INCLUDE_fatal_type_impl_is_complete_h

#include <type_traits>

namespace fatal {
namespace impl_is_complete {

template <typename T, std::size_t = sizeof(T)>
std::true_type sfinae(T *);

template <typename = void, std::size_t = 0>
std::false_type sfinae(...);

} // namespace impl_is_complete {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_is_complete_h
