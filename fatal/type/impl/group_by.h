/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/pair.h>
#include <fatal/type/tag.h>

#ifndef FATAL_INCLUDE_fatal_type_impl_group_by_h
#define FATAL_INCLUDE_fatal_type_impl_group_by_h

namespace fatal {
namespace impl_gby {

// TODO: SKIP 2 AT A TIME
// group by iteration //
template <
  template <typename...> class,
  template <typename...> class,
  typename...
>
struct g;

// group match //
template <
  template <typename...> class Outer,
  template <typename...> class Grouping,
  typename... Result,
  typename Key, typename... Group,
  typename T,
  typename... Args
>
struct g<
  Outer, Grouping,
  list<Result...>,
  Key, list<Group...>,
  pair<Key, T>, Args...
>:
  g<Outer, Grouping, list<Result...>, Key, list<Group..., T>, Args...>
{};

// group no match //
template <
  template <typename...> class Outer,
  template <typename...> class Grouping,
  typename... Result,
  typename G, typename... Group,
  typename Key, typename T,
  typename... Args
>
struct g<
  Outer, Grouping,
  list<Result...>,
  G, list<Group...>,
  pair<Key, T>, Args...
>:
  g<Outer, Grouping, list<Result..., Grouping<Group...>>, Key, list<T>, Args...>
{};

// done //
template <
  template <typename...> class Outer,
  template <typename...> class Grouping,
  typename... Result,
  typename Key, typename... Group
>
struct g<Outer, Grouping, list<Result...>, Key, list<Group...>> {
  using type = Outer<Result..., Grouping<Group...>>;
};

// group by entry point //
template <typename, template <typename...> class...> struct gb;

template <
  template <typename...> class List,
  template <typename...> class Key,
  template <typename...> class Grouping,
  template <typename...> class Outer
>
struct gb<List<>, Key, Grouping, Outer> {
  using type = Outer<>;
};

template <
  template <typename...> class List,
  typename T, typename... Args,
  template <typename...> class Key,
  template <typename...> class Grouping,
  template <typename...> class Outer
>
struct gb<List<T, Args...>, Key, Grouping, Outer>:
  g<Outer, Grouping, list<>, Key<T>, list<T>, pair<Key<Args>, Args>...>
{};

// TODO: SKIP 2 AT A TIME
// group by iteration //
template <
  template <typename...> class,
  template <typename...> class,
  typename...
> struct filtered_group_by_recursion;

// group match //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename... Result,
  typename Key, typename... Group,
  typename T,
  typename... Args
>
struct filtered_group_by_recursion<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  Key, List<Group...>,
  pair<Key, T>,
  Args...
>:
  filtered_group_by_recursion<
    Pair, Grouping,
    List<Filtered...>,
    List<Result...>,
    Key, List<Group..., T>,
    Args...
  >
{};

// group no match //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename... Result,
  typename G, typename... Group,
  typename Key, typename T,
  typename... Args
>
struct filtered_group_by_recursion<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  G, List<Group...>,
  pair<Key, T>,
  Args...
>:
  filtered_group_by_recursion<
    Pair, Grouping,
    List<Filtered...>,
    List<Result..., Grouping<Group...>>,
    Key, List<T>,
    Args...
  >
{};

// filtered out //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename... Result,
  typename G, typename... Group,
  typename T, typename... Args
>
struct filtered_group_by_recursion<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  G, List<Group...>,
  tag<T>,
  Args...
>:
  filtered_group_by_recursion<
    Pair, Grouping,
    List<Filtered..., T>,
    List<Result...>,
    G, List<Group...>,
    Args...
  >
{};

// done //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename... Result,
  typename Key, typename... Group
>
struct filtered_group_by_recursion<Pair, Grouping, List<Filtered...>, List<Result...>, Key, List<Group...>> {
  using type = Pair<List<Filtered...>, List<Result..., Grouping<Group...>>>;
};

// pre-filter //
template <bool, template <typename...> class, typename T>
struct filtered_group_by_pre_filter {
  using type = tag<T>;
};

template <template <typename...> class Key, typename T>
struct filtered_group_by_pre_filter<false, Key, T> {
  using type = pair<Key<T>, T>;
};

// group by entry point //
template <typename, template <typename...> class...>
struct filtered_group_by_entry_point;

template <
  template <typename...> class,
  template <typename...> class,
  typename...
>
struct filtered_group_by_initial_filter;

// filter out all input //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered
>
struct filtered_group_by_initial_filter<Pair, Grouping, List<Filtered...>> {
  using type = Pair<List<Filtered...>, Grouping<>>;
};

// TODO: SKIP 2 AT A TIME
// filter out next element //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename T, typename... Args
>
struct filtered_group_by_initial_filter<Pair, Grouping, List<Filtered...>, tag<T>, Args...>:
  filtered_group_by_initial_filter<Pair, Grouping, List<Filtered..., T>, Args...>
{};

// start recursion //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename Key, typename T,
  typename... Args
>
struct filtered_group_by_initial_filter<Pair, Grouping, List<Filtered...>, pair<Key, T>, Args...>:
  filtered_group_by_recursion<Pair, Grouping, List<Filtered...>, List<>, Key, List<T>, Args...>
{};

// non-empty input //
template <
  template <typename...> class List, typename... Args,
  template <typename...> class Key,
  template <typename...> class Filter,
  template <typename...> class Pair, template <typename...> class Grouping
>
struct filtered_group_by_entry_point<List<Args...>, Key, Filter, Pair, Grouping>:
  filtered_group_by_initial_filter<
    Pair, Grouping,
    List<>,
    typename filtered_group_by_pre_filter<
      Filter<Args>::value,
      Key,
      Args
    >::type...
  >
{};

// empty input //
template <
  template <typename...> class List,
  template <typename...> class Key,
  template <typename...> class Filter,
  template <typename...> class Pair, template <typename...> class Grouping
>
struct filtered_group_by_entry_point<List<>, Key, Filter, Pair, Grouping> {
  using type = Pair<List<>, Grouping<>>;
};




















} // namespace impl_gby {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_group_by_h
