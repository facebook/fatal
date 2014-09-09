/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/pair.h>

#include <fatal/test/driver.h>

namespace ftl {

template <typename> struct T1 {};
template <typename> struct T2 {};
template <typename...> struct T3 {};
template <typename...> struct T4 {};
template <typename, typename...> struct T5 {};
template <typename, typename...> struct T6 {};

///////////////
// type_pair //
///////////////

template <typename TFirst, typename TSecond>
void check_type_pair_types() {
  typedef type_pair<TFirst, TSecond> pair_t;

  expect_same<TFirst, typename pair_t::first>();
  expect_same<TSecond, typename pair_t::second>();
}

TEST(type_pair, types) {
  check_type_pair_types<int, double>();
  check_type_pair_types<int, int>();
  check_type_pair_types<void, long>();
  check_type_pair_types<
    type_pair<bool, int>,
    type_pair<std::string, std::string>
  >();
}

///////////////////////
// type_pair::invert //
///////////////////////

template <typename TFirst, typename TSecond>
void check_invert() {
  typedef type_pair<TFirst, TSecond> pair;
  typedef type_pair<TSecond, TFirst> expected;
  expect_same<expected, typename pair::invert>();
}

TEST(type_pair, invert) {
  check_invert<int, double>();
  check_invert<int, int>();
  check_invert<void, long>();
  check_invert<
    type_pair<bool, int>,
    type_pair<std::string, std::string>
  >();
}

//////////////////////////
// type_pair::transform //
//////////////////////////

template <
  typename TFirst,
  typename TSecond,
  typename TExpectedFirst,
  typename TExpectedSecond,
  template <typename...> class TFirstTransform = transform::identity,
  template <typename...> class TSecondTransform = transform::identity
>
void check_transform() {
  typedef type_pair<TExpectedFirst, TExpectedSecond> expected;
  typedef typename type_pair<TFirst, TSecond>::template transform<
    TFirstTransform, TSecondTransform
  > actual;

  expect_same<expected, actual>();
}

TEST(type_pair, transform) {
  check_transform<int, double, int, double>();
  check_transform<int, double, T1<int>, double, T1>();
  check_transform<int, double, T1<int>, T2<double>, T1, T2>();
  check_transform<int, double, T3<int>, double, T3>();
  check_transform<int, double, T3<int>, T4<double>, T3, T4>();
  check_transform<int, double, T5<int>, double, T5>();
  check_transform<int, double, T5<int>, T6<double>, T5, T6>();
}

//////////////////////////
// type_pair_from::list //
//////////////////////////

template <
  typename T,
  typename TExpectedFirst,
  typename TExpectedSecond,
  template <typename...> class TFirstTransform = transform::identity,
  template <typename...> class TSecondTransform = transform::identity
>
void check_type_pair_from() {
  typedef type_pair<TExpectedFirst, TExpectedSecond> expected;
  typedef typename type_pair_from<
    TFirstTransform, TSecondTransform
  >::template type<T> actual;

  expect_same<expected, actual>();
}

TEST(type_pair_from, list) {
  check_type_pair_from<int, int, int>();
  check_type_pair_from<int, T1<int>, int, T1>();
  check_type_pair_from<int, T1<int>, T2<int>, T1, T2>();
  check_type_pair_from<int, T3<int>, int, T3>();
  check_type_pair_from<int, T3<int>, T4<int>, T3, T4>();
  check_type_pair_from<int, T5<int>, int, T5>();
  check_type_pair_from<int, T5<int>, T6<int>, T5, T6>();
}

//////////////
// type_get //
//////////////

template <typename TFirst, typename TSecond>
void check_type_get() {
  typedef type_pair<TFirst, TSecond> pair_t;

  expect_same<typename pair_t::first, type_get<0>::template type<pair_t>>();
  expect_same<typename pair_t::second, type_get<1>::template type<pair_t>>();

  expect_same<typename pair_t::first, type_get_first<pair_t>>();
  expect_same<typename pair_t::second, type_get_second<pair_t>>();
}

TEST(type_get, type_pair) {
  check_type_get<int, double>();
  check_type_get<int, int>();
  check_type_get<void, long>();
  check_type_get<
    type_pair<bool, int>,
    type_pair<std::string, std::string>
  >();
}

/////////////////////////////
// type_get_first_comparer //
/////////////////////////////

template <int LHSFirst, int LHSSecond, int RHSFirst, int RHSSecond>
class Foo {
  typedef type_pair<
    std::integral_constant<int, LHSFirst>,
    std::integral_constant<int, LHSSecond>
  > lhs;

  typedef type_pair<
    std::integral_constant<int, RHSFirst>,
    std::integral_constant<int, RHSSecond>
  > rhs;

public:
  template <template <typename, typename> class TComparer>
  using comparison = std::integral_constant<
    bool, TComparer<lhs, rhs>::value
  >;
};

TEST(type_get, first_comparer) {
  EXPECT_TRUE((
    Foo<5, 99, 8, 1>::comparison<
      type_get_first_comparer<>::template type
    >::value
  ));

  EXPECT_TRUE((
    Foo<5, 99, 8, 1>::comparison<
      type_get_first_comparer<constants_comparison_lt>::template type
    >::value
  ));

  EXPECT_FALSE((
    Foo<5, 99, 8, 1>::comparison<
      type_get_first_comparer<constants_comparison_gt>::template type
    >::value
  ));
}

//////////////////////////////
// type_get_second_comparer //
//////////////////////////////

TEST(type_get, second_comparer) {
  EXPECT_TRUE((
    Foo<99, 5, 1, 8>::comparison<
      type_get_second_comparer<>::template type
    >::value
  ));

  EXPECT_TRUE((
    Foo<99, 5, 1, 8>::comparison<
      type_get_second_comparer<constants_comparison_lt>::template type
    >::value
  ));

  EXPECT_FALSE((
    Foo<99, 5, 1, 8>::comparison<
      type_get_second_comparer<constants_comparison_gt>::template type
    >::value
  ));
}

} // namespace ftl {
