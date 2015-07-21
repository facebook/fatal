/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_sequence_h
#define FATAL_INCLUDE_fatal_type_sequence_h

#include <fatal/container/constant_array.h>
#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {
namespace detail {
namespace constant_sequence_impl {

template <template <typename U, U...> class TSequence, typename T, T... Values>
struct constant_sequence {
  /**
   * The type of this sequence's values.
   */
  using type = T;

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
  using list = type_list<constant<Values>...>;

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
  using index_of = typename list::template index_of<constant<Needle>>;

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
  using checked_index_of = typename list::template checked_index_of<
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
  using contains = typename list::template contains<constant<Needle>>;

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
  using push_back = TSequence<type, Values..., UValues...>;

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
  using push_front = TSequence<type, UValues..., Values...>;

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
  using concat = typename type_list<Args...>::template apply<
    list::template concat,
    get_member_type::list
  >::template apply_typed_values<type, TSequence>;

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
  using tail = typename list::template tail<Offset>
    ::template apply_typed_values<type, TSequence>;

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
    typename list::template split<Index>::first
      ::template apply_typed_values<type, TSequence>,
    typename list::template split<Index>::second
      ::template apply_typed_values<type, TSequence>
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
  using slice = typename list::template slice<Begin, End>
    ::template apply_typed_values<type, TSequence>;

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
  using left = typename list::template left<N>
    ::template apply_typed_values<type, TSequence>;

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
  using right = typename list::template right<N>
    ::template apply_typed_values<type, TSequence>;

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
  using array = constant_array<type, Values..., Suffix...>;

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
  static constexpr type const *data() { return array<>::data(); }

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
  using z_array = constant_array<
    type, Values..., Suffix..., static_cast<type>(0)
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
  static constexpr type const *z_data() { return z_array<>::data(); }

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

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

template <template <typename U, U...> class TSequence, typename T, T... Values>
constexpr std::size_t constant_sequence<TSequence, T, Values...>::size;

template <template <typename U, U...> class TSequence, typename T, T... Values>
constexpr bool constant_sequence<TSequence, T, Values...>::empty;

} // namespace constant_sequence_impl {
} // namespace detail {

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
struct constant_sequence:
  public detail::constant_sequence_impl::constant_sequence<
    constant_sequence, T, Values...
  >
{};

// TODO: DOCUMENT AND TEST
template <std::size_t... Values>
using size_sequence = constant_sequence<std::size_t, Values...>;

////////////////////////////////////////
// IMPLEMENTATION DETAILS DECLARATION //
////////////////////////////////////////

namespace detail {
namespace range_builder_impl {

template <bool, bool, typename T, T, T> struct build;

} // namespace constant_sequence_impl {
} // namespace detail {

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

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
using constant_range = typename detail::range_builder_impl::build<
  false, OpenEnd, T, Begin, End
>::type;

/**
 * A convenient shortcut that builds a `constant_sequence` of
 * type `std::size_t` with elements in the range `[0, End)`.
 *
 * Example:
 *
 * // yields `constant_sequence<std::size_t, 1, 2, 3, 4>`
 * using result1 = indexes_sequence<5>;
 *
 * // yields `constant_sequence<std::size_t, 1, 2>`
 * using result2 = indexes_sequence<3>;
 *
 * // yields `constant_sequence<std::size_t, 1, 2, 3, 4, 5>`
 * using result3 = indexes_sequence<6>;
 *
 * // yields `constant_sequence<std::size_t,  3>`
 * using result4 = indexes_sequence<4>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Size>
using indexes_sequence = typename detail::range_builder_impl::build<
  false, true, std::size_t, 0, Size
>::type;

///////////////////////////////////////
// IMPLEMENTATION DETAILS DEFINITION //
///////////////////////////////////////

namespace detail {
namespace range_builder_impl {

template <typename T, T End>
struct build<false, true, T, End, End> {
  using type = constant_sequence<T>;
};

template <typename T, T End>
struct build<false, false, T, End, End> {
  using type = constant_sequence<T, End>;
};

template <bool OpenEnd, typename T, T Current, T End>
struct build<false, OpenEnd, T, Current, End> {
  static_assert(Current < End, "begin must not be past end");

  using type = typename build<false, OpenEnd, T, Current + 1, End>::type
    ::template push_front<Current>;
};

} // namespace constant_sequence_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_sequence_h
