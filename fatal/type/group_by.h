/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_group_by_h
#define FATAL_INCLUDE_fatal_type_group_by_h

#include <fatal/type/impl/group_by.h>

namespace fatal {

template <typename T, template <typename> class Key>
using group_by = typename impl_gby::group<Key, T>::type;

template <
  typename T,
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping
>
using filtered_group_by = typename impl_gby::filtered<
  Key,
  RejectPredicate,
  RejectedGrouping,
  T
>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_group_by_h
