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

template <typename T, template <typename...> class Transform>
using transform = typename impl_transform::transform<T>
  ::template apply<Transform>;

template <
  typename T,
  template <typename> class KeyTransform,
  template <typename> class ValueTransform = identity
>
using map_transform = typename impl_transform::map_transform<T>
  ::template apply<KeyTransform, ValueTransform>;

} // namespace fatal {

#include <fatal/type/deprecated/transform.h>

#endif // FATAL_INCLUDE_fatal_type_transform_h
