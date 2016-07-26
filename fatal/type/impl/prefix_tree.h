/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_prefix_tree_h
#define FATAL_INCLUDE_fatal_type_impl_prefix_tree_h

#include <fatal/type/group_by.h>
#include <fatal/type/transform.h>

namespace fatal {
namespace impl_trie {

// TODO: IMPLEMENT LEADING PATH-COMPRESSION

template <typename...>
struct trm {};

// TODO: CAN WE COMPOSE INSTEAD OF CREATING THIS AD-HOC FILTER?
template <std::size_t Depth>
struct flt {
  template <typename T>
  using apply = std::integral_constant<bool, (size<T>::value <= Depth)>;
};

template <std::size_t> struct pfn;

template <typename T, std::size_t Depth>
struct rc {
  using type = transform<
    filtered_group_by<
      T,
      bound::at<Depth>::template apply,
      flt<Depth>::template apply,
      trm
    >,
    pfn<Depth + 1>::template apply
  >;
};

template <template <typename...> class List, typename T, std::size_t Depth>
struct rc<List<T>, Depth> {
  using type = List<trm<T>>;
};

template <typename, std::size_t> struct fn;

template <typename Key, typename Group, std::size_t Depth>
struct fn<pair<Key, Group>, Depth> {
  using type = pair<Key, typename rc<Group, Depth>::type>;
};

template <typename... Args, std::size_t Depth>
struct fn<trm<Args...>, Depth> {
  static_assert(
    sizeof...(Args) == 1,
    "multiple strings map to a path in the prefix tree"
  );
  using type = trm<Args...>;
};

template <std::size_t Depth>
struct pfn {
  template <typename T>
  using apply = typename fn<T, Depth>::type;
};

} // namespace impl_trie {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_prefix_tree_h
