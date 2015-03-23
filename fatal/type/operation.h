/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_operation_h
#define FATAL_INCLUDE_fatal_type_operation_h

#include <fatal/type/transform.h>

namespace fatal {
namespace detail {
namespace operation_impl {

template <template <typename...> class, typename, typename>
struct cartesian_product;

template <template <typename...> class, typename...> struct flatten;

} // namespace operation_impl {
} // namespace detail {

/**
 * Computes the cartesian product between two lists.
 *
 * Arguments:
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
 *  using result = cartesian_product<
 *    list, pair,
 *    list<int, void>,
 *    list<double, bool>,
 *  >;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TList,
  template <typename...> class TPair,
  typename TLHS, typename TRHS
>
using cartesian_product = typename detail::operation_impl::cartesian_product<
  TPair, TLHS, TRHS
>::template apply<TList>;

/**
   * Flattens the arguments of a specific template into a topmost list.
   *
   * Only a single level will be flattened.
   *
   * Arguments:
   *
   *  - TList: the template for the topmost list to be returned
   *  - TWhich: which template to flatten
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
   *  using result1 = fatal::flatten<
   *    list, list,
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
   *  using result2 = list::flatten<
   *    std::tuple, list,
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
  template <typename...> class TList,
  template <typename...> class TWhich,
  typename... Args
>
using flatten = typename detail::operation_impl::flatten<TWhich, Args...>
  ::template apply<TList>;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace operation_impl {

template <typename...> struct list {};

///////////////////////
// cartesian_product //
///////////////////////

template <
  template <typename...> class TPair,
  template <typename...> class T,
  typename... TArgs,
  template <typename...> class U,
  typename... UArgs
>
struct cartesian_product<TPair, T<TArgs...>, U<UArgs...>> {
template <typename V>
  using impl = list<TPair<V, UArgs>...>;

  template <template <typename...> class TList>
  using apply = typename fatal::flatten<TList, list, impl<TArgs>...>;
};

/////////////
// flatten //
/////////////

template <
  template <typename...> class T,
  typename... Args,
  typename... UArgs
>
struct flatten<T, T<Args...>, UArgs...> {
  template <template <typename...> class U, typename... VArgs>
  using apply = typename flatten<T, UArgs...>
    ::template apply<U, VArgs..., Args...>;
};

template <template <typename...> class T, typename THead, typename... Args>
struct flatten<T, THead, Args...> {
  template <template <typename...> class U, typename... VArgs>
  using apply = typename flatten<T, Args...>::template apply<
    U, VArgs..., THead
  >;
};

template <template <typename...> class T>
struct flatten<T> {
  template <template <typename...> class U, typename... Args>
  using apply = U<Args...>;
};

} // namespace operation_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_operation_h
