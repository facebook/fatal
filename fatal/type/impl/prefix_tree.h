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

#include <fatal/type/array.h>
#include <fatal/type/group_by.h>
#include <fatal/type/search.h>
#include <fatal/type/transform.h>

#include <algorithm>
#include <iterator>

#include <cassert>

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

// T must be sorted
template <typename T>
using build = typename impl_trie::rc<T, 0>::type;

template <std::size_t, typename> struct frc;

template <std::size_t Depth, typename Edges>
struct frc {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void f(
    bool &found,
    Begin &&begin,
    End &&end,
    std::size_t const depth,
    Visitor &&visitor,
    Args &&...args
  ) {
    if (begin != end) {
      sorted_map_search<Edges>(
        *begin,
        frc(),
        found,
        std::next(begin),
        std::forward<End>(end),
        depth + 1,
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      );
    }
  }

  // TODO: CAN WE INLINE rc HERE?
  template <
    typename TailEdges,
    std::size_t Index,
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  void operator ()(
    indexed<TailEdges, Index>,
    bool &found,
    Begin &&begin,
    End &&end,
    std::size_t const depth,
    Visitor &&visitor,
    Args &&...args
  ) const {
    frc<Depth + 1, second<TailEdges>>::f(
      found,
      std::forward<Begin>(begin),
      std::forward<End>(end),
      depth,
      std::forward<Visitor>(visitor),
      std::forward<Args>(args)...
    );
  }
};

template <std::size_t Depth, template <typename...> class List, typename T>
struct frc<Depth, List<impl_trie::trm<T>>> {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void f(
    bool &found,
    Begin &&begin,
    End &&end,
    std::size_t const depth,
    Visitor &&visitor,
    Args &&...args
  ) {
    assert(depth <= size<T>::value);
    assert(begin <= end);
    auto const distance = static_cast<std::size_t>(std::distance(begin, end));
    using array = as_array<T>;
    static_assert(size<T>::value == array::get.size(), "internal error");
    if (distance == size<T>::value - depth
      && std::equal(begin, end, std::next(array::get.data(), depth))
    ) {
      visitor(tag<T>(), std::forward<Args>(args)...);
      found = true;
    }
  }
};

template <
  std::size_t Depth,
  template <typename...> class List,
  typename T,
  typename... Edges
>
struct frc<Depth, List<impl_trie::trm<T>, Edges...>> {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void f(
    bool &found,
    Begin &&begin,
    End &&end,
    std::size_t const depth,
    Visitor &&visitor,
    Args &&...args
  ) {
    if (begin != end) {
      frc<Depth + 1, List<Edges...>>::f(
        found,
        std::forward<Begin>(begin),
        std::forward<End>(end),
        depth,
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      );
    } else if (depth == size<T>::value) {
      visitor(tag<T>(), std::forward<Args>(args)...);
      found = true;
    }
  }
};

} // namespace impl_trie {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_prefix_tree_h
