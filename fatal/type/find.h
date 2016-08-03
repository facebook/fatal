/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_find_h
#define FATAL_INCLUDE_fatal_type_find_h

#include <fatal/type/slice.h>

#include <fatal/type/impl/find.h>

namespace fatal {

template <
  typename T,
  typename Key,
  template <typename> class KeyFilter = first,
  typename Default = not_found
>
using find = typename impl_fnd::fnd<T>
  ::template type<Key, KeyFilter, Default>;

template <typename T, typename Key, typename Default = not_found>
using map_find = typename impl_fnd::mfnd<T>
  ::template type<Key, first, second, Default>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_find_h
