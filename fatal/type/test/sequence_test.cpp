/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

#include <fatal/preprocessor.h>
#include <fatal/type/test/parse_sequence_input.h>

#include <iterator>
#include <type_traits>

namespace fatal {

template <int Value> using int_val = std::integral_constant<int, Value>;
template <char... Values> using char_seq = constant_sequence<char, Values...>;
template <int... Values> using int_seq = constant_sequence<int, Values...>;
template <int... Values> using int_lst = type_list<int_val<Values>...>;

using eis = int_seq<>;
using ecs = char_seq<>;

using s01 = int_seq<0, 1>;
using s23 = int_seq<2, 3>;
using s45 = int_seq<4, 5>;
using s67 = int_seq<6, 7>;
using s89 = int_seq<8, 9>;

#define A_STR "a"
#define Z_STR "a\0b\0c"
#define EMPTY_STR ""
#define HELLO_STR "hello"
#define TEST_STR "this is a test"
#define UTF8_STR u8"UTF-8 String"
#define UTF16_STR u"UTF-16 String"
#define UTF32_STR U"UTF-32 String"

FATAL_STR(a_str, A_STR);
FATAL_STR(z_str, Z_STR);
FATAL_STR(empty_str, EMPTY_STR);
FATAL_STR(hello_str, HELLO_STR);
FATAL_STR(test_str, TEST_STR);
FATAL_STR(utf8_str, UTF8_STR);
FATAL_STR(utf16_str, UTF16_STR);
FATAL_STR(utf32_str, UTF32_STR);

FATAL_STR(u0, "0");
FATAL_STR(u1, "1");
FATAL_STR(u42, "42");
FATAL_STR(s_56, "-56");

FATAL_STR(wu0, L"0");
FATAL_STR(wu1, L"1");
FATAL_STR(wu42, L"42");
FATAL_STR(ws_56, L"-56");

#define FATAL_CREATE_TEST_CALLS_IMPL(Fn) \
  Fn<a_str>(A_STR); \
  Fn<z_str>(Z_STR); \
  Fn<empty_str>(EMPTY_STR); \
  Fn<hello_str>(HELLO_STR); \
  Fn<test_str>(TEST_STR); \
  Fn<utf8_str>(UTF8_STR); \
  Fn<utf16_str>(UTF16_STR); \
  Fn<utf32_str>(UTF32_STR)

///////////////
// FATAL_STR //
///////////////

FATAL_STR(string_holder_s, "string_holder");
FATAL_STR(templated_string_holder_s, "templated_string_holder");

struct string_holder {
  FATAL_STR(str, "string_holder");
};

template <typename = void>
struct templated_string_holder {
  FATAL_STR(str, "templated_string_holder");
};

FATAL_TEST(fatal_str, members) {
  FATAL_EXPECT_SAME<string_holder_s, string_holder::str>();
  FATAL_EXPECT_SAME<
    templated_string_holder_s,
    templated_string_holder<>::str
  >();
}

//////////////////////////////////////////
// constant_sequence::type / value_type //
//////////////////////////////////////////

template <typename TExpected, typename TSequence>
void check_type() {
  FATAL_EXPECT_SAME<TExpected, typename TSequence::type>();
  FATAL_EXPECT_SAME<TExpected, typename TSequence::value_type>();
}

template <typename TCSTR, typename TChar, std::size_t Size>
void check_string_type(TChar const (&s)[Size]) {
  FATAL_EXPECT_SAME<TChar, typename TCSTR::type>();
  FATAL_EXPECT_SAME<TChar, typename TCSTR::value_type>();
}

FATAL_TEST(constant_sequence, type / value_type) {
  check_type<int, eis>();
  check_type<int, int_seq<1>>();
  check_type<int, int_seq<1, 2, 3, 4, 5>>();

  check_type<char, ecs>();
  check_type<char, char_seq<'1'>>();
  check_type<char, char_seq<'1', '2', '3', '4', '5'>>();

  FATAL_CREATE_TEST_CALLS_IMPL(check_string_type);
}

///////////////////////////////////
// constant_sequence::value_type //
///////////////////////////////////

template <typename TCSTR, typename TChar, std::size_t Size>
void check_value_type(TChar const (&s)[Size]) {
  FATAL_EXPECT_SAME<TChar, typename TCSTR::value_type>();
}

FATAL_TEST(constant_sequence, value_type) {
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
    typename constant_sequence<T, Values...>::template list<>
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

template <typename TString, std::size_t Expected>
void check_str_size() {
  FATAL_EXPECT_EQ(TString::size, Expected);
}

FATAL_TEST(constant_sequence, size) {
  check_size<int>();
  check_size<int, 1>();
  check_size<int, 1, 2, 3, 4, 5>();

  check_size<char>();
  check_size<char, '1'>();
  check_size<char, '1', '2', '3', '4', '5'>();

  check_str_size<a_str, 1>();
  check_str_size<z_str, 5>();
  check_str_size<empty_str, 0>();
  check_str_size<hello_str, 5>();
  check_str_size<test_str, 14>();
  check_str_size<utf8_str, 12>();
  check_str_size<utf16_str, 13>();
  check_str_size<utf32_str, 13>();
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

////////////////////////////////
// constant_sequence::reverse //
////////////////////////////////

template <typename T, T... Values>
struct check_reverse {
  using seq = constant_sequence<T, Values...>;

  template <T... Suffix>
  struct append {
    template <T... Expected>
    static void check() {
      FATAL_EXPECT_SAME<
        constant_sequence<T, Expected..., Suffix...>,
        typename seq::template reverse<Suffix...>
      >();
    }
  };
};

FATAL_TEST(constant_sequence, reverse) {
  check_reverse<int>::append<>::check<>();
  check_reverse<int, 1>::append<>::check<1>();
  check_reverse<int, 1, 2, 3, 4, 5>::append<>::check<5, 4, 3, 2, 1>();

  check_reverse<int>::append<0, -1>::check<>();
  check_reverse<int, 1>::append<0, -1>::check<1>();
  check_reverse<int, 1, 2, 3, 4, 5>::append<0, -1>
    ::check<5, 4, 3, 2, 1>();

  check_reverse<char>::append<>::check<>();
  check_reverse<char, '1'>::append<>::check<'1'>();
  check_reverse<char, '1', '2', '3', '4', '5'>::append<>
    ::check<'5', '4', '3', '2', '1'>();

  check_reverse<char>::append<'0', '_'>::check<>();
  check_reverse<char, '1'>::append<'0', '_'>::check<'1'>();
  check_reverse<char, '1', '2', '3', '4', '5'>::append<'0', '_'>
    ::check<'5', '4', '3', '2', '1'>();
}

///////////////////////////////////
// constant_sequence::polynomial //
///////////////////////////////////

template <int Expected, int Variable, int... Coefficients>
void check_polynomial() {
  using seq = constant_sequence<int, Coefficients...>;

  FATAL_EXPECT_SAME<
    std::integral_constant<int, Expected>,
    typename seq::template polynomial<Variable>
  >();
};

FATAL_TEST(constant_sequence, polynomial) {
  check_polynomial<-3 * 9 * 9 + 2 * 9 + 5, 9, 5, 2, -3>();
  check_polynomial<21 * 4 + 14, 4, 14, 21>();
  check_polynomial<18 * 9 * 9 + 0 * 9 + 1, 9, 1, 0, 18>();
}

///////////////////////////////////
// constant_sequence::interleave //
///////////////////////////////////

FATAL_TEST(constant_sequence, interleave) {
  FATAL_EXPECT_SAME<
    int_seq<>,
    int_seq<>::interleave<>
  >();

  FATAL_EXPECT_SAME<
    int_seq<>,
    int_seq<>::interleave<10>
  >();

  FATAL_EXPECT_SAME<
    int_seq<>,
    int_seq<>::interleave<10, 20, 30>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 2, 3, 4>,
    int_seq<0, 1, 2, 3, 4>::interleave<>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 10, 1, 10, 2, 10, 3, 10, 4>,
    int_seq<0, 1, 2, 3, 4>::interleave<10>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 10, 20, 30, 1, 10, 20, 30, 2, 10, 20, 30, 3, 10, 20, 30, 4>,
    int_seq<0, 1, 2, 3, 4>::interleave<10, 20, 30>
  >();
}

//////////////////////////////////
// constant_sequence::transform //
//////////////////////////////////

template <int Value>
using square_int = int_val<Value * Value>;

FATAL_TEST(constant_sequence, transform) {
  FATAL_EXPECT_SAME<
    int_seq<>,
    int_seq<>::transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<1>,
    int_seq<1>::transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<81>,
    int_seq<9>::transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 4, 9, 16, 25, 36, 49, 64, 81>,
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>::transform<square_int>
  >();
}

////////////////////////////////////////
// constant_sequence::typed_transform //
////////////////////////////////////////

template <typename T, T Value>
using typed_square_int = std::integral_constant<T, Value * Value>;

FATAL_TEST(constant_sequence, typed_transform) {
  FATAL_EXPECT_SAME<
    int_seq<>,
    int_seq<>::typed_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<1>,
    int_seq<1>::typed_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<81>,
    int_seq<9>::typed_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 4, 9, 16, 25, 36, 49, 64, 81>,
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>::typed_transform<typed_square_int>
  >();
}

///////////////////////////////////////
// constant_sequence::list_transform //
///////////////////////////////////////

FATAL_TEST(constant_sequence, list_transform) {
  FATAL_EXPECT_SAME<
    int_lst<>,
    int_seq<>::list_transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<1>,
    int_seq<1>::list_transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<81>,
    int_seq<9>::list_transform<square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<0, 1, 4, 9, 16, 25, 36, 49, 64, 81>,
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>::list_transform<square_int>
  >();
}

/////////////////////////////////////////////
// constant_sequence::typed_list_transform //
/////////////////////////////////////////////

FATAL_TEST(constant_sequence, typed_list_transform) {
  FATAL_EXPECT_SAME<
    int_lst<>,
    int_seq<>::typed_list_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<1>,
    int_seq<1>::typed_list_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<81>,
    int_seq<9>::typed_list_transform<typed_square_int>
  >();

  FATAL_EXPECT_SAME<
    int_lst<0, 1, 4, 9, 16, 25, 36, 49, 64, 81>,
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>
      ::typed_list_transform<typed_square_int>
  >();
}

//////////////////////////////
// constant_sequence::apply //
//////////////////////////////

template <typename T, T... Values>
struct test_as_array {
  using type = std::array<T, sizeof...(Values)>;

  static type get() { return {{Values...}}; }
};

template <typename T>
struct curried_as_array {
  template <T... Values>
  using type = typename test_as_array<T, Values...>::type;

  template <T... Values>
  static type<Values...> get() { return test_as_array<T, Values...>::get(); }
};

template <typename T, T... Values>
void check_apply() {
  FATAL_EXPECT_SAME<
    typename test_as_array<T, Values...>::type,
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
    typename test_as_array<T, Values...>::type,
    typename seq::template typed_apply<test_as_array>::type
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

//////////////////////////////
// constant_sequence::parse //
//////////////////////////////

FATAL_TEST(constant_sequence, parse) {
# define TEST_IMPL(T, Value, TChar, ...) \
  do { \
    using seq = constant_sequence<TChar, __VA_ARGS__>; \
    \
    FATAL_EXPECT_SAME< \
      std::integral_constant<T, Value>, \
      seq::parse<T> \
    >(); \
  } while (false)

  FATAL_IMPL_PARSE_SEQUENCE_TEST_CALLS(TEST_IMPL);

# undef TEST_IMPL
}

////////////////////////////////////////////////////
// constant_sequence::{array|data|z_array|z_data} //
////////////////////////////////////////////////////

template <typename TArray, typename TAsArray, typename TData>
void check_array_data(TData data) {
  using array = TArray;
  using as_array_t = TAsArray;

  FATAL_EXPECT_EQ(as_array_t::get(), array::get);
}

template <typename T, T... Values>
void check_array_data() {
  using seq = constant_sequence<T, Values...>;

  check_array_data<typename seq::template array<>, test_as_array<T, Values...>>(
    seq::data()
  );

  check_array_data<
    typename seq::template z_array<>,
    test_as_array<T, Values..., 0>
  >(
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


///////////////////////////////
// constant_sequence::string //
///////////////////////////////

template <typename TCSTR, typename TChar, std::size_t Size>
void check_string(TChar const (&s)[Size]) {
  static_assert(Size > 0, "expected a valid null-terminated string");
  constexpr auto size = Size - 1;

  typedef std::basic_string<TChar> string_t;

  FATAL_EXPECT_SAME<string_t, typename TCSTR::template string_type<>>();

  auto const string = TCSTR::string();

  FATAL_EXPECT_EQ(size, string.size());
  FATAL_EXPECT_EQ(string_t(s, size), string);
}

FATAL_TEST(constant_sequence, string) {
  FATAL_CREATE_TEST_CALLS_IMPL(check_string);
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

//////////////////////////
// to_constant_sequence //
//////////////////////////

FATAL_TEST(to_constant_sequence, sanity_check) {
# define TEST_IMPL(T, Value, TChar, ...) \
  do { \
    FATAL_EXPECT_SAME< \
      constant_sequence<TChar, __VA_ARGS__>, \
      to_constant_sequence<TChar>::apply<T, Value> \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constant_sequence<TChar, __VA_ARGS__>, \
      to_constant_sequence<TChar>::bind<T>::apply<Value> \
    >(); \
  } while (false)

  FATAL_IMPL_PARSE_SEQUENCE_TEST_CALLS(TEST_IMPL);

# undef TEST_IMPL
}

////////////////////
// constant_range //
////////////////////

template <typename T, T... Values>
void check_constant_range() {
  using closed_sequence = constant_sequence<T, Values...>;
  using closed_list = typename closed_sequence::template list<>;
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

//////////////////////
// indexes_sequence //
//////////////////////

template <std::size_t... Values>
void check_indexes_sequence() {
  FATAL_EXPECT_SAME<
    size_sequence<Values...>,
    indexes_sequence<sizeof...(Values)>
  >();
}

FATAL_TEST(indexes_sequence, sanity_check) {
  check_indexes_sequence<>();
  check_indexes_sequence<0>();
  check_indexes_sequence<0, 1>();
  check_indexes_sequence<0, 1, 2>();
  check_indexes_sequence<0, 1, 2, 3>();
  check_indexes_sequence<0, 1, 2, 3, 4>();
  check_indexes_sequence<0, 1, 2, 3, 4, 5>();
}

} // namespace fatal {
