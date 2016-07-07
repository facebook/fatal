/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/traits.h>

#include <fatal/preprocessor.h>

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

/////////////////////////////
// remove_rvalue_reference //
/////////////////////////////

FATAL_TEST(traits, remove_rvalue_reference) {
# define TEST_IMPL(Type, Expected) \
  do { \
    FATAL_EXPECT_SAME<Expected, remove_rvalue_reference<Type>::type>(); \
  } while (false)

  TEST_IMPL(int &&, int);
  TEST_IMPL(int &, int &);
  TEST_IMPL(int, int);
  TEST_IMPL(int *&&, int *);
  TEST_IMPL(int *&, int *&);
  TEST_IMPL(int *, int*);
  TEST_IMPL(int const &&, int const);
  TEST_IMPL(int const &, int const &);
  TEST_IMPL(int const, int const);
  TEST_IMPL(int const *&&, int const *);
  TEST_IMPL(int const *&, int const *&);
  TEST_IMPL(int const *, int const *);

# undef TEST_IMPL
}

FATAL_TEST(traits, same_reference_as) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    same_reference_as<FATAL_UNPARENTHESIZE(T), From>::type \
  >();

  TEST_IMPL(int &&, int &&, int &&);
  TEST_IMPL(int &&, int &, int &&);
  TEST_IMPL(int &&, int, int &&);
  TEST_IMPL(int &&, int *&&, int *&&);
  TEST_IMPL(int &&, int *&, int *&&);
  TEST_IMPL(int &&, int *, int *&&);
  TEST_IMPL(int &&, int *const &&, int *const &&);
  TEST_IMPL(int &&, int *const &, int *const &&);
  TEST_IMPL(int &&, int *const, int *const &&);
  TEST_IMPL(int &&, int const &&, int const &&);
  TEST_IMPL(int &&, int const &, int const &&);
  TEST_IMPL(int &&, int const, int const &&);
  TEST_IMPL(int &&, int const *&&, int const *&&);
  TEST_IMPL(int &&, int const *&, int const *&&);
  TEST_IMPL(int &&, int const *, int const *&&);
  TEST_IMPL(int &&, int const *const &&, int const *const &&);
  TEST_IMPL(int &&, int const *const &, int const *const &&);
  TEST_IMPL(int &&, int const *const, int const *const &&);

  TEST_IMPL(int &, int &&, int &);
  TEST_IMPL(int &, int &, int &);
  TEST_IMPL(int &, int, int &);
  TEST_IMPL(int &, int *&&, int *&);
  TEST_IMPL(int &, int *&, int *&);
  TEST_IMPL(int &, int *, int *&);
  TEST_IMPL(int &, int *const &&, int *const &);
  TEST_IMPL(int &, int *const &, int *const &);
  TEST_IMPL(int &, int *const, int *const &);
  TEST_IMPL(int &, int const &&, int const &);
  TEST_IMPL(int &, int const &, int const &);
  TEST_IMPL(int &, int const, int const &);
  TEST_IMPL(int &, int const *&&, int const *&);
  TEST_IMPL(int &, int const *&, int const *&);
  TEST_IMPL(int &, int const *, int const *&);
  TEST_IMPL(int &, int const *const &&, int const *const &);
  TEST_IMPL(int &, int const *const &, int const *const &);
  TEST_IMPL(int &, int const *const, int const *const &);

  TEST_IMPL(int, int &&, int &&);
  TEST_IMPL(int, int &, int &);
  TEST_IMPL(int, int, int);
  TEST_IMPL(int, int *&&, int *&&);
  TEST_IMPL(int, int *&, int *&);
  TEST_IMPL(int, int *, int *);
  TEST_IMPL(int, int *const &&, int *const &&);
  TEST_IMPL(int, int *const &, int *const &);
  TEST_IMPL(int, int *const, int *const);
  TEST_IMPL(int, int const &&, int const &&);
  TEST_IMPL(int, int const &, int const &);
  TEST_IMPL(int, int const, int const);
  TEST_IMPL(int, int const *&&, int const *&&);
  TEST_IMPL(int, int const *&, int const *&);
  TEST_IMPL(int, int const *, int const *);
  TEST_IMPL(int, int const *const &&, int const *const &&);
  TEST_IMPL(int, int const *const &, int const *const &);
  TEST_IMPL(int, int const *const, int const *const);

# undef TEST_IMPL
}

FATAL_TEST(traits, add_reference_from) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    add_reference_from<FATAL_UNPARENTHESIZE(T), From>::type \
  >();

  TEST_IMPL(int &&, int &&, int &&);
  TEST_IMPL(int &&, int &, int &);
  TEST_IMPL(int &&, int, int &&);
  TEST_IMPL(int &&, int *&&, int *&&);
  TEST_IMPL(int &&, int *&, int *&);
  TEST_IMPL(int &&, int *, int * &&);
  TEST_IMPL(int &&, int *const &&, int *const &&);
  TEST_IMPL(int &&, int *const &, int *const &);
  TEST_IMPL(int &&, int *const, int *const &&);
  TEST_IMPL(int &&, int const &&, int const &&);
  TEST_IMPL(int &&, int const &, int const &);
  TEST_IMPL(int &&, int const, int const &&);
  TEST_IMPL(int &&, int const *&&, int const *&&);
  TEST_IMPL(int &&, int const *&, int const *&);
  TEST_IMPL(int &&, int const *, int const * &&);
  TEST_IMPL(int &&, int const *const &&, int const *const &&);
  TEST_IMPL(int &&, int const *const &, int const *const &);
  TEST_IMPL(int &&, int const *const, int const *const &&);

  TEST_IMPL(int &, int &&, int &);
  TEST_IMPL(int &, int &, int &);
  TEST_IMPL(int &, int, int &);
  TEST_IMPL(int &, int *&&, int *&);
  TEST_IMPL(int &, int *&, int *&);
  TEST_IMPL(int &, int *, int *&);
  TEST_IMPL(int &, int *const &&, int *const &);
  TEST_IMPL(int &, int *const &, int *const &);
  TEST_IMPL(int &, int *const, int *const &);
  TEST_IMPL(int &, int const &&, int const &);
  TEST_IMPL(int &, int const &, int const &);
  TEST_IMPL(int &, int const, int const &);
  TEST_IMPL(int &, int const *&&, int const *&);
  TEST_IMPL(int &, int const *&, int const *&);
  TEST_IMPL(int &, int const *, int const *&);
  TEST_IMPL(int &, int const *const &&, int const *const &);
  TEST_IMPL(int &, int const *const &, int const *const &);
  TEST_IMPL(int &, int const *const, int const *const &);

  TEST_IMPL(int, int &&, int &&);
  TEST_IMPL(int, int &, int &);
  TEST_IMPL(int, int, int);
  TEST_IMPL(int, int *&&, int *&&);
  TEST_IMPL(int, int *&, int *&);
  TEST_IMPL(int, int *, int *);
  TEST_IMPL(int, int *const &&, int *const &&);
  TEST_IMPL(int, int *const &, int *const &);
  TEST_IMPL(int, int *const, int *const);
  TEST_IMPL(int, int const &&, int const &&);
  TEST_IMPL(int, int const &, int const &);
  TEST_IMPL(int, int const, int const);
  TEST_IMPL(int, int const *&&, int const *&&);
  TEST_IMPL(int, int const *&, int const *&);
  TEST_IMPL(int, int const *, int const *);
  TEST_IMPL(int, int const *const &&, int const *const &&);
  TEST_IMPL(int, int const *const &, int const *const &);
  TEST_IMPL(int, int const *const, int const *const);

# undef TEST_IMPL
}

FATAL_TEST(traits, add_const_from) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    add_const_from<FATAL_UNPARENTHESIZE(T), From>::type \
  >();

  TEST_IMPL(int, int &&, int &&);
  TEST_IMPL(int, int &, int &);
  TEST_IMPL(int, int *&&, int *&&);
  TEST_IMPL(int, int *&, int *&);
  TEST_IMPL(int, int *, int *);
  TEST_IMPL(int, int, int);
  TEST_IMPL(int, int *const &&, int *const &&);
  TEST_IMPL(int, int *const &, int *const &);
  TEST_IMPL(int, int *const, int *const);
  TEST_IMPL(int, int const &&, int const &&);
  TEST_IMPL(int, int const &, int const &);
  TEST_IMPL(int, int const *&&, int const *&&);
  TEST_IMPL(int, int const *&, int const *&);
  TEST_IMPL(int, int const *, int const *);
  TEST_IMPL(int, int const *const &&, int const *const &&);
  TEST_IMPL(int, int const *const &, int const *const &);
  TEST_IMPL(int, int const *const, int const *const);
  TEST_IMPL(int, int const, int const);

  TEST_IMPL(int const, int &&, int &&);
  TEST_IMPL(int const, int &, int &);
  TEST_IMPL(int const, int, int const);
  TEST_IMPL(int const, int *&&, int *&&);
  TEST_IMPL(int const, int *&, int *&);
  TEST_IMPL(int const, int *, int *const);
  TEST_IMPL(int const, int *const &&, int *const &&);
  TEST_IMPL(int const, int *const &, int *const &);
  TEST_IMPL(int const, int *const, int *const);
  TEST_IMPL(int const, int const &&, int const &&);
  TEST_IMPL(int const, int const &, int const &);
  TEST_IMPL(int const, int const, int const);
  TEST_IMPL(int const, int const *&&, int const *&&);
  TEST_IMPL(int const, int const *&, int const *&);
  TEST_IMPL(int const, int const *, int const *const);
  TEST_IMPL(int const, int const *const &&, int const *const &&);
  TEST_IMPL(int const, int const *const &, int const *const &);
  TEST_IMPL(int const, int const *const, int const *const);

# undef TEST_IMPL
}

FATAL_TEST(traits, constify) {
# define TEST_IMPL(T, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    constify<FATAL_UNPARENTHESIZE(T)>::type \
  >();

  TEST_IMPL(int &&, int const &&);
  TEST_IMPL(int &, int const &);
  TEST_IMPL(int, int const);
  TEST_IMPL(int *&&, int *const &&);
  TEST_IMPL(int *&, int *const &);
  TEST_IMPL(int *, int *const);
  TEST_IMPL(int *const &&, int *const &&);
  TEST_IMPL(int *const &, int *const &);
  TEST_IMPL(int *const, int *const);

  TEST_IMPL(int const &&, int const &&);
  TEST_IMPL(int const &, int const &);
  TEST_IMPL(int const, int const);
  TEST_IMPL(int const *&&, int const *const &&);
  TEST_IMPL(int const *&, int const *const &);
  TEST_IMPL(int const *, int const *const);
  TEST_IMPL(int const *const &&, int const *const &&);
  TEST_IMPL(int const *const &, int const *const &);
  TEST_IMPL(int const *const, int const *const);

# undef TEST_IMPL
}

FATAL_TEST(traits, constify_from) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    constify_from<FATAL_UNPARENTHESIZE(T), From>::type \
  >();

  TEST_IMPL(int, int &&, int &&);
  TEST_IMPL(int, int &, int &);
  TEST_IMPL(int, int, int);
  TEST_IMPL(int, int *&&, int *&&);
  TEST_IMPL(int, int *&, int *&);
  TEST_IMPL(int, int *, int *);
  TEST_IMPL(int, int *const &&, int *const &&);
  TEST_IMPL(int, int *const &, int *const &);
  TEST_IMPL(int, int *const, int *const);
  TEST_IMPL(int, int const &&, int const &&);
  TEST_IMPL(int, int const &, int const &);
  TEST_IMPL(int, int const, int const);
  TEST_IMPL(int, int const *&&, int const *&&);
  TEST_IMPL(int, int const *&, int const *&);
  TEST_IMPL(int, int const *, int const *);
  TEST_IMPL(int, int const *const &&, int const *const &&);
  TEST_IMPL(int, int const *const &, int const *const &);
  TEST_IMPL(int, int const *const, int const *const);

  TEST_IMPL(int const, int &&, int const &&);
  TEST_IMPL(int const, int &, int const &);
  TEST_IMPL(int const, int, int const);
  TEST_IMPL(int const, int *&&, int *const &&);
  TEST_IMPL(int const, int *&, int *const &);
  TEST_IMPL(int const, int *, int *const);
  TEST_IMPL(int const, int *const &&, int *const &&);
  TEST_IMPL(int const, int *const &, int *const &);
  TEST_IMPL(int const, int *const, int *const);
  TEST_IMPL(int const, int const &&, int const &&);
  TEST_IMPL(int const, int const &, int const &);
  TEST_IMPL(int const, int const, int const);
  TEST_IMPL(int const, int const *&&, int const *const &&);
  TEST_IMPL(int const, int const *&, int const *const &);
  TEST_IMPL(int const, int const *, int const *const);
  TEST_IMPL(int const, int const *const &&, int const *const &&);
  TEST_IMPL(int const, int const *const &, int const *const &);
  TEST_IMPL(int const, int const *const, int const *const);

# undef TEST_IMPL
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
    typename = safe_overload_t<overloading_test_t, T>
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
    typename = safe_overload_t<variadic_overloading_test_t, Args...>
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

//////////////////////////////
// FATAL_HAS_MEMBER_TYPEDEF //
//////////////////////////////

namespace has_member_type_test {
  struct foo { using xyz = int; };
  struct bar { typedef int xyz; };
  struct baz {};

  FATAL_HAS_MEMBER_TYPE(has_xyz, xyz);
} // namespace has_member_type_test {

FATAL_TEST(traits, has_member_type) {
  FATAL_EXPECT_SAME<
    std::true_type,
    has_member_type_test::has_xyz::apply<has_member_type_test::foo>
  >();

  FATAL_EXPECT_SAME<
    std::true_type,
    has_member_type_test::has_xyz::apply<has_member_type_test::bar>
  >();

  FATAL_EXPECT_SAME<
    std::false_type,
    has_member_type_test::has_xyz::apply<has_member_type_test::baz>
  >();
}

//////////////////////////////
// FATAL_DATA_MEMBER_GETTER //
//////////////////////////////

namespace data_member_getter_test {

struct data {
  data(
    int i, std::string const &scl, long &&lr, double const dc,
    bool &bl, std::vector<short> v, float const &&fcr
  ):
    i(i), scl(scl), lr(std::move(lr)), dc(dc), bl(bl), v(v), fcr(std::move(fcr))
  {}

  data(data const &rhs):
    i(rhs.i), scl(rhs.scl), lr(std::move(rhs.lr)), dc(rhs.dc),
    bl(rhs.bl), v(rhs.v), fcr(std::move(rhs.fcr))
  {}

  int i;
  std::string const &scl;
  long &&lr;
  double const dc;
  bool &bl;
  std::vector<short> v;
  float const &&fcr;

  bool operator ==(data const &rhs) const {
    return i == rhs.i
      && scl == rhs.scl
      && lr == rhs.lr
      && dc == rhs.dc
      && bl == rhs.bl
      && v == rhs.v
      && fcr == rhs.fcr;
  }
};

struct getter {
# define TEST_IMPL(...) \
  FATAL_DATA_MEMBER_GETTER(__VA_ARGS__, __VA_ARGS__)

  TEST_IMPL(bl);
  TEST_IMPL(cl);
  TEST_IMPL(cr);
  TEST_IMPL(d);
  TEST_IMPL(dc);
  TEST_IMPL(fcr);
  TEST_IMPL(i);
  TEST_IMPL(l);
  TEST_IMPL(lr);
  TEST_IMPL(r);
  TEST_IMPL(scl);
  TEST_IMPL(v);
  TEST_IMPL(non_existent1);
  TEST_IMPL(non_existent2);
  TEST_IMPL(non_existent3);

# undef TEST_IMPL
};

struct getter_name {
# define TEST_IMPL(...) \
  FATAL_STR(__VA_ARGS__, FATAL_TO_STR(__VA_ARGS__))

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL
};

} // namespace data_member_getter_test {

FATAL_TEST(data_member_getter, name) {
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    FATAL_EXPECT_SAME< \
      data_member_getter_test::getter_name::Field, \
      getter::name \
    >(); \
  } while (false)

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, type) {
# define TEST_PREAMBLE_IMPL(Field, ...) \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >;

# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data &>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &&>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data &&>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data const>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data const &>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const &&>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<data const &&>>(); \
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(data_member_getter, has) {
# define TEST_IMPL(Owner, Field, Expected) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    FATAL_EXPECT_EQ(Expected, getter::has<Owner>::value); \
  } while (false)
  // TODO: for when chained_data_member_getter::has is implemented
  // using chained_getter = chained_data_member_getter< \
  //   data_member_getter_test::getter::Field \
  // >; \
  // FATAL_EXPECT_EQ(Expected, chained_getter::has<Owner>::value);

  TEST_IMPL(data, i, true);
  TEST_IMPL(data, scl, true);
  TEST_IMPL(data, lr, true);
  TEST_IMPL(data, dc, true);
  TEST_IMPL(data, bl, true);
  TEST_IMPL(data, v, true);
  TEST_IMPL(data, fcr, true);
  TEST_IMPL(data, non_existent2, false);
  TEST_IMPL(data, non_existent2, false);
  TEST_IMPL(data, non_existent3, false);

  TEST_IMPL(data &, i, true);
  TEST_IMPL(data &, scl, true);
  TEST_IMPL(data &, lr, true);
  TEST_IMPL(data &, dc, true);
  TEST_IMPL(data &, bl, true);
  TEST_IMPL(data &, v, true);
  TEST_IMPL(data &, fcr, true);
  TEST_IMPL(data &, non_existent1, false);
  TEST_IMPL(data &, non_existent2, false);
  TEST_IMPL(data &, non_existent3, false);

  TEST_IMPL(data &&, i, true);
  TEST_IMPL(data &&, scl, true);
  TEST_IMPL(data &&, lr, true);
  TEST_IMPL(data &&, dc, true);
  TEST_IMPL(data &&, bl, true);
  TEST_IMPL(data &&, v, true);
  TEST_IMPL(data &&, fcr, true);
  TEST_IMPL(data &&, non_existent1, false);
  TEST_IMPL(data &&, non_existent2, false);
  TEST_IMPL(data &&, non_existent3, false);

  TEST_IMPL(data const, i, true);
  TEST_IMPL(data const, scl, true);
  TEST_IMPL(data const, lr, true);
  TEST_IMPL(data const, dc, true);
  TEST_IMPL(data const, bl, true);
  TEST_IMPL(data const, v, true);
  TEST_IMPL(data const, fcr, true);
  TEST_IMPL(data const, non_existent1, false);
  TEST_IMPL(data const, non_existent2, false);
  TEST_IMPL(data const, non_existent3, false);

  TEST_IMPL(data const &, i, true);
  TEST_IMPL(data const &, scl, true);
  TEST_IMPL(data const &, lr, true);
  TEST_IMPL(data const &, dc, true);
  TEST_IMPL(data const &, bl, true);
  TEST_IMPL(data const &, v, true);
  TEST_IMPL(data const &, fcr, true);
  TEST_IMPL(data const &, non_existent1, false);
  TEST_IMPL(data const &, non_existent2, false);
  TEST_IMPL(data const &, non_existent3, false);

  TEST_IMPL(data const &&, i, true);
  TEST_IMPL(data const &&, scl, true);
  TEST_IMPL(data const &&, lr, true);
  TEST_IMPL(data const &&, dc, true);
  TEST_IMPL(data const &&, bl, true);
  TEST_IMPL(data const &&, v, true);
  TEST_IMPL(data const &&, fcr, true);
  TEST_IMPL(data const &&, non_existent1, false);
  TEST_IMPL(data const &&, non_existent2, false);
  TEST_IMPL(data const &&, non_existent3, false);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, reference) {
# define TEST_PREAMBLE_IMPL(Field, ...) \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >;

# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<data>>(); \
  } while (false)

  TEST_IMPL(i, int &&);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const &&);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short> &&);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<data &>>(); \
  } while (false)

  TEST_IMPL(i, int &);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &);
  TEST_IMPL(dc, double const &);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short> &);
  TEST_IMPL(fcr, float const &);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data &&>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<data &&>>(); \
  } while (false)

  TEST_IMPL(i, int &&);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long &&);
  TEST_IMPL(dc, double const &&);
  TEST_IMPL(bl, bool &);
  TEST_IMPL(v, std::vector<short> &&);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<data const>>(); \
  } while (false)

  TEST_IMPL(i, int const &&);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long const &&);
  TEST_IMPL(dc, double const &&);
  TEST_IMPL(bl, bool const &);
  TEST_IMPL(v, std::vector<short> const &&);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<data const &>>(); \
  } while (false)

  TEST_IMPL(i, int const &);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long const &);
  TEST_IMPL(dc, double const &);
  TEST_IMPL(bl, bool const &);
  TEST_IMPL(v, std::vector<short> const &);
  TEST_IMPL(fcr, float const &);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const &&>>(); \
    FATAL_EXPECT_SAME< \
      __VA_ARGS__, \
      chained_getter::reference<data const &&> \
    >(); \
  } while (false)

  TEST_IMPL(i, int const &&);
  TEST_IMPL(scl, std::string const &);
  TEST_IMPL(lr, long const &&);
  TEST_IMPL(dc, double const &&);
  TEST_IMPL(bl, bool const &);
  TEST_IMPL(v, std::vector<short> const &&);
  TEST_IMPL(fcr, float const &&);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(data_member_getter, pointer) {
# define TEST_PREAMBLE_IMPL(Field, ...) \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >;

# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data>>(); \
  } while (false)

  TEST_IMPL(i, int *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool *);
  TEST_IMPL(v, std::vector<short> *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data &>>(); \
  } while (false)

  TEST_IMPL(i, int *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool *);
  TEST_IMPL(v, std::vector<short> *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data &&>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data &&>>(); \
  } while (false)

  TEST_IMPL(i, int *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool *);
  TEST_IMPL(v, std::vector<short> *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data const>>(); \
  } while (false)

  TEST_IMPL(i, int const *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long const *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool const *);
  TEST_IMPL(v, std::vector<short> const *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const &>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data const &>>(); \
  } while (false)

  TEST_IMPL(i, int const *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long const *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool const *);
  TEST_IMPL(v, std::vector<short> const *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Field, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const &&>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<data const &&>>(); \
  } while (false)

  TEST_IMPL(i, int const *);
  TEST_IMPL(scl, std::string const *);
  TEST_IMPL(lr, long const *);
  TEST_IMPL(dc, double const *);
  TEST_IMPL(bl, bool const *);
  TEST_IMPL(v, std::vector<short> const *);
  TEST_IMPL(fcr, float const *);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(data_member_getter, getter) {
  int i = 99;
  std::string const scl("hello, world!");
  long lr = 27;
  double const dc = 5.6;
  bool bl = true;
  std::vector<short> v{1, 2, 3, 4, 5, 6};
  float const fcr = 7.2;

  data_member_getter_test::data x(i, scl, std::move(lr), dc, bl, v, std::move(fcr));

  FATAL_ASSERT_EQ(i, x.i);
  FATAL_ASSERT_EQ(scl, x.scl);
  FATAL_ASSERT_EQ(lr, x.lr);
  FATAL_ASSERT_EQ(dc, x.dc);
  FATAL_ASSERT_EQ(bl, x.bl);
  FATAL_ASSERT_EQ(v, x.v);
  FATAL_ASSERT_EQ(fcr, x.fcr);

  auto const &y = x;

  FATAL_ASSERT_EQ(i, y.i);
  FATAL_ASSERT_EQ(scl, y.scl);
  FATAL_ASSERT_EQ(lr, y.lr);
  FATAL_ASSERT_EQ(dc, y.dc);
  FATAL_ASSERT_EQ(bl, y.bl);
  FATAL_ASSERT_EQ(v, y.v);
  FATAL_ASSERT_EQ(fcr, y.fcr);

# define TEST_PREAMBLE_IMPL(Field, ...) \
    using getter = data_member_getter_test::getter::Field; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >;

# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(Field, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Field, getter::ref_getter()(Data)); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref(Data)); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref_getter()(Data)); \
    \
    FATAL_EXPECT_EQ(Data.Field, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data.Field, getter::ref_getter()(Data)); \
    FATAL_EXPECT_EQ(Data.Field, chained_getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data.Field, chained_getter::ref_getter()(Data)); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(getter::ref_getter()(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(chained_getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(chained_getter::ref_getter()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(getter::ref_getter()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(Field, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Field, getter::ref_getter()(std::move(Data))); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref_getter()(std::move(Data))); \
    \
    FATAL_EXPECT_EQ(std::move(Data).Field, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      getter::ref_getter()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      chained_getter::ref(std::move(Data)) \
    ); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      chained_getter::ref_getter()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(getter::ref_getter()(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref_getter()(std::move(Data))) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Field), getter::ptr(Data)); \
    FATAL_EXPECT_EQ(std::addressof(Data.Field), getter::ptr_getter()(Data)); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Field), chained_getter::ptr(Data)); \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      chained_getter::ptr_getter()(Data) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(getter::ptr_getter()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(chained_getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(chained_getter::ptr_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

////////////////////////////////
// chained_data_member_getter //
////////////////////////////////

namespace data_member_getter_test {

struct chained_data {
  chained_data(data d, data &l, data &&r, data const &cl, data const &&cr):
    d(d), l(l), r(std::move(r)), cl(cl), cr(std::move(cr))
  {}

  data d;
  data &l;
  data &&r;
  data const &cl;
  data const &&cr;
};

struct chained_getter {
# define TEST_IMPL(Outer, Inner) \
  using Outer##_##Inner = chained_data_member_getter< \
    data_member_getter_test::getter::Outer, \
    data_member_getter_test::getter::Inner \
  >;

  TEST_IMPL(d, i);
  TEST_IMPL(d, scl);
  TEST_IMPL(d, lr);
  TEST_IMPL(d, dc);
  TEST_IMPL(d, bl);
  TEST_IMPL(d, v);
  TEST_IMPL(d, fcr);

  TEST_IMPL(l, i);
  TEST_IMPL(l, scl);
  TEST_IMPL(l, lr);
  TEST_IMPL(l, dc);
  TEST_IMPL(l, bl);
  TEST_IMPL(l, v);
  TEST_IMPL(l, fcr);

  TEST_IMPL(r, i);
  TEST_IMPL(r, scl);
  TEST_IMPL(r, lr);
  TEST_IMPL(r, dc);
  TEST_IMPL(r, bl);
  TEST_IMPL(r, v);
  TEST_IMPL(r, fcr);

  TEST_IMPL(cl, i);
  TEST_IMPL(cl, scl);
  TEST_IMPL(cl, lr);
  TEST_IMPL(cl, dc);
  TEST_IMPL(cl, bl);
  TEST_IMPL(cl, v);
  TEST_IMPL(cl, fcr);

  TEST_IMPL(cr, i);
  TEST_IMPL(cr, scl);
  TEST_IMPL(cr, lr);
  TEST_IMPL(cr, dc);
  TEST_IMPL(cr, bl);
  TEST_IMPL(cr, v);
  TEST_IMPL(cr, fcr);

# undef TEST_IMPL
};

} // namespace data_member_getter_test {

FATAL_TEST(chained_data_member_getter, type) {
# define TEST_IMPL(T) \
  do { \
    FATAL_EXPECT_SAME<T, chained_data_member_getter<>::type<T>>(); \
  } while (false)

  TEST_IMPL(int);
  TEST_IMPL(int &);
  TEST_IMPL(int &&);
  TEST_IMPL(int const &);
  TEST_IMPL(int const &&);

# undef TEST_IMPL

# define TEST_PREAMBLE_IMPL(Outer, Inner, ...) \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >;

# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const &>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data const &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short>);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &&);
  TEST_IMPL(l, dc, double const);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short>);
  TEST_IMPL(l, fcr, float const &&);

  TEST_IMPL(r, i, int);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short>);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long &&);
  TEST_IMPL(cl, dc, double const);
  TEST_IMPL(cl, bl, bool &);
  TEST_IMPL(cl, v, std::vector<short>);
  TEST_IMPL(cl, fcr, float const &&);

  TEST_IMPL(cr, i, int);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long &&);
  TEST_IMPL(cr, dc, double const);
  TEST_IMPL(cr, bl, bool &);
  TEST_IMPL(cr, v, std::vector<short>);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(chained_data_member_getter, reference) {
# define TEST_IMPL(T) \
  do { \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference<T>::type, \
      chained_data_member_getter<>::reference<T> \
    >(); \
  } while (false)

  TEST_IMPL(int);
  TEST_IMPL(int &);
  TEST_IMPL(int &&);
  TEST_IMPL(int const &);
  TEST_IMPL(int const &&);

# undef TEST_IMPL

# define TEST_PREAMBLE_IMPL(Outer, Inner, ...) \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >;

# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data>>(); \
  } while (false)

  TEST_IMPL(d, i, int &&);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const &&);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short> &&);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int &);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &);
  TEST_IMPL(l, dc, double const &);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short> &);
  TEST_IMPL(l, fcr, float const &);

  TEST_IMPL(r, i, int &&);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const &&);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short> &&);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int const &);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long const &);
  TEST_IMPL(cl, dc, double const &);
  TEST_IMPL(cl, bl, bool const &);
  TEST_IMPL(cl, v, std::vector<short> const &);
  TEST_IMPL(cl, fcr, float const &);

  TEST_IMPL(cr, i, int const &&);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long const &&);
  TEST_IMPL(cr, dc, double const &&);
  TEST_IMPL(cr, bl, bool const &);
  TEST_IMPL(cr, v, std::vector<short> const &&);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data &>>(); \
  } while (false)

  TEST_IMPL(d, i, int &);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &);
  TEST_IMPL(d, dc, double const &);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short> &);
  TEST_IMPL(d, fcr, float const &);

  TEST_IMPL(l, i, int &);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &);
  TEST_IMPL(l, dc, double const &);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short> &);
  TEST_IMPL(l, fcr, float const &);

  TEST_IMPL(r, i, int &);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &);
  TEST_IMPL(r, dc, double const &);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short> &);
  TEST_IMPL(r, fcr, float const &);

  TEST_IMPL(cl, i, int const &);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long const &);
  TEST_IMPL(cl, dc, double const &);
  TEST_IMPL(cl, bl, bool const &);
  TEST_IMPL(cl, v, std::vector<short> const &);
  TEST_IMPL(cl, fcr, float const &);

  TEST_IMPL(cr, i, int const &);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long const &);
  TEST_IMPL(cr, dc, double const &);
  TEST_IMPL(cr, bl, bool const &);
  TEST_IMPL(cr, v, std::vector<short> const &);
  TEST_IMPL(cr, fcr, float const &);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int &&);
  TEST_IMPL(d, scl, std::string const &);
  TEST_IMPL(d, lr, long &&);
  TEST_IMPL(d, dc, double const &&);
  TEST_IMPL(d, bl, bool &);
  TEST_IMPL(d, v, std::vector<short> &&);
  TEST_IMPL(d, fcr, float const &&);

  TEST_IMPL(l, i, int &);
  TEST_IMPL(l, scl, std::string const &);
  TEST_IMPL(l, lr, long &);
  TEST_IMPL(l, dc, double const &);
  TEST_IMPL(l, bl, bool &);
  TEST_IMPL(l, v, std::vector<short> &);
  TEST_IMPL(l, fcr, float const &);

  TEST_IMPL(r, i, int &&);
  TEST_IMPL(r, scl, std::string const &);
  TEST_IMPL(r, lr, long &&);
  TEST_IMPL(r, dc, double const &&);
  TEST_IMPL(r, bl, bool &);
  TEST_IMPL(r, v, std::vector<short> &&);
  TEST_IMPL(r, fcr, float const &&);

  TEST_IMPL(cl, i, int const &);
  TEST_IMPL(cl, scl, std::string const &);
  TEST_IMPL(cl, lr, long const &);
  TEST_IMPL(cl, dc, double const &);
  TEST_IMPL(cl, bl, bool const &);
  TEST_IMPL(cl, v, std::vector<short> const &);
  TEST_IMPL(cl, fcr, float const &);

  TEST_IMPL(cr, i, int const &&);
  TEST_IMPL(cr, scl, std::string const &);
  TEST_IMPL(cr, lr, long const &&);
  TEST_IMPL(cr, dc, double const &&);
  TEST_IMPL(cr, bl, bool const &);
  TEST_IMPL(cr, v, std::vector<short> const &&);
  TEST_IMPL(cr, fcr, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  &&);
  TEST_IMPL(d, scl, std::string const  &);
  TEST_IMPL(d, lr, long const  &&);
  TEST_IMPL(d, dc, double const  &&);
  TEST_IMPL(d, bl, bool const  &);
  TEST_IMPL(d, v, std::vector<short> const  &&);
  TEST_IMPL(d, fcr, float const  &&);

  TEST_IMPL(l, i, int const  &);
  TEST_IMPL(l, scl, std::string const  &);
  TEST_IMPL(l, lr, long const  &);
  TEST_IMPL(l, dc, double const  &);
  TEST_IMPL(l, bl, bool const  &);
  TEST_IMPL(l, v, std::vector<short> const  &);
  TEST_IMPL(l, fcr, float const  &);

  TEST_IMPL(r, i, int const  &&);
  TEST_IMPL(r, scl, std::string const  &);
  TEST_IMPL(r, lr, long const  &&);
  TEST_IMPL(r, dc, double const  &&);
  TEST_IMPL(r, bl, bool const  &);
  TEST_IMPL(r, v, std::vector<short> const  &&);
  TEST_IMPL(r, fcr, float const  &&);

  TEST_IMPL(cl, i, int const  &);
  TEST_IMPL(cl, scl, std::string const  &);
  TEST_IMPL(cl, lr, long const  &);
  TEST_IMPL(cl, dc, double const  &);
  TEST_IMPL(cl, bl, bool const  &);
  TEST_IMPL(cl, v, std::vector<short> const  &);
  TEST_IMPL(cl, fcr, float const  &);

  TEST_IMPL(cr, i, int const  &&);
  TEST_IMPL(cr, scl, std::string const  &);
  TEST_IMPL(cr, lr, long const  &&);
  TEST_IMPL(cr, dc, double const  &&);
  TEST_IMPL(cr, bl, bool const  &);
  TEST_IMPL(cr, v, std::vector<short> const  &&);
  TEST_IMPL(cr, fcr, float const  &&);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const &>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  &);
  TEST_IMPL(d, scl, std::string const  &);
  TEST_IMPL(d, lr, long const  &);
  TEST_IMPL(d, dc, double const  &);
  TEST_IMPL(d, bl, bool const  &);
  TEST_IMPL(d, v, std::vector<short> const  &);
  TEST_IMPL(d, fcr, float const  &);

  TEST_IMPL(l, i, int const  &);
  TEST_IMPL(l, scl, std::string const  &);
  TEST_IMPL(l, lr, long const  &);
  TEST_IMPL(l, dc, double const  &);
  TEST_IMPL(l, bl, bool const  &);
  TEST_IMPL(l, v, std::vector<short> const  &);
  TEST_IMPL(l, fcr, float const  &);

  TEST_IMPL(r, i, int const  &);
  TEST_IMPL(r, scl, std::string const  &);
  TEST_IMPL(r, lr, long const  &);
  TEST_IMPL(r, dc, double const  &);
  TEST_IMPL(r, bl, bool const  &);
  TEST_IMPL(r, v, std::vector<short> const  &);
  TEST_IMPL(r, fcr, float const  &);

  TEST_IMPL(cl, i, int const  &);
  TEST_IMPL(cl, scl, std::string const  &);
  TEST_IMPL(cl, lr, long const  &);
  TEST_IMPL(cl, dc, double const  &);
  TEST_IMPL(cl, bl, bool const  &);
  TEST_IMPL(cl, v, std::vector<short> const  &);
  TEST_IMPL(cl, fcr, float const  &);

  TEST_IMPL(cr, i, int const  &);
  TEST_IMPL(cr, scl, std::string const  &);
  TEST_IMPL(cr, lr, long const  &);
  TEST_IMPL(cr, dc, double const  &);
  TEST_IMPL(cr, bl, bool const  &);
  TEST_IMPL(cr, v, std::vector<short> const  &);
  TEST_IMPL(cr, fcr, float const  &);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<data const &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  &&);
  TEST_IMPL(d, scl, std::string const  &);
  TEST_IMPL(d, lr, long const  &&);
  TEST_IMPL(d, dc, double const  &&);
  TEST_IMPL(d, bl, bool const  &);
  TEST_IMPL(d, v, std::vector<short> const  &&);
  TEST_IMPL(d, fcr, float const  &&);

  TEST_IMPL(l, i, int const  &);
  TEST_IMPL(l, scl, std::string const  &);
  TEST_IMPL(l, lr, long const  &);
  TEST_IMPL(l, dc, double const  &);
  TEST_IMPL(l, bl, bool const  &);
  TEST_IMPL(l, v, std::vector<short> const  &);
  TEST_IMPL(l, fcr, float const  &);

  TEST_IMPL(r, i, int const  &&);
  TEST_IMPL(r, scl, std::string const  &);
  TEST_IMPL(r, lr, long const  &&);
  TEST_IMPL(r, dc, double const  &&);
  TEST_IMPL(r, bl, bool const  &);
  TEST_IMPL(r, v, std::vector<short> const  &&);
  TEST_IMPL(r, fcr, float const  &&);

  TEST_IMPL(cl, i, int const  &);
  TEST_IMPL(cl, scl, std::string const  &);
  TEST_IMPL(cl, lr, long const  &);
  TEST_IMPL(cl, dc, double const  &);
  TEST_IMPL(cl, bl, bool const  &);
  TEST_IMPL(cl, v, std::vector<short> const  &);
  TEST_IMPL(cl, fcr, float const  &);

  TEST_IMPL(cr, i, int const  &&);
  TEST_IMPL(cr, scl, std::string const  &);
  TEST_IMPL(cr, lr, long const  &&);
  TEST_IMPL(cr, dc, double const  &&);
  TEST_IMPL(cr, bl, bool const  &);
  TEST_IMPL(cr, v, std::vector<short> const  &&);
  TEST_IMPL(cr, fcr, float const  &&);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(chained_data_member_getter, pointer) {
# define TEST_IMPL(Owner, Expected) \
  do { \
    FATAL_EXPECT_SAME< \
      Expected, \
      chained_data_member_getter<>::pointer<Owner> \
    >(); \
  } while (false)

  TEST_IMPL(int, int *);
  TEST_IMPL(int &, int *);
  TEST_IMPL(int &&, int *);
  TEST_IMPL(int const &, int const *);
  TEST_IMPL(int const &&, int const *);

# undef TEST_IMPL

# define TEST_PREAMBLE_IMPL(Outer, Inner, ...) \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >;

# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data>>(); \
  } while (false)

  TEST_IMPL(d, i, int *);
  TEST_IMPL(d, scl, std::string const *);
  TEST_IMPL(d, lr, long *);
  TEST_IMPL(d, dc, double const *);
  TEST_IMPL(d, bl, bool *);
  TEST_IMPL(d, v, std::vector<short> *);
  TEST_IMPL(d, fcr, float const *);

  TEST_IMPL(l, i, int *);
  TEST_IMPL(l, scl, std::string const *);
  TEST_IMPL(l, lr, long *);
  TEST_IMPL(l, dc, double const *);
  TEST_IMPL(l, bl, bool *);
  TEST_IMPL(l, v, std::vector<short> *);
  TEST_IMPL(l, fcr, float const *);

  TEST_IMPL(r, i, int *);
  TEST_IMPL(r, scl, std::string const *);
  TEST_IMPL(r, lr, long *);
  TEST_IMPL(r, dc, double const *);
  TEST_IMPL(r, bl, bool *);
  TEST_IMPL(r, v, std::vector<short> *);
  TEST_IMPL(r, fcr, float const *);

  TEST_IMPL(cl, i, int const *);
  TEST_IMPL(cl, scl, std::string const *);
  TEST_IMPL(cl, lr, long const *);
  TEST_IMPL(cl, dc, double const *);
  TEST_IMPL(cl, bl, bool const *);
  TEST_IMPL(cl, v, std::vector<short> const *);
  TEST_IMPL(cl, fcr, float const *);

  TEST_IMPL(cr, i, int const *);
  TEST_IMPL(cr, scl, std::string const *);
  TEST_IMPL(cr, lr, long const *);
  TEST_IMPL(cr, dc, double const *);
  TEST_IMPL(cr, bl, bool const *);
  TEST_IMPL(cr, v, std::vector<short> const *);
  TEST_IMPL(cr, fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data &>>(); \
  } while (false)

  TEST_IMPL(d, i, int *);
  TEST_IMPL(d, scl, std::string const *);
  TEST_IMPL(d, lr, long *);
  TEST_IMPL(d, dc, double const *);
  TEST_IMPL(d, bl, bool *);
  TEST_IMPL(d, v, std::vector<short> *);
  TEST_IMPL(d, fcr, float const *);

  TEST_IMPL(l, i, int *);
  TEST_IMPL(l, scl, std::string const *);
  TEST_IMPL(l, lr, long *);
  TEST_IMPL(l, dc, double const *);
  TEST_IMPL(l, bl, bool *);
  TEST_IMPL(l, v, std::vector<short> *);
  TEST_IMPL(l, fcr, float const *);

  TEST_IMPL(r, i, int *);
  TEST_IMPL(r, scl, std::string const *);
  TEST_IMPL(r, lr, long *);
  TEST_IMPL(r, dc, double const *);
  TEST_IMPL(r, bl, bool *);
  TEST_IMPL(r, v, std::vector<short> *);
  TEST_IMPL(r, fcr, float const *);

  TEST_IMPL(cl, i, int const *);
  TEST_IMPL(cl, scl, std::string const *);
  TEST_IMPL(cl, lr, long const *);
  TEST_IMPL(cl, dc, double const *);
  TEST_IMPL(cl, bl, bool const *);
  TEST_IMPL(cl, v, std::vector<short> const *);
  TEST_IMPL(cl, fcr, float const *);

  TEST_IMPL(cr, i, int const *);
  TEST_IMPL(cr, scl, std::string const *);
  TEST_IMPL(cr, lr, long const *);
  TEST_IMPL(cr, dc, double const *);
  TEST_IMPL(cr, bl, bool const *);
  TEST_IMPL(cr, v, std::vector<short> const *);
  TEST_IMPL(cr, fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int *);
  TEST_IMPL(d, scl, std::string const *);
  TEST_IMPL(d, lr, long *);
  TEST_IMPL(d, dc, double const *);
  TEST_IMPL(d, bl, bool *);
  TEST_IMPL(d, v, std::vector<short> *);
  TEST_IMPL(d, fcr, float const *);

  TEST_IMPL(l, i, int *);
  TEST_IMPL(l, scl, std::string const *);
  TEST_IMPL(l, lr, long *);
  TEST_IMPL(l, dc, double const *);
  TEST_IMPL(l, bl, bool *);
  TEST_IMPL(l, v, std::vector<short> *);
  TEST_IMPL(l, fcr, float const *);

  TEST_IMPL(r, i, int *);
  TEST_IMPL(r, scl, std::string const *);
  TEST_IMPL(r, lr, long *);
  TEST_IMPL(r, dc, double const *);
  TEST_IMPL(r, bl, bool *);
  TEST_IMPL(r, v, std::vector<short> *);
  TEST_IMPL(r, fcr, float const *);

  TEST_IMPL(cl, i, int const *);
  TEST_IMPL(cl, scl, std::string const *);
  TEST_IMPL(cl, lr, long const *);
  TEST_IMPL(cl, dc, double const *);
  TEST_IMPL(cl, bl, bool const *);
  TEST_IMPL(cl, v, std::vector<short> const *);
  TEST_IMPL(cl, fcr, float const *);

  TEST_IMPL(cr, i, int const *);
  TEST_IMPL(cr, scl, std::string const *);
  TEST_IMPL(cr, lr, long const *);
  TEST_IMPL(cr, dc, double const *);
  TEST_IMPL(cr, bl, bool const *);
  TEST_IMPL(cr, v, std::vector<short> const *);
  TEST_IMPL(cr, fcr, float const *);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  *);
  TEST_IMPL(d, scl, std::string const  *);
  TEST_IMPL(d, lr, long const  *);
  TEST_IMPL(d, dc, double const  *);
  TEST_IMPL(d, bl, bool const  *);
  TEST_IMPL(d, v, std::vector<short> const  *);
  TEST_IMPL(d, fcr, float const  *);

  TEST_IMPL(l, i, int const  *);
  TEST_IMPL(l, scl, std::string const  *);
  TEST_IMPL(l, lr, long const  *);
  TEST_IMPL(l, dc, double const  *);
  TEST_IMPL(l, bl, bool const  *);
  TEST_IMPL(l, v, std::vector<short> const  *);
  TEST_IMPL(l, fcr, float const  *);

  TEST_IMPL(r, i, int const  *);
  TEST_IMPL(r, scl, std::string const  *);
  TEST_IMPL(r, lr, long const  *);
  TEST_IMPL(r, dc, double const  *);
  TEST_IMPL(r, bl, bool const  *);
  TEST_IMPL(r, v, std::vector<short> const  *);
  TEST_IMPL(r, fcr, float const  *);

  TEST_IMPL(cl, i, int const  *);
  TEST_IMPL(cl, scl, std::string const  *);
  TEST_IMPL(cl, lr, long const  *);
  TEST_IMPL(cl, dc, double const  *);
  TEST_IMPL(cl, bl, bool const  *);
  TEST_IMPL(cl, v, std::vector<short> const  *);
  TEST_IMPL(cl, fcr, float const  *);

  TEST_IMPL(cr, i, int const  *);
  TEST_IMPL(cr, scl, std::string const  *);
  TEST_IMPL(cr, lr, long const  *);
  TEST_IMPL(cr, dc, double const  *);
  TEST_IMPL(cr, bl, bool const  *);
  TEST_IMPL(cr, v, std::vector<short> const  *);
  TEST_IMPL(cr, fcr, float const  *);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const &>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  *);
  TEST_IMPL(d, scl, std::string const  *);
  TEST_IMPL(d, lr, long const  *);
  TEST_IMPL(d, dc, double const  *);
  TEST_IMPL(d, bl, bool const  *);
  TEST_IMPL(d, v, std::vector<short> const  *);
  TEST_IMPL(d, fcr, float const  *);

  TEST_IMPL(l, i, int const  *);
  TEST_IMPL(l, scl, std::string const  *);
  TEST_IMPL(l, lr, long const  *);
  TEST_IMPL(l, dc, double const  *);
  TEST_IMPL(l, bl, bool const  *);
  TEST_IMPL(l, v, std::vector<short> const  *);
  TEST_IMPL(l, fcr, float const  *);

  TEST_IMPL(r, i, int const  *);
  TEST_IMPL(r, scl, std::string const  *);
  TEST_IMPL(r, lr, long const  *);
  TEST_IMPL(r, dc, double const  *);
  TEST_IMPL(r, bl, bool const  *);
  TEST_IMPL(r, v, std::vector<short> const  *);
  TEST_IMPL(r, fcr, float const  *);

  TEST_IMPL(cl, i, int const  *);
  TEST_IMPL(cl, scl, std::string const  *);
  TEST_IMPL(cl, lr, long const  *);
  TEST_IMPL(cl, dc, double const  *);
  TEST_IMPL(cl, bl, bool const  *);
  TEST_IMPL(cl, v, std::vector<short> const  *);
  TEST_IMPL(cl, fcr, float const  *);

  TEST_IMPL(cr, i, int const  *);
  TEST_IMPL(cr, scl, std::string const  *);
  TEST_IMPL(cr, lr, long const  *);
  TEST_IMPL(cr, dc, double const  *);
  TEST_IMPL(cr, bl, bool const  *);
  TEST_IMPL(cr, v, std::vector<short> const  *);
  TEST_IMPL(cr, fcr, float const  *);

# undef TEST_IMPL
# define TEST_IMPL(Outer, Inner, ...) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner, __VA_ARGS__); \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<data const &&>>(); \
  } while (false)

  TEST_IMPL(d, i, int const  *);
  TEST_IMPL(d, scl, std::string const  *);
  TEST_IMPL(d, lr, long const  *);
  TEST_IMPL(d, dc, double const  *);
  TEST_IMPL(d, bl, bool const  *);
  TEST_IMPL(d, v, std::vector<short> const  *);
  TEST_IMPL(d, fcr, float const  *);

  TEST_IMPL(l, i, int const  *);
  TEST_IMPL(l, scl, std::string const  *);
  TEST_IMPL(l, lr, long const  *);
  TEST_IMPL(l, dc, double const  *);
  TEST_IMPL(l, bl, bool const  *);
  TEST_IMPL(l, v, std::vector<short> const  *);
  TEST_IMPL(l, fcr, float const  *);

  TEST_IMPL(r, i, int const  *);
  TEST_IMPL(r, scl, std::string const  *);
  TEST_IMPL(r, lr, long const  *);
  TEST_IMPL(r, dc, double const  *);
  TEST_IMPL(r, bl, bool const  *);
  TEST_IMPL(r, v, std::vector<short> const  *);
  TEST_IMPL(r, fcr, float const  *);

  TEST_IMPL(cl, i, int const  *);
  TEST_IMPL(cl, scl, std::string const  *);
  TEST_IMPL(cl, lr, long const  *);
  TEST_IMPL(cl, dc, double const  *);
  TEST_IMPL(cl, bl, bool const  *);
  TEST_IMPL(cl, v, std::vector<short> const  *);
  TEST_IMPL(cl, fcr, float const  *);

  TEST_IMPL(cr, i, int const  *);
  TEST_IMPL(cr, scl, std::string const  *);
  TEST_IMPL(cr, lr, long const  *);
  TEST_IMPL(cr, dc, double const  *);
  TEST_IMPL(cr, bl, bool const  *);
  TEST_IMPL(cr, v, std::vector<short> const  *);
  TEST_IMPL(cr, fcr, float const  *);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(chained_data_member_getter, getter) {
  int i = 99;
  std::string const scl("hello, world!");
  long lr = 27;
  double const dc = 5.6;
  bool bl = true;
  std::vector<short> v{1, 2, 3, 4, 5, 6};
  float const fcr = 7.2;

# define TEST_IMPL(Data) \
  do { \
    using getter = chained_data_member_getter<>; \
    \
    FATAL_EXPECT_EQ(Data, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data, getter::ref_getter()(Data)); \
    \
    FATAL_EXPECT_EQ(Data, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Data, getter::ref_getter()(std::move(Data))); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data), \
      std::addressof(getter::ref_getter()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME<decltype(Data) &, decltype(getter::ref(Data))>(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &, \
      decltype(getter::ref_getter()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &&, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &&, \
      decltype(getter::ref_getter()(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::remove_reference<decltype(Data)>::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::remove_reference<decltype(Data)>::type *, \
      decltype(getter::ptr_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL

  int d_i = 99;
  std::string const d_scl("hello, world!");
  long d_lr = 27;
  double const d_dc = 5.6;
  bool d_bl = true;
  std::vector<short> d_v{1, 2, 3, 4, 5, 6};
  float const d_fcr = 7.2;
  data_member_getter_test::data d(
    d_i, d_scl, std::move(d_lr), d_dc, d_bl, d_v, std::move(d_fcr)
  );

  FATAL_ASSERT_EQ(d_i, d.i);
  FATAL_ASSERT_EQ(d_scl, d.scl);
  FATAL_ASSERT_EQ(d_lr, d.lr);
  FATAL_ASSERT_EQ(d_dc, d.dc);
  FATAL_ASSERT_EQ(d_bl, d.bl);
  FATAL_ASSERT_EQ(d_v, d.v);
  FATAL_ASSERT_EQ(d_fcr, d.fcr);

  int l_i = 99;
  std::string const l_scl("hello, world!");
  long l_lr = 27;
  double const l_dc = 5.6;
  bool l_bl = true;
  std::vector<short> l_v{1, 2, 3, 4, 5, 6};
  float const l_fcr = 7.2;
  data_member_getter_test::data l(
    l_i, l_scl, std::move(l_lr), l_dc, l_bl, l_v, std::move(l_fcr)
  );

  FATAL_ASSERT_EQ(l_i, l.i);
  FATAL_ASSERT_EQ(l_scl, l.scl);
  FATAL_ASSERT_EQ(l_lr, l.lr);
  FATAL_ASSERT_EQ(l_dc, l.dc);
  FATAL_ASSERT_EQ(l_bl, l.bl);
  FATAL_ASSERT_EQ(l_v, l.v);
  FATAL_ASSERT_EQ(l_fcr, l.fcr);

  int r_i = 99;
  std::string const r_scl("hello, world!");
  long r_lr = 27;
  double const r_dc = 5.6;
  bool r_bl = true;
  std::vector<short> r_v{1, 2, 3, 4, 5, 6};
  float const r_fcr = 7.2;
  data_member_getter_test::data r(
    r_i, r_scl, std::move(r_lr), r_dc, r_bl, r_v, std::move(r_fcr)
  );

  FATAL_ASSERT_EQ(r_i, r.i);
  FATAL_ASSERT_EQ(r_scl, r.scl);
  FATAL_ASSERT_EQ(r_lr, r.lr);
  FATAL_ASSERT_EQ(r_dc, r.dc);
  FATAL_ASSERT_EQ(r_bl, r.bl);
  FATAL_ASSERT_EQ(r_v, r.v);
  FATAL_ASSERT_EQ(r_fcr, r.fcr);

  int cl_i = 99;
  std::string const cl_scl("hello, world!");
  long cl_lr = 27;
  double const cl_dc = 5.6;
  bool cl_bl = true;
  std::vector<short> cl_v{1, 2, 3, 4, 5, 6};
  float const cl_fcr = 7.2;
  data_member_getter_test::data cl(
    cl_i, cl_scl, std::move(cl_lr), cl_dc, cl_bl, cl_v, std::move(cl_fcr)
  );

  FATAL_ASSERT_EQ(cl_i, cl.i);
  FATAL_ASSERT_EQ(cl_scl, cl.scl);
  FATAL_ASSERT_EQ(cl_lr, cl.lr);
  FATAL_ASSERT_EQ(cl_dc, cl.dc);
  FATAL_ASSERT_EQ(cl_bl, cl.bl);
  FATAL_ASSERT_EQ(cl_v, cl.v);
  FATAL_ASSERT_EQ(cl_fcr, cl.fcr);

  int cr_i = 99;
  std::string const cr_scl("hello, world!");
  long cr_lr = 27;
  double const cr_dc = 5.6;
  bool cr_bl = true;
  std::vector<short> cr_v{1, 2, 3, 4, 5, 6};
  float const cr_fcr = 7.2;
  data_member_getter_test::data cr(
    cr_i, cr_scl, std::move(cr_lr), cr_dc, cr_bl, cr_v, std::move(cr_fcr)
  );

  FATAL_ASSERT_EQ(cr_i, cr.i);
  FATAL_ASSERT_EQ(cr_scl, cr.scl);
  FATAL_ASSERT_EQ(cr_lr, cr.lr);
  FATAL_ASSERT_EQ(cr_dc, cr.dc);
  FATAL_ASSERT_EQ(cr_bl, cr.bl);
  FATAL_ASSERT_EQ(cr_v, cr.v);
  FATAL_ASSERT_EQ(cr_fcr, cr.fcr);

  data_member_getter_test::chained_data x(
    d, l, std::move(r), cl, std::move(cr)
  );

  FATAL_ASSERT_EQ(d, x.d);
  FATAL_ASSERT_EQ(l, x.l);
  FATAL_ASSERT_EQ(r, x.r);
  FATAL_ASSERT_EQ(cl, x.cl);
  FATAL_ASSERT_EQ(cr, x.cr);

  auto const &y = x;

  FATAL_ASSERT_EQ(d, y.d);
  FATAL_ASSERT_EQ(l, y.l);
  FATAL_ASSERT_EQ(r, y.r);
  FATAL_ASSERT_EQ(cl, y.cl);
  FATAL_ASSERT_EQ(cr, y.cr);

# define TEST_PREAMBLE_IMPL(Outer, Inner) \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >;

# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref_getter()(Data)); \
    \
    FATAL_EXPECT_EQ(Data.Outer.Inner, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data.Outer.Inner, getter::ref_getter()(Data)); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      std::addressof(getter::ref_getter()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(Data)>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type \
      >::type, \
      decltype(getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(Data)>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type \
      >::type, \
      decltype(getter::ref_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref_getter()(std::move(Data))); \
    \
    FATAL_EXPECT_EQ( \
      std::move(Data).Outer.Inner, \
      getter::ref(std::move(Data)) \
    ); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Outer.Inner, \
      getter::ref_getter()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      add_reference_from< \
        constify_from< \
          decltype(std::move(Data.Outer).Inner), \
          constify_from< \
            std::remove_reference<decltype(std::move(Data))>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type &&, \
        decltype(std::move(Data).Outer) \
      >::type, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      add_reference_from< \
        constify_from< \
          decltype(std::move(Data).Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(std::move(Data))>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type &&, \
        decltype(std::move(Data).Outer) \
      >::type, \
      decltype(getter::ref_getter()(std::move(Data))) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Outer.Inner), getter::ptr(Data)); \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      getter::ptr_getter()(Data) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Outer.Inner))>::type, \
        constify_from< \
          std::remove_reference<decltype(Data)>::type, \
          std::remove_reference<decltype(Data.Outer)>::type \
        >::type \
      >::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Outer.Inner))>::type, \
        constify_from< \
          std::remove_reference<decltype(Data)>::type, \
          std::remove_reference<decltype(Data.Outer)>::type \
        >::type \
      >::type *, \
      decltype(getter::ptr_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

} // namespace fatal {
