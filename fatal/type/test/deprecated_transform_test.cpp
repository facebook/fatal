/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/deprecated/transform.h>

#include <fatal/test/driver.h>
#include <fatal/preprocessor.h>

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace fatal {

namespace {
struct X0 {};
struct X1 {};
struct X2 {};
struct X3 {};
template <typename T> struct T0 { using t0 = T; };
template <typename T> struct T1 { using t1 = T; };
template <typename T> struct T2 { using t2 = T; };
template <typename T> struct T3 { using t3 = T; };
template <typename...> struct V0 {};
template <typename...> struct V1 {};
template <typename...> struct V2 {};
template <typename...> struct V3 {};

template <typename TFirst, typename TSecond, typename TThird>
struct test_triplet {
  using first = TFirst;
  using second = TSecond;
  using third = TThird;
};

template <int Value> using int_val = std::integral_constant<int, Value>;
template <int Value>
struct op {
  template <typename T>
  using mul = std::integral_constant<int, T::value * Value>;
};

template <typename> struct test_tag {};

template <typename... Args>
struct test_list {
  template <template <typename...> class TPredicate, typename T>
  using conditional = test_list<
    typename std::conditional<TPredicate<Args>::value, Args, T>::type...
  >;

  template <template <typename...> class TTransform, typename... TSuffix>
  using apply = TTransform<Args..., TSuffix...>;

  template <template <typename...> class TTransform>
  using transform = test_list<TTransform<Args>...>;
};

template <bool... Values>
using bool_seq = test_list<std::integral_constant<bool, Values>...>;

template <typename T, T...> struct test_seq {};

template <typename> struct is_list: std::false_type {};
template <typename... Args>
struct is_list<test_list<Args...>>: std::true_type {};

template <typename> struct is_tuple: std::false_type {};
template <typename... Args>
struct is_tuple<std::tuple<Args...>>: std::true_type {};

template <typename> struct is_vector: std::false_type {};
template <typename... Args>
struct is_vector<std::vector<Args...>>: std::true_type {};

template <typename> struct is_string: std::false_type {};
template <typename... Args>
struct is_string<std::basic_string<Args...>>: std::true_type {};

template <typename> struct tuple_as_list;
template <typename... Args>
struct tuple_as_list<std::tuple<Args...>> { using type = test_list<Args...>; };

template <typename T> using get_t0 = typename T::t0;
template <typename T> using get_t1 = typename T::t1;
template <typename T> using get_t2 = typename T::t2;
template <typename T> using get_t3 = typename T::t3;
template <typename T> using get_type = typename T::type;
template <typename T> using get_value_type = typename T::value_type;

template <typename T> using as_bool = std::integral_constant<bool, T::value>;

template <typename T> struct is_T0_list: std::false_type {};
template <typename T> struct is_T0_list<T0<T>>: is_list<T> {};

template <template <typename...> class TTransform, typename T, T... Values>
using transform_values = TTransform<std::integral_constant<T, Values>...>;

template <template <typename...> class TTransform, int... Values>
using transform_int = transform_values<TTransform, int, Values...>;

template <template <typename...> class TTransform, bool... Values>
using transform_bool = transform_values<TTransform, bool, Values...>;

struct complete_type {};
struct incomplete_type;
}

/////////////////
// is_complete //
/////////////////

FATAL_TEST(traits, is_complete) {
  FATAL_EXPECT_TRUE((is_complete<int>::value));
  FATAL_EXPECT_TRUE((is_complete<std::string>::value));
  FATAL_EXPECT_TRUE((is_complete<complete_type>::value));
  FATAL_EXPECT_FALSE((is_complete<incomplete_type>::value));

  using pair = std::pair<void, void>;
  FATAL_EXPECT_TRUE((is_complete<std::tuple_element<0, pair>>::value));
  FATAL_EXPECT_TRUE((is_complete<std::tuple_element<1, pair>>::value));
  FATAL_EXPECT_FALSE((is_complete<std::tuple_element<2, pair>>::value));
}

//////////////
// identity //
//////////////

FATAL_TEST(identity, identity) {
  FATAL_EXPECT_SAME <int, identity<int>>();
  FATAL_EXPECT_SAME <std::string, identity<std::string>>();
  FATAL_EXPECT_SAME <double, identity<identity<double>>>();
}

/////////////////////
// fixed_transform //
/////////////////////

FATAL_TEST(fixed_transform, fixed_transform) {
  typedef fixed_transform<int> i;
  FATAL_EXPECT_SAME<int, i::apply<>>();
  FATAL_EXPECT_SAME<int, i::apply<bool>>();
  FATAL_EXPECT_SAME<int, i::apply<int>>();
  FATAL_EXPECT_SAME<int, i::apply<std::string>>();
  FATAL_EXPECT_SAME<int, i::apply<double>>();
  FATAL_EXPECT_SAME<int, i::apply<double, void>>();
  FATAL_EXPECT_SAME<int, i::apply<identity<double>>>();

  typedef fixed_transform<bool> b;
  FATAL_EXPECT_SAME<bool, b::apply<>>();
  FATAL_EXPECT_SAME<bool, b::apply<bool>>();
  FATAL_EXPECT_SAME<bool, b::apply<int>>();
  FATAL_EXPECT_SAME<bool, b::apply<std::string>>();
  FATAL_EXPECT_SAME<bool, b::apply<double>>();
  FATAL_EXPECT_SAME<bool, b::apply<double, void>>();
  FATAL_EXPECT_SAME<bool, b::apply<identity<double>>>();
}

////////////////////////
// constant_transform //
////////////////////////

FATAL_TEST(constant_transform, constant_transform) {
  constexpr int ic = 12345;
  typedef constant_transform<std::decay<decltype(ic)>::type, ic> i;
  FATAL_EXPECT_EQ(ic, (i::apply<>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<bool>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<int>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<std::string>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<double>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<double, void>::value));
  FATAL_EXPECT_EQ(ic, (i::apply<identity<double>>::value));

  constexpr bool bc = true;
  typedef constant_transform<std::decay<decltype(bc)>::type, bc> b;
  FATAL_EXPECT_EQ(bc, (b::apply<>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<bool>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<int>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<std::string>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<double>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<double, void>::value));
  FATAL_EXPECT_EQ(bc, (b::apply<identity<double>>::value));
}

/////////////
// compose //
/////////////

template <typename TNested, typename TExpected, typename T>
void check_compose() {
  FATAL_EXPECT_SAME<TExpected, typename TNested::template apply<T>>();
}

FATAL_TEST(compose, compose) {
  typedef compose<T1, T2, T3> ttt;

  check_compose<ttt, T3<T2<T1<int>>>, int>();
  check_compose<ttt, T3<T2<T1<int &&>>>, int &&>();
  check_compose<ttt, T3<T2<T1<int const &>>>, int const &>();
  check_compose<ttt, T3<T2<T1<std::string>>>, std::string>();
  check_compose<ttt, T3<T2<T1<std::string &&>>>, std::string &&>();
  check_compose<
    ttt, T3<T2<T1<std::string const &>>>, std::string const &
  >();

  using dttt = compose<
    type_member_transform<std::decay>::template apply, ttt::template apply
  >;

  check_compose<dttt, T3<T2<T1<int>>>, int>();
  check_compose<dttt, T3<T2<T1<int>>>, int &&>();
  check_compose<dttt, T3<T2<T1<int>>>, int const &>();
  check_compose<dttt, T3<T2<T1<std::string>>>, std::string>();
  check_compose<dttt, T3<T2<T1<std::string>>>, std::string &&>();
  check_compose<dttt, T3<T2<T1<std::string>>>, std::string const &>();
}

////////////////
// arithmetic //
////////////////

template <int... Values>
using test_add = transform_int<arithmetic::add, Values...>;

FATAL_TEST(arithmetic, add) {
# define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    using expected = std::decay<decltype(Expected)>::type; \
    FATAL_EXPECT_SAME<expected, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

  FATAL_TEST_IMPL(0,    test_add<0>);
  FATAL_TEST_IMPL(1,    test_add<1>);
  FATAL_TEST_IMPL(2,    test_add<2>);
  FATAL_TEST_IMPL(56,   test_add<56>);
  FATAL_TEST_IMPL(100,  test_add<100>);

  FATAL_TEST_IMPL(200,  test_add<100, 100>);
  FATAL_TEST_IMPL(157,  test_add<100, 57>);
  FATAL_TEST_IMPL(102,  test_add<100, 2>);
  FATAL_TEST_IMPL(103,  test_add<100, 3>);
  FATAL_TEST_IMPL(102,  test_add<2, 100>);
  FATAL_TEST_IMPL(143,  test_add<43, 100>);

  FATAL_TEST_IMPL(400,  test_add<100, 100, 100, 100>);
  FATAL_TEST_IMPL(108,  test_add<100, 5, 2, 1>);
  FATAL_TEST_IMPL(125,  test_add<100, 20, 5>);
  FATAL_TEST_IMPL(121,  test_add<100, 2, 19>);
  FATAL_TEST_IMPL(109,  test_add<100, 3, 6>);
  FATAL_TEST_IMPL(110,  test_add<100, 3, 1, 6>);
  FATAL_TEST_IMPL(1102, test_add<2, 100, 1000>);
  FATAL_TEST_IMPL(1036, test_add<1000, 32, 4>);

# undef FATAL_TEST_IMPL
}

template <int... Values>
using test_subtract = transform_int<arithmetic::subtract, Values...>;

FATAL_TEST(arithmetic, subtract) {
# define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    using expected = std::decay<decltype(Expected)>::type; \
    FATAL_EXPECT_SAME<expected, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

  FATAL_TEST_IMPL(0,     test_subtract<100, 100>);
  FATAL_TEST_IMPL(43,    test_subtract<100, 57>);
  FATAL_TEST_IMPL(98,    test_subtract<100, 2>);
  FATAL_TEST_IMPL(97,    test_subtract<100, 3>);
  FATAL_TEST_IMPL(-98,   test_subtract<2, 100>);
  FATAL_TEST_IMPL(-57,   test_subtract<43, 100>);

  FATAL_TEST_IMPL(-200,  test_subtract<100, 100, 100, 100>);
  FATAL_TEST_IMPL(92,    test_subtract<100, 5, 2, 1>);
  FATAL_TEST_IMPL(75,    test_subtract<100, 20, 5>);
  FATAL_TEST_IMPL(79,    test_subtract<100, 2, 19>);
  FATAL_TEST_IMPL(91,    test_subtract<100, 3, 6>);
  FATAL_TEST_IMPL(90,    test_subtract<100, 3, 1, 6>);
  FATAL_TEST_IMPL(-1098, test_subtract<2, 100, 1000>);
  FATAL_TEST_IMPL(964,   test_subtract<1000, 32, 4>);

# undef FATAL_TEST_IMPL
}

template <int... Values>
using test_multiply = transform_int<arithmetic::multiply, Values...>;

FATAL_TEST(arithmetic, multiply) {
# define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    using expected = std::decay<decltype(Expected)>::type; \
    FATAL_EXPECT_SAME<expected, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

  FATAL_TEST_IMPL(0,         test_multiply<0>);
  FATAL_TEST_IMPL(1,         test_multiply<1>);
  FATAL_TEST_IMPL(2,         test_multiply<2>);
  FATAL_TEST_IMPL(56,        test_multiply<56>);
  FATAL_TEST_IMPL(100,       test_multiply<100>);

  FATAL_TEST_IMPL(10000,     test_multiply<100, 100>);
  FATAL_TEST_IMPL(5700,      test_multiply<100, 57>);
  FATAL_TEST_IMPL(200,       test_multiply<100, 2>);
  FATAL_TEST_IMPL(300,       test_multiply<100, 3>);
  FATAL_TEST_IMPL(200,       test_multiply<2, 100>);
  FATAL_TEST_IMPL(4300,      test_multiply<43, 100>);

  FATAL_TEST_IMPL(100000000, test_multiply<100, 100, 100, 100>);
  FATAL_TEST_IMPL(1000,      test_multiply<100, 5, 2, 1>);
  FATAL_TEST_IMPL(10000,     test_multiply<100, 20, 5>);
  FATAL_TEST_IMPL(3800,      test_multiply<100, 2, 19>);
  FATAL_TEST_IMPL(1800,      test_multiply<100, 3, 6>);
  FATAL_TEST_IMPL(1800,      test_multiply<100, 3, 1, 6>);
  FATAL_TEST_IMPL(200000,    test_multiply<2, 100, 1000>);
  FATAL_TEST_IMPL(128000,    test_multiply<1000, 32, 4>);

# undef FATAL_TEST_IMPL
}

template <int... Values>
using test_divide = transform_int<arithmetic::divide, Values...>;

FATAL_TEST(arithmetic, divide) {
# define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    using expected = std::decay<decltype(Expected)>::type; \
    FATAL_EXPECT_SAME<expected, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

  FATAL_TEST_IMPL(1,   test_divide<100, 100>);
  FATAL_TEST_IMPL(1,   test_divide<100, 57>);
  FATAL_TEST_IMPL(50,  test_divide<100, 2>);
  FATAL_TEST_IMPL(33,  test_divide<100, 3>);
  FATAL_TEST_IMPL(0,   test_divide<2, 100>);
  FATAL_TEST_IMPL(0,   test_divide<43, 100>);

  FATAL_TEST_IMPL(0,   test_divide<100, 100, 100, 100>);
  FATAL_TEST_IMPL(10,  test_divide<100, 5, 2, 1>);
  FATAL_TEST_IMPL(1,   test_divide<100, 20, 5>);
  FATAL_TEST_IMPL(2,   test_divide<100, 2, 19>);
  FATAL_TEST_IMPL(5,   test_divide<100, 3, 6>);
  FATAL_TEST_IMPL(5,   test_divide<100, 3, 1, 6>);
  FATAL_TEST_IMPL(0,   test_divide<2, 100, 1000>);
  FATAL_TEST_IMPL(7,   test_divide<1000, 32, 4>);

# undef FATAL_TEST_IMPL
}

template <int... Values>
using test_modulo = transform_int<arithmetic::modulo, Values...>;

FATAL_TEST(arithmetic, modulo) {
# define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    using expected = std::decay<decltype(Expected)>::type; \
    FATAL_EXPECT_SAME<expected, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

  FATAL_TEST_IMPL(0,  test_modulo<100, 100>);
  FATAL_TEST_IMPL(43, test_modulo<100, 57>);
  FATAL_TEST_IMPL(0,  test_modulo<100, 2>);
  FATAL_TEST_IMPL(1,  test_modulo<100, 3>);
  FATAL_TEST_IMPL(2,  test_modulo<2, 100>);
  FATAL_TEST_IMPL(43, test_modulo<43, 100>);

  FATAL_TEST_IMPL(0,  test_modulo<100, 100, 100, 100>);
  FATAL_TEST_IMPL(0,  test_modulo<100, 57, 100, 1>);
  FATAL_TEST_IMPL(3,  test_modulo<100, 57, 5>);
  FATAL_TEST_IMPL(0,  test_modulo<100, 2, 99>);
  FATAL_TEST_IMPL(1,  test_modulo<100, 3, 6>);
  FATAL_TEST_IMPL(0,  test_modulo<100, 3, 1, 6>);
  FATAL_TEST_IMPL(2,  test_modulo<2, 100, 1000>);
  FATAL_TEST_IMPL(43, test_modulo<43, 1000, 100>);

# undef FATAL_TEST_IMPL
}

/////////////
// logical //
/////////////

#define FATAL_TEST_IMPL(Transform, Expected, ...) \
  do { \
    using result = bool_seq<__VA_ARGS__>::apply<logical::Transform>; \
    FATAL_EXPECT_SAME<bool, result::value_type>(); \
    auto const actual = result::value; \
    bool const expected = Expected; \
    FATAL_EXPECT_EQ(expected, actual); \
  } while (false)

FATAL_TEST(logical, all) {
  FATAL_TEST_IMPL(all, true);

  FATAL_TEST_IMPL(all, true, true);
  FATAL_TEST_IMPL(all, false, false);

  FATAL_TEST_IMPL(all, true, true, true);
  FATAL_TEST_IMPL(all, false, true, false);
  FATAL_TEST_IMPL(all, false, false, true);
  FATAL_TEST_IMPL(all, false, false, false);

  FATAL_TEST_IMPL(all, true, true, true, true);
  FATAL_TEST_IMPL(all, false, true, true, false);
  FATAL_TEST_IMPL(all, false, true, false, true);
  FATAL_TEST_IMPL(all, false, true, false, false);
  FATAL_TEST_IMPL(all, false, false, true, true);
  FATAL_TEST_IMPL(all, false, false, true, false);
  FATAL_TEST_IMPL(all, false, false, false, true);
  FATAL_TEST_IMPL(all, false, false, false, false);

  FATAL_TEST_IMPL(all, true, true, true, true, true);
  FATAL_TEST_IMPL(all, false, true, true, true, false);
  FATAL_TEST_IMPL(all, false, true, true, false, true);
  FATAL_TEST_IMPL(all, false, true, true, false, false);
  FATAL_TEST_IMPL(all, false, true, false, true, true);
  FATAL_TEST_IMPL(all, false, true, false, true, false);
  FATAL_TEST_IMPL(all, false, true, false, false, true);
  FATAL_TEST_IMPL(all, false, true, false, false, false);
  FATAL_TEST_IMPL(all, false, false, true, true, true);
  FATAL_TEST_IMPL(all, false, false, true, true, false);
  FATAL_TEST_IMPL(all, false, false, true, false, true);
  FATAL_TEST_IMPL(all, false, false, true, false, false);
  FATAL_TEST_IMPL(all, false, false, false, true, true);
  FATAL_TEST_IMPL(all, false, false, false, true, false);
  FATAL_TEST_IMPL(all, false, false, false, false, true);
  FATAL_TEST_IMPL(all, false, false, false, false, false);
}

FATAL_TEST(logical, any) {
  FATAL_TEST_IMPL(any, false);

  FATAL_TEST_IMPL(any, true, true);
  FATAL_TEST_IMPL(any, false, false);

  FATAL_TEST_IMPL(any, true, true, true);
  FATAL_TEST_IMPL(any, true, true, false);
  FATAL_TEST_IMPL(any, true, false, true);
  FATAL_TEST_IMPL(any, false, false, false);

  FATAL_TEST_IMPL(any, true, true, true, true);
  FATAL_TEST_IMPL(any, true, true, true, false);
  FATAL_TEST_IMPL(any, true, true, false, true);
  FATAL_TEST_IMPL(any, true, true, false, false);
  FATAL_TEST_IMPL(any, true, false, true, true);
  FATAL_TEST_IMPL(any, true, false, true, false);
  FATAL_TEST_IMPL(any, true, false, false, true);
  FATAL_TEST_IMPL(any, false, false, false, false);

  FATAL_TEST_IMPL(any, true, true, true, true, true);
  FATAL_TEST_IMPL(any, true, true, true, true, false);
  FATAL_TEST_IMPL(any, true, true, true, false, true);
  FATAL_TEST_IMPL(any, true, true, true, false, false);
  FATAL_TEST_IMPL(any, true, true, false, true, true);
  FATAL_TEST_IMPL(any, true, true, false, true, false);
  FATAL_TEST_IMPL(any, true, true, false, false, true);
  FATAL_TEST_IMPL(any, true, true, false, false, false);
  FATAL_TEST_IMPL(any, true, false, true, true, true);
  FATAL_TEST_IMPL(any, true, false, true, true, false);
  FATAL_TEST_IMPL(any, true, false, true, false, true);
  FATAL_TEST_IMPL(any, true, false, true, false, false);
  FATAL_TEST_IMPL(any, true, false, false, true, true);
  FATAL_TEST_IMPL(any, true, false, false, true, false);
  FATAL_TEST_IMPL(any, true, false, false, false, true);
  FATAL_TEST_IMPL(any, false, false, false, false, false);
}

#undef FATAL_TEST_IMPL

FATAL_TEST(logical, negate) {
  FATAL_EXPECT_TRUE(logical::negate<std::false_type>::value);
  FATAL_EXPECT_FALSE(logical::negate<std::true_type>::value);
}

/////////////
// bitwise //
/////////////

#define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using actual = __VA_ARGS__; \
    FATAL_EXPECT_SAME<int, actual::value_type>(); \
    FATAL_EXPECT_EQ(Expected, actual::value); \
  } while (false)

template <int... Args>
using all_test_impl = bitwise::all<
  std::integral_constant<int, Args>...
>;

FATAL_TEST(bitwise, all) {
  FATAL_TEST_IMPL(99, all_test_impl<99>);
  FATAL_TEST_IMPL(0, all_test_impl<1, 2, 4>);
  FATAL_TEST_IMPL(3, all_test_impl<7, 11>);
  FATAL_TEST_IMPL(8 & 9 & 57, all_test_impl<8, 9, 57>);
}

template <int... Args>
using any_test_impl = bitwise::any<
  std::integral_constant<int, Args>...
>;

FATAL_TEST(bitwise, any) {
  FATAL_TEST_IMPL(99, any_test_impl<99>);
  FATAL_TEST_IMPL(7, any_test_impl<1, 2, 4>);
  FATAL_TEST_IMPL(8 | 9 | 57, any_test_impl<8, 9, 57>);
}

template <int... Args>
using diff_test_impl = bitwise::diff<
  std::integral_constant<int, Args>...
>;

FATAL_TEST(bitwise, diff) {
  FATAL_TEST_IMPL(99, diff_test_impl<99>);
  FATAL_TEST_IMPL(3, diff_test_impl<1, 2>);
  FATAL_TEST_IMPL(12, diff_test_impl<7, 11>);
  FATAL_TEST_IMPL(1 ^ 2 ^ 4, diff_test_impl<1, 2, 4>);
  FATAL_TEST_IMPL(8 ^ 9 ^ 57, diff_test_impl<8, 9, 57>);
}

#undef FATAL_TEST_IMPL

FATAL_TEST(bitwise, complement) {
# define FATAL_TEST_IMPL(x) \
  do { \
    using result = bitwise::complement< \
      std::integral_constant<unsigned, (x)> \
    >; \
    FATAL_EXPECT_SAME<unsigned, result::value_type>(); \
    FATAL_EXPECT_EQ(~static_cast<unsigned>(x), result::value);\
  } while(false)

  FATAL_TEST_IMPL(0);
  FATAL_TEST_IMPL(2);
  FATAL_TEST_IMPL(3);
  FATAL_TEST_IMPL(99);

  FATAL_EXPECT_EQ(
    static_cast<uint8_t>(0xf0),
    (bitwise::complement<std::integral_constant<uint8_t, 0xf>>::value)
  );

# undef FATAL_TEST_IMPL
}

//////////////////////////
// comparison_transform //
//////////////////////////

#define FATAL_TEST_IMPL(Expected, ...) \
  do { \
    using result = __VA_ARGS__; \
    auto const actual = result::value; \
    FATAL_EXPECT_SAME<bool, result::value_type>(); \
    auto const expected = Expected; \
    FATAL_EXPECT_EQ(expected, actual); \
  } while (false)

FATAL_TEST(comparison_transform, equal) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(false, comparison_transform::equal<A, B>);
  FATAL_TEST_IMPL(false, comparison_transform::equal<B, A>);
  FATAL_TEST_IMPL(true, comparison_transform::equal<A, A>);
}

FATAL_TEST(comparison_transform, not_equal) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(true, comparison_transform::not_equal<A, B>);
  FATAL_TEST_IMPL(true, comparison_transform::not_equal<B, A>);
  FATAL_TEST_IMPL(false, comparison_transform::not_equal<A, A>);
}

FATAL_TEST(comparison_transform, less_than) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(true, comparison_transform::less_than<A, B>);
  FATAL_TEST_IMPL(false, comparison_transform::less_than<B, A>);
  FATAL_TEST_IMPL(false, comparison_transform::less_than<A, A>);
}

FATAL_TEST(comparison_transform, less_than_equal) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(true, comparison_transform::less_than_equal<A, B>);
  FATAL_TEST_IMPL(false, comparison_transform::less_than_equal<B, A>);
  FATAL_TEST_IMPL(true, comparison_transform::less_than_equal<A, A>);
}

FATAL_TEST(comparison_transform, greater_than) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(false, comparison_transform::greater_than<A, B>);
  FATAL_TEST_IMPL(true, comparison_transform::greater_than<B, A>);
  FATAL_TEST_IMPL(false, comparison_transform::greater_than<A, A>);
}

FATAL_TEST(comparison_transform, greater_than_equal) {
  using A = std::integral_constant<int, 10>;
  using B = std::integral_constant<int, 20>;

  FATAL_TEST_IMPL(false, comparison_transform::greater_than_equal<A, B>);
  FATAL_TEST_IMPL(true, comparison_transform::greater_than_equal<B, A>);
  FATAL_TEST_IMPL(true, comparison_transform::greater_than_equal<A, A>);
}

#undef FATAL_TEST_IMPL

/////////////////////
// get_member_type //
/////////////////////

struct get_member_type_test {
  using tag = test_tag<void>;

  using types = test_list<void, int, bool, long>;
  using values = test_seq<int, 0, 2, 4, 6>;
  using args = test_list<std::string, double, std::vector<char>>;

  using pair = std::pair<float, long>;
  using tuple = std::tuple<int, double, bool>;
  using list = std::list<short>;
  using map = std::map<int, bool>;
  using array = std::array<int, 100>;
  using set = std::set<double>;
  using string = std::wstring;
  using index = std::integral_constant<std::size_t, 5>;
  using flag = std::true_type;
};

FATAL_TEST(get_member_type, get_member_type) {
# define CREATE_TEST(Member, Type) \
  do { \
    FATAL_EXPECT_SAME<Type::Member, get_member_type::Member<Type>>(); \
  } while (false)

  CREATE_TEST(tag, get_member_type_test);

  CREATE_TEST(types, get_member_type_test);
  CREATE_TEST(values, get_member_type_test);
  CREATE_TEST(args, get_member_type_test);

  CREATE_TEST(pair, get_member_type_test);
  CREATE_TEST(tuple, get_member_type_test);
  CREATE_TEST(list, get_member_type_test);
  CREATE_TEST(map, get_member_type_test);
  CREATE_TEST(array, get_member_type_test);
  CREATE_TEST(set, get_member_type_test);
  CREATE_TEST(string, get_member_type_test);
  CREATE_TEST(index, get_member_type_test);
  CREATE_TEST(flag, get_member_type_test);

  typedef std::add_const<int> a_c;
  CREATE_TEST(type, a_c);

  typedef std::pair<double, long> pair;
  CREATE_TEST(first_type, pair);
  CREATE_TEST(second_type, pair);

  typedef std::map<std::string, bool> map;
  CREATE_TEST(key_type, map);
  CREATE_TEST(mapped_type, map);
  CREATE_TEST(value_type, map);

  typedef std::shared_ptr<float> ptr;
  CREATE_TEST(element_type, ptr);

  typedef std::char_traits<char> ctraits;
  CREATE_TEST(char_type, ctraits);

  typedef std::string str;
  CREATE_TEST(traits_type, str);
  CREATE_TEST(allocator_type, str);

  CREATE_TEST(size_type, str);
  CREATE_TEST(difference_type, str);

  CREATE_TEST(reference, str);
  CREATE_TEST(const_reference, str);

  CREATE_TEST(pointer, str);
  CREATE_TEST(const_pointer, str);

  CREATE_TEST(iterator, str);
  CREATE_TEST(const_iterator, str);
  CREATE_TEST(reverse_iterator, str);
  CREATE_TEST(const_reverse_iterator, str);
# undef CREATE_TEST
}

///////////////////////////
// conditional_transform //
///////////////////////////

FATAL_TEST(conditional_transform, when_true) {
  using transform = conditional_transform<std::is_integral, T1>;
  FATAL_EXPECT_SAME<T1<long>, transform::apply<long>>();
  FATAL_EXPECT_SAME<std::string, transform::apply<std::string>>();
}

FATAL_TEST(conditional_transform, ternary) {
  using transform = conditional_transform<std::is_integral, T1, T2>;
  FATAL_EXPECT_SAME<T1<long>, transform::apply<long>>();
  FATAL_EXPECT_SAME<T2<std::string>, transform::apply<std::string>>();
}

//////////////////////
// transform_traits //
//////////////////////

FATAL_TEST(transform_traits, supports) {
# define CHECK_SUPPORTS(Expected, T, ...) \
  do { \
    using expected = std::FATAL_CAT(Expected, _type); \
    using actual = transform_traits<__VA_ARGS__>::supports<T>; \
    FATAL_EXPECT_SAME<expected, actual>(); \
  } while (false)

  CHECK_SUPPORTS(true, int, T0);
  CHECK_SUPPORTS(true, std::decay<int>, T0);
  CHECK_SUPPORTS(false, int, get_type);
  CHECK_SUPPORTS(true, std::decay<int>, get_type);

  using pair = std::pair<short, double>;
  CHECK_SUPPORTS(true, pair, type_get_first);
  CHECK_SUPPORTS(true, pair, type_get_second);
  CHECK_SUPPORTS(false, pair, type_get_third);
  CHECK_SUPPORTS(false, pair, type_get_fourth);
  CHECK_SUPPORTS(false, pair, type_get_fifth);

# undef CHECK_SUPPORTS
}

///////////////////
// try_transform //
///////////////////

FATAL_TEST(try_transform, try_transform) {
# define CHECK_TRY_TRANSFORM(T, TTransform, TFallback, ...) \
  do { \
    using expected = __VA_ARGS__; \
    using actual = try_transform<TTransform, TFallback>::apply<T>; \
    FATAL_EXPECT_SAME<expected, actual>(); \
  } while (false)

  CHECK_TRY_TRANSFORM(int, T0, T1, T0<int>);
  CHECK_TRY_TRANSFORM(std::decay<int>, T0, T1, T0<std::decay<int>>);
  CHECK_TRY_TRANSFORM(int, get_type, T2, T2<int>);
  CHECK_TRY_TRANSFORM(std::decay<int>, get_type, T2, int);

# undef CHECK_TRY_TRANSFORM
}

//////////////////////////
// transform_aggregator //
//////////////////////////

FATAL_TEST(transform_aggregator, decay) {
  FATAL_EXPECT_SAME<
    std::decay<T0<int>>,
    transform_aggregator<std::decay, T0>::apply<int>
  >();
}

FATAL_TEST(transform_aggregator, is_same) {
  FATAL_EXPECT_SAME<
    std::is_same<T0<int>, T1<int>>,
    transform_aggregator<std::is_same, T0, T1>::apply<int>
  >();
}

FATAL_TEST(transform_aggregator, pair) {
  FATAL_EXPECT_SAME<
    std::pair<T0<int>, T1<int>>,
    transform_aggregator<std::pair, T0, T1>::apply<int>
  >();
}

FATAL_TEST(transform_aggregator, triplet) {
  FATAL_EXPECT_SAME<
    test_triplet<T0<int>, T1<int>, T2<int>>,
    transform_aggregator<test_triplet, T0, T1, T2>::apply<int>
  >();
}

FATAL_TEST(transform_aggregator, tuple) {
  FATAL_EXPECT_SAME<
    std::tuple<>,
    transform_aggregator<std::tuple>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<T0<int>>,
    transform_aggregator<std::tuple, T0>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<T0<int>, T1<int>>,
    transform_aggregator<std::tuple, T0, T1>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<T0<int>, T1<int>, T2<int>>,
    transform_aggregator<std::tuple, T0, T1, T2>::apply<int>
  >();
}

FATAL_TEST(transform_aggregator, type_list) {
  FATAL_EXPECT_SAME<
    test_list<>,
    transform_aggregator<test_list>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    test_list<T0<int>>,
    transform_aggregator<test_list, T0>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    test_list<T0<int>, T1<int>>,
    transform_aggregator<test_list, T0, T1>::apply<int>
  >();

  FATAL_EXPECT_SAME<
    test_list<T0<int>, T1<int>, T2<int>>,
    transform_aggregator<test_list, T0, T1, T2>::apply<int>
  >();
}

////////////////////////
// variadic_transform //
////////////////////////

template <
  template <typename...> class TCombiner,
  template <typename...> class... TTransforms
>
struct check_variadic_transform {
  using transform = variadic_transform<TCombiner, TTransforms...>;

  template <typename... Args>
  using actual = typename transform::template apply<Args...>;

  template <typename TExpected, typename... Args>
  static void check_value() {
    FATAL_EXPECT_EQ(TExpected::value, actual<Args...>::value);
  }

  template <typename... TExpected>
  struct expect {
    using expected = TCombiner<TExpected...>;

    template <typename... Args>
    static void check_type() {
      FATAL_EXPECT_SAME<expected, actual<Args...>>();
    }

    template <typename... Args>
    static void check_value() {
      FATAL_EXPECT_EQ(expected::value, actual<Args...>::value);
    }
  };
};

FATAL_TEST(variadic_transform, types) {
  check_variadic_transform<test_list>::expect<>::check_type<>();

  check_variadic_transform<test_list, T0>::expect<T0<X0>>::check_type<X0>();

  check_variadic_transform<
    test_list, T0, T1
  >::expect<
    T0<X0>, T1<X1>
  >::check_type<
    X0, X1
  >();

  check_variadic_transform<
    test_list, T0, T1, T2
  >::expect<
    T0<X0>, T1<X1>, T2<X2>
  >::check_type<
    X0, X1, X2
  >();

  check_variadic_transform<
    test_list, T0, T1, T2, T3
  >::expect<
    T0<X0>, T1<X1>, T2<X2>, T3<X3>
  >::check_type<
    X0, X1, X2, X3
  >();
}

FATAL_TEST(variadic_transform, arithmetic) {
  check_variadic_transform<
    arithmetic::add,
    op<5>::mul, op<3>::mul, op<1>::mul, op<0>::mul, op<2>::mul
  >::expect<
    int_val<5 * 1 + 3 * 2 + 1 * 3 + 0 * 4 + 2 * 5>
  >::check_value<
    int_val<1>, int_val<2>, int_val<3>, int_val<4>, int_val<5>
  >();
}

FATAL_TEST(variadic_transform, comparison_transform) {
  check_variadic_transform<
    comparison_transform::less_than,
    identity, identity
  >::check_value<
    std::true_type,
    int_val<1>, int_val<2>
  >();

  check_variadic_transform<
    comparison_transform::less_than,
    identity, op<9>::mul
  >::check_value<
    std::true_type,
    int_val<1>, int_val<2>
  >();

  check_variadic_transform<
    comparison_transform::less_than,
    op<5>::mul, identity
  >::check_value<
    std::false_type,
    int_val<1>, int_val<2>
  >();

  check_variadic_transform<
    comparison_transform::less_than,
    op<3>::mul, op<3>::mul
  >::check_value<
    std::true_type,
    int_val<1>, int_val<2>
  >();

  check_variadic_transform<
    comparison_transform::less_than,
    op<5>::mul, op<2>::mul
  >::check_value<
    std::false_type,
    int_val<1>, int_val<2>
  >();

  check_variadic_transform<
    comparison_transform::less_than,
    op<5>::mul, op<9>::mul
  >::check_value<
    std::true_type,
    int_val<1>, int_val<2>
  >();
}

///////////////////////////
// type_member_transform //
///////////////////////////

template <template <typename...> class TTransform, typename T>
void check_type_member_transform() {
  FATAL_EXPECT_SAME<
    typename TTransform<T>::type,
    typename type_member_transform<TTransform>::template apply<T>
  >();
}

FATAL_TEST(type_member_transform, type_member_transform) {
  check_type_member_transform<std::add_const, int>();
  check_type_member_transform<std::add_const, bool>();
  check_type_member_transform<std::add_const, double>();
  check_type_member_transform<std::add_const, std::string>();
  check_type_member_transform<std::add_pointer, int>();
  check_type_member_transform<std::add_pointer, bool>();
  check_type_member_transform<std::add_pointer, double>();
  check_type_member_transform<std::add_pointer, std::string>();
  check_type_member_transform<std::add_lvalue_reference, int>();
  check_type_member_transform<std::add_lvalue_reference, bool>();
  check_type_member_transform<std::add_lvalue_reference, double>();
  check_type_member_transform<std::add_lvalue_reference, std::string>();
  check_type_member_transform<std::add_rvalue_reference, int>();
  check_type_member_transform<std::add_rvalue_reference, bool>();
  check_type_member_transform<std::add_rvalue_reference, double>();
  check_type_member_transform<std::add_rvalue_reference, std::string>();
}

////////////////////////////
// transform_alias::apply //
////////////////////////////

FATAL_TEST(transform_alias, apply) {
  typedef transform_alias<std::tuple, int, double> c1;

  FATAL_EXPECT_SAME<
    std::tuple<int, double>,
    c1::apply<>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<int, double, long, std::string, bool, float>,
    c1::apply<long, std::string, bool, float>
  >();
}

///////////////////////////
// transform_alias::type //
///////////////////////////

FATAL_TEST(transform_alias, type) {
  typedef transform_alias<std::tuple, int, double> c1;

  FATAL_EXPECT_SAME<
    std::tuple<int, double>,
    c1::apply<>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<int, double, long, std::string, bool, float>,
    c1::apply<long, std::string, bool, float>
  >();
}

////////////////////////////
// transform_alias::curry //
////////////////////////////

FATAL_TEST(transform_alias, curry) {
  typedef transform_alias<std::tuple, int, double> c1;
  typedef c1::curry<long, std::string> c2;

  FATAL_EXPECT_SAME<
    std::tuple<int, double, long, std::string>,
    c2::apply<>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<int, double, long, std::string, bool, float>,
    c2::apply<bool, float>
  >();
}

/////////////////////////////
// transform_alias::rebind //
/////////////////////////////

FATAL_TEST(transform_alias, rebind) {
  typedef transform_alias<std::tuple, int, double> c1;
  typedef c1::rebind<long, std::string> c2;

  FATAL_EXPECT_SAME<
    std::tuple<long, std::string>,
    c2::apply<>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<long, std::string, bool, float>,
    c2::apply<bool, float>
  >();
}

//////////////////////////////
// transform_alias::uncurry //
//////////////////////////////

FATAL_TEST(transform_alias, uncurry) {
  typedef transform_alias<std::tuple, int, double> c1;

  FATAL_EXPECT_SAME<
    std::tuple<long, std::string>,
    c1::uncurry<long, std::string>
  >();

  typedef transform_alias<c1::template uncurry, long, std::string> c2;

  FATAL_EXPECT_SAME<
    std::tuple<long, std::string>,
    c2::apply<>
  >();

  FATAL_EXPECT_SAME<
    std::tuple<long, std::string, bool, float>,
    c2::apply<bool, float>
  >();
}

//////////////////////////////////
// transform_alias::rebind_args //
//////////////////////////////////

FATAL_TEST(transform_alias, rebind_args) {
  typedef transform_alias<std::tuple, int, double> c1;

  FATAL_EXPECT_SAME<
    V1<int, double>,
    c1::rebind_args<V1>::apply<>
  >();

  FATAL_EXPECT_SAME<
    V1<int, double, bool, float>,
    c1::rebind_args<V1, bool, float>::apply<>
  >();
}

/////////////////////////////////
// transform_alias::apply_args //
/////////////////////////////////

FATAL_TEST(transform_alias, apply_args) {
  typedef transform_alias<std::tuple, int, double> c1;

  FATAL_EXPECT_SAME<
    V1<int, double>,
    c1::apply_args<V1>
  >();

  FATAL_EXPECT_SAME<
    V1<int, double, bool, float>,
    c1::apply_args<V1, bool, float>
  >();
}

//////////////////////
// transform_switch //
//////////////////////

FATAL_TEST(transform_switch, empty_identity) {
  using transform = identity_switch<>;

  FATAL_EXPECT_SAME<int, transform::apply<int>>();
}

FATAL_TEST(transform_switch, empty_T1) {
  using transform = transform_switch<T1>;

  FATAL_EXPECT_SAME<T1<int>, transform::apply<int>>();
}

FATAL_TEST(transform_switch, identity) {
  using transform = identity_switch<
    is_tuple, type_member_transform<tuple_as_list>::template apply,
    is_vector, get_value_type,
    is_string, get_value_type
  >;

  FATAL_EXPECT_SAME<int, transform::apply<int>>();
  FATAL_EXPECT_SAME<bool, transform::apply<bool>>();
  FATAL_EXPECT_SAME<char, transform::apply<std::string>>();
  FATAL_EXPECT_SAME<wchar_t, transform::apply<std::wstring>>();
  FATAL_EXPECT_SAME<int, transform::apply<std::vector<int>>>();
  FATAL_EXPECT_SAME<double, transform::apply<std::vector<double>>>();
  FATAL_EXPECT_SAME<test_list<>, transform::apply<std::tuple<>>>();
  FATAL_EXPECT_SAME<
    test_list<int, double, bool>,
    transform::apply<std::tuple<int, double, bool>>
  >();
  FATAL_EXPECT_SAME<test_list<>, transform::apply<test_list<>>>();
  FATAL_EXPECT_SAME<
    test_list<int, double, bool>,
    transform::apply<test_list<int, double, bool>>
  >();
}

FATAL_TEST(transform_switch, T1) {
  using transform = transform_switch<
    T1,
    is_tuple, type_member_transform<tuple_as_list>::template apply,
    is_vector, get_value_type,
    is_string, get_value_type
  >;

  FATAL_EXPECT_SAME<T1<int>, transform::apply<int>>();
  FATAL_EXPECT_SAME<T1<bool>, transform::apply<bool>>();
  FATAL_EXPECT_SAME<char, transform::apply<std::string>>();
  FATAL_EXPECT_SAME<wchar_t, transform::apply<std::wstring>>();
  FATAL_EXPECT_SAME<int, transform::apply<std::vector<int>>>();
  FATAL_EXPECT_SAME<double, transform::apply<std::vector<double>>>();
  FATAL_EXPECT_SAME<test_list<>, transform::apply<std::tuple<>>>();
  FATAL_EXPECT_SAME<
    test_list<int, double, bool>,
    transform::apply<std::tuple<int, double, bool>>
  >();
  FATAL_EXPECT_SAME<T1<test_list<>>, transform::apply<test_list<>>>();
  FATAL_EXPECT_SAME<
    T1<test_list<int, double, bool>>,
    transform::apply<test_list<int, double, bool>>
  >();
}

////////////////////////
// member_transformer //
////////////////////////

FATAL_TEST(member_transformer, transform) {
# define TEST_IMPL(TTransform, ...) \
  do { \
    using input = test_list<__VA_ARGS__>; \
    using expected = input::transform<TTransform>; \
    using actual_use = member_transformer::transform<> \
      ::use<input, TTransform>; \
    FATAL_EXPECT_SAME<expected, actual_use>(); \
    using actual_bind = member_transformer::transform<> \
      ::bind<TTransform>::use<input>; \
    FATAL_EXPECT_SAME<expected, actual_bind>(); \
  } while (false)

  TEST_IMPL(identity);
  TEST_IMPL(identity, void);
  TEST_IMPL(identity, int);
  TEST_IMPL(identity, int, void);
  TEST_IMPL(identity, int, void, test_list<>);
  TEST_IMPL(identity, int, void, test_list<float>);
  TEST_IMPL(identity, int, void, test_list<float, bool>);
  TEST_IMPL(T1);
  TEST_IMPL(T1, void);
  TEST_IMPL(T1, int);
  TEST_IMPL(T1, int, void);
  TEST_IMPL(T1, int, void, test_list<>);
  TEST_IMPL(T1, int, void, test_list<float>);
  TEST_IMPL(T1, int, void, test_list<float, bool>);

# undef TEST_IMPL
}

template <
  template <
    typename, template <typename...> class, typename...
  > class TMemberTransform,
  typename T,
  template <typename...> class TTransform
>
struct member_transformer_test_va_args_proxy_use {
  template <typename... Args>
  using apply = TMemberTransform<T, TTransform, Args...>;
};

template <
  template <template <typename...> class, typename...> class TMemberTransform,
  template <typename...> class TTransform
>
struct member_transformer_test_va_args_proxy_bind {
  template <typename... Args>
  using apply = TMemberTransform<TTransform, Args...>;
};

FATAL_TEST(member_transformer, apply_with_args) {
# define TEST_IMPL(TTransform, ...) \
  do { \
    using input = test_list<__VA_ARGS__>; \
    using expected = typename member_transformer_test_va_args_proxy_bind< \
      input::apply, TTransform \
    >::template apply<__VA_ARGS__>; \
    using actual_use = typename member_transformer_test_va_args_proxy_use< \
      member_transformer::apply<>::use, input, TTransform \
    >::template apply<__VA_ARGS__>; \
    FATAL_EXPECT_SAME<expected, actual_use>(); \
    using actual_bind = typename member_transformer_test_va_args_proxy_bind< \
      member_transformer::apply<>::bind, TTransform \
    >::template apply<__VA_ARGS__>::template use<input>; \
    FATAL_EXPECT_SAME<expected, actual_bind>(); \
  } while (false)

  TEST_IMPL(test_list);
  TEST_IMPL(test_list, void);
  TEST_IMPL(test_list, int);
  TEST_IMPL(test_list, int, void);
  TEST_IMPL(test_list, int, void, test_list<>);
  TEST_IMPL(test_list, int, void, test_list<float>);
  TEST_IMPL(test_list, int, void, test_list<float, bool>);
  TEST_IMPL(V1);
  TEST_IMPL(V1, void);
  TEST_IMPL(V1, int);
  TEST_IMPL(V1, int, void);
  TEST_IMPL(V1, int, void, test_list<>);
  TEST_IMPL(V1, int, void, test_list<float>);
  TEST_IMPL(V1, int, void, test_list<float, bool>);

# undef TEST_IMPL
}

//////////////////////////////
// member_transformer_stack //
//////////////////////////////

FATAL_TEST(member_transformer_stack, use) {
  using input = test_list<
    int, double, unsigned long, unsigned short, float, void
  >;

  using stack = member_transformer_stack<
    member_transformer::transform<>::use
  >;

  using expected = test_list<
    T0<int>, T0<double>, T0<unsigned long>,
    T0<unsigned short>, T0<float>, T0<void>
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, pre) {
  using input = T1<
    test_list<
      int, double, unsigned long, unsigned short, float, void
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1>;

  using expected = test_list<
    T0<int>, T0<double>, T0<unsigned long>,
    T0<unsigned short>, T0<float>, T0<void>
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, multi_pre) {
  using input = T1<
    T2<
      T3<
        test_list<
          int, double, unsigned long, unsigned short, float, void
        >
      >
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1, get_t2, get_t3>;

  using expected = test_list<
    T0<int>, T0<double>, T0<unsigned long>,
    T0<unsigned short>, T0<float>, T0<void>
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, post) {
  using input = test_list<
    int, double, unsigned long, unsigned short, float, void
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::post<V1>;

  using expected = V1<
    test_list<
      T0<int>, T0<double>, T0<unsigned long>,
      T0<unsigned short>, T0<float>, T0<void>
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, multi_post) {
  using input = test_list<
    int, double, unsigned long, unsigned short, float, void
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::post<V3, V2, V1>;

  using expected = V1<
    V2<
      V3<
        test_list<
          T0<int>, T0<double>, T0<unsigned long>,
          T0<unsigned short>, T0<float>, T0<void>
        >
      >
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, pre_post) {
  using input = T1<
    test_list<
      int, double, unsigned long, unsigned short, float, void
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1>
    ::post<V1>;

  using expected = V1<
    test_list<
      T0<int>, T0<double>, T0<unsigned long>,
      T0<unsigned short>, T0<float>, T0<void>
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, multi_pre_post) {
  using input = T1<
    T2<
      T3<
        test_list<
          int, double, unsigned long, unsigned short, float, void
        >
      >
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1, get_t2, get_t3>
    ::post<V1>;

  using expected = V1<
    test_list<
      T0<int>, T0<double>, T0<unsigned long>,
      T0<unsigned short>, T0<float>, T0<void>
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, pre_multi_post) {
  using input = T1<
    test_list<
      int, double, unsigned long, unsigned short, float, void
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1>
    ::post<V3, V2, V1>;

  using expected = V1<
    V2<
      V3<
        test_list<
          T0<int>, T0<double>, T0<unsigned long>,
          T0<unsigned short>, T0<float>, T0<void>
        >
      >
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, multi_pre_multi_post) {
  using input = T1<
    T2<
      T3<
        test_list<
          int, double, unsigned long, unsigned short, float, void
        >
      >
    >
  >;

  using stack = member_transformer_stack<member_transformer::transform<>::use>
    ::pre<get_t1, get_t2, get_t3>
    ::post<V3, V2, V1>;

  using expected = V1<
    V2<
      V3<
        test_list<
          T0<int>, T0<double>, T0<unsigned long>,
          T0<unsigned short>, T0<float>, T0<void>
        >
      >
    >
  >;

  using actual = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(member_transformer_stack, ad_hoc_stack_0) {
  using input = T1<
    T2<
      test_list<
        int, double, unsigned long, unsigned short, float, void
      >
    >
  >;

  using stack = member_transformer_stack<
    member_transformer::transform<>::use
  >::pre<
    get_t1,
    get_t2,
    member_transformer::conditional<>::bind<std::is_integral, int>::use
  >::post<
    member_transformer::apply<>::bind<V1>::use,
    T3
  >;

  using expected0 = T3<V1<int, int, long, short, int, int>>;

  using actual0 = stack::use<
    input,
    type_member_transform<std::make_signed>::apply
  >;

  FATAL_EXPECT_SAME<expected0, actual0>();

  using expected1 = T3<
    V1<
      T0<int>, T0<int>, T0<unsigned long>,
      T0<unsigned short>, T0<int>, T0<int>
    >
  >;

  using actual1 = stack::use<input, T0>;

  FATAL_EXPECT_SAME<expected1, actual1>();
}

/////////////////////////
// recursive_transform //
/////////////////////////

template <
  typename TExpected, typename T,
  template <typename...> class TPredicate,
  template <typename...> class TTransform,
  std::size_t Depth,
  template <typename...> class TPreTransform = identity,
  template <typename...> class TPostTransform = identity,
  template <typename, template <typename...> class, typename...>
    class TTransformer = member_transformer::transform<>::use
>
void check_recursive_list_transform() {
  FATAL_EXPECT_SAME<
    TExpected,
    typename recursive_transform<
      TPredicate,
      TTransformer,
      TTransform
    >::template with<
      TPreTransform,
      TPostTransform,
      Depth
    >::template apply<T>
  >();
}

FATAL_TEST(recursive_transform, identity) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity, std::numeric_limits<std::size_t>::max()
  >();
}

FATAL_TEST(recursive_transform, identity_0) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity, 0
  >();
}

FATAL_TEST(recursive_transform, identity_1) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity, 1
  >();
}

FATAL_TEST(recursive_transform, identity_2) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity, 2
  >();
}

FATAL_TEST(recursive_transform, identity_3) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity, 3
  >();
}

FATAL_TEST(recursive_transform, foo) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>>,
    test_list<void>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, T1<int>>,
    test_list<void, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>>,
    test_list<void, test_list<>>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>, T1<int>>,
    test_list<void, test_list<>, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>>, T1<int>>,
    test_list<void, test_list<double>, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>>, T1<int>>,
    test_list<void, test_list<double, bool>, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>, test_list<>>, T1<int>>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<
      T1<void>, test_list<T1<double>, T1<bool>, test_list<T1<float>>>, T1<int>
    >,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, T1, std::numeric_limits<std::size_t>::max()
  >();
}

FATAL_TEST(recursive_transform, foo_0) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, T1, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, T1, 0
  >();
}

FATAL_TEST(recursive_transform, foo_1) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>>,
    test_list<void>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, T1<int>>,
    test_list<void, int>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>>,
    test_list<void, test_list<>>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>, T1<int>>,
    test_list<void, test_list<>, int>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<double>, T1<int>>,
    test_list<void, test_list<double>, int>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<double, bool>, T1<int>>,
    test_list<void, test_list<double, bool>, int>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<double, bool, test_list<>>, T1<int>>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, T1, 1
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<double, bool, test_list<float>>, T1<int>>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, T1, 1
  >();
}

FATAL_TEST(recursive_transform, foo_2) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>>,
    test_list<void>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, T1<int>>,
    test_list<void, int>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>>,
    test_list<void, test_list<>>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>, T1<int>>,
    test_list<void, test_list<>, int>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>>, T1<int>>,
    test_list<void, test_list<double>, int>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>>, T1<int>>,
    test_list<void, test_list<double, bool>, int>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>, test_list<>>, T1<int>>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, T1, 2
  >();
  check_recursive_list_transform<
    test_list<
      T1<void>, test_list<T1<double>, T1<bool>, test_list<float>>, T1<int>
    >,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, T1, 2
  >();
}

FATAL_TEST(recursive_transform, foo_3) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>>,
    test_list<void>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, T1<int>>,
    test_list<void, int>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>>,
    test_list<void, test_list<>>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<>, T1<int>>,
    test_list<void, test_list<>, int>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>>, T1<int>>,
    test_list<void, test_list<double>, int>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>>, T1<int>>,
    test_list<void, test_list<double, bool>, int>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<T1<void>, test_list<T1<double>, T1<bool>, test_list<>>, T1<int>>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, T1, 3
  >();
  check_recursive_list_transform<
    test_list<
      T1<void>, test_list<T1<double>, T1<bool>, test_list<T1<float>>>, T1<int>
    >,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, T1, 3
  >();
}

FATAL_TEST(recursive_transform, pre_post_identity) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_identity_0) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>>>,
    T0<test_list<void, test_list<>>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>, int>>,
    T0<test_list<void, test_list<>, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double>, int>>,
    T0<test_list<void, test_list<double>, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool>, int>>,
    T0<test_list<void, test_list<double, bool>, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<>>, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    is_T0_list, identity, 0, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_identity_1) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, test_list<>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, test_list<>>>, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, test_list<float>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, test_list<float>>>, int>>,
    is_T0_list, identity, 1, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_identity_2) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity, 2, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_identity_3) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity, 3, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_foo) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>>>,
    T0<test_list<void>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, T1<int>>>,
    T0<test_list<void, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>, T1<int>>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>, T1<bool>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>, V1<test_list<T1<double>, T1<bool>, V1<test_list<>>>>, T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>,
      V1<test_list<T1<double>, T1<bool>, V1<test_list<T1<float>>>>>,
      T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, T1, std::numeric_limits<std::size_t>::max(), get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_foo_0) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>>>,
    T0<test_list<void, test_list<>>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>, int>>,
    T0<test_list<void, test_list<>, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double>, int>>,
    T0<test_list<void, test_list<double>, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool>, int>>,
    T0<test_list<void, test_list<double, bool>, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<>>, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    is_T0_list, T1, 0, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_foo_1) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>>>,
    T0<test_list<void>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, T1<int>>>,
    T0<test_list<void, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>, T1<int>>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<double>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<double, bool>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<double, bool, test_list<>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double, bool, test_list<>>>, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>, V1<test_list<double, bool, test_list<float>>>, T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, test_list<float>>>, int>>,
    is_T0_list, T1, 1, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_foo_2) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>>>,
    T0<test_list<void>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, T1<int>>>,
    T0<test_list<void, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>, T1<int>>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>, T1<bool>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>, V1<test_list<T1<double>, T1<bool>, V1<test_list<>>>>, T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>,
      V1<test_list<
        T1<double>, T1<bool>, V1<test_list<float>>
      >>,
      T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, T1, 2, get_t0, V1
  >();
}

FATAL_TEST(recursive_transform, pre_post_foo_3) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>>>,
    T0<test_list<void>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, T1<int>>>,
    T0<test_list<void, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<>>, T1<int>>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<T1<void>, V1<test_list<T1<double>, T1<bool>>>, T1<int>>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>, V1<test_list<T1<double>, T1<bool>, V1<test_list<>>>>, T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<
      T1<void>,
      V1<test_list<T1<double>, T1<bool>, V1<test_list<T1<float>>>>>,
      T1<int>
    >>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, T1, 3, get_t0, V1
  >();
}

//////////////
// type_get //
//////////////

template <typename, std::size_t, typename...> struct check_type_get_impl;

template <typename T, std::size_t Index, typename TExpected, typename... Args>
struct check_type_get_impl<T, Index, TExpected, Args...> {
  static void check() {
    typedef typename type_get<Index>::template from<T> TActual;
    FATAL_EXPECT_SAME<TExpected, TActual>();
    check_type_get_impl<T, Index + 1, Args...>::check();
  }
};

template <typename T, std::size_t Index>
struct check_type_get_impl<T, Index> {
  static void check() {}
};

template <typename... Args>
void check_type_get_std_pair() {
  check_type_get_impl<std::pair<Args...>, 0, Args...>::check();
}

FATAL_TEST(type_get, std_pair) {
  check_type_get_std_pair<bool, bool>();
  check_type_get_std_pair<bool, int>();
  check_type_get_std_pair<int, double>();
  check_type_get_std_pair<int, std::string>();
  check_type_get_std_pair<bool, int>();
  check_type_get_std_pair<std::string, std::string>();
  check_type_get_std_pair<std::string, float>();
}

template <typename... Args>
void check_type_get_std_tuple() {
  check_type_get_impl<std::tuple<Args...>, 0, Args...>::check();
}

FATAL_TEST(type_get, std_tuple) {
  check_type_get_std_tuple<>();
  check_type_get_std_tuple<bool>();
  check_type_get_std_tuple<int, double>();
  check_type_get_std_tuple<int, int, float>();
  check_type_get_std_tuple<
    std::tuple<bool, int>,
    std::tuple<std::string, std::string>,
    std::tuple<std::string, std::string, bool>
  >();
}

template <int LHS, int RHS>
class type_get_first_comparer_test {
  using lhs = std::pair<std::integral_constant<int, LHS>, void>;
  using rhs = std::pair<std::integral_constant<int, RHS>, double>;

public:
  template <template <typename...> class TComparer>
  using comparison = std::integral_constant<
    bool, TComparer<lhs, rhs>::value
  >;
};

FATAL_TEST(type_get, first_comparer) {
  FATAL_EXPECT_TRUE((
    type_get_first_comparer_test<5, 8>
      ::comparison<type_get_first_comparer<>::compare>
      ::value
  ));

  FATAL_EXPECT_FALSE((
    type_get_first_comparer_test<5, 8>::comparison<
      type_get_first_comparer<comparison_transform::greater_than>::compare
    >::value
  ));
}

template <int LHS, int RHS>
class type_get_second_comparer_test {
  using lhs = std::pair<void, std::integral_constant<int, LHS>>;
  using rhs = std::pair<double, std::integral_constant<int, RHS>>;

public:
  template <template <typename...> class TComparer>
  using comparison = std::integral_constant<
    bool, TComparer<lhs, rhs>::value
  >;
};

FATAL_TEST(type_get, second_comparer) {
  FATAL_EXPECT_TRUE((
    type_get_second_comparer_test<5, 8>
      ::comparison<type_get_second_comparer<>::compare>
      ::value
  ));

  FATAL_EXPECT_FALSE((
    type_get_second_comparer_test<5, 8>::comparison<
      type_get_second_comparer<comparison_transform::greater_than>::compare
    >::value
  ));
}

} // namespace fatal {
