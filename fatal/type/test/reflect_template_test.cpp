/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/reflect_template.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

struct foo {};

template <int> struct T {};
template <int> struct S {};

template <typename... T> struct lst { typedef type_list<T...> list; };
template <typename... T> struct lst2 {};

template <int... values> using ilstt = lst<T<values>...>;
template <int... values> using ilsts = lst<S<values>...>;

template <typename T, T... values> struct seq {
  typedef constant_sequence<T, values...> cseq;
};
template <typename T, T... values> struct seq2 {};

template <int... values> using iseq = seq<int, values...>;
template <long... values> using lseq = seq<long, values...>;

/////////////////////////////////////
// reflect_template - non_template //
/////////////////////////////////////

TEST(reflect_template_non_template, type) {
  expect_same<
    foo,
    reflect_template<foo>::type
  >();
}

TEST(reflect_template_non_template, tag) {
  EXPECT_EQ(
    reflect_template_category::non_template,
    (reflect_template<foo>::category::value)
  );
}

TEST(reflect_template_non_template, is_same_tl) {
  EXPECT_FALSE((reflect_template<foo>::is_same_tl<lst>::value));
  EXPECT_FALSE((reflect_template<foo>::is_same_tl<lst2>::value));
}

TEST(reflect_template_non_template, is_same_tvl) {
  EXPECT_FALSE((reflect_template<foo>::is_same_tvl<seq>::value));
  EXPECT_FALSE((reflect_template<foo>::is_same_tvl<seq2>::value));
}

//////////////////////////////////
// reflect_template - type_list //
//////////////////////////////////

TEST(reflect_template_type_list, type) {
  expect_same<
    ilstt<0, 1, 2>,
    reflect_template<ilstt<0, 1, 2>>::type
  >();
}

TEST(reflect_template_type_list, tag) {
  EXPECT_EQ(
    reflect_template_category::type_list,
    (reflect_template<ilstt<0, 1, 2>>::category::value)
  );
}

TEST(reflect_template_type_list, rebind) {
  expect_same<
    ilsts<0, 1, 2>,
    reflect_template<ilstt<0, 1, 2>>::rebind<S<0>, S<1>, S<2>>
  >();
}

TEST(reflect_template_type_list, types) {
  expect_same<
    type_list<T<0>, T<1>, T<2>>,
    reflect_template<ilstt<0, 1, 2>>::types
  >();
}

TEST(reflect_template_type_list, is_same_tl) {
  EXPECT_TRUE((reflect_template<ilstt<0, 1, 2>>::is_same_tl<lst>::value));
  EXPECT_FALSE((reflect_template<ilstt<0, 1, 2>>::is_same_tl<lst2>::value));
}

TEST(reflect_template_type_list, is_same_tvl) {
  EXPECT_FALSE((reflect_template<ilstt<0, 1, 2>>::is_same_tvl<seq>::value));
  EXPECT_FALSE((reflect_template<ilstt<0, 1, 2>>::is_same_tvl<seq2>::value));
}

/////////////////////////////////////////
// reflect_template - typed_value_list //
/////////////////////////////////////////

TEST(reflect_template_typed_value_list, type) {
  expect_same<
    iseq<0, 1, 2>,
    reflect_template<iseq<0, 1, 2>>::type
  >();
}

TEST(reflect_template_typed_value_list, tag) {
  EXPECT_EQ(
    reflect_template_category::typed_value_list,
    (reflect_template<iseq<0, 1, 2>>::category::value)
  );
}

TEST(reflect_template_typed_value_list, value_type) {
  expect_same<reflect_template<iseq<0, 1, 2>>::value_type, int>();
  expect_same<reflect_template<lseq<0, 1, 2>>::value_type, long>();
}

TEST(reflect_template_typed_value_list, rebind) {
  expect_same<
    iseq<3, 4, 5>,
    reflect_template<iseq<0, 1, 2>>::rebind<>::type<3, 4, 5>
  >();

  expect_same<
    lseq<3, 4, 5>,
    reflect_template<iseq<0, 1, 2>>::rebind<long>::type<3, 4, 5>
  >();
}

TEST(reflect_template_typed_value_list, values) {
  expect_same<
    constant_sequence<int, 0, 1, 2>,
    reflect_template<iseq<0, 1, 2>>::values
  >();
}

TEST(reflect_template_typed_value_list, types) {
  expect_same<
    type_list<
      std::integral_constant<int, 0>,
      std::integral_constant<int, 1>,
      std::integral_constant<int, 2>
    >,
    reflect_template<iseq<0, 1, 2>>::types
  >();
}

TEST(reflect_template_typed_value_list, is_same_tl) {
  EXPECT_FALSE((reflect_template<iseq<0, 1, 2>>::is_same_tl<lst>::value));
  EXPECT_FALSE((reflect_template<iseq<0, 1, 2>>::is_same_tl<lst2>::value));
}

TEST(reflect_template_typed_value_list, is_same_tvl) {
  EXPECT_TRUE((reflect_template<iseq<0, 1, 2>>::is_same_tvl<seq>::value));
  EXPECT_FALSE((reflect_template<iseq<0, 1, 2>>::is_same_tvl<seq2>::value));
}

} // namespace fatal {
