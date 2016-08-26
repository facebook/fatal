/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_trie_h
#define FATAL_INCLUDE_fatal_type_impl_trie_h

#include <fatal/type/array.h>
#include <fatal/type/longest_common_prefix.h>
#include <fatal/type/search.h>
#include <fatal/type/select.h>
#include <fatal/type/group_by.h>
#include <fatal/type/size.h>
#include <fatal/type/tag.h>

#include <algorithm>
#include <iterator>
#include <type_traits>

#include <cassert>

namespace fatal {
namespace impl_tr {

// represents a node in the trie //
template <
  typename Haystack,
  bool IsTerminal,
  // the range of the haystack string this node represents //
  std::size_t Begin, std::size_t End,
  typename... Children
>
struct n {
  using haystack = Haystack;
};

// bound node - to expose it as a transform //
template <typename T, bool IsTerminal, std::size_t Begin, std::size_t End>
struct N {
  template <typename... Args>
  using apply = n<T, IsTerminal, Begin, End, Args...>;
};

// node filter for sorted search (lookup) //
template <std::size_t Index>
struct node_filter {
  template <typename T>
  using apply = at<typename T::haystack, Index>;
};

// trie lookup implementation //
template <std::size_t, typename...> struct l;

// empty subtrie //
template <>
struct l<0> {
  template <typename... Args>
  static constexpr bool f(Args &&...) { return false; }
};

// subtrie with single node //
template <
  std::size_t Offset,
  typename Haystack,
  bool IsTerminal,
  std::size_t Begin,
  std::size_t End,
  typename... Children
>
struct l<Offset, n<Haystack, IsTerminal, Begin, End, Children...>> {
  static_assert(Offset + Begin <= End, "internal error");

  template <
    typename NeedleBegin,
    typename Visitor,
    typename... VArgs
  >
  static bool f(
    std::size_t const size,
    NeedleBegin &&begin,
    Visitor &&visitor,
    VArgs &&...args
  ) {
    if (size < End - Begin - Offset) {
      return false;
    }

    if (!IsTerminal && size == End - Begin - Offset) {
      return false;
    };

    using value_type = typename std::iterator_traits<
      typename std::decay<NeedleBegin>::type
    >::value_type;

    auto i = std::next(begin, End - Begin - Offset);
    if (!std::equal(
      begin,
      i,
      std::next(z_data<Haystack, value_type>(), Offset + Begin)
    )) {
      return false;
    }

    if (IsTerminal && size == End - Begin - Offset) {
      visitor(tag<Haystack>(), std::forward<VArgs>(args)...);
      return true;
    }

    return l<0, Children...>::f(
      size - (End - Begin - Offset),
      std::move(i),
      std::forward<Visitor>(visitor),
      std::forward<VArgs>(args)...
    );
  }
};

// siblings //
template <
  typename Haystack,
  bool IsTerminal,
  std::size_t Begin,
  std::size_t End,
  typename... Children,
  typename Node,
  typename... Siblings
>
struct l<
  0,
  n<Haystack, IsTerminal, Begin, End, Children...>,
  Node,
  Siblings...
> {
  template <typename NeedleBegin, typename Visitor, typename... VArgs>
  static bool f(
    std::size_t const size,
    NeedleBegin &&begin,
    Visitor &&visitor,
    VArgs &&...args
  ) {
    if (!size) {
      return false;
    }

    bool found = false;
    sorted_search<
      list<n<Haystack, IsTerminal, Begin, End, Children...>, Node, Siblings...>,
      node_filter<Begin>::template apply
    >(
      *begin,
      l(),
      found,
      size,
      std::forward<NeedleBegin>(begin),
      std::forward<Visitor>(visitor),
      std::forward<VArgs>(args)...
    );

    return found;
  }

  template <
    typename n,
    std::size_t Index,
    typename NeedleBegin,
    typename Visitor,
    typename... VArgs
  >
  void operator ()(
    indexed<n, Index>,
    bool &found,
    std::size_t const size,
    NeedleBegin &&begin,
    Visitor &&visitor,
    VArgs &&...args
  ) const {
    found = l<1, n>::f(
      size - 1,
      std::next(begin),
      std::forward<Visitor>(visitor),
      std::forward<VArgs>(args)...
    );
  }
};

// helper to expose the trie lookup implementation as a transform //
template <typename... T>
using L = l<0, T...>;

// trie build recursion //
template <std::size_t, typename...> struct r;

// helper to expose the trie build recursion as a transform //
template <std::size_t Depth>
struct R {
  template <typename... Args>
  using apply = typename r<Depth, Args...>::type;
};

// leaf //
template <std::size_t Depth, typename T>
struct r<Depth, T> {
  using type = n<T, true, Depth, size<T>::value>;
};

// internal node //
template <std::size_t Depth, typename T, typename... Args>
struct r<Depth, T, Args...> {
  using common = longest_common_prefix<
    at,
    Depth,
    vmin<less, size<T>, size<Args>...>::value,
    T, Args...
  >;

  using type = group_by<
    typename std::conditional<
      size<T>::value == common::value,
      list<Args...>,
      list<T, Args...>
    >::type,
    bound::at<common::value>::template apply,
    R<common::value>::template apply,
    N<
      T,
      common::value == size<T>::value,
      Depth,
      common::value
    >::template apply
  >;
};

// trie build entry point helper //
template <bool, std::size_t, typename...> struct h;

// no common prefix and no empty string //
template <std::size_t Common, typename T, typename... Args>
struct h<true, Common, T, Args...> {
  using type = group_by<
    list<T, Args...>,
    bound::at<0>::template apply,
    R<0>::template apply,
    L
  >;
};

// common prefix or empty string //
template <std::size_t Common, typename T, typename... Args>
struct h<false, Common, T, Args...> {
  using type = l<
    0,
    group_by<
      typename std::conditional<
        size<T>::value == Common,
        list<Args...>,
        list<T, Args...>
      >::type,
      bound::at<Common>::template apply,
      R<Common>::template apply,
      N<T, Common == size<T>::value, 0, Common>::template apply
    >
  >;
};

// trie build entry point //
template <typename...> struct e;

// non-empty input - find longest common prefix //
template <template <typename...> class Variadic, typename T, typename... Args>
struct e<Variadic<T, Args...>>:
  h<
    (size<T>::value > 0) && !longest_common_prefix<
      at,
      0,
      vmin<less, size<Args>...>::value,
      Args...
    >::value,
    longest_common_prefix<
      at,
      0,
      vmin<less, size<T>, size<Args>...>::value,
      T, Args...
    >::value,
    T, Args...
  >
{};

// unitary input //
template <template <typename...> class Variadic, typename T>
struct e<Variadic<T>> {
  using type = l<0, n<T, true, 0, size<T>::value>>;
};

// empty input //
template <template <typename...> class Variadic>
struct e<Variadic<>> {
  using type = l<0>;
};

} // namespace impl_tr {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_trie_h
