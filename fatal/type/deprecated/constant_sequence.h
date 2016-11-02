/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_deprecated_constant_sequence_h
#define FATAL_INCLUDE_fatal_type_deprecated_constant_sequence_h

#include <fatal/preprocessor.h>
#include <fatal/type/deprecated/type_list.h>

#include <memory>
#include <string>
#include <type_traits>

namespace fatal {

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace detail {
namespace constant_sequence_impl {

template <typename, typename...> struct concat;
template <typename, typename> struct reverse;
template <typename T, typename, T, T, T> struct polynomial;
template <typename, typename TChar, TChar, TChar...> struct parse_sequence;
template <typename, typename> struct from_sequence;
template <typename T, template <typename, T...> class, typename U, U>
struct to_sequence;
template <std::size_t, typename T, T...> struct array;

} // namespace constant_sequence_impl {
} // namespace detail {

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
    using apply = typename detail::constant_sequence_impl::parse_sequence<
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
  using from = typename detail::constant_sequence_impl::from_sequence<
    T, TSequence
  >::type;
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
 *  using result1 = to_sequence<std::integer_sequence>::apply<std::size_t, 42>;
 *
 *  // yields `std::integer_sequence<wchar_t, L'-', L'5', L'6'>`
 *  using result2 = to_sequence<std::integer_sequence, wchar_t>
 *    ::apply<int, -56>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename U, U...> class TSequence, typename TChar = char>
struct to_sequence {
  template <typename T>
  struct bind {
    template <T Value>
    using apply = typename detail::constant_sequence_impl::to_sequence<
      TChar, TSequence, T, Value
    >::type;
  };

  template <typename T, T Value>
  using apply = typename detail::constant_sequence_impl::to_sequence<
    TChar, TSequence, T, Value
  >::type;
};

//////////////////////////////////////
// constant_sequence IMPLEMENTATION //
//////////////////////////////////////

/**
 * A compile-time sequence of values for template metaprogramming.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: Operations that would
 * mutate the sequence upon which they are performed actually create a new
 * sequence.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T... Values>
class constant_sequence {
  using default_char_traits = std::char_traits<T>;
  using default_allocator = std::allocator<T>;

public:
  /**
   * The type of the elements in this sequence.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = T;

  /**
   * The type of the elements in this sequence.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using value_type = type;

  /**
   * Gets a `std::integral_constant` for the given value,
   * with the same type as this sequence's elements.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `std::integral_constant<int, 4>`
   *  using result = seq::constant<4>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type Value>
  using constant = std::integral_constant<type, Value>;

  /**
   * This sequence as a type_list of std::integral_constant.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `type_list<
   *  //   std::integral_constant<int, 1>,
   *  //   std::integral_constant<int, 2>,
   *  //   std::integral_constant<int, 3>
   *  // >`
   *  using result = seq::list;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class List = type_list>
  using list = List<constant<Values>...>;

  /**
   * Tells how many values this sequence has.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr std::size_t size = sizeof...(Values);

  /**
   * Tells whether this sequence is empty or not.
   * This is the same as `size == 0`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr bool empty = size == 0;

  /**
   * Gets the index of the given element in this sequence
   * as a `std::integral_constant` of `std::size_t`.
   *
   * Returns the size of the sequence when not found.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 10, 20, 30>;
   *
   *  // yields `std::integral_constant<int, 1>`
   *  using result1 = seq::index_of<20>;
   *
   *  // yields `std::integral_constant<int, 3>`
   *  using result2 = seq::index_of<40>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type Needle>
  using index_of = typename list<>::template index_of<constant<Needle>>;

  /**
   * Gets the index of the given element in this sequence
   * as a `std::integral_constant` of `std::size_t`.
   *
   * Results in a compilation error when not found.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 10, 20, 30>;
   *
   *  // yields `std::integral_constant<int, 1>`
   *  using result1 = seq::index_of<20>;
   *
   *  // fails to compile
   *  using result2 = seq::index_of<40>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type Needle>
  using checked_index_of = typename list<>::template checked_index_of<
    constant<Needle>
  >;

  /**
   * Returns a `std::integral_constant` of type `bool` telling
   * whether the given element is contained in this sequence or not.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 10, 20, 30>;
   *
   *  // yields `std::true_type`
   *  using result1 = seq::index_of<20>;
   *
   *  // yields `std::false_type`
   *  using result2 = seq::index_of<40>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type Needle>
  using contains = typename list<>::template contains<constant<Needle>>;

  /**
   * Adds values to the end of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `constant_sequence<int, 1, 2, 3, 9>`
   *  using result = seq::push_back<9>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... UValues>
  using push_back = constant_sequence<type, Values..., UValues...>;

  /**
   * Adds values to the beginning of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `constant_sequence<int, 9, 1, 2, 3>`
   *  using result = seq::push_front<9>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... UValues>
  using push_front = constant_sequence<type, UValues..., Values...>;

  /**
   * Appends the elements of the given sequences, in
   * the order given, to the end of this sequence.
   *
   * Example:
   *
   *  using a = constant_sequence<int, 1, 2, 3>;
   *  using b = constant_sequence<int, 4, 5, 6>;
   *  using c = constant_sequence<int, 7, 8, 9>;
   *
   *  // yields `constant_sequence<int, 1, 2, 3, 4, 5, 6>
   *  using result1 = a::concat<b>;
   *
   *  // yields `constant_sequence<int, 1, 2, 3, 4, 5, 6, 7, 8, 9>
   *  using result2 = a::concat<b, c>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using concat = typename detail::constant_sequence_impl::concat<
    constant_sequence, Args...
  >::type;

  /**
   * Gets a sequence with all the elements positioned in the
   * range `[Offset, Size)`, where `Size` is the size of this
   * sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result1 = seq::tail<0>;
   *
   *  // yields `constant_sequence<int, 5, 6>`
   *  using result2 = seq::tail<1>;
   *
   *  // yields `constant_sequence<int>`
   *  using result3 = seq::tail<3>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Offset>
  using tail = typename list<>::template tail<Offset>
    ::template apply_typed_values<type, fatal::constant_sequence>;

  /**
   * Gets a pair of sequences where the first one contains
   * the elements positioned in the range `[0, Index)`, and
   * the second one contains the elements positioned in the
   * range `[Index, Size)`, where `Size` is the size of this
   * sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `type_pair<
   *  //   constant_sequence<int>,
   *  //   constant_sequence<int, 4, 5, 6>
   *  // >`
   *  using result1 = seq::split<0>;
   *
   *  // yields `type_pair<
   *  //   constant_sequence<int, 4>,
   *  //   constant_sequence<int, 5, 6>
   *  // >`
   *  using result2 = seq::split<1>;
   *
   *  // yields `type_pair<
   *  //   constant_sequence<int, 4, 5, 6>,
   *  //   constant_sequence<int>
   *  // >`
   *  using result3 = seq::split<3>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index = (size / 2)>
  using split = type_pair<
    typename list<>::template split<Index>::first
      ::template apply_typed_values<type, fatal::constant_sequence>,
    typename list<>::template split<Index>::second
      ::template apply_typed_values<type, fatal::constant_sequence>
  >;

  /**
   * Gets a sequence with all the elements positioned in the
   * range `[Begin, End)`.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result1 = seq::slice<0, 3>;
   *
   *  // yields `constant_sequence<int, 5, 6>`
   *  using result2 = seq::slice<1, 3>;
   *
   *  // yields `constant_sequence<int>`
   *  using result3 = seq::slice<2, 2>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Begin, std::size_t End>
  using slice = typename list<>::template slice<Begin, End>
    ::template apply_typed_values<type, fatal::constant_sequence>;

  /**
   * Gets a sequence with the leftmost `N` elements of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result1 = seq::left<3>;
   *
   *  // yields `constant_sequence<int, 4, 5>`
   *  using result2 = seq::left<2>;
   *
   *  // yields `constant_sequence<int>`
   *  using result3 = seq::left<0>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t N>
  using left = typename list<>::template left<N>
    ::template apply_typed_values<type, fatal::constant_sequence>;

  /**
   * Gets a sequence with the rightmost `N` elements of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result1 = seq::right<3>;
   *
   *  // yields `constant_sequence<int, 5, 6>`
   *  using result2 = seq::right<2>;
   *
   *  // yields `constant_sequence<int>`
   *  using result3 = seq::right<0>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t N>
  using right = typename list<>::template right<N>
    ::template apply_typed_values<type, fatal::constant_sequence>;

  /**
   * Reverses the elements of the sequence.
   *
   * Optionally, appends a suffix to the reversed list.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `constant_sequence<int, 3, 2, 1>`
   *  using result1 = seq::reverse<>;
   *
   *  // yields `constant_sequence<int, 3, 2, 1, 0, -1>`
   *  using result2 = seq::reverse<0, -1>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <T... Suffix>
  using reverse = typename detail::constant_sequence_impl::reverse<
    constant_sequence, constant_sequence<T, Suffix...>
  >::type;

  /**
   * Evaluates the polynomial whose coefficients are represented by this
   * sequences' elements, from least to most significant.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 5, 2, -3>;
   *
   *  // yields `std::integral_constant<int, -3 * 9 * 9 + 2 * 9 + 5>`
   *  using result1 = seq::polynomial<9>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <T Variable>
  using polynomial = typename detail::constant_sequence_impl::polynomial<
    T, constant_sequence, Variable, 1, 0
  >::type;

  /**
   * Interleaves the given separators between each pair of elements of this
   * sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 0, 5, 0, 6>`
   *  using result1 = seq::interleave<0>;
   *
   *  // yields `constant_sequence<int, 4, 0, 1, 5, 0, 1, 6>`
   *  using result2 = seq::interleave<0, 1>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result3 = seq::interleave<>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Separator>
  using interleave = typename list<>::template interleave<
    std::integral_constant<type, Separator>...
  >::template apply_typed_values<type, fatal::constant_sequence>;

  /**
   * Applies a metafunction to each element of this sequence.
   *
   * The metafunction should take a single parameter of type `type` and provide
   * a `value` member with the result of type `type`.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  template <int Value>
   *  using square = std::integral_constant<int, Value * Value>;
   *
   *  // yields `constant_sequence<int, 16, 25, 36>`
   *  using result1 = seq::transform<square>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <type> class Transform>
  using transform = constant_sequence<type, Transform<Values>::value...>;

  /**
   * Applies a metafunction to each element of this sequence.
   *
   * The metafunction should take two parameters, the first being a type `T`
   * and the second a value of type `T`. It must provide a `value` member with
   * the result of type `T`.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  template <typename T , T Value>
   *  using square = std::integral_constant<T, Value * Value>;
   *
   *  // yields `constant_sequence<int, 16, 25, 36>`
   *  using result1 = seq::typed_transform<square>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename, type> class Transform>
  using typed_transform = constant_sequence<
    type,
    Transform<type, Values>::value...
  >;

  /**
   * Applies a metafunction to each element of this sequence, and returns the
   * result as a type list.
   *
   * The metafunction should take a single parameter of type `type`.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  template <int Value>
   *  using square = std::integral_constant<int, Value * Value>;
   *
   *  // yields `type_list<
   *  //   std::integral_constant<int, 16>,
   *  //   std::integral_constant<int, 25>,
   *  //   std::integral_constant<int, 36>
   *  // >`
   *  using result1 = seq::list_transform<square>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <type> class Transform>
  using list_transform = type_list<Transform<Values>...>;

  /**
   * Applies a metafunction to each element of this sequence, and returns the
   * result as a type list.
   *
   * The metafunction should take two parameters, the first being a type `T`
   * and the second a value of type `T`.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  template <typename T , T Value>
   *  using square = std::integral_constant<T, Value * Value>;
   *
   *  // yields `type_list<
   *  //   std::integral_constant<int, 16>,
   *  //   std::integral_constant<int, 25>,
   *  //   std::integral_constant<int, 36>
   *  // >`
   *  using result1 = seq::typed_list_transform<square>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename, type> class Transform>
  using typed_list_transform = type_list<Transform<type, Values>...>;

  /**
   * Applies the values of this sequence to the given template.
   *
   * Example:
   *
   *  template <int... V> struct foo {};
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `foo<1, 2, 3>`
   *  using result = seq::apply<foo>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <type...> class U>
  using apply = U<Values...>;

  /**
   * Applies the type and values of this sequence to the given template.
   *
   * Example:
   *
   *  template <typename T, int... V> struct foo {};
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `foo<int, 1, 2, 3>`
   *  using result = seq::typed_apply<foo>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename, type...> class U>
  using typed_apply = U<type, Values...>;

  /**
   * Parses the integral value represented by this sequence.
   *
   * Parameters:
   *
   *  - TIntegral: the type of the integral value that will be parsed.
   *
   * Example:
   *
   *  using seq = constant_sequence<char, '-', '5', '6'>;
   *
   *  // yields `std::integral_constant<int, -56>`
   *  using result = seq::parse<int>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TIntegral>
  using parse = typename parse_sequence<TIntegral>::template apply<
    T, Values...
  >;

  /**
   * Gets a constexpr array with the values from this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `std::array<int, 3>` with values `{1, 2, 3}`
   *  auto result = seq::array;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  using array = detail::constant_sequence_impl::array<
    0, type, Values..., Suffix...
  >;

  /**
   * Gets a pointer to a statically allocated version of this sequence.
   *
   * An optional suffix can be provided as well, resulting in the sequence,
   * followed by the suffix.
   *
   * Example:
   *
   *  using str = constant_sequence<char, 'h', 'e', 'l', 'l', 'o'>;
   *
   *  // prints `hello`
   *  std::cout << std::string(str::data(), std::size) << std::endl;
   *
   *  // prints `hello`
   *  std::cout << str::data<'\0'>() << std::endl;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  static type const *data() { return z_data<Suffix...>(); }

  /**
   * Gets a constexpr array with the values from this sequence,
   * followed by a 0 terminator.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `std::array<int, 4>` with values `{1, 2, 3, 0}`
   *  auto result1 = seq::z_array;
   *
   *  // yields `std::array<int, 3>` with values `"hi"`
   *  auto hi = constant_sequence<char, 'h', 'i'>::z_array;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  using z_array = detail::constant_sequence_impl::array<
    1, type, Values..., Suffix..., static_cast<type>(0)
  >;

  /**
   * Gets a pointer to a statically allocated version of this sequence,
   * with a trailing null terminator.
   *
   * An optional suffix can be provided as well, resulting in the sequence,
   * followed by the suffix, followed by the null terminator.
   *
   * Example:
   *
   *  using str = constant_sequence<char, 'h', 'e', 'l', 'l', 'o'>;
   *
   *  // prints `hello`
   *  std::cout << str::z_data() << std::endl;
   *
   *  // prints `hello world`
   *  std::cout << str::z_data<' ', 'w', 'o', 'r', 'l', 'd'>() << std::endl;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  static type const *z_data() { return z_array<Suffix...>::data; }

  /**
   * The `std::basic_string` type returned by the `string()` method.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  using string_type = std::basic_string<value_type, TTraits, TAllocator>;

  /**
   * Constructs a `std::basic_string` corresponding to this
   * `constant_sequence`, using the given character traits and allocator.
   * Defaults to `std::char_traits` and `std::allocator`.
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  using hi = constant_sequence<char, 'h', 'i'>;
   *
   *  // yields `std::string("hi")`
   *  auto result = hi::string();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  static string_type<TTraits, TAllocator> string() {
    return string_type<TTraits, TAllocator>{Values...};
  }

  /**
   * Constructs the type `U`, passing the elements of
   * this sequence as the arguments to the constructor,
   * followed by a perfect forwardind of `args`.
   *
   * Example:
   *
   *  using str = constant_sequence<char, 'h', 'e', 'l', 'l', 'o'>;
   *
   *  // yields `std::vector<char>{'h', 'e', 'l', 'l', 'o'}`
   *  auto v = str::init<std::vector<char>>();
   *
   *  auto s = str::init<std::vector<char>>('\0');
   *
   *  // prints `hello`
   *  std::cout << s.data() << std::endl;
   */
  template <typename U, typename... UArgs>
  static constexpr U init(UArgs &&...args) {
    return U{Values..., std::forward<UArgs>(args)...};
  }
};

//////////////////////////
// to_constant_sequence //
//////////////////////////

/**
 * Converts an integral value to its string representation, as a type string.
 *
 * Parameters:
 *
 *  - T: the type of the integral value
 *  - Value: the value to be converted
 *  - TChar: the type of the string's character. Defaults to `char`.
 *
 * Example:
 *
 *  // yields `constant_sequence<char, '4', '2'>`
 *  using result1 = to_constant_sequence<std::size_t, 42>;
 *
 *  // yields `constant_sequence<wchar_t, L'-', L'5', L'6'>`
 *  using result2 = to_constant_sequence<int, -56, wchar_t>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TChar = char>
struct to_constant_sequence {
  template <typename T>
  struct bind {
    template <T Value>
    using apply = typename to_sequence<constant_sequence, TChar>
      ::template apply<T, Value>;
  };

  template <typename T, T Value>
  using apply = typename to_sequence<constant_sequence, TChar>
    ::template apply<T, Value>;
};

///////////////
// FATAL_STR //
///////////////

/**
 * Instantiates a compile time string out of a string literal, in the form of a
 *`constant_sequence` class template instantiation.
 *
 * The string is provided as a type, named as the given identifier.
 *
 * General form:
 *
 *  FATAL_STR(identifier, "string");
 *
 * Parameters:
 *
 *  identifier: the identifier for the type representing the compile time string
 *  string: the string literal to be represented by the compile time string
 *
 * Example:
 *
 *  // this is equivalent to
 *  // `using hi = constant_sequence<char, 'h', 'i'>;`
 *  FATAL_STR(hi, "hi");
 *
 *  // this is equivalent to
 *  // `using h = constant_sequence<std::char32_t, U'h', U'e', U'y'>;`
 *  FATAL_STR(h, U"hey");
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_STR(Id, String) \
  FATAL_BUILD_STR_IMPL( \
    Id, \
    String, \
    FATAL_UID(FATAL_CAT(constant_sequence_string_impl_, Id)), \
    FATAL_UID(Indexes) \
  )

/**
 * Instantiates a compile time string out of the given identifier, in the form
 * of a `constant_sequence` class template instantiation. The characters are
 * typed as `char`.
 *
 * The string is provided as a type, named as the identifier itself.
 *
 * General form:
 *
 *  FATAL_ID_STRING(identifier);
 *
 * Example:
 *
 *  // this is equivalent to
 *  // `using hi = constant_sequence<char, 'h', 'i'>;`
 *  FATAL_ID_STRING(hi);
 *
 *  // this is equivalent to
 *  // `using hey = constant_sequence<char, 'h', 'e', 'y'>;`
 *  FATAL_ID_STRING(hey);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_ID_STRING(Id) \
  FATAL_STR(Id, FATAL_TO_STR(Id))

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

template <typename T, T... Values>
constexpr std::size_t constant_sequence<T, Values...>::size;

template <typename T, T... Values>
constexpr bool constant_sequence<T, Values...>::empty;

// TODO: DOCUMENT AND TEST
template <std::size_t... Values>
using size_sequence = constant_sequence<std::size_t, Values...>;

////////////////////////////////////////
// IMPLEMENTATION DETAILS DECLARATION //
////////////////////////////////////////

namespace detail {
namespace constant_sequence_impl {

enum class build_strategy { done, repeat, recurse };

constexpr build_strategy strategy(std::size_t current, std::size_t end) {
  return current >= end
    ? build_strategy::done
    : current * 2 <= end
      ? build_strategy::repeat
      : build_strategy::recurse;
}

template<typename T, T Offset, typename> struct build_coerce;

template<typename T>
constexpr std::size_t distance(T begin, T end) {
  return begin <= end
    ? static_cast<std::size_t>(end - begin)
    : throw "The start of the constant_range must not be greater than the end";
}

template<
  std::size_t End,
  typename State = size_sequence<0>,
  build_strategy Status = strategy(State::size, End)
>
struct build;

/* Disabled due to https://llvm.org/bugs/show_bug.cgi?id=28519
#if ((defined(__clang__) && __clang_major__ >= 3 && __clang_minor__ >= 8) || \
     (defined(_MSC_VER) && _MSC_FULL_VER >= 190023918))
# define FATAL_IMPL_HAS_MAKE_INTEGER_SEQ 1
#endif
*/

#if FATAL_IMPL_HAS_MAKE_INTEGER_SEQ
template <class T, T N>
__make_integer_seq<constant_sequence, T, N> make_constant_sequence();
#endif

} // namespace constant_sequence_impl {
} // namespace detail {

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

/**
 * A convenient shortcut that builds a `constant_sequence` of
 * type `std::size_t` with elements in the range `[0, End)`.
 *
 * Example:
 *
 * // yields `constant_sequence<std::size_t, 0, 1, 2, 3, 4>`
 * using result1 = indexes_sequence<5>;
 *
 * // yields `constant_sequence<std::size_t, 0, 1, 2>`
 * using result2 = indexes_sequence<3>;
 *
 * // yields `constant_sequence<std::size_t, 0, 1, 2, 3, 4, 5>`
 * using result3 = indexes_sequence<6>;
 *
 * // yields `constant_sequence<std::size_t, 0, 1, 2, 3>`
 * using result4 = indexes_sequence<4>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#if FATAL_IMPL_HAS_MAKE_INTEGER_SEQ
template <std::size_t Size>
using indexes_sequence = decltype(
  detail::constant_sequence_impl::make_constant_sequence<std::size_t, Size>()
);
#else
template <std::size_t Size>
using indexes_sequence = typename detail::constant_sequence_impl::build<
  Size
>::type;
#endif

/**
 * Builds a `constant_sequence` with elements in the range `[Begin, End)`,
 * when `OpenEnd` is `true`, or in the range `[Begin, End]` otherwise.
 *
 * The default value for `OpenEnd` is `true`.
 *
 * Example:
 *
 * // yields `constant_sequence<int, 1, 2, 3, 4>`
 * using result1 = constant_range<int, 1, 5>;
 *
 * // yields `constant_sequence<int, 1, 2>`
 * using result2 = constant_range<int, 1, 3, true>;
 *
 * // yields `constant_sequence<int, 1, 2, 3, 4, 5>`
 * using result3 = constant_range<int, 1, 5, false>;
 *
 * // yields `constant_sequence<int, 1, 2, 3>`
 * using result4 = constant_range<int, 1, 3, false>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Begin, T End, bool OpenEnd = true>
using constant_range = typename detail::constant_sequence_impl::build_coerce<
  T,
  Begin,
  indexes_sequence<
    detail::constant_sequence_impl::distance(Begin, End) + !OpenEnd
  >
>::type;

///////////////////////////////////////
// IMPLEMENTATION DETAILS DEFINITION //
///////////////////////////////////////

namespace detail {
namespace constant_sequence_impl {

template<typename, typename> struct build_concat;

template<std::size_t... UValues, std::size_t... VValues>
struct build_concat<size_sequence<UValues...>, size_sequence<VValues...>> {
  using type = size_sequence<UValues..., (VValues + sizeof...(UValues))...>;
};

template<std::size_t End, typename State, build_strategy Status>
struct build {
  using type = State;
};

template<>
struct build<0u, size_sequence<0>, build_strategy::done> {
  using type = size_sequence<>;
};

template<std::size_t End, std::size_t... Values>
struct build<End, size_sequence<Values...>, build_strategy::repeat>:
  build<End, size_sequence<Values..., (Values + sizeof...(Values))...>>
{};

template<std::size_t End, typename State>
struct build<End, State, build_strategy::recurse>:
  build_concat<State, typename build<End - State::size>::type>
{};

template<typename T, T Offset, std::size_t... Values>
struct build_coerce<T, Offset, size_sequence<Values...>> {
  using type = constant_sequence<T, static_cast<T>(
    static_cast<T>(Values) + Offset)...
  >;
};

////////////
// concat //
////////////

template <typename T>
struct concat<T> {
  using type = T;
};

template <typename T, T... LHS, T... RHS>
struct concat<constant_sequence<T, LHS...>, constant_sequence<T, RHS...>> {
  using type = constant_sequence<T, LHS..., RHS...>;
};

template <typename T, typename U, typename... Tail>
struct concat<T, U, Tail...> {
  using type = typename concat<
    typename concat<T, U>::type,
    Tail...
  >::type;
};

//////////////
// reverse  //
//////////////

template<typename T, T Head, T... Tail, T... Values>
struct reverse<
  constant_sequence<T, Head, Tail...>, constant_sequence<T, Values...>
>:
  reverse<constant_sequence<T, Tail...>, constant_sequence<T, Head, Values...>>
{};

template<typename T, T... Values>
struct reverse<constant_sequence<T>, constant_sequence<T, Values...>> {
  using type = constant_sequence<T, Values...>;
};

////////////////
// polynomial //
////////////////

template <
  typename T,
  T Coefficient,
  T... Coefficients,
  T Variable,
  T VariableAccumulator,
  T Accumulator
>
struct polynomial<
  T,
  constant_sequence<T, Coefficient, Coefficients...>,
  Variable,
  VariableAccumulator,
  Accumulator
>:
  polynomial<T, constant_sequence<T, Coefficients...>,
    Variable,
    VariableAccumulator * Variable,
    Accumulator + VariableAccumulator * Coefficient
  >
{};

template <typename T, T Variable, T VariableAccumulator, T Accumulator>
struct polynomial<
  T, constant_sequence<T>, Variable, VariableAccumulator, Accumulator
> {
  using type = std::integral_constant<T, Accumulator>;
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

///////////////
// FATAL_STR //
///////////////

template <typename T, std::size_t Size>
constexpr std::size_t size(T const (&)[Size]) {
  static_assert(
    Size > 0,
    "expecting a string containing at least the null terminator"
  );
  return Size - 1;
}

#define FATAL_BUILD_STR_IMPL(Id, String, Class, Indexes) \
  struct Class { \
    using value_type = typename ::std::decay<decltype(*(String))>::type; \
    \
  private: \
    template <::std::size_t... Indexes> \
    static ::fatal::constant_sequence< \
      value_type, (String)[Indexes]... \
    > make_string(::fatal::size_sequence<Indexes...> *); \
    \
    static_assert( \
      !((String)[::fatal::detail::constant_sequence_impl::size(String)]), \
      "expecting a valid null-terminated string" \
    ); \
    \
  public: \
    using type = decltype(Class::make_string(static_cast< \
      ::fatal::indexes_sequence< \
        ::fatal::detail::constant_sequence_impl::size(String) \
      > *>(nullptr))); \
  }; \
  \
  using Id = typename Class::type

#undef FATAL_IMPL_HAS_MAKE_INTEGER_SEQ

template <std::size_t Excess, typename T, T... Values>
struct array {
  using size = std::integral_constant<std::size_t, sizeof...(Values) - Excess>;
  static constexpr T const data[sizeof...(Values)] = { Values... };
};

template <std::size_t Excess, typename T, T... Values>
constexpr T const array<Excess, T, Values...>::data[sizeof...(Values)];

} // namespace constant_sequence_impl {
} // namespace detail {

template <typename T, T... Values>
struct type_get_traits<constant_sequence<T, Values...>>:
  public type_get_traits<type_list<std::integral_constant<T, Values>...>>
{};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_deprecated_constant_sequence_h
