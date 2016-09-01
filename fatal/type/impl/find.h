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

// not found //
struct n {};

// find sfinae //
template <typename, typename T>
static T sfinae(tag<T>);

template <typename, typename Key, typename Value>
static Value sfinae(pair<Key, Value>);

template <typename Default, typename...>
static Default sfinae(...);

// find //
template <typename...> struct f;

template <
  template <typename...> class List, typename... Args,
  typename Key, typename Default
>
struct f<List<Args...>, Key, Default> {
  using type = decltype(
    sfinae<Default, Key>(inherit<tag<Args>...>())
  );
};

template <
  template <typename...> class List, typename... Args,
  typename Key, typename Default, typename KeyFilter
>
struct f<List<Args...>, Key, Default, KeyFilter> {
  using type = decltype(
    sfinae<Default, Key>(
      inherit<pair<typename KeyFilter::template apply<Args>, Args>...>()
    )
  );
};

template <
  template <typename...> class List, typename... Args,
  typename Key, typename Default,
  typename KeyFilter, typename PostFilter
>
struct f<List<Args...>, Key, Default, KeyFilter, PostFilter> {
  using type = decltype(
    sfinae<Default, Key>(
      inherit<
        pair<
          typename KeyFilter::template apply<Args>,
          typename PostFilter::template apply<Args>
        >...
      >()
    )
  );
};

template <typename...> struct c;

template <template <typename...> class List, typename... Args, typename Key>
struct c<List<Args...>, Key> {
  using type = decltype(
    sfinae<std::false_type, Key>(
      inherit<pair<Args, std::true_type>...>()
    )
  );
};

template <
  template <typename...> class List, typename... Args,
  typename Key, typename KeyFilter
>
struct c<List<Args...>, Key, KeyFilter> {
  using type = decltype(
    sfinae<std::false_type, Key>(
      inherit<
        pair<typename KeyFilter::template apply<Args>, std::true_type>...
      >()
    )
  );
};

} // namespace impl_fnd {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_find_h
