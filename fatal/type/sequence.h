/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/list.h>

#include <type_traits>
#include <array>

namespace ftl {

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
struct constant_sequence {
  /**
   * The type of this sequence's values.
   */
  typedef T type;

  /**
   * This sequence as a type_list of std::integral_constant.
   *
   * Example:
   *
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `type_list<
   *  //   std::integral_constant<int, 1>,
   *  //   std::integral_constant<int, 2>,
   *  //   std::integral_constant<int, 3>
   *  // >`
   *  typedef seq::list result;
   */
  typedef type_list<std::integral_constant<type, Values>...> list;

  /**
   * Tells how many values this sequence has.
   */
  static constexpr std::size_t size = sizeof...(Values);

  /**
   * Tells whether this sequence is empty or not.
   * This is the same as `size == 0`.
   */
  static constexpr bool empty = size == 0;

  /**
   * Adds values to the beginning of this sequence.
   *
   * Example:
   *
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `constant_sequence<int, 9, 1, 2, 3>`
   *  typedef seq::push_front<9> result;
   */
  template <type... UValues>
  using push_front = constant_sequence<type, UValues..., Values...>;

  /**
   * Adds values to the end of this sequence.
   *
   * Example:
   *
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `constant_sequence<int, 1, 2, 3, 9>`
   *  typedef seq::push_back<9> result;
   */
  template <type... UValues>
  using push_back = constant_sequence<type, Values..., UValues...>;

  /**
   * Applies the values of this sequence to the given template.
   *
   * Example:
   *
   *  template <int... V> struct foo {};
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `foo<1, 2, 3>`
   *  typedef seq::apply<foo> result;
   */
  template <template <type...> class U>
  using apply = U<Values...>;

  /**
   * Applies the type and values of this sequence to the given template.
   *
   * Example:
   *
   *  template <typename T, int... V> struct foo {};
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `foo<int, 1, 2, 3>`
   *  typedef seq::typed_apply<foo> result;
   */
  template <template <typename, type...> class U>
  using typed_apply = U<type, Values...>;

  /**
   * Gets an array with the values from this sequence.
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `std::array<int, 3>` with values `{1, 2, 3}`
   *  auto result = seq::array();
   */
  typedef std::array<type, size> array_type;

  static constexpr array_type array() { return {{Values...}}; }

  /**
   * Gets an array with the values from this sequence, followed by
   * the given terminator value (defaults to 0).
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  typedef constant_sequence<int, 1, 2, 3> seq;
   *
   *  // yields `std::array<int, 4>` with values `{1, 2, 3, 0}`
   *  auto result1 = seq::z_array();
   *
   *  // yields `std::array<int, 4>` with values `{1, 2, 3, 99}`
   *  auto result2 = seq::z_array<99>();
   *
   *  // yields `std::array<int, 3>` with values `"hi"`
   *  auto hi = constant_sequence<char, 'h', 'i'>::z_array();
   */
  typedef std::array<type, size + 1> z_array_type;

  template <type Terminator = 0>
  static constexpr z_array_type z_array() { return {{Values..., Terminator}}; }
};

////////////////////////////////////////
// IMPLEMENTATION DETAILS DECLARATION //
////////////////////////////////////////

namespace detail {
namespace constant_sequence_impl {

template <typename T, T...> struct range_builder;

} // namespace constant_sequence_impl {
} // namespace detail {

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

/**
 * Builds a constant_sequence with elements in the range `[Begin, End]`.
 *
 * Example:
 *
 * // yields `constant_sequence<int, 1, 2, 3, 4>`
 * typedef constant_range<int, 1, 4> result1;
 *
 * // yields `constant_sequence<int, 1, 2>`
 * typedef constant_range<int, 1, 2> result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Begin, T End>
using constant_range = typename detail::constant_sequence_impl::range_builder<
  T, Begin, End
>::type;

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

template <typename T, T... Values>
constexpr std::size_t constant_sequence<T, Values...>::size;

template <typename T, T... Values>
constexpr bool constant_sequence<T, Values...>::empty;

///////////////////////////////////////
// IMPLEMENTATION DETAILS DEFINITION //
///////////////////////////////////////

namespace detail {
namespace constant_sequence_impl {

template <typename T>
struct range_builder<T> { typedef constant_sequence<T> type; };

template <typename T, T End>
struct range_builder<T, End, End> { typedef constant_sequence<T> type; };

template <typename T, T Begin, T End>
struct range_builder<T, Begin, End> {
  typedef typename range_builder<
    T, Begin == End ? End : Begin + 1, End
  >::type tail;

  static_assert(Begin <= End, "begin must not be past end");
  typedef typename std::conditional<
    Begin == End,
    constant_sequence<T>,
    typename tail::template push_front<Begin>
  >::type type;
};

} // namespace constant_sequence_impl {
} // namespace detail {
} // namespace ftl {
