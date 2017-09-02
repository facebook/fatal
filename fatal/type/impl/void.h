/*
 *  Copyright (c) 2017, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_void_h
#define FATAL_INCLUDE_fatal_type_impl_void_h

namespace fatal {
namespace detail {
namespace void_impl {

template <typename...>
struct void_t_ {
  using type = void;
};

} // namespace void_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_void_h
