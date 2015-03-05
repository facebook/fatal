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

#include <iterator>

namespace fatal {

/////////////////////////////
// constant_sequence::type //
/////////////////////////////

template <typename TExpected, typename TSequence>
void check_type() {
  FATAL_EXPECT_SAME<TExpected, typename TSequence::type>();
}

FATAL_TEST(constant_sequence, type) {
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

///////////////////////////////////
// constant_sequence::push_front //
///////////////////////////////////

template <typename TExpected, typename TSeq, typename TSeq::type... Values>
void check_push_front() {
  FATAL_EXPECT_SAME<TExpected, typename TSeq::template push_front<Values...>>();
}

FATAL_TEST(constant_sequence, push_front) {
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
  FATAL_EXPECT_SAME<TExpected, typename TSeq::template push_back<Values...>>();
}

FATAL_TEST(constant_sequence, push_back) {
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
  FATAL_EXPECT_SAME<constant_sequence<int>, constant_range<int, 0, 0>>();
  FATAL_EXPECT_SAME<constant_sequence<int>, constant_range<int, 0, 0, true>>();

  FATAL_EXPECT_SAME<constant_sequence<char>, constant_range<char, 'a', 'a'>>();
  FATAL_EXPECT_SAME<
    constant_sequence<char>,
    constant_range<char, 'a', 'a', true>
  >();
}

FATAL_TEST(constant_range, unitary) {
  FATAL_EXPECT_SAME<constant_sequence<int, 0>, constant_range<int, 0, 1>>();
  FATAL_EXPECT_SAME<
    constant_sequence<int, 0>,
    constant_range<int, 0, 1, true>
  >();

  FATAL_EXPECT_SAME<
    constant_sequence<int, 0>,
    constant_range<int, 0, 0, false>
  >();

  FATAL_EXPECT_SAME<
    constant_sequence<char, 'a'>,
    constant_range<char, 'a', 'b'>
  >();
  FATAL_EXPECT_SAME<
    constant_sequence<char, 'a'>,
    constant_range<char, 'a', 'b', true>
  >();

  FATAL_EXPECT_SAME<
    constant_sequence<char, 'a'>,
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
