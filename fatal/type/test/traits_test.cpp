/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/traits.h>

#include <fatal/test/driver.h>

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>

namespace fatal {

template <std::size_t> struct T {};
template <std::size_t> struct S {};

template <typename> struct test_tag {};
template <typename...> struct test_list {};
template <typename T, T...> struct test_seq {};

/////////////////
// is_complete //
/////////////////

struct complete_type {};
struct incomplete_type;

TEST(type_traits, is_complete) {
  EXPECT_TRUE((is_complete<int>::value));
  EXPECT_TRUE((is_complete<std::string>::value));
  EXPECT_TRUE((is_complete<complete_type>::value));
  EXPECT_FALSE((is_complete<incomplete_type>::value));
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

/////////////
// type_of //
/////////////

TEST(type_traits, type_of) {
  typedef std::integral_constant<int, 5> i5;
  FATAL_EXPECT_SAME<int, type_of<i5>>();

  FATAL_EXPECT_SAME<bool, type_of<std::true_type>>();
}

//////////////////
// add_const_if //
//////////////////

template <typename T, typename TWhenTrue, typename TWhenFalse>
void check_add_const_if() {
  FATAL_EXPECT_SAME<TWhenTrue, add_const_if<T, true>>();
  FATAL_EXPECT_SAME<TWhenFalse, add_const_if<T, false>>();
}

TEST(type_traits, add_const_if) {
  check_add_const_if<int, int const, int>();
  check_add_const_if<int const, int const, int const>();
  check_add_const_if<int &, int &, int &>();
  check_add_const_if<int const &, int const &, int const &>();
}

/////////////////////
// negate_constant //
/////////////////////

TEST(type_traits, negate_constant) {
  EXPECT_TRUE(negate_constant<std::false_type>::value);
  EXPECT_FALSE(negate_constant<std::true_type>::value);
}

template <typename A, typename B, typename C>
using all_equal_test_impl = logical_and_constants<
  std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
>;

///////////////////////////
// logical_and_constants //
///////////////////////////

TEST(type_traits, logical_and_constants) {
  EXPECT_FALSE((all_equal_test_impl<int, bool, double>::value));
  EXPECT_FALSE((all_equal_test_impl<int, bool, int>::value));
  EXPECT_TRUE((all_equal_test_impl<int, int, int>::value));
}

//////////////////////////
// logical_or_constants //
//////////////////////////

template <typename A, typename B, typename C>
using has_duplicate_test_impl = logical_or_constants<
  std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
>;

TEST(type_traits, logical_or_constants) {
  EXPECT_FALSE((has_duplicate_test_impl<int, bool, double>::value));
  EXPECT_TRUE((has_duplicate_test_impl<int, bool, int>::value));
  EXPECT_TRUE((has_duplicate_test_impl<int, int, int>::value));
}

/////////////////////////
// complement_constant //
/////////////////////////

TEST(type_traits, complement_constant) {
# define CHECK_COMPLEMENT_CONSTANT(x) \
  do { \
    EXPECT_EQ( \
      ~static_cast<unsigned>(x), \
      (complement_constant<std::integral_constant<unsigned, (x)>>::value) \
    );\
  } while(false)

  CHECK_COMPLEMENT_CONSTANT(0);
  CHECK_COMPLEMENT_CONSTANT(2);
  CHECK_COMPLEMENT_CONSTANT(3);
  CHECK_COMPLEMENT_CONSTANT(99);

  EXPECT_EQ(
    static_cast<uint8_t>(0xf0),
    (complement_constant<std::integral_constant<uint8_t, 0xf>>::value)
  );

# undef CHECK_COMPLEMENT_CONSTANT
}

///////////////////////////
// bitwise_and_constants //
///////////////////////////

template <int... Args>
using bitwise_and_test_impl = bitwise_and_constants<
  std::integral_constant<int, Args>...
>;

TEST(type_traits, bitwise_and_constants) {
  EXPECT_EQ(99, (bitwise_and_test_impl<99>::value));
  EXPECT_EQ(0, (bitwise_and_test_impl<1, 2, 4>::value));
  EXPECT_EQ(3, (bitwise_and_test_impl<7, 11>::value));
  EXPECT_EQ(8 & 9 & 57, (bitwise_and_test_impl<8, 9, 57>::value));
}

//////////////////////////
// bitwise_or_constants //
//////////////////////////

template <int... Args>
using bitwise_or_test_impl = bitwise_or_constants<
  std::integral_constant<int, Args>...
>;

TEST(type_traits, bitwise_or_constants) {
  EXPECT_EQ(99, (bitwise_or_test_impl<99>::value));
  EXPECT_EQ(7, (bitwise_or_test_impl<1, 2, 4>::value));
  EXPECT_EQ(8 | 9 | 57, (bitwise_or_test_impl<8, 9, 57>::value));
}

///////////////////////////
// bitwise_xor_constants //
///////////////////////////

template <int... Args>
using bitwise_xor_test_impl = bitwise_xor_constants<
  std::integral_constant<int, Args>...
>;

TEST(type_traits, bitwise_xor_constants) {
  EXPECT_EQ(99, (bitwise_xor_test_impl<99>::value));
  EXPECT_EQ(3, (bitwise_xor_test_impl<1, 2>::value));
  EXPECT_EQ(12, (bitwise_xor_test_impl<7, 11>::value));
  EXPECT_EQ(1 ^ 2 ^ 4, (bitwise_xor_test_impl<1, 2, 4>::value));
  EXPECT_EQ(8 ^ 9 ^ 57, (bitwise_xor_test_impl<8, 9, 57>::value));
}

/////////////////////////////
// constants_comparison_lt //
/////////////////////////////

TEST(type_traits, constants_comparison_lt) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_TRUE((constants_comparison_lt<A, B>::value));
  EXPECT_FALSE((constants_comparison_lt<B, A>::value));
  EXPECT_FALSE((constants_comparison_lt<A, A>::value));
}

/////////////////////////////
// constants_comparison_gt //
/////////////////////////////

TEST(type_traits, constants_comparison_gt) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((constants_comparison_gt<A, B>::value));
  EXPECT_TRUE((constants_comparison_gt<B, A>::value));
  EXPECT_FALSE((constants_comparison_gt<A, A>::value));
}

/////////////////////////////
// constants_comparison_eq //
/////////////////////////////

TEST(type_traits, constants_comparison_eq) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((constants_comparison_eq<A, B>::value));
  EXPECT_FALSE((constants_comparison_eq<B, A>::value));
  EXPECT_TRUE((constants_comparison_eq<A, A>::value));
}

//////////////////////////////
// constants_comparison_lte //
//////////////////////////////

TEST(type_traits, constants_comparison_lte) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_TRUE((constants_comparison_lte<A, B>::value));
  EXPECT_FALSE((constants_comparison_lte<B, A>::value));
  EXPECT_TRUE((constants_comparison_lte<A, A>::value));
}

//////////////////////////////
// constants_comparison_gte //
//////////////////////////////

TEST(type_traits, constants_comparison_gte) {
  typedef std::integral_constant<int, 10> A;
  typedef std::integral_constant<int, 20> B;

  EXPECT_FALSE((constants_comparison_gte<A, B>::value));
  EXPECT_TRUE((constants_comparison_gte<B, A>::value));
  EXPECT_TRUE((constants_comparison_gte<A, A>::value));
}

/////////////////
// is_template //
/////////////////

template <typename T, bool Expected, template <typename...> class... TTemplates>
void check_is_template() {
  bool expected = Expected;
  using checker = is_template<TTemplates...>;
  bool actual = checker::template type<T>::value;
  if (expected != actual) {
    LOG(ERROR) << "checker: " << type_str<checker>();
    LOG(ERROR) << "type: " << type_str<T>();
    EXPECT_EQ(expected, actual);
  }
}

TEST(type_traits, is_template) {
  check_is_template<std::string, false, std::tuple>();
  check_is_template<std::wstring, false, std::tuple>();
  check_is_template<std::basic_string<int>, false, std::tuple>();
  check_is_template<std::pair<int, double>, false, std::tuple>();
  check_is_template<std::vector<int>, false, std::tuple>();
  check_is_template<std::tuple<int>, true, std::tuple>();
  check_is_template<std::tuple<int, double>, true, std::tuple>();

  check_is_template<std::string, false, std::vector>();
  check_is_template<std::wstring, false, std::vector>();
  check_is_template<std::basic_string<int>, false, std::vector>();
  check_is_template<std::pair<int, double>, false, std::vector>();
  check_is_template<std::vector<int>, true, std::vector>();
  check_is_template<std::tuple<int>, false, std::vector>();
  check_is_template<std::tuple<int, double>, false, std::vector>();

  check_is_template<std::string, true, std::basic_string>();
  check_is_template<std::wstring, true, std::basic_string>();
  check_is_template<std::basic_string<int>, true, std::basic_string>();
  check_is_template<std::pair<int, double>, false, std::basic_string>();
  check_is_template<std::vector<int>, false, std::basic_string>();
  check_is_template<std::tuple<int>, false, std::basic_string>();
  check_is_template<std::tuple<int, double>, false, std::basic_string>();

  check_is_template<std::string, false, std::tuple, std::vector>();
  check_is_template<std::wstring, false, std::tuple, std::vector>();
  check_is_template<std::basic_string<int>, false, std::tuple, std::vector>();
  check_is_template<std::pair<int, double>, false, std::tuple, std::vector>();
  check_is_template<std::vector<int>, true, std::tuple, std::vector>();
  check_is_template<std::tuple<int>, true, std::tuple, std::vector>();
  check_is_template<std::tuple<int, double>, true, std::tuple, std::vector>();

  check_is_template<std::string, true, std::tuple, std::basic_string>();
  check_is_template<std::wstring, true, std::tuple, std::basic_string>();
  check_is_template<
    std::basic_string<int>, true,
    std::tuple, std::basic_string
  >();
  check_is_template<
    std::pair<int, double>, false,
    std::tuple, std::basic_string
  >();
  check_is_template<std::vector<int>, false, std::tuple, std::basic_string>();
  check_is_template<std::tuple<int>, true, std::tuple, std::basic_string>();
  check_is_template<
    std::tuple<int, double>, true,
    std::tuple, std::basic_string
  >();
}

///////////////////////////
// curried_type_comparer //
///////////////////////////

template <typename LHS, typename RHS>
struct curried_type_comparer_foo {
  template <template <typename, typename> class TComparer>
  using comparison = std::integral_constant<
    bool, TComparer<LHS, RHS>::value
  >;
};

template <int X>
struct curried_type_comparer_bar {
  typedef std::integral_constant<int, X> type;
};

TEST(type_traits, curried_type_comparer) {
  typedef curried_type_comparer_foo<
    std::integral_constant<int, 5>,
    std::integral_constant<int, 8>
  > values_5_8;

  EXPECT_TRUE((
    values_5_8::comparison<
      curried_type_comparer<>::template compare
    >::value
  ));

  EXPECT_FALSE((
    values_5_8::comparison<
      curried_type_comparer<
        constants_comparison_gt
      >::template compare
    >::value
  ));

  typedef curried_type_comparer_foo<
    curried_type_comparer_bar<80>,
    curried_type_comparer_bar<10>
  > values_80_10;

  EXPECT_TRUE((
    values_80_10::comparison<
      curried_type_comparer<
        constants_comparison_gt,
        get_member_typedef::template type
      >::template compare
    >::value
  ));
}

////////////////////////
// fast_pass_by_value //
////////////////////////

TEST(type_traits, fast_pass_by_value) {
  EXPECT_TRUE(fast_pass_by_value<bool>::value);
  EXPECT_TRUE(fast_pass_by_value<bool &>::value);
  EXPECT_TRUE(fast_pass_by_value<bool &&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const &>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const &&>::value);

  EXPECT_TRUE(fast_pass_by_value<bool *>::value);
  EXPECT_TRUE(fast_pass_by_value<bool *&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<bool *const &&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const *>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const *&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<bool const *const &&>::value);

  EXPECT_TRUE(fast_pass_by_value<int>::value);
  EXPECT_TRUE(fast_pass_by_value<int &>::value);
  EXPECT_TRUE(fast_pass_by_value<int &&>::value);
  EXPECT_TRUE(fast_pass_by_value<int const>::value);
  EXPECT_TRUE(fast_pass_by_value<int const &>::value);
  EXPECT_TRUE(fast_pass_by_value<int const &&>::value);

  EXPECT_TRUE(fast_pass_by_value<int *>::value);
  EXPECT_TRUE(fast_pass_by_value<int *&>::value);
  EXPECT_TRUE(fast_pass_by_value<int *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<int *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<int *const &&>::value);
  EXPECT_TRUE(fast_pass_by_value<int const *>::value);
  EXPECT_TRUE(fast_pass_by_value<int const *&>::value);
  EXPECT_TRUE(fast_pass_by_value<int const *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<int const *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<int const *const &&>::value);

  EXPECT_FALSE(fast_pass_by_value<std::string>::value);
  EXPECT_FALSE(fast_pass_by_value<std::string &>::value);
  EXPECT_FALSE(fast_pass_by_value<std::string &&>::value);
  EXPECT_FALSE(fast_pass_by_value<std::string const>::value);
  EXPECT_FALSE(fast_pass_by_value<std::string const &>::value);
  EXPECT_FALSE(fast_pass_by_value<std::string const &&>::value);

  EXPECT_TRUE(fast_pass_by_value<std::string *>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string *&>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string *const &&>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string const *>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string const *&>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string const *&&>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string const *const &>::value);
  EXPECT_TRUE(fast_pass_by_value<std::string const *const &&>::value);
}

///////////////
// fast_pass //
///////////////

TEST(type_traits, fast_pass) {
  FATAL_EXPECT_SAME<bool const, fast_pass<bool>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool &>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool &&>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const &>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const &&>>();

  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *&>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *&&>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool * const &>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool * const &&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *&&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const * const &>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const * const &&>>();

  FATAL_EXPECT_SAME<int const, fast_pass<int>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int &>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int &&>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const &>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const &&>>();

  FATAL_EXPECT_SAME<int *const, fast_pass<int *>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int *&>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int *&&>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int * const &>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int * const &&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *&&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const * const &>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const * const &&>>();

  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string &>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string &&>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const &>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const &&>>();

  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *&>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *&&>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string * const &>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string * const &&>>();
  FATAL_EXPECT_SAME<std::string const *const, fast_pass<std::string const *>>();
  FATAL_EXPECT_SAME<std::string const *const, fast_pass<std::string const *&>>();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const *&&>
  >();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const * const &>
  >();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const * const &&>
  >();
}

////////////////
// is_functor //
////////////////

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

typedef void(*foonction)();
typedef void(*foonction_is)(int, std::string);

TEST(type_traits, is_functor) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  EXPECT_TRUE((is_functor<foonctor>::value));
  EXPECT_FALSE((is_functor<foonctor, int>::value));
  EXPECT_FALSE((is_functor<foonctor, int, double>::value));
  EXPECT_TRUE((is_functor<foonctor, int, std::string>::value));

  EXPECT_TRUE((is_functor<decltype(lambda)>::value));
  EXPECT_FALSE((is_functor<decltype(lambda), int>::value));
  EXPECT_FALSE((is_functor<decltype(lambda), int, double>::value));
  EXPECT_FALSE((is_functor<decltype(lambda), int, std::string>::value));

  EXPECT_FALSE((is_functor<decltype(lambda_is)>::value));
  EXPECT_FALSE((is_functor<decltype(lambda_is), int>::value));
  EXPECT_FALSE((is_functor<decltype(lambda_is), int, double>::value));
  EXPECT_TRUE((is_functor<decltype(lambda_is), int, std::string>::value));

  EXPECT_FALSE((is_functor<foonction>::value));
  EXPECT_FALSE((is_functor<foonction, int>::value));
  EXPECT_FALSE((is_functor<foonction, int, double>::value));
  EXPECT_FALSE((is_functor<foonction, int, std::string>::value));

  EXPECT_FALSE((is_functor<foonction_is>::value));
  EXPECT_FALSE((is_functor<foonction_is, int>::value));
  EXPECT_FALSE((is_functor<foonction_is, int, double>::value));
  EXPECT_FALSE((is_functor<foonction_is, int, std::string>::value));
}

/////////////////
// is_callable //
/////////////////

TEST(type_traits, is_callable) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  EXPECT_TRUE((is_callable<foonctor>::value));
  EXPECT_FALSE((is_callable<foonctor, int>::value));
  EXPECT_FALSE((is_callable<foonctor, int, double>::value));
  EXPECT_TRUE((is_callable<foonctor, int, std::string>::value));

  EXPECT_TRUE((is_callable<decltype(lambda)>::value));
  EXPECT_FALSE((is_callable<decltype(lambda), int>::value));
  EXPECT_FALSE((is_callable<decltype(lambda), int, double>::value));
  EXPECT_FALSE((is_callable<decltype(lambda), int, std::string>::value));

  EXPECT_FALSE((is_callable<decltype(lambda_is)>::value));
  EXPECT_FALSE((is_callable<decltype(lambda_is), int>::value));
  EXPECT_FALSE((is_callable<decltype(lambda_is), int, double>::value));
  EXPECT_TRUE((is_callable<decltype(lambda_is), int, std::string>::value));

  EXPECT_TRUE((is_callable<foonction>::value));
  EXPECT_FALSE((is_callable<foonction, int>::value));
  EXPECT_FALSE((is_callable<foonction, int, double>::value));
  EXPECT_FALSE((is_callable<foonction, int, std::string>::value));

  EXPECT_FALSE((is_callable<foonction_is>::value));
  EXPECT_FALSE((is_callable<foonction_is, int>::value));
  EXPECT_FALSE((is_callable<foonction_is, int, double>::value));
  EXPECT_TRUE((is_callable<foonction_is, int, std::string>::value));
}

/////////////////////////////////////////////
// FATAL_CREATE_HAS_MEMBER_FUNCTION_TRAITS //
/////////////////////////////////////////////

namespace fbhmft {
  FATAL_CREATE_HAS_MEMBER_FUNCTION_TRAITS(traits, fn);

  struct A {};
  struct B {};
  struct F {
    int fn() { return 0; }
    float fn(char) { return 0; }
    short fn(double, A &) { return 0; }
    double fn(A &&) { return 0; }
    double fn(B const &&) { return 0; }
    bool doit(std::string) { return false; }
  };
};

TEST(type_traits, create_has_member_function_traits) {
  using namespace fbhmft;

  EXPECT_TRUE((traits<F>::has_member::value));

  EXPECT_TRUE((traits<F, int>::has_member::value));

  EXPECT_TRUE((traits<F, char>::has_member::value));

  EXPECT_TRUE((traits<F, double>::has_member::value));

  EXPECT_FALSE((traits<F, double, A>::has_member::value));
  EXPECT_TRUE((traits<F, double, A &>::has_member::value));

  EXPECT_TRUE((traits<F, A>::has_member::value));
  EXPECT_FALSE((traits<F, A &>::has_member::value));
  EXPECT_FALSE((traits<F, A const &>::has_member::value));
  EXPECT_TRUE((traits<F, A &&>::has_member::value));
  EXPECT_FALSE((traits<F, A const &&>::has_member::value));

  EXPECT_TRUE((traits<F, B>::has_member::value));
  EXPECT_FALSE((traits<F, B &>::has_member::value));
  EXPECT_FALSE((traits<F, B const &>::has_member::value));
  EXPECT_TRUE((traits<F, B &&>::has_member::value));
  EXPECT_TRUE((traits<F, B const &&>::has_member::value));

  EXPECT_FALSE((traits<F, std::string>::has_member::value));
  EXPECT_FALSE((traits<F, std::string &>::has_member::value));
  EXPECT_FALSE((traits<F, std::string const &>::has_member::value));
  EXPECT_FALSE((traits<F, std::string &&>::has_member::value));
  EXPECT_FALSE((traits<F, std::string const &&>::has_member::value));
}

////////////////////////
// safe_ctor_overload //
////////////////////////

struct Base {};
struct Derived: public Base {};
struct Foo {};
enum class ctor { def, copy, move, universal };

TEST(type_traits, safe_ctor_overload) {
  EXPECT_FALSE((safe_ctor_overload<Base, Base>::value));
  EXPECT_FALSE((safe_ctor_overload<Base, Derived>::value));
  EXPECT_TRUE((safe_ctor_overload<Base>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, int>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, void>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, Foo>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, int, int>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, void, void>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, Foo, Foo>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, int, int, int>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, void, void, void>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, Foo, Foo, Foo>::value));
  EXPECT_TRUE((safe_ctor_overload<Base, int, void, Foo, bool>::value));
}

struct overloading_test {
  overloading_test(): type(ctor::def) {}
  overloading_test(overloading_test const &) noexcept: type(ctor::copy) {}
  overloading_test(overloading_test &&) noexcept: type(ctor::move) {}
  template <
    typename T,
    typename X = typename std::enable_if<
      safe_ctor_overload<overloading_test, T>::value, void
    >::type
  >
  explicit overloading_test(T &&): type(ctor::universal) {}

  ctor type;
};

TEST(type_traits, safe_ctor_overload_nonvariadic) {
  overloading_test def;
  EXPECT_EQ(ctor::def, def.type);
  overloading_test copy(def);
  EXPECT_EQ(ctor::copy, copy.type);
  overloading_test move(std::move(def));
  EXPECT_EQ(ctor::move, move.type);
  overloading_test universal(0);
  EXPECT_EQ(ctor::universal, universal.type);
  overloading_test foo{Foo()};
  EXPECT_EQ(ctor::universal, foo.type);
}

struct variadic_overloading_test {
  variadic_overloading_test(): type(ctor::def) {}
  variadic_overloading_test(variadic_overloading_test const &) noexcept:
    type(ctor::copy)
  {}
  variadic_overloading_test(variadic_overloading_test &&) noexcept:
    type(ctor::move)
  {}
  template <
    typename... Args,
    typename X = typename std::enable_if<
      safe_ctor_overload<variadic_overloading_test, Args...>::value, void
    >::type
  >
  explicit variadic_overloading_test(Args &&...): type(ctor::universal) {}

  ctor type;
};

TEST(type_traits, safe_ctor_overload_variadic) {
  variadic_overloading_test def;
  EXPECT_EQ(ctor::def, def.type);
  variadic_overloading_test copy(def);
  EXPECT_EQ(ctor::copy, copy.type);
  variadic_overloading_test move(std::move(def));
  EXPECT_EQ(ctor::move, move.type);
  variadic_overloading_test i(0);
  EXPECT_EQ(ctor::universal, i.type);
  variadic_overloading_test foo{Foo()};
  EXPECT_EQ(ctor::universal, foo.type);
  variadic_overloading_test universal(copy, move);
  EXPECT_EQ(ctor::universal, universal.type);
}

struct overloading_test_t {
  overloading_test_t(): type(ctor::def) {}
  overloading_test_t(overloading_test_t const &) noexcept: type(ctor::copy) {}
  overloading_test_t(overloading_test_t &&) noexcept: type(ctor::move) {}
  template <
    typename T,
    typename X = safe_ctor_overload_t<overloading_test_t, T>
  >
  explicit overloading_test_t(T &&): type(ctor::universal) {}
  ctor type;
};

TEST(type_traits, safe_ctor_overload_nonvariadic_t) {
  overloading_test_t def;
  EXPECT_EQ(ctor::def, def.type);
  overloading_test_t copy(def);
  EXPECT_EQ(ctor::copy, copy.type);
  overloading_test_t move(std::move(def));
  EXPECT_EQ(ctor::move, move.type);
  overloading_test_t universal(0);
  EXPECT_EQ(ctor::universal, universal.type);
  overloading_test_t foo{Foo()};
  EXPECT_EQ(ctor::universal, foo.type);
}

struct variadic_overloading_test_t {
  variadic_overloading_test_t(): type(ctor::def) {}
  variadic_overloading_test_t(variadic_overloading_test_t const &) noexcept:
    type(ctor::copy)
  {}
  variadic_overloading_test_t(variadic_overloading_test_t &&) noexcept:
    type(ctor::move)
  {}
  template <
    typename... Args,
    typename X = safe_ctor_overload_t<variadic_overloading_test_t, Args...>
  >
  explicit variadic_overloading_test_t(Args &&...): type(ctor::universal) {}
  ctor type;
};

TEST(type_traits, safe_ctor_overload_variadic_t) {
  variadic_overloading_test_t def;
  EXPECT_EQ(ctor::def, def.type);
  variadic_overloading_test_t copy(def);
  EXPECT_EQ(ctor::copy, copy.type);
  variadic_overloading_test_t move(std::move(def));
  EXPECT_EQ(ctor::move, move.type);
  variadic_overloading_test_t i(0);
  EXPECT_EQ(ctor::universal, i.type);
  variadic_overloading_test_t foo{Foo()};
  EXPECT_EQ(ctor::universal, foo.type);
  variadic_overloading_test_t universal(copy, move);
  EXPECT_EQ(ctor::universal, universal.type);
}

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

TEST(type_get, std_pair) {
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

TEST(type_get, std_tuple) {
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

} // namespace fatal {
