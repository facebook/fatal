/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/cartesian_product.h>
#include <fatal/type/deprecated/flatten.h>
#include <fatal/type/deprecated/transform.h>
#include <fatal/type/size.h>
#include <fatal/type/slice.h>

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace fatal {
namespace detail {
namespace operation_impl {

template <typename...> struct list;
namespace expand_recursive_map {
template <typename, typename...> struct breadth;
template <template <typename...> class, typename...> struct depth;

} // namespace expand_recursive_map {
} // namespace operation_impl {
} // namespace detail {

////////////
// expand //
////////////

// TODO: DOCUMENT
template <template <typename...> class T, typename U>
struct expand {
  template <typename... UArgs>
  using front = fatal::apply<T, U, UArgs...>;

  template <typename... UArgs>
  using back = fatal::apply<T, UArgs..., U>;
};

template <
  template <typename...> class T,
  template <typename...> class U,
  typename... Args
>
struct expand<T, U<Args...>> {
  template <typename... UArgs>
  using front = fatal::apply<T, Args..., UArgs...>;

  template <typename... UArgs>
  using back = fatal::apply<T, UArgs..., Args...>;
};

//////////////////////////
// expand_recursive_map //
//////////////////////////

// TODO: DOCUMENT
template <
  template <typename...> class Which,
  template <typename...> class List,
  template <typename...> class Row = List
>
struct expand_recursive_map {
  template <typename T>
  using apply = typename detail::operation_impl::expand_recursive_map::depth<
    Which, T
  >::template apply<detail::operation_impl::list<>, Row>::template apply<List>;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace operation_impl {

template <typename... Args>
struct list {
  template <template <typename...> class T>
  using apply = fatal::apply<T, Args...>;

  template <typename... Suffix>
  using push_back = list<Args..., Suffix...>;
};

//////////////////////////
// expand_recursive_map //
//////////////////////////

namespace expand_recursive_map {

template <template <typename...> class Which, typename... Pairs>
struct depth<Which, Which<Pairs...>> {
  template <
    typename Results,
    template <typename...> class Row,
    typename... Prefix
  >
  using apply = typename breadth<list<Prefix...>, Pairs...>::template apply<
    Which, Row, Results
  >;
};

template <template <typename...> class Which, typename Terminal>
struct depth<Which, Terminal> {
  template <
    typename Results,
    template <typename...> class Row,
    typename... Prefix
  >
  using apply = typename Results::template push_back<
    fatal::apply<Row, Prefix..., Terminal>
  >;
};

template <typename... Prefix, typename Pair, typename... Siblings>
struct breadth<list<Prefix...>, Pair, Siblings...> {
  template <
    template <typename...> class Which,
    template <typename...> class Row,
    typename Results
  >
  using apply = typename breadth<list<Prefix...>, Siblings...>::template apply<
    Which, Row, typename depth<Which, second<Pair>>::template apply<
      Results, Row, Prefix..., first<Pair>
    >
  >;
};

template <typename... Prefix>
struct breadth<list<Prefix...>> {
  template <
    template <typename...> class,
    template <typename...> class,
    typename Results
  >
  using apply = Results;
};

} // namespace expand_recursive_map {
} // namespace operation_impl {
} // namespace detail {
} // namespace fatal {
