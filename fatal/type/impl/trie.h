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
#include <fatal/type/conditional.h>
#include <fatal/type/longest_common_prefix.h>
#include <fatal/type/search.h>
#include <fatal/type/select.h>
#include <fatal/type/group_by.h>
#include <fatal/type/size.h>
#include <fatal/type/tag.h>

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

#include <cassert>

namespace fatal {
namespace i_t {

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

// filtered at
template <std::size_t Index, typename Filter>
struct a {
  template <typename T>
  using apply = at<typename Filter::template apply<T>, Index>;
};

// node filter for sorted search (lookup) //
template <std::size_t Index, typename Filter>
struct F {
  template <typename T>
  using apply = at<
    typename Filter::template apply<typename T::haystack>,
    Index
  >;
};

// trie lookup implementation //
template <std::size_t, typename...> struct l;

// empty subtrie //
template <typename Filter>
struct l<0, Filter> {
  template <typename... Args>
  static constexpr inline bool f(Args &&...) { return false; }
};

// subtrie with single node //
template <
  std::size_t Offset,
  typename Filter,
  typename Haystack,
  bool IsTerminal,
  std::size_t Begin,
  std::size_t End,
  typename... Children
>
struct l<Offset, Filter, n<Haystack, IsTerminal, Begin, End, Children...>> {
  static_assert(Offset + Begin <= End, "internal error");

  template <
    typename NeedleBegin,
    typename Visitor,
    typename... VArgs
  >
  static inline bool f(
    std::size_t const size,
    NeedleBegin &&begin,
    Visitor &&visitor,
    VArgs &&...args
  ) {
    using haystack_data = typename Filter::template apply<Haystack>;

    if (size < End - Begin - Offset) {
      return false;
    }

    if (!IsTerminal && size == End - Begin - Offset) {
      return false;
    }

    using value_type = typename std::iterator_traits<
      typename std::decay<NeedleBegin>::type
    >::value_type;

    auto i = std::next(begin, End - Begin - Offset);
    if (!std::equal(
      begin,
      i,
      std::next(z_data<haystack_data, value_type>(), Offset + Begin)
    )) {
      return false;
    }

    if (IsTerminal && size == End - Begin - Offset) {
      visitor(tag<Haystack>(), static_cast<VArgs &&>(args)...);
      return true;
    }

    return l<0, Filter, Children...>::f(
      size - (End - Begin - Offset),
      std::move(i),
      static_cast<Visitor &&>(visitor),
      static_cast<VArgs &&>(args)...
    );
  }
};

// siblings //
template <
  typename Filter,
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
  Filter,
  n<Haystack, IsTerminal, Begin, End, Children...>,
  Node,
  Siblings...
> {
  template <typename NeedleBegin, typename Visitor, typename... VArgs>
  static inline bool f(
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
      F<Begin, Filter>
    >(
      *begin,
      l(),
      found,
      size,
      static_cast<NeedleBegin &&>(begin),
      static_cast<Visitor &&>(visitor),
      static_cast<VArgs &&>(args)...
    );

    return found;
  }

  template <
    typename Match,
    std::size_t Index,
    typename NeedleBegin,
    typename Visitor,
    typename... VArgs
  >
  void operator ()(
    indexed<Match, Index>,
    bool &found,
    std::size_t const size,
    NeedleBegin &&begin,
    Visitor &&visitor,
    VArgs &&...args
  ) const {
    found = l<1, Filter, Match>::f(
      size - 1,
      std::next(begin),
      static_cast<Visitor &&>(visitor),
      static_cast<VArgs &&>(args)...
    );
  }
};

// helper to expose the trie lookup implementation as a transform //
template <typename Filter>
struct L {
  template <typename... T>
  using apply = l<0, Filter, T...>;
};

// trie build recursion //
template <std::size_t, typename...> struct r;

// helper to expose the trie build recursion as a transform //
template <std::size_t Depth, typename Filter>
struct R {
  template <typename... Args>
  using apply = typename r<Depth, Filter, Args...>::type;
};

// leaf //
template <std::size_t Depth, typename Filter, typename T>
struct r<Depth, Filter, T> {
  using type = n<
    T, true, Depth, size<typename Filter::template apply<T>>::value
  >;
};

// internal node //
template <std::size_t Depth, typename Filter, typename T, typename... Args>
struct r<Depth, Filter, T, Args...> {
  using common = longest_common_prefix<
    at,
    Depth,
    vmin<
      less,
      size<typename Filter::template apply<T>>,
      size<typename Filter::template apply<Args>>...
    >::value,
    typename Filter::template apply<T>, typename Filter::template apply<Args>...
  >;

  using type = group_by<
    conditional<
      size<typename Filter::template apply<T>>::value == common::value,
      list<Args...>,
      list<T, Args...>
    >,
    a<common::value, Filter>,
    R<common::value, Filter>::template apply,
    N<
      T,
      common::value == size<typename Filter::template apply<T>>::value,
      Depth,
      common::value
    >::template apply
  >;
};

// trie build entry point helper //
template <bool, std::size_t, typename...> struct h;

// no common prefix and no empty string //
template <std::size_t Common, typename Filter, typename T, typename... Args>
struct h<true, Common, Filter, T, Args...> {
  using type = group_by<
    list<T, Args...>,
    a<0, Filter>,
    R<0, Filter>::template apply,
    L<Filter>::template apply
  >;
};

// common prefix or empty string //
template <std::size_t Common, typename Filter, typename T, typename... Args>
struct h<false, Common, Filter, T, Args...> {
  using type = l<
    0,
    Filter,
    group_by<
      conditional<
        size<typename Filter::template apply<T>>::value == Common,
        list<Args...>,
        list<T, Args...>
      >,
      a<Common, Filter>,
      R<Common, Filter>::template apply,
      N<
        T, Common == size<typename Filter::template apply<T>>::value, 0, Common
      >::template apply
    >
  >;
};

// trie build entry point //
template <typename...> struct e;

// non-empty input - find longest common prefix //
template <
  typename Filter,
  template <typename...> class Variadic,
  typename T, typename... Args
>
struct e<Filter, Variadic<T, Args...>>:
  h<
    (size<typename Filter::template apply<T>>::value > 1)
      && !longest_common_prefix<
        at,
        0,
        vmin<less, size<typename Filter::template apply<Args>>...>::value,
        typename Filter::template apply<Args>...
      >::value,
    longest_common_prefix<
      at,
      0,
      vmin<
        less,
        size<typename Filter::template apply<T>>,
        size<typename Filter::template apply<Args>>...
      >::value,
      typename Filter::template apply<T>,
      typename Filter::template apply<Args>...
    >::value,
    Filter,
    T, Args...
  >
{};

// unitary input //
template <typename Filter, template <typename...> class Variadic, typename T>
struct e<Filter, Variadic<T>> {
  using type = l<
    0,
    Filter,
    n<T, true, 0, size<typename Filter::template apply<T>>::value>
  >;
};

// empty input //
template <typename Filter, template <typename...> class Variadic>
struct e<Filter, Variadic<>> {
  using type = l<0, Filter>;
};

} // namespace i_t {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_trie_h
