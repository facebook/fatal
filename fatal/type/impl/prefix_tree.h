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
#include <fatal/type/size.h>
#include <fatal/type/transform.h>

#include <algorithm>
#include <iterator>

#include <cassert>

namespace fatal {
namespace old_TODO_REMOVE {

template <template <typename> class, template <typename> class, typename...>
struct fgby;

template <template <typename> class, template <typename> class, typename...>
struct fgbys;

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename GroupKey,
  typename... Group,
  typename... Result,
  typename NextKey,
  typename T,
  typename... Args
>
struct fgbys<
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  GroupKey, Variadics<Group...>,
  Variadics<Result...>,
  NextKey, T,
  Args...
>:
  fgby<
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected...>,
    Variadics<T>,
    Variadics<Result..., pair<GroupKey, Variadics<Group...>>>,
    Args...
  >
{};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename GroupKey,
  typename... Group,
  typename... Result,
  typename T,
  typename... Args
>
struct fgbys<
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  GroupKey, Variadics<Group...>,
  Variadics<Result...>,
  GroupKey, T,
  Args...
>:
  fgby<
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected...>,
    Variadics<Group..., T>,
    Variadics<Result...>,
    Args...
  >
{};

template <
  bool,
  template <typename> class,
  template <typename> class,
  typename...
>
struct fgbyr;

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename G,
  typename... Group,
  typename... Result,
  typename T,
  typename... Args
>
struct fgbyr<
  false,
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<G, Group...>,
  Variadics<Result...>,
  T, Args...
> {
  using type = fgbys<
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected...>,
    Key<G>, Variadics<G, Group...>,
    Variadics<Result...>,
    Key<T>, T,
    Args...
  >;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename... Result,
  typename T,
  typename... Args
>
struct fgbyr<
  false,
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<>,
  Variadics<Result...>,
  T, Args...
> {
  using type = fgby<
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected...>,
    Variadics<T>,
    Variadics<Result...>,
    Args...
  >;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename... Group,
  typename... Result,
  typename T,
  typename... Args
>
struct fgbyr<
  true,
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<Group...>,
  Variadics<Result...>,
  T, Args...
> {
  using type = fgby<
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected..., T>,
    Variadics<Group...>,
    Variadics<Result...>,
    Args...
  >;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  template <typename...> class Variadics,
  typename... Result
>
struct fgby<
  Key,
  RejectPredicate,
  RejectedGrouping<>,
  Variadics<>,
  Variadics<Result...>
> {
  using type = Variadics<Result...>;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename... Result
>
struct fgby<
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<>,
  Variadics<Result...>
> {
  using type = Variadics<RejectedGrouping<Rejected...>, Result...>;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  template <typename...> class Variadics,
  typename G,
  typename... Group,
  typename... Result
>
struct fgby<
  Key,
  RejectPredicate,
  RejectedGrouping<>,
  Variadics<G, Group...>,
  Variadics<Result...>
> {
  using type = Variadics<Result..., pair<Key<G>, Variadics<G, Group...>>>;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename G,
  typename... Group,
  typename... Result
>
struct fgby<
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<G, Group...>,
  Variadics<Result...>
> {
  using type = Variadics<
    RejectedGrouping<Rejected...>,
    Result...,
    pair<Key<G>, Variadics<G, Group...>>
  >;
};

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  typename... Rejected,
  template <typename...> class Variadics,
  typename... Group,
  typename... Result,
  typename T,
  typename... Args
>
struct fgby<
  Key,
  RejectPredicate,
  RejectedGrouping<Rejected...>,
  Variadics<Group...>,
  Variadics<Result...>,
  T, Args...
>:
  fgbyr<
    RejectPredicate<T>::value,
    Key,
    RejectPredicate,
    RejectedGrouping<Rejected...>,
    Variadics<Group...>,
    Variadics<Result...>,
    T, Args...
  >::type
{};

template <
  template <typename> class,
  template <typename> class,
  template <typename...> class,
  typename...
>
struct filtered;

template <
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping,
  template <typename...> class Variadics,
  typename... Args
>
struct filtered<
  Key,
  RejectPredicate,
  RejectedGrouping,
  Variadics<Args...>
>:
  fgby<
    Key,
    RejectPredicate,
    RejectedGrouping<>,
    Variadics<>,
    Variadics<>,
    Args...
  >
{};

} // namespace old_TODO_REMOVE {

template <
  typename T,
  template <typename> class Key,
  template <typename> class RejectPredicate,
  template <typename...> class RejectedGrouping
>
using TODO_REMOVE_filtered_group_by = typename old_TODO_REMOVE::filtered<
  Key,
  RejectPredicate,
  RejectedGrouping,
  T
>::type;

namespace impl_pt {

// TODO: IMPLEMENT LEADING PATH-COMPRESSION

// terminal //
template <typename...> struct t {};

// terminal filter //
template <std::size_t Depth>
struct flt {
  template <typename T>
  using apply = std::integral_constant<bool, (size<T>::value <= Depth)>;
};

template <std::size_t> struct pfn;

// build prefix tree recursion //
template <typename T, std::size_t Depth>
struct rc {
  using type = transform<
    TODO_REMOVE_filtered_group_by<
      T,
      bound::at<Depth>::template apply,
      flt<Depth>::template apply,
      t
    >,
    pfn<Depth + 1>::template apply
  >;
};

// trailing path compression //
template <template <typename...> class List, typename T, std::size_t Depth>
struct rc<List<T>, Depth> {
  using type = List<t<T>>;
};

// empty list - TODO: needed? //
template <template <typename...> class List, std::size_t Depth>
struct rc<List<>, Depth> {
  using type = List<>;
};

template <typename, std::size_t> struct fn;

template <typename Key, typename Group, std::size_t Depth>
struct fn<pair<Key, Group>, Depth> {
  using type = pair<Key, typename rc<Group, Depth>::type>;
};

template <typename... Args, std::size_t Depth>
struct fn<t<Args...>, Depth> {
  static_assert(
    sizeof...(Args) == 1,
    "multiple strings map to a path in the prefix tree"
  );
  using type = t<Args...>;
};

template <std::size_t Depth>
struct pfn {
  template <typename T>
  using apply = typename fn<T, Depth>::type;
};

// T must be sorted
template <typename T>
using build = typename rc<T, 0>::type;

// prefix tree find recursion //
template <std::size_t, typename> struct f;

template <std::size_t Depth, typename Edges>
struct f {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void v(
    bool &found,
    Begin &&begin,
    End &&end,
    Visitor &&visitor,
    Args &&...args
  ) {
    if (begin != end) {
      sorted_map_search<Edges>(
        *begin,
        f(),
        found,
        std::next(begin),
        std::forward<End>(end),
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      );
    }
  }

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
    Visitor &&visitor,
    Args &&...args
  ) const {
    f<Depth + 1, second<TailEdges>>::v(
      found,
      std::forward<Begin>(begin),
      std::forward<End>(end),
      std::forward<Visitor>(visitor),
      std::forward<Args>(args)...
    );
  }
};

template <std::size_t Depth, template <typename...> class List, typename T>
struct f<Depth, List<t<T>>> {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void v(
    bool &found,
    Begin &&begin,
    End &&end,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Depth <= size<T>::value, "internal error");
    static_assert(size<T>::value == size<T>::value, "internal error");

    assert(begin <= end);
    auto const length = static_cast<std::size_t>(std::distance(begin, end));

    if (length == size<T>::value - Depth
      && std::equal(begin, end, std::next(z_data<T>(), Depth))
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
struct f<Depth, List<t<T>, Edges...>> {
  template <
    typename Begin,
    typename End,
    typename Visitor,
    typename... Args
  >
  static void v(
    bool &found,
    Begin &&begin,
    End &&end,
    Visitor &&visitor,
    Args &&...args
  ) {
    if (begin != end) {
      f<Depth, List<Edges...>>::v(
        found,
        std::forward<Begin>(begin),
        std::forward<End>(end),
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      );
    } else if (Depth == size<T>::value) {
      visitor(tag<T>(), std::forward<Args>(args)...);
      found = true;
    }
  }
};

} // namespace impl_pt {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_prefix_tree_h
