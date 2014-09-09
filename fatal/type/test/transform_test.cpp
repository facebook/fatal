/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/transform.h>

#include <fatal/test/driver.h>

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace ftl {
namespace transform {

template <typename> struct T1 {};
template <typename> struct T2 {};
template <typename> struct T3 {};
template <typename...> struct V1 {};

//////////////
// identity //
//////////////

TEST(transform_identity, identity) {
  expect_same<int, identity<int>>();
  expect_same<std::string, identity<std::string>>();
  expect_same<double, identity<identity<double>>>();
}

///////////
// fixed //
///////////

TEST(transform_fixed, fixed) {
  typedef fixed<int> i;
  expect_same<int, i::type<>>();
  expect_same<int, i::type<bool>>();
  expect_same<int, i::type<int>>();
  expect_same<int, i::type<std::string>>();
  expect_same<int, i::type<double>>();
  expect_same<int, i::type<double, void>>();
  expect_same<int, i::type<transform::identity<double>>>();

  typedef fixed<bool> b;
  expect_same<bool, b::type<>>();
  expect_same<bool, b::type<bool>>();
  expect_same<bool, b::type<int>>();
  expect_same<bool, b::type<std::string>>();
  expect_same<bool, b::type<double>>();
  expect_same<bool, b::type<double, void>>();
  expect_same<bool, b::type<transform::identity<double>>>();
}

//////////////
// constant //
//////////////

TEST(transform_constant, constant) {
  constexpr int ic = 12345;
  typedef constant<std::decay<decltype(ic)>::type, ic> i;
  EXPECT_EQ(ic, (i::type<>::value));
  EXPECT_EQ(ic, (i::type<bool>::value));
  EXPECT_EQ(ic, (i::type<int>::value));
  EXPECT_EQ(ic, (i::type<std::string>::value));
  EXPECT_EQ(ic, (i::type<double>::value));
  EXPECT_EQ(ic, (i::type<double, void>::value));
  EXPECT_EQ(ic, (i::type<transform::identity<double>>::value));

  constexpr bool bc = true;
  typedef constant<std::decay<decltype(bc)>::type, bc> b;
  EXPECT_EQ(bc, (b::type<>::value));
  EXPECT_EQ(bc, (b::type<bool>::value));
  EXPECT_EQ(bc, (b::type<int>::value));
  EXPECT_EQ(bc, (b::type<std::string>::value));
  EXPECT_EQ(bc, (b::type<double>::value));
  EXPECT_EQ(bc, (b::type<double, void>::value));
  EXPECT_EQ(bc, (b::type<transform::identity<double>>::value));
}

//////////////
// type_mod //
//////////////

template <template <typename...> class TTransform, typename T>
void check_type_mod() {
  expect_same<
    typename TTransform<T>::type,
    typename type_mod<TTransform>::template type<T>
  >();
}

TEST(transform_type_mod, type_mod) {
  check_type_mod<std::add_const, int>();
  check_type_mod<std::add_const, bool>();
  check_type_mod<std::add_const, double>();
  check_type_mod<std::add_const, std::string>();
  check_type_mod<std::add_pointer, int>();
  check_type_mod<std::add_pointer, bool>();
  check_type_mod<std::add_pointer, double>();
  check_type_mod<std::add_pointer, std::string>();
  check_type_mod<std::add_lvalue_reference, int>();
  check_type_mod<std::add_lvalue_reference, bool>();
  check_type_mod<std::add_lvalue_reference, double>();
  check_type_mod<std::add_lvalue_reference, std::string>();
  check_type_mod<std::add_rvalue_reference, int>();
  check_type_mod<std::add_rvalue_reference, bool>();
  check_type_mod<std::add_rvalue_reference, double>();
  check_type_mod<std::add_rvalue_reference, std::string>();
}

////////////
// nested //
////////////

template <typename TNested, typename TExpected, typename T>
void check_nested() {
  expect_same<TExpected, typename TNested::template type<T>>();
}

TEST(transform_nested, nested) {
  typedef nested<T1, T2, T3> ttt;

  check_nested<ttt, T3<T2<T1<int>>>, int>();
  check_nested<ttt, T3<T2<T1<int &&>>>, int &&>();
  check_nested<ttt, T3<T2<T1<int const &>>>, int const &>();
  check_nested<ttt, T3<T2<T1<std::string>>>, std::string>();
  check_nested<ttt, T3<T2<T1<std::string &&>>>, std::string &&>();
  check_nested<ttt, T3<T2<T1<std::string const &>>>, std::string const &>();

  typedef nested<type_mod<std::decay>::template type, ttt::template type> dttt;

  check_nested<dttt, T3<T2<T1<int>>>, int>();
  check_nested<dttt, T3<T2<T1<int>>>, int &&>();
  check_nested<dttt, T3<T2<T1<int>>>, int const &>();
  check_nested<dttt, T3<T2<T1<std::string>>>, std::string>();
  check_nested<dttt, T3<T2<T1<std::string>>>, std::string &&>();
  check_nested<dttt, T3<T2<T1<std::string>>>, std::string const &>();
}

//////////////////
// alias::apply //
//////////////////

TEST(transform_alias, apply) {
  typedef alias<std::tuple, int, double> c1;

  expect_same<
    std::tuple<int, double>,
    c1::apply<>
  >();

  expect_same<
    std::tuple<int, double, long, std::string, bool, float>,
    c1::apply<long, std::string, bool, float>
  >();
}

/////////////////
// alias::type //
/////////////////

TEST(transform_alias, type) {
  typedef alias<std::tuple, int, double> c1;

  expect_same<
    std::tuple<int, double>,
    c1::type<>
  >();

  expect_same<
    std::tuple<int, double, long, std::string, bool, float>,
    c1::type<long, std::string, bool, float>
  >();
}

//////////////////
// alias::curry //
//////////////////

TEST(transform_alias, curry) {
  typedef alias<std::tuple, int, double> c1;
  typedef c1::curry<long, std::string> c2;

  expect_same<
    std::tuple<int, double, long, std::string>,
    c2::type<>
  >();

  expect_same<
    std::tuple<int, double, long, std::string, bool, float>,
    c2::type<bool, float>
  >();
}

///////////////////
// alias::rebind //
///////////////////

TEST(transform_alias, rebind) {
  typedef alias<std::tuple, int, double> c1;
  typedef c1::rebind<long, std::string> c2;

  expect_same<
    std::tuple<long, std::string>,
    c2::type<>
  >();

  expect_same<
    std::tuple<long, std::string, bool, float>,
    c2::type<bool, float>
  >();
}

////////////////////
// alias::uncurry //
////////////////////

TEST(transform_alias, uncurry) {
  typedef alias<std::tuple, int, double> c1;

  expect_same<
    std::tuple<long, std::string>,
    c1::uncurry<long, std::string>
  >();

  typedef alias<c1::template uncurry, long, std::string> c2;

  expect_same<
    std::tuple<long, std::string>,
    c2::type<>
  >();

  expect_same<
    std::tuple<long, std::string, bool, float>,
    c2::type<bool, float>
  >();
}

/////////////////////////
// alias:: rebind_args //
/////////////////////////

TEST(transform_alias, rebind_args) {
  typedef alias<std::tuple, int, double> c1;

  expect_same<
    V1<int, double>,
    c1::rebind_args<V1>::type<>
  >();

  expect_same<
    V1<int, double, bool, float>,
    c1::rebind_args<V1, bool, float>::type<>
  >();
}

////////////////////////
// alias:: apply_args //
////////////////////////

TEST(transform_alias, apply_args) {
  typedef alias<std::tuple, int, double> c1;

  expect_same<
    V1<int, double>,
    c1::apply_args<V1>
  >();

  expect_same<
    V1<int, double, bool, float>,
    c1::apply_args<V1, bool, float>
  >();
}

} // namespace transform {
} // namespace ftl {
