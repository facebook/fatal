/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_transform_h
#define FATAL_INCLUDE_fatal_type_impl_transform_h

#include <fatal/type/pair.h>

namespace fatal {
namespace impl_transform {

template <typename> struct transform;

template <template <typename...> class T, typename... Args>
struct transform<T<Args...>> {
  template <template <typename> class Transform>
  using apply = T<Transform<Args>...>;
};

template <typename> struct map_transform;

template <template <typename...> class T, typename... Args>
struct map_transform<T<Args...>> {
  template <
    template <typename> class KeyTransform,
    template <typename> class ValueTransform
  >
  using apply = T<
    pair<
      KeyTransform<first<Args>>,
      ValueTransform<second<Args>>
    >...
  >;
};

} // namespace impl_transform {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_transform_h
