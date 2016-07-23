/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/pair.h>

#ifndef FATAL_INCLUDE_fatal_type_impl_group_by_h
#define FATAL_INCLUDE_fatal_type_impl_group_by_h

namespace fatal {
namespace impl_gby {

template <typename...> struct gby;

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result,
  typename Label,
  typename... Group
>
struct gby<
  Filter,
  Predicate,
  Variadics<Result...>,
  pair<Label, Variadics<Group...>>
> {
  using type = Variadics<Result..., pair<Label, Variadics<Group...>>>;
};

template <bool, typename...> struct gbys;

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result,
  typename Label,
  typename... Group,
  typename T,
  typename... Args
>
struct gbys<
  true,
  Filter,
  Predicate,
  Variadics<Result...>,
  pair<Label, Variadics<Group...>>,
  T, Args...
>:
  gby<
    Filter,
    Predicate,
    Variadics<Result...>,
    pair<Label, Variadics<Group..., T>>,
    Args...
  >
{};

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result,
  typename Label,
  typename... Group,
  typename T,
  typename... Args
>
struct gbys<
  false,
  Filter,
  Predicate,
  Variadics<Result...>,
  pair<Label, Variadics<Group...>>,
  T, Args...
>:
  gby<
    Filter,
    Predicate,
    Variadics<Result..., pair<Label, Variadics<Group...>>>,
    pair<typename Filter::template apply<T>, Variadics<T>>,
    Args...
  >
{};

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename... Result,
  typename Label,
  typename... Group,
  typename T,
  typename... Args
>
struct gby<
  Filter,
  Predicate,
  Variadics<Result...>,
  pair<Label, Variadics<Group...>>,
  T, Args...
>:
  gbys<
    Predicate::template apply<Label, typename Filter::template apply<T>>::value,
    Filter,
    Predicate,
    Variadics<Result...>,
    pair<Label, Variadics<Group...>>,
    T, Args...
  >
{};

template <typename...> struct group_by;

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics
>
struct group_by<Filter, Predicate, Variadics<>> {
  using type = Variadics<>;
};

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename T
>
struct group_by<Filter, Predicate, Variadics<T>> {
  using type = Variadics<
    pair<
      typename Filter::template apply<T>,
      Variadics<T>
    >
  >;
};

template <
  typename Filter,
  typename Predicate,
  template <typename...> class Variadics,
  typename T,
  typename... Args
>
struct group_by<Filter, Predicate, Variadics<T, Args...>>:
  gby<
    Filter,
    Predicate,
    Variadics<>,
    pair<typename Filter::template apply<T>, Variadics<T>>,
    Args...
  >
{};

} // namespace impl_gby {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_group_by_h
