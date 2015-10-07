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
template <typename...> struct list;
template <template <typename...> class, typename, typename>
struct cartesian_product;
template <template <typename...> class, typename...> struct flatten;
template <typename T, template <typename, T...> class, typename U, U>
struct to_sequence;
template <typename, typename TChar, TChar, TChar...> struct parse_sequence;
template <typename, typename> struct from_sequence;
namespace flatten_map {
template <typename, typename...> struct breadth;
template <template <typename...> class, typename...> struct depth;
} // namespace flatten_map {
} // namespace operation_impl {
} // namespace detail {

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
template <template <typename...> class List, template <typename...> class Which>
struct flatten {
  template <typename... Args>
  using apply = typename detail::operation_impl::flatten<Which, Args...>
  ::template apply<List>;
};

// TODO: DOCUMENT
template <
  template <typename...> class Which,
  template <typename...> class List,
  template <typename...> class Row = List
>
struct flatten_map {
  template <typename T>
  using apply = typename detail::operation_impl::flatten_map::depth<Which, T>
    ::template apply<detail::operation_impl::list<>, Row>
    ::template apply<List>;
};

/////////////////
// to_sequence //
/////////////////

/**
 * Converts an integral value to its string representation,
 * as a sequence of characters.
 *
 * Parameters:
 *
 *  - T: the type of the integral value
 *  - Value: the value to be converted
 *  - TChar: the type of the characters in the sequence. Defaults to `char`.
 *
 * Example:
 *
 *  // yields `std::integer_sequence<char, '4', '2'>`
 *  using result1 = to_sequence<std::size_t, std::integer_sequence, 42>;
 *
 *  // yields `std::integer_sequence<wchar_t, L'-', L'5', L'6'>`
 *  using result2 = to_sequence<int, -56, std::integer_sequence, wchar_t>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename T, T Value,
  template <typename U, U...> class TSequence,
  typename TChar = char
>
using to_sequence = typename detail::operation_impl::to_sequence<
  TChar, TSequence, T, Value
>::type;

////////////////////
// parse_sequence //
////////////////////

/**
 * Provides mechanisms to parse the integral value of type `T`
 * represented by a sequence of characters.
 *
 * Parameters:
 *
 *  - T: the type of the integral value that will be parsed.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct parse_sequence {
  /**
   * Provides mechanisms to parse the integral value represented
   * by a sequence of characters of type `TChar`.
   *
   * Parameters:
   *
   *  - TChar: the type of the characters in the sequence to be parsed.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TChar>
  struct bind {
    /**
     * Parses the integral value represented by the
     * given sequence of characters.
     *
     * Parameters:
     *
     *  - Chars: the sequence of characters to be parsed.
     *
     * Example:
     *
     *  // yields `std::integral_constant<int, -56>`
     *  using result = parse_sequence<int>::bind<char>::apply<'-', '5', '6'>;
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <TChar... Chars>
    using apply = typename detail::operation_impl::parse_sequence<
      T, TChar, Chars...
    >::type;
  };

  /**
   * Parses the integral value represented by the
   * given sequence of `characters.
   *
   * Parameters:
   *
   *  - TChar: the type of the characters in the sequence.
   *  - Chars: the sequence of characters to be parsed.
   *
   * Example:
   *
   *  // yields `std::integral_constant<int, -56>`
   *  using result = parse_sequence<int>::bind<char>::apply<'-', '5', '6'>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TChar, TChar... Chars>
  using apply = typename bind<TChar>::template apply<Chars...>;

  /**
   * Parses the given sequence as an integral value.
   *
   * Parameters:
   *
   *  - TSequence: the sequence to be parsed.
   *
   * Example:
   *
   *  using str = std::integer_sequence<char, '-', '5', '6'>;
   *
   *  // yields `std::integral_constant<int, -56>`
   *  using result = parse_sequence<int>::from<str>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TSequence>
  using from = typename detail::operation_impl::from_sequence<
    T, TSequence
  >::type;
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

/////////////////
// to_sequence //
/////////////////

template <
  bool Negative, typename TChar, template <typename, TChar...> class TSequence,
  typename T, T Value, bool Done = (Value == 0)
>
struct to_sequence_impl {
  template <TChar... Suffix>
  using apply = typename to_sequence_impl<
    Negative, TChar, TSequence, T, Value / 10
  >::template apply<
    static_cast<TChar>("9876543210123456789"[(Value % 10) + 9]), Suffix...
  >;
};

template <
  bool Negative, typename TChar, template <typename, TChar...> class TSequence,
  typename T, T Value
>
struct to_sequence_impl<Negative, TChar, TSequence, T, Value, true> {
  template <TChar... Chars>
  using apply = typename std::conditional<
    Negative,
    TSequence<TChar, static_cast<TChar>('-'), Chars...>,
    TSequence<TChar, Chars...>
  >::type;
};

template <
  typename TChar, template <typename, TChar...> class TSequence,
  typename T, T Value
>
struct to_sequence {
  using type = typename std::conditional<
    Value == 0,
    TSequence<TChar, static_cast<TChar>('0')>,
    typename to_sequence_impl<
      (Value < 0), TChar, TSequence, T, Value
    >::template apply<>
  >::type;
};

////////////////////
// parse_sequence //
////////////////////

template <typename T, typename TChar, TChar Char>
struct parse_type_char {
  static_assert(
    Char == static_cast<TChar>('0')
    || Char == static_cast<TChar>('1')
    || Char == static_cast<TChar>('2')
    || Char == static_cast<TChar>('3')
    || Char == static_cast<TChar>('4')
    || Char == static_cast<TChar>('5')
    || Char == static_cast<TChar>('6')
    || Char == static_cast<TChar>('7')
    || Char == static_cast<TChar>('8')
    || Char == static_cast<TChar>('9'),
    "character is not a valid digit"
  );

  using type = std::integral_constant<
    T,
    Char == static_cast<TChar>('0') ? static_cast<T>(0)
    : Char == static_cast<TChar>('1') ? static_cast<T>(1)
    : Char == static_cast<TChar>('2') ? static_cast<T>(2)
    : Char == static_cast<TChar>('3') ? static_cast<T>(3)
    : Char == static_cast<TChar>('4') ? static_cast<T>(4)
    : Char == static_cast<TChar>('5') ? static_cast<T>(5)
    : Char == static_cast<TChar>('6') ? static_cast<T>(6)
    : Char == static_cast<TChar>('7') ? static_cast<T>(7)
    : Char == static_cast<TChar>('8') ? static_cast<T>(8)
    : Char == static_cast<TChar>('9') ? static_cast<T>(9)
    : 10
  >;

  static_assert(
    type::value >= 0 && type::value <= 9,
    "character is not a valid digit"
  );
};

template <typename T>
struct parse_sequence_add_tail {
  template <T Value, T Next>
  using positive = std::integral_constant<T, Value + Next>;

  template <T Value, T Next>
  using negative = std::integral_constant<T, Value - Next>;
};

template <typename T, T, template <T, T> class, typename TChar, TChar...>
struct parse_sequence_recursion;

template <
  typename T, T Value, template <T, T> class TAddTail,
  typename TChar, TChar Head, TChar... Tail
>
struct parse_sequence_recursion<T, Value, TAddTail, TChar, Head, Tail...> {
  using type = typename parse_sequence_recursion<
    T,
    TAddTail<Value * 10, parse_type_char<T, TChar, Head>::type::value>::value,
    TAddTail, TChar, Tail...
  >::type;
};

template <typename T, T Value, template <T, T> class TAddTail, typename TChar>
struct parse_sequence_recursion<T, Value, TAddTail, TChar> {
  using type = std::integral_constant<T, Value>;
};

template <bool, typename T, typename TChar, TChar Head, TChar... Tail>
struct parse_sequence_selector {
  using head = typename parse_type_char<T, TChar, Head>::type;

  static_assert(
    sizeof...(Tail) == 0 || head::value > 0,
    "integral can't have a leading zero"
  );

  using type = typename parse_sequence_recursion<
    T, head::value,
    parse_sequence_add_tail<T>::template positive,
    TChar, Tail...
  >::type;
};

template <typename T, typename TChar, TChar Sign, TChar Head, TChar... Tail>
struct parse_sequence_selector<true, T, TChar, Sign, Head, Tail...> {
  static_assert(Sign == static_cast<TChar>('-'), "unrecognized sign");

  using head = typename parse_type_char<T, TChar, Head>::type;

  static_assert(head::value > 0, "negative integral can't have a leading zero");

  using type = typename parse_sequence_recursion<
    T, -head::value,
    parse_sequence_add_tail<T>::template negative,
    TChar, Tail...
  >::type;
};

template <typename T, typename TChar, TChar Head, TChar... Tail>
struct parse_sequence {
  using is_sign = std::integral_constant<bool, Head == static_cast<TChar>('-')>;

  static_assert(
    std::is_signed<T>::value || !is_sign::value,
    "unsigned integral can't have a sign"
  );

  using type = typename parse_sequence_selector<
    is_sign::value, T, TChar, Head, Tail...
  >::type;
};

///////////////////
// from_sequence //
///////////////////

template <
  typename T,
  typename TChar,
  template <typename, TChar...> class U,
  TChar... Chars
>
struct from_sequence<T, U<TChar, Chars...>> {
  using type = typename parse_sequence<T, TChar, Chars...>::type;
};

/////////////////
// flatten_map //
/////////////////

namespace flatten_map {

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
    Which, Row, typename depth<Which, type_get_second<Pair>>::template apply<
      Results, Row, Prefix..., type_get_first<Pair>
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

} // namespace flatten_map {
} // namespace operation_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_operation_h
