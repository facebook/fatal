/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/traits.h>

#include <fatal/test/driver.h>

#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace fatal {

template <std::size_t> struct T {};
template <std::size_t> struct S {};
enum class Ell: long long { a = 5, b = 7, c = 9 };

/////////////////
// is_template //
/////////////////

template <typename T, bool Expected, template <typename...> class... TTemplates>
void check_is_template() {
  bool expected = Expected;
  using checker = is_template<TTemplates...>;
  bool actual = checker::template type<T>::value;
  if (expected != actual) {
    FATAL_LOG(ERROR) << "checker: " << type_str<checker>();
    FATAL_LOG(ERROR) << "type: " << type_str<T>();
    FATAL_EXPECT_EQ(expected, actual);
  }
}

template <typename, typename...> struct X0 {};
template <typename, typename, typename...> struct X1 {};
template <typename, typename, typename, typename...> struct X2 {};
template <typename, typename, typename, typename, typename...> struct X3 {};

FATAL_TEST(traits, is_template) {
  check_is_template<X0<void>, true, X0>();
  check_is_template<X0<void, int>, true, X0>();
  check_is_template<X0<void, int, bool>, true, X0>();
  check_is_template<std::string, false, X0>();
  check_is_template<void, false, X0>();
  check_is_template<int, false, X0>();

  check_is_template<X1<void, double>, true, X1>();
  check_is_template<X1<void, double, int>, true, X1>();
  check_is_template<X1<void, double, int, bool>, true, X1>();
  check_is_template<std::string, false, X1>();
  check_is_template<void, false, X1>();
  check_is_template<int, false, X1>();

  check_is_template<X2<void, double, short>, true, X2>();
  check_is_template<X2<void, double, short, int>, true, X2>();
  check_is_template<X2<void, double, short, int, bool>, true, X2>();
  check_is_template<std::string, false, X2>();
  check_is_template<void, false, X2>();
  check_is_template<int, false, X2>();

  check_is_template<X3<void, double, short, float>, true, X3>();
  check_is_template<X3<void, double, short, float, int>, true, X3>();
  check_is_template<X3<void, double, short, float, int, bool>, true, X3>();
  check_is_template<std::string, false, X3>();
  check_is_template<void, false, X3>();
  check_is_template<int, false, X3>();

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

/////////////////
// integral_of //
/////////////////

FATAL_TEST(traits, integral_of) {
  FATAL_EXPECT_SAME<bool, integral_of<bool>>();
  FATAL_EXPECT_SAME<char, integral_of<char>>();
  FATAL_EXPECT_SAME<short, integral_of<short>>();
  FATAL_EXPECT_SAME<unsigned, integral_of<unsigned>>();
  FATAL_EXPECT_SAME<std::underlying_type<Ell>::type, integral_of<Ell>>();
  FATAL_EXPECT_SAME<std::true_type::value_type, integral_of<std::true_type>>();
}

/////////////////
// as_integral //
/////////////////

template <typename T, T... Values>
struct as_integral_constant_test {
  static_assert(sizeof...(Values) == 0, "wrong specialization");

  as_integral_constant_test() {
    {
      using type = std::integral_constant<T, std::numeric_limits<T>::min()>;
      FATAL_EXPECT_SAME<T, decltype(as_integral(type()))>();
      FATAL_EXPECT_EQ(type::value, as_integral(type()));
    }
    {
      using type = std::integral_constant<T, std::numeric_limits<T>::max()>;
      FATAL_EXPECT_SAME<T, decltype(as_integral(type()))>();
      FATAL_EXPECT_EQ(type::value, as_integral(type()));
    }
  }
};

template <typename T, T Value, T... Values>
struct as_integral_constant_test<T, Value, Values...> {
  as_integral_constant_test() {
    using type = std::integral_constant<T, Value>;
    FATAL_EXPECT_SAME<T, decltype(as_integral(type()))>();
    FATAL_EXPECT_EQ(Value, as_integral(type()));
  }

private:
  as_integral_constant_test<T, Values...> tail_;
};

FATAL_TEST(traits, as_integral) {
# define TEST_IMPL(ExpectedValue, ExpectedType, Value) \
  do { \
    FATAL_EXPECT_SAME<ExpectedType, decltype(as_integral(Value))>(); \
    FATAL_EXPECT_EQ(ExpectedValue, as_integral(Value)); \
  } while (false)

  {
    int v = 10;
    TEST_IMPL(10, int, v);

    int const c = 10;
    TEST_IMPL(10, int, c);
  }

  {
    long long v = 20;
    TEST_IMPL(20, long long, v);

    long long const c = 20;
    TEST_IMPL(20, long long, c);
  }

  TEST_IMPL(5, std::underlying_type<Ell>::type, Ell::a);

# undef TEST_IMPL

  as_integral_constant_test<bool, true, false>();

  as_integral_constant_test<char, ' ', 'h', 'e', 'l', 'o'>();
  as_integral_constant_test<unsigned char, ' ', 'h', 'e', 'l', 'o'>();

  as_integral_constant_test<std::size_t, 0, 1, 2, 3, 5, 8, 13, 21, 1000>();
  as_integral_constant_test<unsigned short, 0, 1, 2, 3, 5, 8, 13, 21, 1000>();
  as_integral_constant_test<unsigned, 0, 1, 2, 3, 5, 8, 13, 21, 1000>();
  as_integral_constant_test<unsigned long, 0, 1, 2, 3, 5, 8, 13, 21, 1000>();
  as_integral_constant_test<
    unsigned long long, 0, 1, 2, 3, 5, 8, 13, 21, 1000
  >();

  as_integral_constant_test<
    short,
    0, 1, 2, 3, 5, 8, 13, 21, 1000,
    -1, -2, -3, -5, -8, -13, -21, -1000
  >();
  as_integral_constant_test<
    int,
    0, 1, 2, 3, 5, 8, 13, 21, 1000,
    -1, -2, -3, -5, -8, -13, -21, -1000
  >();
  as_integral_constant_test<
    long,
    0, 1, 2, 3, 5, 8, 13, 21, 1000,
    -1, -2, -3, -5, -8, -13, -21, -1000
  >();
  as_integral_constant_test<
    long long,
    0, 1, 2, 3, 5, 8, 13, 21, 1000,
    -1, -2, -3, -5, -8, -13, -21, -1000
  >();
}

/////////////////
// is_callable //
/////////////////

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

using foonction = void(*)();
using foonction_is = void(*)(int, std::string);

FATAL_TEST(traits, is_callable) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  FATAL_EXPECT_TRUE((is_callable<foonctor>::value));
  FATAL_EXPECT_FALSE((is_callable<foonctor, int>::value));
  FATAL_EXPECT_FALSE((is_callable<foonctor, int, double>::value));
  FATAL_EXPECT_TRUE((is_callable<foonctor, int, std::string>::value));

  FATAL_EXPECT_TRUE((is_callable<decltype(lambda)>::value));
  FATAL_EXPECT_FALSE((is_callable<decltype(lambda), int>::value));
  FATAL_EXPECT_FALSE((is_callable<decltype(lambda), int, double>::value));
  FATAL_EXPECT_FALSE((is_callable<decltype(lambda), int, std::string>::value));

  FATAL_EXPECT_FALSE((is_callable<decltype(lambda_is)>::value));
  FATAL_EXPECT_FALSE((is_callable<decltype(lambda_is), int>::value));
  FATAL_EXPECT_FALSE((is_callable<decltype(lambda_is), int, double>::value));
  FATAL_EXPECT_TRUE((
    is_callable<decltype(lambda_is), int, std::string>::value
  ));

  FATAL_EXPECT_TRUE((is_callable<foonction>::value));
  FATAL_EXPECT_FALSE((is_callable<foonction, int>::value));
  FATAL_EXPECT_FALSE((is_callable<foonction, int, double>::value));
  FATAL_EXPECT_FALSE((is_callable<foonction, int, std::string>::value));

  FATAL_EXPECT_FALSE((is_callable<foonction_is>::value));
  FATAL_EXPECT_FALSE((is_callable<foonction_is, int>::value));
  FATAL_EXPECT_FALSE((is_callable<foonction_is, int, double>::value));
  FATAL_EXPECT_TRUE((is_callable<foonction_is, int, std::string>::value));
}

//////////////////////////
// enable_when::is_true //
//////////////////////////

FATAL_TEST(enable_when, is_true) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_true, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_true, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_true, std::is_const<int const &>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_true, std::is_const<int &>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_true, std::is_const<int const &&>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_true, std::is_const<int &&>
  );
}

///////////////////////////
// enable_when::all_true //
///////////////////////////

FATAL_TEST(enable_when, all_true) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_true, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_true, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_true,
    std::is_const<int const>, std::is_integral<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_true,
    std::is_const<double const>, std::is_integral<double const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_true,
    std::is_const<int const>,
    std::is_integral<int const>,
    std::is_signed<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_true,
    std::is_const<unsigned const>,
    std::is_integral<unsigned const>,
    std::is_signed<unsigned const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_true,
    std::is_const<double const>,
    std::is_integral<double const>,
    std::is_signed<double const>
  );
}

///////////////////////////
// enable_when::any_true //
///////////////////////////

FATAL_TEST(enable_when, any_true) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_true, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true,
    std::is_const<int const>, std::is_integral<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true,
    std::is_const<double const>, std::is_integral<double const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_true,
    std::is_const<double>, std::is_integral<double>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true,
    std::is_const<int const>,
    std::is_integral<int const>,
    std::is_unsigned<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true,
    std::is_const<unsigned const>,
    std::is_integral<unsigned const>,
    std::is_unsigned<unsigned const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_true,
    std::is_const<double const>,
    std::is_integral<double const>,
    std::is_unsigned<double const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_true,
    std::is_const<double>,
    std::is_integral<double>,
    std::is_unsigned<double>
  );
}

///////////////////////////
// enable_when::is_false //
///////////////////////////

FATAL_TEST(enable_when, is_false) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_false, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_false, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_false, std::is_const<int const &>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_false, std::is_const<int &>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_false, std::is_const<int const &&>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_false, std::is_const<int &&>
  );
}

////////////////////////////
// enable_when::all_false //
////////////////////////////

FATAL_TEST(enable_when, all_false) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_false, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false,
    std::is_const<int const>, std::is_integral<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false,
    std::is_const<double const>, std::is_integral<double const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_false,
    std::is_const<double>, std::is_integral<double>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false,
    std::is_const<int const>,
    std::is_integral<int const>,
    std::is_unsigned<int const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false,
    std::is_const<unsigned const>,
    std::is_integral<unsigned const>,
    std::is_unsigned<unsigned const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::all_false,
    std::is_const<double const>,
    std::is_integral<double const>,
    std::is_unsigned<double const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::all_false,
    std::is_const<double>,
    std::is_integral<double>,
    std::is_unsigned<double>
  );
}

////////////////////////////
// enable_when::any_false //
////////////////////////////

FATAL_TEST(enable_when, any_false) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_false, std::is_const<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_false, std::is_const<int>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_false,
    std::is_const<int const>, std::is_integral<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_false,
    std::is_const<double const>, std::is_integral<double const>
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::any_false,
    std::is_const<int const>,
    std::is_integral<int const>,
    std::is_signed<int const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_false,
    std::is_const<unsigned const>,
    std::is_integral<unsigned const>,
    std::is_signed<unsigned const>
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::any_false,
    std::is_const<double const>,
    std::is_integral<double const>,
    std::is_signed<double const>
  );
}

///////////////////////////
// enable_when::is_const //
///////////////////////////

FATAL_TEST(enable_when, is_const) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_const, int const
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_const, int
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_const, int const &
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_const, int &
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::is_const, int const &&
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::is_const, int &&
  );
}

////////////////////////////
// enable_when::non_const //
////////////////////////////

FATAL_TEST(enable_when, non_const) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::non_const, int const
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::non_const, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::non_const, int const &
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::non_const, int &
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::non_const, int const &&
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::non_const, int &&
  );
}

///////////////////////////////////
// enable_when::forwarded_rvalue //
///////////////////////////////////

FATAL_TEST(enable_when, forwarded_rvalue) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::forwarded_rvalue, int const
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::forwarded_rvalue, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::forwarded_rvalue, int const &
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::forwarded_rvalue, int &
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::forwarded_rvalue, int const &&
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::forwarded_rvalue, int &&
  );
}

//////////////////////////
// enable_when::movable //
//////////////////////////

FATAL_TEST(enable_when, movable) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::movable, int const
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::movable, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::movable, int const &
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::movable, int &
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::movable, int const &&
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::movable, int &&
  );
}

///////////////////////////
// enable_when::callable //
///////////////////////////

struct non_callable {};

FATAL_TEST(enable_when, callable [non-callable object]) {
  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, std::string, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, non_callable, int
  );
}

struct callable_nullary { void operator ()(); };
struct callable_i { void operator ()(int); };
struct callable_d { void operator ()(double); };
struct callable_id { void operator ()(int, double); };
struct callable_overloaded_i_d {
  void operator ()(int);
  void operator ()(double);
};
struct callable_overloaded_f_id {
  void operator ()(float);
  void operator ()(int, double);
};

FATAL_TEST(enable_when, callable [callable object]) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_nullary
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_nullary, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_nullary, std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_nullary, int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_nullary, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_i
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_i, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_i, std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_i, int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_i, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_d
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_d, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_d, std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_d, int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_d, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_i_d
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_overloaded_i_d, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_i_d, std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_i_d, int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_i_d, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_f_id
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_overloaded_f_id, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_overloaded_f_id, std::string
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_overloaded_f_id, int, double
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_overloaded_f_id, int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_id
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_id, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, callable_id, std::string
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_id, int, double
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, callable_id, int, int
  );
}

void fun_nullary();
void fun_i(int);
void fun_d(double);
void fun_id(int, double);

FATAL_TEST(enable_when, callable [function]) {
  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, decltype(fun_nullary)
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_nullary), int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_nullary), std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_nullary), int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_nullary), int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_i)
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, decltype(fun_i), int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_i), std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_i), int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_i), int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_d)
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, decltype(fun_d), int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_d), std::string
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_d), int, double
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_d), int, int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_id)
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_id), int
  );

  FATAL_EXPECT_TEMPLATE_DOESNT_COMPILE(
    enable_when::callable, decltype(fun_id), std::string
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, decltype(fun_id), int, double
  );

  FATAL_EXPECT_TEMPLATE_COMPILES(
    enable_when::callable, decltype(fun_id), int, int
  );
}

//////////////////////
// is_safe_overload //
//////////////////////

struct Base {};
struct Derived: public Base {};
struct Foo {};
enum class ctor { def, copy, move, universal };

FATAL_TEST(traits, is_safe_overload) {
  FATAL_EXPECT_FALSE((is_safe_overload<Base, Base>::value));
  FATAL_EXPECT_FALSE((is_safe_overload<Base, Derived>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, int>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, void>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, Foo>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, int, int>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, void, void>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, Foo, Foo>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, int, int, int>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, void, void, void>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, Foo, Foo, Foo>::value));
  FATAL_EXPECT_TRUE((is_safe_overload<Base, int, void, Foo, bool>::value));
}

struct overloading_test {
  overloading_test(): type(ctor::def) {}
  overloading_test(overloading_test const &) noexcept: type(ctor::copy) {}
  overloading_test(overloading_test &&) noexcept: type(ctor::move) {}
  template <
    typename T,
    typename = typename std::enable_if<
      is_safe_overload<overloading_test, T>::value, void
    >::type
  >
  explicit overloading_test(T &&): type(ctor::universal) {}

  ctor type;
};

FATAL_TEST(traits, safe_overload_nonvariadic) {
  overloading_test def;
  FATAL_EXPECT_EQ(ctor::def, def.type);
  overloading_test copy(def);
  FATAL_EXPECT_EQ(ctor::copy, copy.type);
  overloading_test move(std::move(def));
  FATAL_EXPECT_EQ(ctor::move, move.type);
  overloading_test universal(0);
  FATAL_EXPECT_EQ(ctor::universal, universal.type);
  overloading_test foo{Foo()};
  FATAL_EXPECT_EQ(ctor::universal, foo.type);
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
    typename = typename std::enable_if<
      is_safe_overload<variadic_overloading_test, Args...>::value, void
    >::type
  >
  explicit variadic_overloading_test(Args &&...): type(ctor::universal) {}

  ctor type;
};

FATAL_TEST(traits, safe_overload_variadic) {
  variadic_overloading_test def;
  FATAL_EXPECT_EQ(ctor::def, def.type);
  variadic_overloading_test copy(def);
  FATAL_EXPECT_EQ(ctor::copy, copy.type);
  variadic_overloading_test move(std::move(def));
  FATAL_EXPECT_EQ(ctor::move, move.type);
  variadic_overloading_test i(0);
  FATAL_EXPECT_EQ(ctor::universal, i.type);
  variadic_overloading_test foo{Foo()};
  FATAL_EXPECT_EQ(ctor::universal, foo.type);
  variadic_overloading_test universal(copy, move);
  FATAL_EXPECT_EQ(ctor::universal, universal.type);
}

struct overloading_test_t {
  overloading_test_t(): type(ctor::def) {}
  overloading_test_t(overloading_test_t const &) noexcept: type(ctor::copy) {}
  overloading_test_t(overloading_test_t &&) noexcept: type(ctor::move) {}
  template <
    typename T,
    typename = safe_overload<overloading_test_t, T>
  >
  explicit overloading_test_t(T &&): type(ctor::universal) {}
  ctor type;
};

FATAL_TEST(traits, safe_overload_nonvariadic_t) {
  overloading_test_t def;
  FATAL_EXPECT_EQ(ctor::def, def.type);
  overloading_test_t copy(def);
  FATAL_EXPECT_EQ(ctor::copy, copy.type);
  overloading_test_t move(std::move(def));
  FATAL_EXPECT_EQ(ctor::move, move.type);
  overloading_test_t universal(0);
  FATAL_EXPECT_EQ(ctor::universal, universal.type);
  overloading_test_t foo{Foo()};
  FATAL_EXPECT_EQ(ctor::universal, foo.type);
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
    typename = safe_overload<variadic_overloading_test_t, Args...>
  >
  explicit variadic_overloading_test_t(Args &&...): type(ctor::universal) {}
  ctor type;
};

FATAL_TEST(traits, safe_overload_variadic_t) {
  variadic_overloading_test_t def;
  FATAL_EXPECT_EQ(ctor::def, def.type);
  variadic_overloading_test_t copy(def);
  FATAL_EXPECT_EQ(ctor::copy, copy.type);
  variadic_overloading_test_t move(std::move(def));
  FATAL_EXPECT_EQ(ctor::move, move.type);
  variadic_overloading_test_t i(0);
  FATAL_EXPECT_EQ(ctor::universal, i.type);
  variadic_overloading_test_t foo{Foo()};
  FATAL_EXPECT_EQ(ctor::universal, foo.type);
  variadic_overloading_test_t universal(copy, move);
  FATAL_EXPECT_EQ(ctor::universal, universal.type);
}

} // namespace fatal {
