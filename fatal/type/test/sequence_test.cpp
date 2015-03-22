/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

#include <fatal/preprocessor.h>

#include <iterator>

namespace fatal {

template <char... Values> using char_seq = constant_sequence<char, Values...>;
template <int... Values> using int_seq = constant_sequence<int, Values...>;

using eis = int_seq<>;
using ecs = char_seq<>;

using s01 = int_seq<0, 1>;
using s23 = int_seq<2, 3>;
using s45 = int_seq<4, 5>;
using s67 = int_seq<6, 7>;
using s89 = int_seq<8, 9>;

/////////////////////////////
// constant_sequence::type //
/////////////////////////////

template <typename TExpected, typename TSequence>
void check_type() {
  FATAL_EXPECT_SAME<TExpected, typename TSequence::type>();
}

FATAL_TEST(constant_sequence, type) {
  check_type<int, eis>();
  check_type<int, int_seq<1>>();
  check_type<int, int_seq<1, 2, 3, 4, 5>>();

  check_type<char, ecs>();
  check_type<char, char_seq<'1'>>();
  check_type<char, char_seq<'1', '2', '3', '4', '5'>>();
}

/////////////////////////////////
// constant_sequence::constant //
/////////////////////////////////

template <typename T, T...> struct constant_test_impl;

template <typename T, T Value, T... Values>
struct constant_test_impl<T, Value, Values...> {
  static void test() {
    using seq = constant_sequence<T>;

    FATAL_EXPECT_SAME<
      std::integral_constant<T, Value>,
      typename seq::template constant<Value>
    >();

    constant_test_impl<T, Values...>::test();
  }
};

template <typename T>
struct constant_test_impl<T> {
  static void test() {}
};

template <typename T, T... Values>
void constant_test() {
  constant_test_impl<T, Values...>::test();
}

FATAL_TEST(constant_sequence, constant) {
  constant_test<unsigned short, 1, 1, 2, 3, 4, 5, 6>();
  constant_test<int, 0, 1, 2, 3, 4, 5, 6>();
  constant_test<long, 0, 1, 2, 3, 4, 5, 6>();
  constant_test<char, '0', '1', '2', '3', '4', '5', '6'>();
}

/////////////////////////////
// constant_sequence::list //
/////////////////////////////

template <typename T, T... Values>
void check_list() {
  FATAL_EXPECT_SAME<
    type_list<std::integral_constant<T, Values>...>,
    typename constant_sequence<T, Values...>::list
  >();
}

FATAL_TEST(constant_sequence, list) {
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
  FATAL_EXPECT_EQ(sizeof...(Values), (constant_sequence<T, Values...>::size));
}

FATAL_TEST(constant_sequence, size) {
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
  FATAL_EXPECT_EQ(
    b,
    (constant_sequence<T, Values...>::empty)
  );
}

FATAL_TEST(constant_sequence, empty) {
  check_empty<int>();
  check_empty<int, 1>();
  check_empty<int, 1, 2, 3, 4, 5>();

  check_empty<char>();
  check_empty<char, '1'>();
  check_empty<char, '1', '2', '3', '4', '5'>();
}

/////////////////////////////////
// constant_sequence::index_of //
/////////////////////////////////

FATAL_TEST(constant_sequence, index_of) {
  using is = int_seq<0, 10, 20, 30, 40, 50, 60>;
  FATAL_EXPECT_EQ(0, is::index_of<0>::value);
  FATAL_EXPECT_EQ(1, is::index_of<10>::value);
  FATAL_EXPECT_EQ(2, is::index_of<20>::value);
  FATAL_EXPECT_EQ(3, is::index_of<30>::value);
  FATAL_EXPECT_EQ(4, is::index_of<40>::value);
  FATAL_EXPECT_EQ(5, is::index_of<50>::value);
  FATAL_EXPECT_EQ(6, is::index_of<60>::value);
  FATAL_EXPECT_EQ(7, is::index_of<99>::value);

  using cs = char_seq<'a', 'b', 'c', 'd', 'e', 'f'>;
  FATAL_EXPECT_EQ(0, cs::index_of<'a'>::value);
  FATAL_EXPECT_EQ(1, cs::index_of<'b'>::value);
  FATAL_EXPECT_EQ(2, cs::index_of<'c'>::value);
  FATAL_EXPECT_EQ(3, cs::index_of<'d'>::value);
  FATAL_EXPECT_EQ(4, cs::index_of<'e'>::value);
  FATAL_EXPECT_EQ(5, cs::index_of<'f'>::value);
  FATAL_EXPECT_EQ(6, cs::index_of<'g'>::value);
  FATAL_EXPECT_EQ(6, cs::index_of<'h'>::value);

  FATAL_EXPECT_EQ(0, eis::index_of<0>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<10>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<20>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<30>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<40>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<50>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<60>::value);
  FATAL_EXPECT_EQ(0, eis::index_of<99>::value);

  FATAL_EXPECT_EQ(0, ecs::index_of<'a'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'b'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'c'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'d'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'e'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'f'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'g'>::value);
  FATAL_EXPECT_EQ(0, ecs::index_of<'h'>::value);
}

/////////////////////////////////////////
// constant_sequence::checked_index_of //
/////////////////////////////////////////

FATAL_TEST(constant_sequence, checked_index_of) {
  using is = int_seq<0, 10, 20, 30, 40, 50, 60>;
  FATAL_EXPECT_EQ(0, is::checked_index_of<0>::value);
  FATAL_EXPECT_EQ(1, is::checked_index_of<10>::value);
  FATAL_EXPECT_EQ(2, is::checked_index_of<20>::value);
  FATAL_EXPECT_EQ(3, is::checked_index_of<30>::value);
  FATAL_EXPECT_EQ(4, is::checked_index_of<40>::value);
  FATAL_EXPECT_EQ(5, is::checked_index_of<50>::value);
  FATAL_EXPECT_EQ(6, is::checked_index_of<60>::value);

  using cs = char_seq<'a', 'b', 'c', 'd', 'e', 'f'>;
  FATAL_EXPECT_EQ(0, cs::checked_index_of<'a'>::value);
  FATAL_EXPECT_EQ(1, cs::checked_index_of<'b'>::value);
  FATAL_EXPECT_EQ(2, cs::checked_index_of<'c'>::value);
  FATAL_EXPECT_EQ(3, cs::checked_index_of<'d'>::value);
  FATAL_EXPECT_EQ(4, cs::checked_index_of<'e'>::value);
  FATAL_EXPECT_EQ(5, cs::checked_index_of<'f'>::value);
}

/////////////////////////////////
// constant_sequence::contains //
/////////////////////////////////

FATAL_TEST(constant_sequence, contains) {
  using is = int_seq<0, 10, 20, 30, 40, 50, 60>;
  FATAL_EXPECT_TRUE(is::contains<0>::value);
  FATAL_EXPECT_TRUE(is::contains<10>::value);
  FATAL_EXPECT_TRUE(is::contains<20>::value);
  FATAL_EXPECT_TRUE(is::contains<30>::value);
  FATAL_EXPECT_TRUE(is::contains<40>::value);
  FATAL_EXPECT_TRUE(is::contains<50>::value);
  FATAL_EXPECT_TRUE(is::contains<60>::value);
  FATAL_EXPECT_FALSE(is::contains<99>::value);

  using cs = char_seq<'a', 'b', 'c', 'd', 'e', 'f'>;
  FATAL_EXPECT_TRUE(cs::contains<'a'>::value);
  FATAL_EXPECT_TRUE(cs::contains<'b'>::value);
  FATAL_EXPECT_TRUE(cs::contains<'c'>::value);
  FATAL_EXPECT_TRUE(cs::contains<'d'>::value);
  FATAL_EXPECT_TRUE(cs::contains<'e'>::value);
  FATAL_EXPECT_TRUE(cs::contains<'f'>::value);
  FATAL_EXPECT_FALSE(cs::contains<'g'>::value);
  FATAL_EXPECT_FALSE(cs::contains<'h'>::value);

  FATAL_EXPECT_FALSE(eis::contains<0>::value);
  FATAL_EXPECT_FALSE(eis::contains<10>::value);
  FATAL_EXPECT_FALSE(eis::contains<20>::value);
  FATAL_EXPECT_FALSE(eis::contains<30>::value);
  FATAL_EXPECT_FALSE(eis::contains<40>::value);
  FATAL_EXPECT_FALSE(eis::contains<50>::value);
  FATAL_EXPECT_FALSE(eis::contains<60>::value);
  FATAL_EXPECT_FALSE(eis::contains<99>::value);

  FATAL_EXPECT_FALSE(ecs::contains<'a'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'b'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'c'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'d'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'e'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'f'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'g'>::value);
  FATAL_EXPECT_FALSE(ecs::contains<'h'>::value);
}

//////////////////////////////////
// constant_sequence::push_back //
//////////////////////////////////

template <typename TExpected, typename TSeq, typename TSeq::type... Values>
void check_push_back() {
  FATAL_EXPECT_SAME<TExpected, typename TSeq::template push_back<Values...>>();
}

FATAL_TEST(constant_sequence, push_back) {
  check_push_back<int_seq<1>, eis, 1>();
  check_push_back<int_seq<1>, int_seq<1>>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, int_seq<1, 2, 3, 4, 5>>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, int_seq<1, 2, 3, 4>, 5>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, int_seq<1, 2, 3>, 4, 5>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, int_seq<1, 2>, 3, 4, 5>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, int_seq<1>, 2, 3, 4, 5>();
  check_push_back<int_seq<1, 2, 3, 4, 5>, eis, 1, 2, 3, 4, 5>();
  check_push_back<char_seq<'1'>, ecs, '1'>();
  check_push_back<char_seq<'1'>, char_seq<'1'>>();

  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1', '2', '3', '4', '5'>
  >();
  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1', '2', '3', '4'>, '5'
  >();
  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1', '2', '3'>, '4', '5'
  >();
  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1', '2'>, '3', '4', '5'
  >();
  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1'>, '2', '3', '4', '5'
  >();
  check_push_back<
    char_seq<'1', '2', '3', '4', '5'>,
    ecs, '1', '2', '3', '4', '5'
  >();
}

///////////////////////////////////
// constant_sequence::push_front //
///////////////////////////////////

template <typename TExpected, typename TSeq, typename TSeq::type... Values>
void check_push_front() {
  FATAL_EXPECT_SAME<TExpected, typename TSeq::template push_front<Values...>>();
}

FATAL_TEST(constant_sequence, push_front) {
  check_push_front<int_seq<1>, eis, 1>();
  check_push_front<int_seq<1>, int_seq<1>>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, int_seq<1, 2, 3, 4, 5>>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, int_seq<2, 3, 4, 5>, 1>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, int_seq<3, 4, 5>, 1, 2>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, int_seq<4, 5>, 1, 2, 3>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, int_seq<5>, 1, 2, 3, 4>();
  check_push_front<int_seq<1, 2, 3, 4, 5>, eis, 1, 2, 3, 4, 5>();
  check_push_front<char_seq<'1'>, ecs, '1'>();
  check_push_front<char_seq<'1'>, char_seq<'1'>>();

  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'1', '2', '3', '4', '5'>
  >();
  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'2', '3', '4', '5'>, '1'
  >();
  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'3', '4', '5'>, '1', '2'
  >();
  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'4', '5'>, '1', '2', '3'
  >();
  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    char_seq<'5'>, '1', '2', '3', '4'
  >();
  check_push_front<
    char_seq<'1', '2', '3', '4', '5'>,
    ecs, '1', '2', '3', '4', '5'
  >();
}

///////////////////////////////
// constant_sequence::concat //
///////////////////////////////

FATAL_TEST(constant_sequence, concat) {
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<s23>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<eis, s23>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<eis, s23, eis>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<eis, eis, s23>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<s23, eis, eis>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, s01::concat<s23, eis>>();
  FATAL_EXPECT_SAME<int_seq<0, 1, 2, 3>, eis::concat<s01, s23>>();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<s23, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<eis, s23, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<s23, eis, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<s23, s45, eis, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<s23, s45, s67, eis, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    s01::concat<s23, s45, s67, s89, eis>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, s23, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<eis, s01, s23, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, eis, s23, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, s23, eis, s45, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, s23, s45, eis, s67, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, s23, s45, s67, eis, s89>
  >();
  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    eis::concat<s01, s23, s45, s67, s89, eis>
  >();

  FATAL_EXPECT_SAME<eis, eis::concat<>>();
  FATAL_EXPECT_SAME<eis, eis::concat<eis>>();
  FATAL_EXPECT_SAME<eis, eis::concat<eis, eis>>();
}

/////////////////////////////
// constant_sequence::tail //
/////////////////////////////

FATAL_TEST(constant_sequence, tail) {
  using s = int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80, 90>;

  FATAL_EXPECT_SAME<s, s::tail<0>>();
  FATAL_EXPECT_SAME<int_seq<10, 20, 30, 40, 50, 60, 70, 80, 90>, s::tail<1>>();
  FATAL_EXPECT_SAME<int_seq<20, 30, 40, 50, 60, 70, 80, 90>, s::tail<2>>();
  FATAL_EXPECT_SAME<int_seq<30, 40, 50, 60, 70, 80, 90>, s::tail<3>>();
  FATAL_EXPECT_SAME<int_seq<40, 50, 60, 70, 80, 90>, s::tail<4>>();
  FATAL_EXPECT_SAME<int_seq<50, 60, 70, 80, 90>, s::tail<5>>();
  FATAL_EXPECT_SAME<int_seq<60, 70, 80, 90>, s::tail<6>>();
  FATAL_EXPECT_SAME<int_seq<70, 80, 90>, s::tail<7>>();
  FATAL_EXPECT_SAME<int_seq<80, 90>, s::tail<8>>();
  FATAL_EXPECT_SAME<int_seq<90>, s::tail<9>>();
  FATAL_EXPECT_SAME<eis, s::tail<10>>();

  FATAL_EXPECT_SAME<s01, s01::tail<0>>();
  FATAL_EXPECT_SAME<int_seq<1>, s01::tail<1>>();
  FATAL_EXPECT_SAME<eis, s01::tail<2>>();

  FATAL_EXPECT_SAME<eis, eis::tail<0>>();
}

//////////////////////////////
// constant_sequence::split //
//////////////////////////////

FATAL_TEST(constant_sequence, split) {
# define TEST_IMPL(What, Index, Left, Right) \
  FATAL_EXPECT_SAME< \
    type_pair<FATAL_UNPARENTHESIZE(Left), FATAL_UNPARENTHESIZE(Right)>, \
    What::split<Index> \
  >()

  using s = int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80, 90>;

  TEST_IMPL(s, 0, eis, s);
  TEST_IMPL(s, 1, int_seq<0>, (int_seq<10, 20, 30, 40, 50, 60, 70, 80, 90>));
  TEST_IMPL(s, 2, (int_seq<0, 10>), (int_seq<20, 30, 40, 50, 60, 70, 80, 90>));
  TEST_IMPL(s, 3, (int_seq<0, 10, 20>), (int_seq<30, 40, 50, 60, 70, 80, 90>));
  TEST_IMPL(s, 4, (int_seq<0, 10, 20, 30>), (int_seq<40, 50, 60, 70, 80, 90>));
  TEST_IMPL(s, 5, (int_seq<0, 10, 20, 30, 40>), (int_seq<50, 60, 70, 80, 90>));
  TEST_IMPL(s, 6, (int_seq<0, 10, 20, 30, 40, 50>), (int_seq<60, 70, 80, 90>));
  TEST_IMPL(s, 7, (int_seq<0, 10, 20, 30, 40, 50, 60>), (int_seq<70, 80, 90>));
  TEST_IMPL(s, 8, (int_seq<0, 10, 20, 30, 40, 50, 60, 70>), (int_seq<80, 90>));
  TEST_IMPL(s, 9, (int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80>), (int_seq<90>));
  TEST_IMPL(s, 10, s, eis);

  TEST_IMPL(s01, 0, eis, s01);
  TEST_IMPL(s01, 1, int_seq<0>, int_seq<1>);
  TEST_IMPL(s01, 2, s01, eis);

  TEST_IMPL(eis, 0, eis, eis);

# undef TEST_IMPL
}

//////////////////////////////
// constant_sequence::slice //
//////////////////////////////

FATAL_TEST(constant_sequence, slice) {
# define TEST_IMPL(What, Begin, End, ...) \
  FATAL_EXPECT_SAME<int_seq<__VA_ARGS__>, What::slice<Begin, End>>()

  using s = int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80, 90>;

  TEST_IMPL(s, 0, 0);
  TEST_IMPL(s, 0, 1, 0);
  TEST_IMPL(s, 0, 2, 0, 10);
  TEST_IMPL(s, 0, 3, 0, 10, 20);
  TEST_IMPL(s, 0, 4, 0, 10, 20, 30);
  TEST_IMPL(s, 0, 5, 0, 10, 20, 30, 40);
  TEST_IMPL(s, 0, 6, 0, 10, 20, 30, 40, 50);
  TEST_IMPL(s, 0, 7, 0, 10, 20, 30, 40, 50, 60);
  TEST_IMPL(s, 0, 8, 0, 10, 20, 30, 40, 50, 60, 70);
  TEST_IMPL(s, 0, 9, 0, 10, 20, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 0, 10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 1, 1);
  TEST_IMPL(s, 1, 2, 10);
  TEST_IMPL(s, 1, 3, 10, 20);
  TEST_IMPL(s, 1, 4, 10, 20, 30);
  TEST_IMPL(s, 1, 5, 10, 20, 30, 40);
  TEST_IMPL(s, 1, 6, 10, 20, 30, 40, 50);
  TEST_IMPL(s, 1, 7, 10, 20, 30, 40, 50, 60);
  TEST_IMPL(s, 1, 8, 10, 20, 30, 40, 50, 60, 70);
  TEST_IMPL(s, 1, 9, 10, 20, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 1, 10, 10, 20, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 2, 2);
  TEST_IMPL(s, 2, 3, 20);
  TEST_IMPL(s, 2, 4, 20, 30);
  TEST_IMPL(s, 2, 5, 20, 30, 40);
  TEST_IMPL(s, 2, 6, 20, 30, 40, 50);
  TEST_IMPL(s, 2, 7, 20, 30, 40, 50, 60);
  TEST_IMPL(s, 2, 8, 20, 30, 40, 50, 60, 70);
  TEST_IMPL(s, 2, 9, 20, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 2, 10, 20, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 3, 3);
  TEST_IMPL(s, 3, 4, 30);
  TEST_IMPL(s, 3, 5, 30, 40);
  TEST_IMPL(s, 3, 6, 30, 40, 50);
  TEST_IMPL(s, 3, 7, 30, 40, 50, 60);
  TEST_IMPL(s, 3, 8, 30, 40, 50, 60, 70);
  TEST_IMPL(s, 3, 9, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 3, 10, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 4, 4);
  TEST_IMPL(s, 4, 5, 40);
  TEST_IMPL(s, 4, 6, 40, 50);
  TEST_IMPL(s, 4, 7, 40, 50, 60);
  TEST_IMPL(s, 4, 8, 40, 50, 60, 70);
  TEST_IMPL(s, 4, 9, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 4, 10, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 5, 5);
  TEST_IMPL(s, 5, 6, 50);
  TEST_IMPL(s, 5, 7, 50, 60);
  TEST_IMPL(s, 5, 8, 50, 60, 70);
  TEST_IMPL(s, 5, 9, 50, 60, 70, 80);
  TEST_IMPL(s, 5, 10, 50, 60, 70, 80, 90);

  TEST_IMPL(s, 6, 6);
  TEST_IMPL(s, 6, 7, 60);
  TEST_IMPL(s, 6, 8, 60, 70);
  TEST_IMPL(s, 6, 9, 60, 70, 80);
  TEST_IMPL(s, 6, 10, 60, 70, 80, 90);

  TEST_IMPL(s, 7, 7);
  TEST_IMPL(s, 7, 8, 70);
  TEST_IMPL(s, 7, 9, 70, 80);
  TEST_IMPL(s, 7, 10, 70, 80, 90);

  TEST_IMPL(s, 8, 8);
  TEST_IMPL(s, 8, 9, 80);
  TEST_IMPL(s, 8, 10, 80, 90);

  TEST_IMPL(s, 9, 9);
  TEST_IMPL(s, 9, 10, 90);

  TEST_IMPL(s, 10, 10);

  TEST_IMPL(s01, 0, 0);
  TEST_IMPL(s01, 0, 1, 0);
  TEST_IMPL(s01, 0, 2, 0, 1);
  TEST_IMPL(s01, 1, 1);
  TEST_IMPL(s01, 1, 2, 1);
  TEST_IMPL(s01, 2, 2);

  TEST_IMPL(eis, 0, 0);

# undef TEST_IMPL
}

/////////////////////////////
// constant_sequence::left //
/////////////////////////////

FATAL_TEST(constant_sequence, left) {
# define TEST_IMPL(What, Size, ...) \
  FATAL_EXPECT_SAME<int_seq<__VA_ARGS__>, What::left<Size>>()

  using s = int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80, 90>;

  TEST_IMPL(s, 0);
  TEST_IMPL(s, 1, 0);
  TEST_IMPL(s, 2, 0, 10);
  TEST_IMPL(s, 3, 0, 10, 20);
  TEST_IMPL(s, 4, 0, 10, 20, 30);
  TEST_IMPL(s, 5, 0, 10, 20, 30, 40);
  TEST_IMPL(s, 6, 0, 10, 20, 30, 40, 50);
  TEST_IMPL(s, 7, 0, 10, 20, 30, 40, 50, 60);
  TEST_IMPL(s, 8, 0, 10, 20, 30, 40, 50, 60, 70);
  TEST_IMPL(s, 9, 0, 10, 20, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(s, 10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s01, 0);
  TEST_IMPL(s01, 1, 0);
  TEST_IMPL(s01, 2, 0, 1);

  TEST_IMPL(eis, 0);

# undef TEST_IMPL
}

//////////////////////////////
// constant_sequence::right //
//////////////////////////////

FATAL_TEST(constant_sequence, right) {
# define TEST_IMPL(What, Size, ...) \
  FATAL_EXPECT_SAME<int_seq<__VA_ARGS__>, What::right<Size>>()

  using s = int_seq<0, 10, 20, 30, 40, 50, 60, 70, 80, 90>;

  TEST_IMPL(s, 0);
  TEST_IMPL(s, 1, 90);
  TEST_IMPL(s, 2, 80, 90);
  TEST_IMPL(s, 3, 70, 80, 90);
  TEST_IMPL(s, 4, 60, 70, 80, 90);
  TEST_IMPL(s, 5, 50, 60, 70, 80, 90);
  TEST_IMPL(s, 6, 40, 50, 60, 70, 80, 90);
  TEST_IMPL(s, 7, 30, 40, 50, 60, 70, 80, 90);
  TEST_IMPL(s, 8, 20, 30, 40, 50, 60, 70, 80, 90);
  TEST_IMPL(s, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90);
  TEST_IMPL(s, 10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90);

  TEST_IMPL(s01, 0);
  TEST_IMPL(s01, 1, 1);
  TEST_IMPL(s01, 2, 0, 1);

  TEST_IMPL(eis, 0);

# undef TEST_IMPL
}

//////////////////////////////
// constant_sequence::apply //
//////////////////////////////

template <typename T, T... Values>
struct as_array {
  using type = std::array<T, sizeof...(Values)>;

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
  FATAL_EXPECT_SAME<
    typename as_array<T, Values...>::type,
    typename constant_sequence<T, Values...>::template apply<
      curried_as_array<T>::template type
    >
  >();
}

FATAL_TEST(constant_sequence, apply) {
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

  FATAL_EXPECT_SAME<
    typename as_array<T, Values...>::type,
    typename seq::template typed_apply<as_array>::type
  >();
}

FATAL_TEST(constant_sequence, typed_apply) {
  check_typed_apply<int>();
  check_typed_apply<int, 1>();
  check_typed_apply<int, 1, 2, 3, 4, 5>();

  check_typed_apply<char>();
  check_typed_apply<char, '1'>();
  check_typed_apply<char, '1', '2', '3', '4', '5'>();
}

////////////////////////////////////////////////////
// constant_sequence::{array|data|z_array|z_data} //
////////////////////////////////////////////////////

template <typename TArray, typename TAsArray, typename TData>
void check_array_data(TData data) {
  using array = TArray;
  using as_array_t = TAsArray;

  FATAL_EXPECT_SAME<
    typename as_array_t::type::value_type,
    typename array::value_type
  >();

  FATAL_EXPECT_SAME<typename as_array_t::type, typename array::type>();

  FATAL_EXPECT_EQ(as_array_t::get().empty(), array::empty);
  FATAL_EXPECT_EQ(as_array_t::get().size(), array::size);

  FATAL_EXPECT_EQ(as_array_t::get(), array::get);

  for (std::size_t i = 0; i < as_array_t::get().size(); ++i) {
    FATAL_EXPECT_LT(i, array::size);
    FATAL_EXPECT_EQ(as_array_t::get().data()[i], array::data()[i]);
  }
}

template <typename T, T... Values>
void check_array_data() {
  using seq = constant_sequence<T, Values...>;

  check_array_data<typename seq::template array<>, as_array<T, Values...>>(
    seq::data()
  );

  check_array_data<typename seq::template z_array<>, as_array<T, Values..., 0>>(
    seq::z_data()
  );
}

FATAL_TEST(constant_sequence, array_data) {
  check_array_data<int>();
  check_array_data<int, 1>();
  check_array_data<int, 1, 2, 3, 4, 5>();
  check_array_data<int, 99, 43, 57, 0, 100>();

  check_array_data<char>();
  check_array_data<char, '1'>();
  check_array_data<char, '1', '2', '3', '4', '5'>();
  check_array_data<char, 'a', 'r', 'r', 'a', 'y'>();
  check_array_data<char, 'z', '_', 'a', 'r', 'r', 'a', 'y'>();
}

/////////////////////////////
// constant_sequence::init //
/////////////////////////////

FATAL_TEST(constant_sequence, init) {
# define TEST_IMPL(...) \
  FATAL_EXPECT_EQ( \
    (std::vector<int>{__VA_ARGS__}), \
    (int_seq<__VA_ARGS__>::init<std::vector<int>>()) \
  );

  TEST_IMPL();
  TEST_IMPL(0);
  TEST_IMPL(0, 10);
  TEST_IMPL(0, 10, 20);
  TEST_IMPL(0, 10, 20, 30);
  TEST_IMPL(0, 10, 20, 30, 40);
  TEST_IMPL(0, 10, 20, 30, 40, 50);
  TEST_IMPL(0, 10, 20, 30, 40, 50, 60);
  TEST_IMPL(0, 10, 20, 30, 40, 50, 60, 70);
  TEST_IMPL(0, 10, 20, 30, 40, 50, 60, 70, 80);
  TEST_IMPL(0, 10, 20, 30, 40, 50, 60, 70, 80, 90);

# undef TEST_IMPL
}

////////////////////
// constant_range //
////////////////////

template <typename T, T... Values>
void check_constant_range() {
  using closed_sequence = constant_sequence<T, Values...>;
  using closed_list = typename closed_sequence::list;
  using open_list = typename closed_list::template slice<
    0, closed_list::size - 1
  >;
  using open_sequence = typename open_list::template apply_typed_values<
    T, constant_sequence
  >;
  using first = typename closed_list::template at<0>;
  using last = typename closed_list::template at<closed_list::size - 1>;

  FATAL_EXPECT_SAME<
    open_sequence,
    constant_range<T, first::value, last::value>
  >();

  FATAL_EXPECT_SAME<
    open_sequence,
    constant_range<T, first::value, last::value, true>
  >();

  FATAL_EXPECT_SAME<
    closed_sequence,
    constant_range<T, first::value, last::value, false>
  >();
}

FATAL_TEST(constant_range, empty) {
  FATAL_EXPECT_SAME<eis, constant_range<int, 0, 0>>();
  FATAL_EXPECT_SAME<eis, constant_range<int, 0, 0, true>>();

  FATAL_EXPECT_SAME<ecs, constant_range<char, 'a', 'a'>>();
  FATAL_EXPECT_SAME<ecs, constant_range<char, 'a', 'a', true>>();
}

FATAL_TEST(constant_range, unitary) {
  FATAL_EXPECT_SAME<int_seq<0>, constant_range<int, 0, 1>>();
  FATAL_EXPECT_SAME<
    int_seq<0>,
    constant_range<int, 0, 1, true>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0>,
    constant_range<int, 0, 0, false>
  >();

  FATAL_EXPECT_SAME<
    char_seq<'a'>,
    constant_range<char, 'a', 'b'>
  >();
  FATAL_EXPECT_SAME<
    char_seq<'a'>,
    constant_range<char, 'a', 'b', true>
  >();

  FATAL_EXPECT_SAME<
    char_seq<'a'>,
    constant_range<char, 'a', 'a', false>
  >();
}

FATAL_TEST(constant_range, int) {
  //check_constant_range<int, 1>();
  check_constant_range<int, 1, 2>();
  check_constant_range<int, 1, 2, 3>();
  check_constant_range<int, 1, 2, 3, 4>();
  check_constant_range<int, 1, 2, 3, 4, 5>();
}

FATAL_TEST(constant_range, char) {
  //check_constant_range<char, '1'>();
  check_constant_range<char, '1', '2'>();
  check_constant_range<char, '1', '2', '3'>();
  check_constant_range<char, '1', '2', '3', '4'>();
  check_constant_range<char, '1', '2', '3', '4', '5'>();
}

} // namespace fatal {
