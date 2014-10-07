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

#include <list>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace fatal {

template <typename T> struct T0 { using t0 = T; };
template <typename T> struct T1 { using t1 = T; };
template <typename T> struct T2 { using t2 = T; };
template <typename T> struct T3 { using t3 = T; };
template <typename...> struct V0 {};
template <typename...> struct V1 {};
template <typename...> struct V2 {};
template <typename...> struct V3 {};

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

////////////////////////
// identity_transform //
////////////////////////

TEST(identity_transform, identity_transform) {
  FATAL_EXPECT_SAME <int, identity_transform<int>>();
  FATAL_EXPECT_SAME <std::string, identity_transform<std::string>>();
  FATAL_EXPECT_SAME <double, identity_transform<identity_transform<double>>>();
}

/////////////////////
// fixed_transform //
/////////////////////

TEST(fixed_transform, fixed_transform) {
  typedef fixed_transform<int> i;
  FATAL_EXPECT_SAME<int, i::apply<>>();
  FATAL_EXPECT_SAME<int, i::apply<bool>>();
  FATAL_EXPECT_SAME<int, i::apply<int>>();
  FATAL_EXPECT_SAME<int, i::apply<std::string>>();
  FATAL_EXPECT_SAME<int, i::apply<double>>();
  FATAL_EXPECT_SAME<int, i::apply<double, void>>();
  FATAL_EXPECT_SAME<int, i::apply<identity_transform<double>>>();

  typedef fixed_transform<bool> b;
  FATAL_EXPECT_SAME<bool, b::apply<>>();
  FATAL_EXPECT_SAME<bool, b::apply<bool>>();
  FATAL_EXPECT_SAME<bool, b::apply<int>>();
  FATAL_EXPECT_SAME<bool, b::apply<std::string>>();
  FATAL_EXPECT_SAME<bool, b::apply<double>>();
  FATAL_EXPECT_SAME<bool, b::apply<double, void>>();
  FATAL_EXPECT_SAME<bool, b::apply<identity_transform<double>>>();
}

////////////////////////
// constant_transform //
////////////////////////

TEST(constant_transform, constant_transform) {
  constexpr int ic = 12345;
  typedef constant_transform<std::decay<decltype(ic)>::type, ic> i;
  EXPECT_EQ(ic, (i::apply<>::value));
  EXPECT_EQ(ic, (i::apply<bool>::value));
  EXPECT_EQ(ic, (i::apply<int>::value));
  EXPECT_EQ(ic, (i::apply<std::string>::value));
  EXPECT_EQ(ic, (i::apply<double>::value));
  EXPECT_EQ(ic, (i::apply<double, void>::value));
  EXPECT_EQ(ic, (i::apply<identity_transform<double>>::value));

  constexpr bool bc = true;
  typedef constant_transform<std::decay<decltype(bc)>::type, bc> b;
  EXPECT_EQ(bc, (b::apply<>::value));
  EXPECT_EQ(bc, (b::apply<bool>::value));
  EXPECT_EQ(bc, (b::apply<int>::value));
  EXPECT_EQ(bc, (b::apply<std::string>::value));
  EXPECT_EQ(bc, (b::apply<double>::value));
  EXPECT_EQ(bc, (b::apply<double, void>::value));
  EXPECT_EQ(bc, (b::apply<identity_transform<double>>::value));
}

////////////////////////
// transform_sequence //
////////////////////////

template <typename TNested, typename TExpected, typename T>
void check_transform_sequence() {
  FATAL_EXPECT_SAME<TExpected, typename TNested::template apply<T>>();
}

TEST(transform_sequence, transform_sequence) {
  typedef transform_sequence<T1, T2, T3> ttt;

  check_transform_sequence<ttt, T3<T2<T1<int>>>, int>();
  check_transform_sequence<ttt, T3<T2<T1<int &&>>>, int &&>();
  check_transform_sequence<ttt, T3<T2<T1<int const &>>>, int const &>();
  check_transform_sequence<ttt, T3<T2<T1<std::string>>>, std::string>();
  check_transform_sequence<ttt, T3<T2<T1<std::string &&>>>, std::string &&>();
  check_transform_sequence<
    ttt, T3<T2<T1<std::string const &>>>, std::string const &
  >();

  using dttt = transform_sequence<
    type_member_transform<std::decay>::template apply, ttt::template apply
  >;

  check_transform_sequence<dttt, T3<T2<T1<int>>>, int>();
  check_transform_sequence<dttt, T3<T2<T1<int>>>, int &&>();
  check_transform_sequence<dttt, T3<T2<T1<int>>>, int const &>();
  check_transform_sequence<dttt, T3<T2<T1<std::string>>>, std::string>();
  check_transform_sequence<dttt, T3<T2<T1<std::string>>>, std::string &&>();
  check_transform_sequence<
    dttt, T3<T2<T1<std::string>>>, std::string const &
  >();
}

//////////////////////////
// arithmetic_transform //
//////////////////////////

template <int... Values>
using test_add = transform_int<arithmetic_transform::add, Values...>;

TEST(arithmetic_transform, add) {
  EXPECT_EQ(0,    (test_add<0>::value));
  EXPECT_EQ(1,    (test_add<1>::value));
  EXPECT_EQ(2,    (test_add<2>::value));
  EXPECT_EQ(56,   (test_add<56>::value));
  EXPECT_EQ(100,  (test_add<100>::value));

  EXPECT_EQ(200,  (test_add<100, 100>::value));
  EXPECT_EQ(157,  (test_add<100, 57>::value));
  EXPECT_EQ(102,  (test_add<100, 2>::value));
  EXPECT_EQ(103,  (test_add<100, 3>::value));
  EXPECT_EQ(102,  (test_add<2, 100>::value));
  EXPECT_EQ(143,  (test_add<43, 100>::value));

  EXPECT_EQ(400,  (test_add<100, 100, 100, 100>::value));
  EXPECT_EQ(108,  (test_add<100, 5, 2, 1>::value));
  EXPECT_EQ(125,  (test_add<100, 20, 5>::value));
  EXPECT_EQ(121,  (test_add<100, 2, 19>::value));
  EXPECT_EQ(109,  (test_add<100, 3, 6>::value));
  EXPECT_EQ(110,  (test_add<100, 3, 1, 6>::value));
  EXPECT_EQ(1102, (test_add<2, 100, 1000>::value));
  EXPECT_EQ(1036, (test_add<1000, 32, 4>::value));
}

template <int... Values>
using test_subtract = transform_int<arithmetic_transform::subtract, Values...>;

TEST(arithmetic_transform, subtract) {
  EXPECT_EQ(0,     (test_subtract<100, 100>::value));
  EXPECT_EQ(43,    (test_subtract<100, 57>::value));
  EXPECT_EQ(98,    (test_subtract<100, 2>::value));
  EXPECT_EQ(97,    (test_subtract<100, 3>::value));
  EXPECT_EQ(-98,   (test_subtract<2, 100>::value));
  EXPECT_EQ(-57,   (test_subtract<43, 100>::value));

  EXPECT_EQ(-200,  (test_subtract<100, 100, 100, 100>::value));
  EXPECT_EQ(92,    (test_subtract<100, 5, 2, 1>::value));
  EXPECT_EQ(75,    (test_subtract<100, 20, 5>::value));
  EXPECT_EQ(79,    (test_subtract<100, 2, 19>::value));
  EXPECT_EQ(91,    (test_subtract<100, 3, 6>::value));
  EXPECT_EQ(90,    (test_subtract<100, 3, 1, 6>::value));
  EXPECT_EQ(-1098, (test_subtract<2, 100, 1000>::value));
  EXPECT_EQ(964,   (test_subtract<1000, 32, 4>::value));
}

template <int... Values>
using test_multiply = transform_int<arithmetic_transform::multiply, Values...>;

TEST(arithmetic_transform, multiply) {
  EXPECT_EQ(0,         (test_multiply<0>::value));
  EXPECT_EQ(1,         (test_multiply<1>::value));
  EXPECT_EQ(2,         (test_multiply<2>::value));
  EXPECT_EQ(56,        (test_multiply<56>::value));
  EXPECT_EQ(100,       (test_multiply<100>::value));

  EXPECT_EQ(10000,     (test_multiply<100, 100>::value));
  EXPECT_EQ(5700,      (test_multiply<100, 57>::value));
  EXPECT_EQ(200,       (test_multiply<100, 2>::value));
  EXPECT_EQ(300,       (test_multiply<100, 3>::value));
  EXPECT_EQ(200,       (test_multiply<2, 100>::value));
  EXPECT_EQ(4300,      (test_multiply<43, 100>::value));

  EXPECT_EQ(100000000, (test_multiply<100, 100, 100, 100>::value));
  EXPECT_EQ(1000,      (test_multiply<100, 5, 2, 1>::value));
  EXPECT_EQ(10000,     (test_multiply<100, 20, 5>::value));
  EXPECT_EQ(3800,      (test_multiply<100, 2, 19>::value));
  EXPECT_EQ(1800,      (test_multiply<100, 3, 6>::value));
  EXPECT_EQ(1800,      (test_multiply<100, 3, 1, 6>::value));
  EXPECT_EQ(200000,    (test_multiply<2, 100, 1000>::value));
  EXPECT_EQ(128000,    (test_multiply<1000, 32, 4>::value));
}

template <int... Values>
using test_divide = transform_int<arithmetic_transform::divide, Values...>;

TEST(arithmetic_transform, divide) {
  EXPECT_EQ(1,   (test_divide<100, 100>::value));
  EXPECT_EQ(1,   (test_divide<100, 57>::value));
  EXPECT_EQ(50,  (test_divide<100, 2>::value));
  EXPECT_EQ(33,  (test_divide<100, 3>::value));
  EXPECT_EQ(0,   (test_divide<2, 100>::value));
  EXPECT_EQ(0,   (test_divide<43, 100>::value));

  EXPECT_EQ(0,   (test_divide<100, 100, 100, 100>::value));
  EXPECT_EQ(10,  (test_divide<100, 5, 2, 1>::value));
  EXPECT_EQ(1,   (test_divide<100, 20, 5>::value));
  EXPECT_EQ(2,   (test_divide<100, 2, 19>::value));
  EXPECT_EQ(5,   (test_divide<100, 3, 6>::value));
  EXPECT_EQ(5,   (test_divide<100, 3, 1, 6>::value));
  EXPECT_EQ(0,   (test_divide<2, 100, 1000>::value));
  EXPECT_EQ(7,   (test_divide<1000, 32, 4>::value));
}

template <int... Values>
using test_modulo = transform_int<arithmetic_transform::modulo, Values...>;

TEST(arithmetic_transform, modulo) {
  EXPECT_EQ(0,   (test_modulo<100, 100>::value));
  EXPECT_EQ(43,  (test_modulo<100, 57>::value));
  EXPECT_EQ(0,   (test_modulo<100, 2>::value));
  EXPECT_EQ(1,   (test_modulo<100, 3>::value));
  EXPECT_EQ(2,   (test_modulo<2, 100>::value));
  EXPECT_EQ(43,  (test_modulo<43, 100>::value));

  EXPECT_EQ(0,   (test_modulo<100, 100, 100, 100>::value));
  EXPECT_EQ(0,   (test_modulo<100, 57, 100, 1>::value));
  EXPECT_EQ(3,   (test_modulo<100, 57, 5>::value));
  EXPECT_EQ(0,   (test_modulo<100, 2, 99>::value));
  EXPECT_EQ(1,   (test_modulo<100, 3, 6>::value));
  EXPECT_EQ(0,   (test_modulo<100, 3, 1, 6>::value));
  EXPECT_EQ(2,   (test_modulo<2, 100, 1000>::value));
  EXPECT_EQ(43,  (test_modulo<43, 1000, 100>::value));
}

///////////////////////
// logical_transform //
///////////////////////

template <typename A, typename B, typename C>
using all_equal_test_impl = logical_transform::all<
  std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
>;

TEST(logical_transform, all) {
  EXPECT_FALSE((all_equal_test_impl<int, bool, double>::value));
  EXPECT_FALSE((all_equal_test_impl<int, bool, int>::value));
  EXPECT_TRUE((all_equal_test_impl<int, int, int>::value));
}

template <typename A, typename B, typename C>
using has_duplicate_test_impl = logical_transform::any<
  std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
>;

TEST(logical_transform, any) {
  EXPECT_FALSE((has_duplicate_test_impl<int, bool, double>::value));
  EXPECT_TRUE((has_duplicate_test_impl<int, bool, int>::value));
  EXPECT_TRUE((has_duplicate_test_impl<int, int, int>::value));
}

TEST(logical_transform, negate) {
  EXPECT_TRUE(logical_transform::negate<std::false_type>::value);
  EXPECT_FALSE(logical_transform::negate<std::true_type>::value);
}

///////////////////////
// bitwise_transform //
///////////////////////

template <int... Args>
using all_test_impl = bitwise_transform::all<
  std::integral_constant<int, Args>...
>;

TEST(bitwise_transform, all) {
  EXPECT_EQ(99, (all_test_impl<99>::value));
  EXPECT_EQ(0, (all_test_impl<1, 2, 4>::value));
  EXPECT_EQ(3, (all_test_impl<7, 11>::value));
  EXPECT_EQ(8 & 9 & 57, (all_test_impl<8, 9, 57>::value));
}

template <int... Args>
using any_test_impl = bitwise_transform::any<
  std::integral_constant<int, Args>...
>;

TEST(bitwise_transform, any) {
  EXPECT_EQ(99, (any_test_impl<99>::value));
  EXPECT_EQ(7, (any_test_impl<1, 2, 4>::value));
  EXPECT_EQ(8 | 9 | 57, (any_test_impl<8, 9, 57>::value));
}

template <int... Args>
using diff_test_impl = bitwise_transform::diff<
  std::integral_constant<int, Args>...
>;

TEST(bitwise_transform, diff) {
  EXPECT_EQ(99, (diff_test_impl<99>::value));
  EXPECT_EQ(3, (diff_test_impl<1, 2>::value));
  EXPECT_EQ(12, (diff_test_impl<7, 11>::value));
  EXPECT_EQ(1 ^ 2 ^ 4, (diff_test_impl<1, 2, 4>::value));
  EXPECT_EQ(8 ^ 9 ^ 57, (diff_test_impl<8, 9, 57>::value));
}

TEST(bitwise_transform, complement) {
# define FATAL_TEST_IMPL(x) \
  do { \
    EXPECT_EQ( \
      ~static_cast<unsigned>(x), \
      (bitwise_transform::complement< \
        std::integral_constant<unsigned, (x)> \
      >::value) \
    );\
  } while(false)

  FATAL_TEST_IMPL(0);
  FATAL_TEST_IMPL(2);
  FATAL_TEST_IMPL(3);
  FATAL_TEST_IMPL(99);

  EXPECT_EQ(
    static_cast<uint8_t>(0xf0),
    (bitwise_transform::complement<std::integral_constant<uint8_t, 0xf>>::value)
  );

# undef FATAL_TEST_IMPL
}

//////////////////////////
// comparison_transform //
//////////////////////////

TEST(comparison_transform, equal) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((comparison_transform::equal<A, B>::value));
  EXPECT_FALSE((comparison_transform::equal<B, A>::value));
  EXPECT_TRUE((comparison_transform::equal<A, A>::value));
}

TEST(comparison_transform, not_equal) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_TRUE((comparison_transform::not_equal<A, B>::value));
  EXPECT_TRUE((comparison_transform::not_equal<B, A>::value));
  EXPECT_FALSE((comparison_transform::not_equal<A, A>::value));
}

TEST(comparison_transform, less_than) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_TRUE((comparison_transform::less_than<A, B>::value));
  EXPECT_FALSE((comparison_transform::less_than<B, A>::value));
  EXPECT_FALSE((comparison_transform::less_than<A, A>::value));
}

TEST(comparison_transform, less_than_equal) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_TRUE((comparison_transform::less_than_equal<A, B>::value));
  EXPECT_FALSE((comparison_transform::less_than_equal<B, A>::value));
  EXPECT_TRUE((comparison_transform::less_than_equal<A, A>::value));
}

TEST(comparison_transform, greater_than) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((comparison_transform::greater_than<A, B>::value));
  EXPECT_TRUE((comparison_transform::greater_than<B, A>::value));
  EXPECT_FALSE((comparison_transform::greater_than<A, A>::value));
}

TEST(comparison_transform, greater_than_equal) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((comparison_transform::greater_than_equal<A, B>::value));
  EXPECT_TRUE((comparison_transform::greater_than_equal<B, A>::value));
  EXPECT_TRUE((comparison_transform::greater_than_equal<A, A>::value));
}

////////////////////////
// get_member_typedef //
////////////////////////

struct get_member_typedef_test {
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

TEST(type_traits, get_member_typedef) {
# define CREATE_TEST(Member, Type) \
  do { \
    FATAL_EXPECT_SAME<Type::Member, get_member_typedef::Member<Type>>(); \
  } while (false)

  CREATE_TEST(tag, get_member_typedef_test);

  CREATE_TEST(types, get_member_typedef_test);
  CREATE_TEST(values, get_member_typedef_test);
  CREATE_TEST(args, get_member_typedef_test);

  CREATE_TEST(pair, get_member_typedef_test);
  CREATE_TEST(tuple, get_member_typedef_test);
  CREATE_TEST(list, get_member_typedef_test);
  CREATE_TEST(map, get_member_typedef_test);
  CREATE_TEST(array, get_member_typedef_test);
  CREATE_TEST(set, get_member_typedef_test);
  CREATE_TEST(string, get_member_typedef_test);
  CREATE_TEST(index, get_member_typedef_test);
  CREATE_TEST(flag, get_member_typedef_test);

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

TEST(conditional_transform, when_true) {
  using transform = conditional_transform<std::is_integral, T1>;
  FATAL_EXPECT_SAME<T1<long>, transform::apply<long>>();
  FATAL_EXPECT_SAME<std::string, transform::apply<std::string>>();
}

TEST(conditional_transform, ternary) {
  using transform = conditional_transform<std::is_integral, T1, T2>;
  FATAL_EXPECT_SAME<T1<long>, transform::apply<long>>();
  FATAL_EXPECT_SAME<T2<std::string>, transform::apply<std::string>>();
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

TEST(type_member_transform, type_member_transform) {
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

TEST(transform_alias, apply) {
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

TEST(transform_alias, type) {
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

TEST(transform_alias, curry) {
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

TEST(transform_alias, rebind) {
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

TEST(transform_alias, uncurry) {
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

TEST(transform_alias, rebind_args) {
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

TEST(transform_alias, apply_args) {
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

TEST(transform_switch, empty_identity) {
  using transform = identity_transform_switch<>;

  FATAL_EXPECT_SAME<int, transform::apply<int>>();
}

TEST(transform_switch, empty_T1) {
  using transform = transform_switch<T1>;

  FATAL_EXPECT_SAME<T1<int>, transform::apply<int>>();
}

TEST(transform_switch, identity) {
  using transform = identity_transform_switch<
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

TEST(transform_switch, T1) {
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

TEST(member_transformer, transform) {
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

  TEST_IMPL(identity_transform);
  TEST_IMPL(identity_transform, void);
  TEST_IMPL(identity_transform, int);
  TEST_IMPL(identity_transform, int, void);
  TEST_IMPL(identity_transform, int, void, test_list<>);
  TEST_IMPL(identity_transform, int, void, test_list<float>);
  TEST_IMPL(identity_transform, int, void, test_list<float, bool>);
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

TEST(member_transformer, apply_with_args) {
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

TEST(member_transformer_stack, use) {
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

TEST(member_transformer_stack, pre) {
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

TEST(member_transformer_stack, multi_pre) {
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

TEST(member_transformer_stack, post) {
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

TEST(member_transformer_stack, multi_post) {
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

TEST(member_transformer_stack, pre_post) {
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

TEST(member_transformer_stack, multi_pre_post) {
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

TEST(member_transformer_stack, pre_multi_post) {
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

TEST(member_transformer_stack, multi_pre_multi_post) {
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

TEST(member_transformer_stack, ad_hoc_stack_0) {
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
  template <typename...> class TPreTransform = identity_transform,
  template <typename...> class TPostTransform = identity_transform,
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

TEST(recursive_transform, identity) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity_transform, std::numeric_limits<std::size_t>::max()
  >();
}

TEST(recursive_transform, identity_0) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity_transform, 0
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity_transform, 0
  >();
}

TEST(recursive_transform, identity_1) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity_transform, 1
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity_transform, 1
  >();
}

TEST(recursive_transform, identity_2) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity_transform, 2
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity_transform, 2
  >();
}

TEST(recursive_transform, identity_3) {
  check_recursive_list_transform<
    test_list<>,
    test_list<>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void>,
    test_list<void>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, int>,
    test_list<void, int>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>>,
    test_list<void, test_list<>>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<>, int>,
    test_list<void, test_list<>, int>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double>, int>,
    test_list<void, test_list<double>, int>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool>, int>,
    test_list<void, test_list<double, bool>, int>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<>>, int>,
    test_list<void, test_list<double, bool, test_list<>>, int>,
    is_list, identity_transform, 3
  >();
  check_recursive_list_transform<
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    test_list<void, test_list<double, bool, test_list<float>>, int>,
    is_list, identity_transform, 3
  >();
}

TEST(recursive_transform, foo) {
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

TEST(recursive_transform, foo_0) {
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

TEST(recursive_transform, foo_1) {
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

TEST(recursive_transform, foo_2) {
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

TEST(recursive_transform, foo_3) {
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

TEST(recursive_transform, pre_post_identity) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity_transform, std::numeric_limits<std::size_t>::max(),
    get_t0, V1
  >();
}

TEST(recursive_transform, pre_post_identity_0) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>>>,
    T0<test_list<void, test_list<>>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<>, int>>,
    T0<test_list<void, test_list<>, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double>, int>>,
    T0<test_list<void, test_list<double>, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool>, int>>,
    T0<test_list<void, test_list<double, bool>, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<>>, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    T0<test_list<void, test_list<double, bool, test_list<float>>, int>>,
    is_T0_list, identity_transform, 0, get_t0, V1
  >();
}

TEST(recursive_transform, pre_post_identity_1) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, test_list<>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, test_list<>>>, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, test_list<float>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, test_list<float>>>, int>>,
    is_T0_list, identity_transform, 1, get_t0, V1
  >();
}

TEST(recursive_transform, pre_post_identity_2) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity_transform, 2, get_t0, V1
  >();
}

TEST(recursive_transform, pre_post_identity_3) {
  check_recursive_list_transform<
    V1<test_list<>>,
    T0<test_list<>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void>>,
    T0<test_list<void>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, int>>,
    T0<test_list<void, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>>>,
    T0<test_list<void, T0<test_list<>>>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<>>, int>>,
    T0<test_list<void, T0<test_list<>>, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double>>, int>>,
    T0<test_list<void, T0<test_list<double>>, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool>>, int>>,
    T0<test_list<void, T0<test_list<double, bool>>, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<>>>>, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
  check_recursive_list_transform<
    V1<test_list<void, V1<test_list<double, bool, V1<test_list<float>>>>, int>>,
    T0<test_list<void, T0<test_list<double, bool, T0<test_list<float>>>>, int>>,
    is_T0_list, identity_transform, 3, get_t0, V1
  >();
}

TEST(recursive_transform, pre_post_foo) {
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

TEST(recursive_transform, pre_post_foo_0) {
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

TEST(recursive_transform, pre_post_foo_1) {
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

TEST(recursive_transform, pre_post_foo_2) {
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

TEST(recursive_transform, pre_post_foo_3) {
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

} // namespace fatal {
