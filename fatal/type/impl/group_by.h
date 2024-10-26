/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/list.h>
#include <fatal/type/pair.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace i_g {

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
template <typename, typename, template <typename...> class...> struct G;

template <
  template <typename...> class List,
  typename Key,
  template <typename...> class Grouping,
  template <typename...> class Outer
>
struct G<List<>, Key, Grouping, Outer> {
  using type = Outer<>;
};

template <
  template <typename...> class List,
  typename T, typename... Args,
  typename Key,
  template <typename...> class Grouping,
  template <typename...> class Outer
>
struct G<List<T, Args...>, Key, Grouping, Outer>:
  g<
    Outer,
    Grouping,
    list<>,
    typename Key::template apply<T>,
    list<T>,
    pair<typename Key::template apply<Args>, Args>...
  >
{};

// TODO: SKIP 2 AT A TIME
// filtered_group_by: recursion //
template <
  template <typename...> class,
  template <typename...> class,
  typename...
> struct f;

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
struct f<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  Key, List<Group...>,
  pair<Key, T>,
  Args...
>:
  f<
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
struct f<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  G, List<Group...>,
  pair<Key, T>,
  Args...
>:
  f<
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
struct f<
  Pair, Grouping,
  List<Filtered...>,
  List<Result...>,
  G, List<Group...>,
  tag<T>,
  Args...
>:
  f<
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
struct f<
  Pair,
  Grouping,
  List<Filtered...>,
  List<Result...>,
  Key,
  List<Group...>
> {
  using type = Pair<List<Filtered...>, List<Result..., Grouping<Group...>>>;
};

// pre-filter //
template <bool, typename, typename T>
struct p {
  using type = tag<T>;
};

template <typename Key, typename T>
struct p<false, Key, T> {
  using type = pair<typename Key::template apply<T>, T>;
};

// group by entry point //
template <typename, typename, typename, template <typename...> class...>
struct F;

// filtered_group_by: initial filter //
template <
  template <typename...> class,
  template <typename...> class,
  typename...
>
struct i;

// filter out all input //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered
>
struct i<Pair, Grouping, List<Filtered...>> {
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
struct i<Pair, Grouping, List<Filtered...>, tag<T>, Args...>:
  i<Pair, Grouping, List<Filtered..., T>, Args...>
{};

// start recursion //
template <
  template <typename...> class Pair, template <typename...> class Grouping,
  template <typename...> class List,
  typename... Filtered,
  typename Key, typename T,
  typename... Args
>
struct i<Pair, Grouping, List<Filtered...>, pair<Key, T>, Args...>:
  f<Pair, Grouping, List<Filtered...>, List<>, Key, List<T>, Args...>
{};

// non-empty input //
template <
  template <typename...> class List, typename... Args,
  typename Key,
  typename Filter,
  template <typename...> class Pair, template <typename...> class Grouping
>
struct F<List<Args...>, Key, Filter, Pair, Grouping>:
  i<
    Pair, Grouping,
    List<>,
    typename p<Filter::template apply<Args>::value, Key, Args>::type...
  >
{};

// empty input //
template <
  template <typename...> class List,
  typename Key,
  typename Filter,
  template <typename...> class Pair, template <typename...> class Grouping
>
struct F<List<>, Key, Filter, Pair, Grouping> {
  using type = Pair<List<>, Grouping<>>;
};

} // namespace i_g {
} // namespace fatal {
