/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

namespace fatal {

/////////////////////////////
// constant_sequence::type //
/////////////////////////////

template <typename TExpected, typename TSequence>
void check_type() {
  expect_same<TExpected, typename TSequence::type>();
}

TEST(constant_sequence, type) {
  check_type<int, constant_sequence<int>>();
  check_type<int, constant_sequence<int, 1>>();
  check_type<int, constant_sequence<int, 1, 2, 3, 4, 5>>();

  check_type<char, constant_sequence<char>>();
  check_type<char, constant_sequence<char, '1'>>();
  check_type<char, constant_sequence<char, '1', '2', '3', '4', '5'>>();
}

/////////////////////////////
// constant_sequence::list //
/////////////////////////////

template <typename T, T... Values>
void check_list() {
  expect_same<
    type_list<std::integral_constant<T, Values>...>,
    typename constant_sequence<T, Values...>::list
  >();
}

TEST(constant_sequence, list) {
  check_list<int>();
  check_list<int, 1>();
  check_list<int, 1, 2, 3, 4, 5>();

  check_list<char>();
  check_list<char, '1'>();
  check_list<char, '1', '2', '3', '4', '5'>();
}

/////////////////////////////
// constant_sequence::size //
/////////////////////////////

template <typename T, T... Values>
void check_size() {
  EXPECT_EQ(sizeof...(Values), (constant_sequence<T, Values...>::size));
}

TEST(constant_sequence, size) {
  check_size<int>();
  check_size<int, 1>();
  check_size<int, 1, 2, 3, 4, 5>();

  check_size<char>();
  check_size<char, '1'>();
  check_size<char, '1', '2', '3', '4', '5'>();
}

//////////////////////////////
// constant_sequence::empty //
//////////////////////////////

template <typename T, T... Values>
void check_empty() {
  bool b = sizeof...(Values) == 0;
  EXPECT_EQ(
    b,
    (constant_sequence<T, Values...>::empty)
  );
}

TEST(constant_sequence, empty) {
  check_empty<int>();
  check_empty<int, 1>();
  check_empty<int, 1, 2, 3, 4, 5>();

  check_empty<char>();
  check_empty<char, '1'>();
  check_empty<char, '1', '2', '3', '4', '5'>();
}

///////////////////////////////////
// constant_sequence::push_front //
///////////////////////////////////

template <typename TExpected, typename TSeq, typename TSeq::type... Values>
void check_push_front() {
  expect_same<TExpected, typename TSeq::template push_front<Values...>>();
}

TEST(constant_sequence, push_front) {
  check_push_front<
    constant_sequence<int, 1>,
    constant_sequence<int>, 1
  >();

  check_push_front<
    constant_sequence<int, 1>,
    constant_sequence<int, 1>
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1, 2, 3, 4, 5>
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 2, 3, 4, 5>, 1
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 3, 4, 5>, 1, 2
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 4, 5>, 1, 2, 3
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 5>, 1, 2, 3, 4
  >();

  check_push_front<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int>, 1, 2, 3, 4, 5
  >();

  check_push_front<
    constant_sequence<char, '1'>,
    constant_sequence<char>, '1'
  >();

  check_push_front<
    constant_sequence<char, '1'>,
    constant_sequence<char, '1'>
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1', '2', '3', '4', '5'>
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '2', '3', '4', '5'>, '1'
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '3', '4', '5'>, '1', '2'
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '4', '5'>, '1', '2', '3'
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '5'>, '1', '2', '3', '4'
  >();

  check_push_front<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char>, '1', '2', '3', '4', '5'
  >();
}

//////////////////////////////////
// constant_sequence::push_back //
//////////////////////////////////

template <typename TExpected, typename TSeq, typename TSeq::type... Values>
void check_push_back() {
  expect_same<TExpected, typename TSeq::template push_back<Values...>>();
}

TEST(constant_sequence, push_back) {
  check_push_back<
    constant_sequence<int, 1>,
    constant_sequence<int>, 1
  >();

  check_push_back<
    constant_sequence<int, 1>,
    constant_sequence<int, 1>
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1, 2, 3, 4, 5>
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1, 2, 3, 4>, 5
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1, 2, 3>, 4, 5
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1, 2>, 3, 4, 5
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int, 1>, 2, 3, 4, 5
  >();

  check_push_back<
    constant_sequence<int, 1, 2, 3, 4, 5>,
    constant_sequence<int>, 1, 2, 3, 4, 5
  >();

  check_push_back<
    constant_sequence<char, '1'>,
    constant_sequence<char>, '1'
  >();

  check_push_back<
    constant_sequence<char, '1'>,
    constant_sequence<char, '1'>
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1', '2', '3', '4', '5'>
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1', '2', '3', '4'>, '5'
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1', '2', '3'>, '4', '5'
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1', '2'>, '3', '4', '5'
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char, '1'>, '2', '3', '4', '5'
  >();

  check_push_back<
    constant_sequence<char, '1', '2', '3', '4', '5'>,
    constant_sequence<char>, '1', '2', '3', '4', '5'
  >();
}

//////////////////////////////
// constant_sequence::apply //
//////////////////////////////

template <typename T, T... Values>
struct as_array {
  typedef std::array<T, sizeof...(Values)> type;

  static type get() { return {{Values...}}; }
};

template <typename T>
struct curried_as_array {
  template <T... Values>
  using type = typename as_array<T, Values...>::type;

  template <T... Values>
  static type<Values...> get() { return as_array<T, Values...>::get(); }
};

template <typename T, T... Values>
void check_apply() {
  expect_same<
    typename as_array<T, Values...>::type,
    typename constant_sequence<T, Values...>::template apply<
      curried_as_array<T>::template type
    >
  >();
}

TEST(constant_sequence, apply) {
  check_apply<int>();
  check_apply<int, 1>();
  check_apply<int, 1, 2, 3, 4, 5>();

  check_apply<char>();
  check_apply<char, '1'>();
  check_apply<char, '1', '2', '3', '4', '5'>();
}

////////////////////////////////////
// constant_sequence::typed_apply //
////////////////////////////////////

template <typename T, T... Values>
void check_typed_apply() {
  typedef constant_sequence<T, Values...> seq;

  expect_same<
    typename as_array<T, Values...>::type,
    typename seq::template typed_apply<as_array>::type
  >();
}

TEST(constant_sequence, typed_apply) {
  check_typed_apply<int>();
  check_typed_apply<int, 1>();
  check_typed_apply<int, 1, 2, 3, 4, 5>();

  check_typed_apply<char>();
  check_typed_apply<char, '1'>();
  check_typed_apply<char, '1', '2', '3', '4', '5'>();
}

//////////////////////////////
// constant_sequence::array //
//////////////////////////////

template <typename T, T... Values>
void check_array() {
  typedef constant_sequence<T, Values...> seq;
  typedef as_array<T, Values...> as_array_t;

  expect_same<typename as_array_t::type, typename seq::array_type>();

  EXPECT_EQ(as_array_t::get(), seq::array());
}

TEST(constant_sequence, array) {
  check_array<int>();
  check_array<int, 1>();
  check_array<int, 1, 2, 3, 4, 5>();

  check_array<char>();
  check_array<char, '1'>();
  check_array<char, '1', '2', '3', '4', '5'>();
}

////////////////////////////////
// constant_sequence::z_array //
////////////////////////////////

template <typename T, T Terminator, T... Values>
void check_z_array() {
  typedef constant_sequence<T, Values...> seq;

  constexpr T terminator = 0;
  typedef as_array<T, Values..., Terminator> as_array_t;
  typedef as_array<T, Values..., terminator> z_as_array_t;

  expect_same<typename as_array_t::type, typename seq::z_array_type>();

  auto const array = seq::template z_array<Terminator>();
  EXPECT_EQ(as_array_t::get(), array);

  expect_same<typename z_as_array_t::type, typename seq::z_array_type>();

  auto const zarray = seq::template z_array<terminator>();
  EXPECT_EQ(z_as_array_t::get(), zarray);
}

TEST(constant_sequence, z_array) {
  check_z_array<int, 99>();
  check_z_array<int, 99, 1>();
  check_z_array<int, 99, 1, 2, 3, 4, 5>();

  check_z_array<char, 'x'>();
  check_z_array<char, 'x', '1'>();
  check_z_array<char, 'x', '1', '2', '3', '4', '5'>();
}

////////////////////
// constant_range //
////////////////////

template <typename T, T First, T... Mid, T Last>
void check_constant_range() {
  expect_same<
    constant_sequence<T, First, Mid..., Last>,
    constant_range<T, First, Last>
  >();
}

TEST(constant_range, constant_range) {
  check_array<int, 1, 2>();
  check_array<int, 1, 2, 3>();
  check_array<int, 1, 2, 3, 4>();
  check_array<int, 1, 2, 3, 4, 5>();

  check_array<char, '1', '2'>();
  check_array<char, '1', '2', '3'>();
  check_array<char, '1', '2', '3', '4'>();
  check_array<char, '1', '2', '3', '4', '5'>();
}

} // namespace fatal {
