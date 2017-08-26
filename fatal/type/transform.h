/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_transform_h
#define FATAL_INCLUDE_fatal_type_transform_h

#include <fatal/type/identity.h>

#include <fatal/type/impl/transform.h>

namespace fatal {

template <typename T, typename... Transform>
using transform = typename i_t::t<T, Transform...>::type;

template <
  typename T,
  typename Predicate,
  typename WhenTrue,
  typename WhenFalse = get_identity
>
using transform_if = transform<T, i_t::c<Predicate, WhenTrue, WhenFalse>>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_transform_h
