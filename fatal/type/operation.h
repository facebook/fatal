/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_operation_h
#define FATAL_INCLUDE_fatal_type_operation_h

#include <fatal/type/slice.h>
#include <fatal/type/deprecated/flatten.h>
#include <fatal/type/deprecated/flatten_sequence.h>
#include <fatal/type/deprecated/transform.h>

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace fatal {
namespace detail {
namespace operation_impl {

template <typename...> struct list;
template <template <typename...> class, typename, typename>
struct cartesian_product;
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

///////////////////////
// cartesian_product //
///////////////////////

/**
 * Computes the cartesian product between two lists.
 *
 * Parameters:
 *
 * Example:
 *
 *  template <typename, typename> struct pair {};
 *  template <typename...> struct list {};
 *
 *  // yields `list<
 *  //   pair<int, double>,
 *  //   pair<int, bool>,
 *  //   pair<void, double>,
 *  //   pair<void, bool>
 *  // >
 *  using result = cartesian_product<list, pair>::apply<
 *    list<int, void>,
 *    list<double, bool>,
 *  >;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class List, template <typename...> class Pair >
struct cartesian_product {
  template <typename TLHS, typename TRHS>
  using apply = typename detail::operation_impl::cartesian_product<
    Pair, TLHS, TRHS
  >::template apply<List>;
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

///////////////////////
// cartesian_product //
///////////////////////

template <
  template <typename...> class Pair,
  template <typename...> class T,
  typename... TArgs,
  template <typename...> class U,
  typename... UArgs
>
struct cartesian_product<Pair, T<TArgs...>, U<UArgs...>> {
template <typename V>
  using impl = list<Pair<V, UArgs>...>;

  template <template <typename...> class List>
  using apply = typename fatal::flatten<List, list>
    ::template apply<impl<TArgs>...>;
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

#endif // FATAL_INCLUDE_fatal_type_operation_h
