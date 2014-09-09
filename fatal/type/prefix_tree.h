/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/map.h>
#include <fatal/type/reflection.h>

#include <type_traits>
#include <iterator>
#include <utility>

#include <cassert>

namespace ftl {

//////////////////////
// type_prefix_tree //
//////////////////////

struct non_terminal_tag {};

/**
 * Type prefix tree for template metaprogramming and efficient
 * switch/case implementation of strings and other sequences known
 * at compile time.
 *
 * This is a recursive structure so the same type used to define a tree
 * is used to define a child node (root of a subtree).
 *
 * NOTE: use the `type_prefix_tree_builder` below to build a prefix tree
 * instead of doing it manually.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: operations that would
 * mutate the prefix tree upon which they are performed actually create a new
 * prefix tree.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TSequence, typename... TNodes>
struct type_prefix_tree {
  /**
   * The sequence ending at this prefix tree node, or
   * `non_terminal_tag` if this is not a terminal node.
   */
  typedef TSequence sequence;

  /**
   * A boolean std::integral_constant-like typedef
   * telling whether this is a terminal node or not.
   */
  typedef negate_constant<std::is_same<sequence, non_terminal_tag>> is_terminal;

  /**
   * A `type_map` of all child nodes reachable from this node.
   *
   * For the example below, assume this sequence template:
   *
   *  template <char c> using chr = std::integral_constant<char, c>;
   *  template <char... s> using str = type_list<chr<s>...>;
   *
   * Say this is the top-most level of a prefix-tree containing sequences
   * of words "fit", "hit" and "hint", as in `str<'f', 'i', 't'>` and so forth,
   * `map` should be equivalent to:
   *
   *  type_map<
   *    type_pair<chr<'f'>, type_prefix_tree<non_terminal_tag,
   *      type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
   *        type_pair<chr<'t'>, type_prefix_tree<str<'f', 'i', 't'>>>
   *      >>
   *    >>,
   *    type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
   *      type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
   *        type_pair<chr<'t'>, type_prefix_tree<str<'h', 'i', 't'>>>
   *        type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
   *          type_pair<chr<'t'>, type_prefix_tree<str<'h', 'i', 'n', 't'>>>
   *        >>,
   *      >>
   *    >>,
   *  >
   *
   * In the above example, `map` has size 2, where the only two keys
   * are `chr<'f'>` and `chr<'h'>, corresponding to the first character
   * of all three sequences "fit", "hit" and "hint".
   */
  typedef type_map<TNodes...> map;

  /**
   * `match` contains methods for looking up sequences in the prefix-tree
   * for matches.
   *
   * The comparison is performed using the given `TComparer`'s method, whose
   * signature must follow this pattern:
   *
   *  template <typename TNeedle, typename TKey, std::size_t Index>
   *  static int compare(TNeedle &&needle, indexed_type_tag<TKey, Index>);
   *
   * which effectivelly compares `needle` against the key type `TKey`. The
   * result must be < 0, > 0 or == 0 if `needle` is, respectively, less than,
   * greather than or equal to `TKey`. `Index` is the position of the pair
   * with key `TKey` in this type map and can also be used in the comparison
   * if needed.  Refer to `type_map::binary_search::exact` for further details
   * on how the comparison works.
   *
   * `TComparer` defaults to `type_value_comparer` which compares an
   * std::integral_constant-like value to a runtime value.
   *
   * Example:
   *
   *  template <char c> using chr = std::integral_constant<char, c>;
   *  template <char... s> using str = type_list<chr<s>...>;
   *
   *  typedef type_prefix_tree_builder<>::build<
   *    str<'h', 'a', 't'>,
   *    str<'h', 'e', 'a', 'r', 't'>,
   *    str<'h', 'i', 'n', 't'>,
   *    str<'h', 'i', 't'>,
   *    str<'h', 'o', 't'>,
   *    str<'h', 'u', 't'>,
   *    str<'h', 'u', 'n', 't'>
   *  > prefix_tree;
   *
   *  struct cmp {
   *    template <char c, std::size_t Index>
   *    static int compare(char needle, indexed_type_tag<chr<c>, Index>) {
   *      return static_cast<int>(needle) - c;
   *    };
   *  };
   *
   *  void foo(std::string const &s) {
   *    // actual visitor implementation is omitted in this example and
   *    // explained on each method's documentation further down below
   *
   *    if (!prefix_tree::match<cmp>::exact(s.begin(), s.end(), visitor())) {
   *      // retrying with default comparer
   *      if (!prefix_tree::match<>::exact(s.begin(), s.end(), visitor())) {
   *        throw std::invalid_argument("not found in prefix tree");
   *      }
   *    }
   *
   *    do_something(s);
   *  }
   */
  template <typename TComparer = type_value_comparer>
  struct match {
    /**
     * Matches the range defined by `[begin, end)` againts this prefix
     * tree, looking for an exact match.
     *
     * If a match is found, the visitor is called with the following arguments:
     *  - an instance of `type_tag<TMatchingSequence>`
     *  - the perfectly forwarded list of additional arguments `args` given to
     *    the visitor
     *
     * in other words, with this general signature:
     *
     *  template <typename TSequence, typename... VArgs>
     *  void operator ()(type_tag<TSequence>, VArgs &&...args);
     *
     * Returns `true` if a match is found, `false` otherwise.
     *
     * The sole purpose of `args` is to be passed along to the visitor, it is
     * not used by this method in any way. It could also be safely omitted.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  template <char c> using chr = std::integral_constant<char, c>;
     *  template <char... s> struct str: public type_list<chr<s>...> {
     *    static std::string string() { return std::string{s...}; };
     *  };
     *
     *  typedef type_prefix_tree_builder<>::build<
     *    str<'h', 'i', 't'>,
     *    str<'h', 'o', 't'>,
     *    str<'h', 'u', 't'>
     *  > prefix_tree;
     *
     *  struct visitor {
     *    template <typename TString>
     *    void operator()(type_tag<TString>, std::string const &s) {
     *      cout << '\'' << s << "' matches sequence '"
     *        << TString::string() << '\''
     *        << endl;
     *    }
     *  };
     *
     *  bool match(std::string const &s) {
     *    return prefix_tree::match<>::exact(s.begin(), s.end(), visitor(), s);
     *  }
     *
     *  // yields `true` and prints "'hunt' matches sequence 'hunt'"
     *  bool result = match("hunt");
     *
     *  // yields `false` and prints nothing
     *  result = match("hunted");
     *
     *  // yields `false` and prints nothing
     *  result = match("hunt!");
     *
     *  // yields `false` and prints nothing
     *  result = match("hun");
     */
    template <typename TIterator, typename TVisitor, typename... VArgs>
    static bool exact(
      TIterator begin,
      TIterator end,
      TVisitor &&visitor,
      VArgs &&...args
    );

    /**
     * Matches the range defined by `[begin, end)` againts this prefix
     * tree, looking for all prefixes of this range stored as terminal
     * nodes in this prefix tree.
     *
     * For each matching prefix, the visitor is called with the following
     * arguments:
     *  - an instance of `type_tag<TMatchingSequence>`
     *  - the perfectly forwarded list of additional arguments `args` given to
     *    the visitor
     *
     * in other words, with this general signature:
     *
     *  template <typename TSequence, typename... VArgs>
     *  void operator ()(type_tag<TSequence>, VArgs &&...args);
     *
     * Returns the number of prefix matches.
     *
     * The sole purpose of `args` is to be passed along to the visitor, it is
     * not used by this method in any way. It could also be safely omitted.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  template <char c> using chr = std::integral_constant<char, c>;
     *  template <char... s> struct str: public type_list<chr<s>...>  {
     *    static std::string string() { return std::string{s...}; };
     *  };
     *
     *  typedef type_prefix_tree_builder<>::build<
     *    str<'i', 't'>,
     *    str<'i', 't', ' ', 'i', 's'>,
     *    str<'i', 't', ' ', 'i', 's', ' ', 'a'>,
     *    str<'i', 't', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't'>
     *  > prefix_tree;
     *
     *  struct visitor {
     *    template <typename TString>
     *    void operator()(type_tag<TString>, std::string const &s) {
     *      cout << '\'' << s << "' starts with '"
     *        << TString::string() << '\''
     *        << endl;
     *    }
     *  };
     *
     *  std::size_t match(std::string const &s) {
     *    return prefix_tree::match<>::exact(s.begin(), s.end(), visitor(), s);
     *  }
     *
     *  // yields `1` and prints "'it' starts with 'it'"
     *  auto result = match("it");
     *
     *  // yields `4` and prints "'it is a test' starts with 'it'",
     *  // "'it is a test' starts with 'it is'",
     *  // "'it is a test' starts with 'it is a'" and
     *  // "'it is a test' starts with 'it is a test'"
     *  result = match("it is a test");
     *
     *  // yields `0` and prints nothing
     *  result = match("this is a test");
     *
     *  // yields `4` and prints "'it is a test, dude' starts with 'it'",
     *  // "'it is a test, dude' starts with 'it is'",
     *  // "'it is a test, dude' starts with 'it is a'" and
     *  // "'it is a test, dude' starts with 'it is a test'"
     *  result = match("it is a test, dude");
     */
    template <typename TIterator, typename TVisitor, typename... VArgs>
    static std::size_t prefixes(
      TIterator begin,
      TIterator end,
      TVisitor &&visitor,
      VArgs &&...args
    );
  };
};

//////////////////////////////
// type_prefix_tree_builder //
//////////////////////////////

namespace detail {
namespace type_prefix_tree_impl {

template <typename...> struct builder;

} // namespace type_prefix_tree_impl {
} // namespace detail {

/**
 * Convenience mechanism to construct new type prefix trees.
 *
 * It takes a type comparer `TLessComparer` and a list of `TSequences` and
 * outputs the corresponding `type_prefix_tree`. The prefix tree is built such
 * that the nth element of a sequence is in the nth level of the tree. The
 * depth of the tree is the same as the longest sequence's size.
 *
 * `TLessComparer` must represent a total order relation between the sequence
 * elements. It defaults to `constants_comparison_lt` when omitted.
 *
 * See also: `type_string` and `FTL_STR`
 *
 * Example:
 *
 *  template <char c> using chr = std::integral_constant<char, c>;
 *  template <char... s> using str = type_list<chr<s>...>;
 *
 *  // yields an empty `type_prefix_tree<>`
 *  typedef type_prefix_tree_builder<>::build<> empty;
 *
 *  // yields `type_prefix_tree<non_terminal_tag,
 *  //   type_pair<chr<'f'>, type_prefix_tree<non_terminal_tag,
 *  //     type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
 *  //       type_pair<chr<'t'>, type_prefix_tree<str<'f', 'i', 't'>>>
 *  //     >>
 *  //   >>,
 *  //   type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
 *  //     type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
 *  //       type_pair<chr<'t'>, type_prefix_tree<str<'h', 'i', 't'>>>
 *  //       type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
 *  //         type_pair<chr<'t'>, type_prefix_tree<str<'h', 'i', 'n', 't'>>>
 *  //       >>,
 *  //     >>
 *  //   >>,
 *  // >
 *  typedef type_prefix_tree_builder<>::build<
 *    str<'f', 'i', 't'>,
 *    str<'h', 'i', 'n', 't'>,
 *    str<'h', 'i', 't'>
 *  > result;
 */
template <
  template <typename, typename> class TLessComparer = constants_comparison_lt
>
struct type_prefix_tree_builder {
  template <typename... TSequences>
  using build = typename detail::type_prefix_tree_impl::builder<
    TSequences...
  >::template tree<TLessComparer>;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace type_prefix_tree_impl {

/////////////////
// linear_tree //
/////////////////

template <typename, typename...> struct linear_tree;

template <typename TFullSequence, typename TElement>
struct linear_tree<TFullSequence, TElement> {
  typedef type_pair<TElement, type_prefix_tree<TFullSequence>> type;
};

template <typename TFullSequence, typename TElement, typename... TSuffix>
struct linear_tree<TFullSequence, TElement, TSuffix...> {
  typedef typename linear_tree<TFullSequence, TSuffix...>::type child;

  typedef type_pair<TElement, type_prefix_tree<non_terminal_tag, child>> type;
};

///////////////////
// insert_suffix //
///////////////////

template <typename, typename, typename...> struct insert_suffix;

/**
 * For the specializations below, consider a prefix tree
 * containing the words HI, HINT and HOT (nodes with an
 * asterisk are terminal - they mark the end of a word):
 *
 *           H
 *          / \
 *         I*  O
 *        /     \
 *       N       T*
 *      /
 *     T*
 */

// terminal with no siblings (T, from both HOT and
// HINT, in the example tree above)
template <typename TFullSequence>
struct insert_suffix<type_not_found_tag, TFullSequence> {
  template <template <typename, typename> class>
  using tree = type_prefix_tree<TFullSequence>;
};

// terminal with siblings (I, from HI, in the example tree above)
template <typename TTree, typename TFullSequence>
struct insert_suffix<TTree, TFullSequence> {
  template <template <typename, typename> class>
  using tree = typename TTree::map::contents::template apply_front<
    type_prefix_tree,
    TFullSequence
  >;
};

// non-terminal with no siblings (N, from HINT, and H,
// from all words, in the example tree above)
template <typename TFullSequence, typename TPrefix, typename... TSuffix>
struct insert_suffix<type_not_found_tag, TFullSequence, TPrefix, TSuffix...> {
  template <template <typename, typename> class>
  using tree = type_prefix_tree<
    non_terminal_tag,
    typename linear_tree<TFullSequence, TPrefix, TSuffix...>::type
  >;
};

// non-terminal with siblings (O, from HOT, in the example tree above)
template <
  typename TTree, typename TFullSequence, typename TPrefix, typename... TSuffix
>
struct insert_suffix<TTree, TFullSequence, TPrefix, TSuffix...> {
  typedef typename TTree::map::template find<TPrefix> subtree;

  template <template <typename, typename> class TLessComparer>
  using tree = typename std::conditional<
    std::is_same<subtree, type_not_found_tag>::value,
    typename TTree::map::template insert_pair_sorted<
      typename linear_tree<TFullSequence, TPrefix, TSuffix...>::type,
      TLessComparer
    >,
    typename TTree::map::template replace<
      TPrefix,
      typename insert_suffix<
        subtree, TFullSequence, TSuffix...
      >::template tree<TLessComparer>
    >
  >::type::contents::template apply_front<
    type_prefix_tree,
    typename TTree::sequence
  >;
};

} // namespace type_prefix_tree_impl {
} // namespace detail {

namespace detail {
namespace type_prefix_tree_impl {

/////////////
// builder //
/////////////

template <> struct builder<> {
  template <template <typename, typename> class>
  using tree = type_prefix_tree<non_terminal_tag>;
};

template <typename TSequence, typename... TSequences>
struct builder<TSequence, TSequences...> {
  typedef typename reflect_template<TSequence>::types sequence;

  template <template <typename, typename> class TLessComparer>
  using tree = typename sequence::template apply_front<
    detail::type_prefix_tree_impl::insert_suffix,
    typename builder<TSequences...>::template tree<TLessComparer>,
    TSequence
  >::template tree<TLessComparer>;
};

} // namespace type_prefix_tree_impl {
} // namespace detail {

/////////////////////////////////
// type_prefix_tree DEFINITION //
/////////////////////////////////

namespace detail {
namespace type_prefix_tree_impl {

template <typename TSequence>
struct match_visitor {
  template <typename TVisitor, typename... VArgs>
  static void visit(TVisitor &&visitor, VArgs &&...args) {
    visitor(type_tag<TSequence>{}, std::forward<VArgs>(args)...);
  }
};

// avoids the need of a visitor overload where TSequence = non_terminal_tag
// this should never be called
template <>
struct match_visitor<non_terminal_tag> {
  template <typename... VArgs> static void visit(VArgs &&...) {
    assert(false);
  }
};

template <typename TComparer>
struct match_exact {
  template <
    typename TKey, typename TSubtree, std::size_t Index,
    typename TNeedle, typename TIterator,
    typename TVisitor, typename... VArgs
  >
  void operator ()(
    indexed_type_tag<type_pair<TKey, TSubtree>, Index>,
    TNeedle &&,
    TIterator begin, TIterator end, bool &found,
    TVisitor &&visitor, VArgs &&...args
  ) {
    if (begin != end) {
      TSubtree::map::template binary_search<TComparer>::exact(
        *begin,
        *this,
        std::next(begin), end, found,
        std::forward<TVisitor>(visitor), std::forward<VArgs>(args)...
      );
    } else if (TSubtree::is_terminal::value) {
      match_visitor<typename TSubtree::sequence>::visit(
        std::forward<TVisitor>(visitor),
        std::forward<VArgs>(args)...
      );
      found = true;
    }
  }
};

template <typename TComparer>
struct match_prefixes {
  template <
    typename TKey, typename TSubtree, std::size_t Index,
    typename TNeedle, typename TIterator,
    typename TVisitor, typename... VArgs
  >
  void operator ()(
    indexed_type_tag<type_pair<TKey, TSubtree>, Index>,
    TNeedle &&,
    TIterator begin, TIterator end, std::size_t &found,
    TVisitor &&visitor, VArgs &&...args
  ) {
    if (TSubtree::is_terminal::value) {
      match_visitor<typename TSubtree::sequence>::visit(
        std::forward<TVisitor>(visitor),
        std::forward<VArgs>(args)...
      );

      ++found;
    }

    if (begin != end) {
      TSubtree::map::template binary_search<TComparer>::exact(
        *begin,
        *this,
        std::next(begin), end, found,
        std::forward<TVisitor>(visitor), std::forward<VArgs>(args)...
      );
    }
  }
};

} // namespace type_prefix_tree_impl {
} // namespace detail {

template <typename TSequence, typename... TNodes>
template <typename TComparer>
template <typename TIterator, typename TVisitor, typename... VArgs>
bool type_prefix_tree<TSequence, TNodes...>::match<TComparer>::exact(
  TIterator begin,
  TIterator end,
  TVisitor &&visitor,
  VArgs &&...args
) {
  if (begin == end) {
    return is_terminal::value;
  }

  bool found = false;

  type_prefix_tree::map::template binary_search<TComparer>::exact(
    *begin,
    detail::type_prefix_tree_impl::match_exact<TComparer>{},
    std::next(begin), end, found,
    std::forward<TVisitor>(visitor), std::forward<VArgs>(args)...
  );

  return found;
}

template <typename TSequence, typename... TNodes>
template <typename TComparer>
template <typename TIterator, typename TVisitor, typename... VArgs>
std::size_t type_prefix_tree<TSequence, TNodes...>::match<TComparer>::prefixes(
  TIterator begin,
  TIterator end,
  TVisitor &&visitor,
  VArgs &&...args
) {
  if (begin == end) {
    return is_terminal::value;
  }

  std::size_t found = 0;

  type_prefix_tree::map::template binary_search<TComparer>::exact(
    *begin,
    detail::type_prefix_tree_impl::match_prefixes<TComparer>{},
    std::next(begin), end, found,
    std::forward<TVisitor>(visitor), std::forward<VArgs>(args)...
  );

  return found;
}

} // namespace ftl {
