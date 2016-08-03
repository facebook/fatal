/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_find_h
#define FATAL_INCLUDE_fatal_type_impl_find_h

#include <fatal/type/inherit.h>
#include <fatal/type/pair.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace impl_fnd {

template <typename Default, typename Key, typename Value>
static Value sfinae(pair<Key, Value>);

template <typename Default, typename...>
static Default sfinae(...);

template <typename> struct fnd;

template <template <typename...> class List, typename... Args>
struct fnd<List<Args...>> {
  template <typename Key, template <typename> class KeyFilter, typename Default>
  using type = decltype(
    sfinae<Default, Key>(
      inherit<pair<KeyFilter<Args>, Args>...>()
    )
  );
};

template <typename> struct mfnd;

template <template <typename...> class List, typename... Args>
struct mfnd<List<Args...>> {
  template <
    typename Key,
    template <typename> class KeyFilter,
    template <typename> class ValueTransform,
    typename Default
  >
  using type = decltype(
    sfinae<Default, Key>(
      inherit<pair<KeyFilter<Args>, ValueTransform<Args>>...>()
    )
  );
};

} // namespace impl_fnd {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_find_h
