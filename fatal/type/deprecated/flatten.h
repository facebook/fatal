/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace fatal {
namespace detail {
namespace operation_impl {

template <template <typename...> class, typename...> struct flatten;
template <typename...>
struct flatten_sequence;

} // namespace operation_impl {
} // namespace detail {

/////////////
// flatten //
/////////////

/**
  * Flattens the arguments of a specific template into a topmost list.
  *
  * Only a single level will be flattened.
  *
  * Parameters:
  *
  *  - List: the template for the topmost list to be returned
  *  - Which: which template to flatten
  *  - Args...: the list of types to be flattened
  *
  * Example:
  *
  *  template <typename...> struct list {};
  *
  *  // yields `list<
  *  //   int,
  *  //   double,
  *  //   float,
  *  //   list<short>,
  *  //   bool
  *  // >`
  *  using result1 = fatal::flatten<list, list>::apply<
  *    int,
  *    list<
  *      double,
  *      float,
  *      list<short>
  *    >,
  *    bool
  *  >;
  *
  *  // yields `std::tuple<
  *  //   int,
  *  //   double,
  *  //   float,
  *  //   list<short>,
  *  //   bool
  *  // >`
  *  using result2 = list::flatten<std::tuple, list>::apply<
  *    int,
  *    list<
  *      double,
  *      float,
  *      list<short>
  *    >,
  *    bool
  *  >;
  *
  * @author: Marcelo Juchem <marcelo@fb.com>
  */
template <
  template <typename...> class List,
  template <typename...> class Which = List
>
struct flatten {
  template <typename... Args>
  using apply = typename detail::operation_impl::flatten<Which, Args...>
    ::template apply<List>;
};

// TODO: DOCUMENT AND TEST
template <typename T, template <typename U, U...> class Sequence>
struct flatten_sequence {
  template <typename... Args>
  using apply = typename detail::operation_impl::flatten_sequence<Args...>
    ::template apply<Sequence, T>;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace operation_impl {

/////////////
// flatten //
/////////////

// specialization that matches `Which`
template <
  template <typename...> class Which,
  typename... Args,
  typename... UArgs
>
struct flatten<Which, Which<Args...>, UArgs...> {
  template <template <typename...> class List, typename... VArgs>
  using apply = typename flatten<Which, UArgs...>
    ::template apply<List, VArgs..., Args...>;
};

// specialization for items that don't match `Which`
template <template <typename...> class Which, typename THead, typename... Args>
struct flatten<Which, THead, Args...> {
  template <template <typename...> class List, typename... VArgs>
  using apply = typename flatten<Which, Args...>::template apply<
    List, VArgs..., THead
  >;
};

// base case
template <template <typename...> class Which>
struct flatten<Which> {
  template <template <typename...> class List, typename... Args>
  using apply = List<Args...>;
};

//////////////////////
// flatten_sequence //
//////////////////////

// specialization that matches `Which`
template <
  template <typename U, U...> class Which,
  typename V, V... Args, typename... Tail
>
struct flatten_sequence<Which<V, Args...>, Tail...> {
  template <template <typename U, U...> class Sequence, typename T, T... Prefix>
  using apply = typename flatten_sequence<Tail...>
    ::template apply<Sequence, T, Prefix..., Args...>;
};

// base case
template <>
struct flatten_sequence<> {
  template <template <typename U, U...> class Sequence, typename T, T... Prefix>
  using apply = Sequence<T, Prefix...>;
};

} // namespace operation_impl {
} // namespace detail {
} // namespace fatal {
