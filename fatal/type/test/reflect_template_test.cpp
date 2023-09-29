/*
 *  Copyright (c) 2016, Facebook, Inc.
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

template <typename... T> struct lst { using list = type_list<T...>; };
template <typename... T> struct lst2 {};

template <int... values> using ilstt = lst<T<values>...>;
template <int... values> using ilsts = lst<S<values>...>;

template <typename T, T... values> struct seq {
  using cseq = sequence<T, values...>;
};
template <typename T, T... values> struct seq2 {};

template <int... values> using iseq = seq<int, values...>;
template <long... values> using lseq = seq<long, values...>;

/////////////////////////////////////
// reflect_template - non_template //
/////////////////////////////////////

FATAL_TEST(reflect_template_non_template, type) {
  FATAL_EXPECT_SAME<
    foo,
    reflect_template<foo>::type
  >();
}

FATAL_TEST(reflect_template_non_template, tag) {
  FATAL_EXPECT_EQ(
    reflect_template_category::non_template,
    (reflect_template<foo>::category::value)
  );
}

FATAL_TEST(reflect_template_non_template, is_same_tl) {
  FATAL_EXPECT_FALSE((reflect_template<foo>::is_same_tl<lst>::value));
  FATAL_EXPECT_FALSE((reflect_template<foo>::is_same_tl<lst2>::value));
}

FATAL_TEST(reflect_template_non_template, is_same_tvl) {
  FATAL_EXPECT_FALSE((reflect_template<foo>::is_same_tvl<seq>::value));
  FATAL_EXPECT_FALSE((reflect_template<foo>::is_same_tvl<seq2>::value));
}

//////////////////////////////////
// reflect_template - type_list //
//////////////////////////////////

FATAL_TEST(reflect_template_type_list, type) {
  FATAL_EXPECT_SAME<
    ilstt<0, 1, 2>,
    reflect_template<ilstt<0, 1, 2>>::type
  >();
}

FATAL_TEST(reflect_template_type_list, tag) {
  FATAL_EXPECT_EQ(
    reflect_template_category::type_list,
    (reflect_template<ilstt<0, 1, 2>>::category::value)
  );
}

FATAL_TEST(reflect_template_type_list, rebind) {
  FATAL_EXPECT_SAME<
    ilsts<0, 1, 2>,
    reflect_template<ilstt<0, 1, 2>>::rebind<S<0>, S<1>, S<2>>
  >();
}

FATAL_TEST(reflect_template_type_list, types) {
  FATAL_EXPECT_SAME<
    type_list<T<0>, T<1>, T<2>>,
    reflect_template<ilstt<0, 1, 2>>::types
  >();
}

FATAL_TEST(reflect_template_type_list, is_same_tl) {
  FATAL_EXPECT_TRUE((reflect_template<ilstt<0, 1, 2>>::is_same_tl<lst>::value));
  FATAL_EXPECT_FALSE(
    (reflect_template<ilstt<0, 1, 2>>::is_same_tl<lst2>::value)
  );
}

FATAL_TEST(reflect_template_type_list, is_same_tvl) {
  FATAL_EXPECT_FALSE(
    (reflect_template<ilstt<0, 1, 2>>::is_same_tvl<seq>::value)
  );
  FATAL_EXPECT_FALSE(
    (reflect_template<ilstt<0, 1, 2>>::is_same_tvl<seq2>::value)
  );
}

/////////////////////////////////////////
// reflect_template - typed_value_list //
/////////////////////////////////////////

FATAL_TEST(reflect_template_typed_value_list, type) {
  FATAL_EXPECT_SAME<
    iseq<0, 1, 2>,
    reflect_template<iseq<0, 1, 2>>::type
  >();
}

FATAL_TEST(reflect_template_typed_value_list, tag) {
  FATAL_EXPECT_EQ(
    reflect_template_category::typed_value_list,
    (reflect_template<iseq<0, 1, 2>>::category::value)
  );
}

FATAL_TEST(reflect_template_typed_value_list, value_type) {
  FATAL_EXPECT_SAME<reflect_template<iseq<0, 1, 2>>::value_type, int>();
  FATAL_EXPECT_SAME<reflect_template<lseq<0, 1, 2>>::value_type, long>();
}

FATAL_TEST(reflect_template_typed_value_list, rebind) {
  FATAL_EXPECT_SAME<
    iseq<3, 4, 5>,
    reflect_template<iseq<0, 1, 2>>::rebind<>::apply<3, 4, 5>
  >();

  FATAL_EXPECT_SAME<
    lseq<3, 4, 5>,
    reflect_template<iseq<0, 1, 2>>::rebind<long>::apply<3, 4, 5>
  >();
}

FATAL_TEST(reflect_template_typed_value_list, values) {
  FATAL_EXPECT_SAME<
    sequence<int, 0, 1, 2>,
    reflect_template<iseq<0, 1, 2>>::values
  >();
}

FATAL_TEST(reflect_template_typed_value_list, types) {
  FATAL_EXPECT_SAME<
    type_list<
      std::integral_constant<int, 0>,
      std::integral_constant<int, 1>,
      std::integral_constant<int, 2>
    >,
    reflect_template<iseq<0, 1, 2>>::types
  >();
}

FATAL_TEST(reflect_template_typed_value_list, is_same_tl) {
  FATAL_EXPECT_FALSE((reflect_template<iseq<0, 1, 2>>::is_same_tl<lst>::value));
  FATAL_EXPECT_FALSE(
    (reflect_template<iseq<0, 1, 2>>::is_same_tl<lst2>::value)
  );
}

FATAL_TEST(reflect_template_typed_value_list, is_same_tvl) {
  FATAL_EXPECT_TRUE((reflect_template<iseq<0, 1, 2>>::is_same_tvl<seq>::value));
  FATAL_EXPECT_FALSE(
    (reflect_template<iseq<0, 1, 2>>::is_same_tvl<seq2>::value)
  );
}

} // namespace fatal {
