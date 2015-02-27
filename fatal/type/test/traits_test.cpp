/*
 *  Copyright (c) 2015, Facebook, Inc.
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

/////////////////////////////
// remove_rvalue_reference //
/////////////////////////////

TEST(traits, remove_rvalue_reference) {
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

TEST(traits, same_reference_as) {
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

TEST(traits, add_reference_from) {
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

TEST(traits, add_const_from) {
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

TEST(traits, constify) {
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

TEST(traits, constify_from) {
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
    LOG(ERROR) << "checker: " << type_str<checker>();
    LOG(ERROR) << "type: " << type_str<T>();
    EXPECT_EQ(expected, actual);
  }
}

template <typename, typename...> struct X0 {};
template <typename, typename, typename...> struct X1 {};
template <typename, typename, typename, typename...> struct X2 {};
template <typename, typename, typename, typename, typename...> struct X3 {};

TEST(traits, is_template) {
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

////////////////////////
// fast_pass_by_value //
////////////////////////

TEST(traits, fast_pass_by_value) {
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

TEST(traits, fast_pass) {
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
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const *&>
  >();
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

///////////////////
// safe_overload //
///////////////////

struct Base {};
struct Derived: public Base {};
struct Foo {};
enum class ctor { def, copy, move, universal };

TEST(traits, safe_overload) {
  EXPECT_FALSE((safe_overload<Base, Base>::value));
  EXPECT_FALSE((safe_overload<Base, Derived>::value));
  EXPECT_TRUE((safe_overload<Base>::value));
  EXPECT_TRUE((safe_overload<Base, int>::value));
  EXPECT_TRUE((safe_overload<Base, void>::value));
  EXPECT_TRUE((safe_overload<Base, Foo>::value));
  EXPECT_TRUE((safe_overload<Base, int, int>::value));
  EXPECT_TRUE((safe_overload<Base, void, void>::value));
  EXPECT_TRUE((safe_overload<Base, Foo, Foo>::value));
  EXPECT_TRUE((safe_overload<Base, int, int, int>::value));
  EXPECT_TRUE((safe_overload<Base, void, void, void>::value));
  EXPECT_TRUE((safe_overload<Base, Foo, Foo, Foo>::value));
  EXPECT_TRUE((safe_overload<Base, int, void, Foo, bool>::value));
}

struct overloading_test {
  overloading_test(): type(ctor::def) {}
  overloading_test(overloading_test const &) noexcept: type(ctor::copy) {}
  overloading_test(overloading_test &&) noexcept: type(ctor::move) {}
  template <
    typename T,
    typename = typename std::enable_if<
      safe_overload<overloading_test, T>::value, void
    >::type
  >
  explicit overloading_test(T &&): type(ctor::universal) {}

  ctor type;
};

TEST(traits, safe_overload_nonvariadic) {
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
    typename = typename std::enable_if<
      safe_overload<variadic_overloading_test, Args...>::value, void
    >::type
  >
  explicit variadic_overloading_test(Args &&...): type(ctor::universal) {}

  ctor type;
};

TEST(traits, safe_overload_variadic) {
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
    typename = safe_overload_t<overloading_test_t, T>
  >
  explicit overloading_test_t(T &&): type(ctor::universal) {}
  ctor type;
};

TEST(traits, safe_overload_nonvariadic_t) {
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
    typename = safe_overload_t<variadic_overloading_test_t, Args...>
  >
  explicit variadic_overloading_test_t(Args &&...): type(ctor::universal) {}
  ctor type;
};

TEST(traits, safe_overload_variadic_t) {
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

/////////////////
// is_callable //
/////////////////

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

typedef void(*foonction)();
typedef void(*foonction_is)(int, std::string);

TEST(traits, is_callable) {
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
  EXPECT_TRUE((
    is_callable<decltype(lambda_is), int, std::string>::value
  ));

  EXPECT_TRUE((is_callable<foonction>::value));
  EXPECT_FALSE((is_callable<foonction, int>::value));
  EXPECT_FALSE((is_callable<foonction, int, double>::value));
  EXPECT_FALSE((is_callable<foonction, int, std::string>::value));

  EXPECT_FALSE((is_callable<foonction_is>::value));
  EXPECT_FALSE((is_callable<foonction_is, int>::value));
  EXPECT_FALSE((is_callable<foonction_is, int, double>::value));
  EXPECT_TRUE((is_callable<foonction_is, int, std::string>::value));
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

TEST(traits, has_member_type) {
  FATAL_EXPECT_SAME<
    std::true_type,
    has_member_type_test::has_xyz::check<has_member_type_test::foo>
  >();

  FATAL_EXPECT_SAME<
    std::true_type,
    has_member_type_test::has_xyz::check<has_member_type_test::bar>
  >();

  FATAL_EXPECT_SAME<
    std::false_type,
    has_member_type_test::has_xyz::check<has_member_type_test::baz>
  >();
}

//////////////////////////////
// FATAL_DATA_MEMBER_GETTER //
//////////////////////////////

namespace data_member_getter_test {

struct data {
  data(
    int i, std::string const &s, long &&l, double const d,
    bool &b, std::vector<short> v, float const &&f
  ):
    i(i), s(s), l(std::move(l)), d(d), b(b), v(v), f(std::move(f))
  {}

  int i;
  std::string const &s;
  long &&l;
  double const d;
  bool &b;
  std::vector<short> v;
  float const &&f;
};

struct getter {
# define TEST_IMPL(...) \
  FATAL_DATA_MEMBER_GETTER(__VA_ARGS__, __VA_ARGS__)

  TEST_IMPL(i);
  TEST_IMPL(s);
  TEST_IMPL(l);
  TEST_IMPL(d);
  TEST_IMPL(b);
  TEST_IMPL(v);
  TEST_IMPL(f);

# undef TEST_IMPL
};

} // namespace data_member_getter_test {

TEST(data_member_getter, type) {
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data>>();\
  } while (false)

  TEST_IMPL(i, int);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long &&);
  TEST_IMPL(d, double const);
  TEST_IMPL(b, bool &);
  TEST_IMPL(v, std::vector<short>);
  TEST_IMPL(f, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &>>();\
  } while (false)

  TEST_IMPL(i, int &);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long &);
  TEST_IMPL(d, double const &);
  TEST_IMPL(b, bool &);
  TEST_IMPL(v, std::vector<short> &);
  TEST_IMPL(f, float const &);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<data &&>>();\
  } while (false)

  TEST_IMPL(i, int &&);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long &&);
  TEST_IMPL(d, double const &&);
  TEST_IMPL(b, bool &);
  TEST_IMPL(v, std::vector<short> &&);
  TEST_IMPL(f, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::Field::type<data const>>(); \
  } while (false)

  TEST_IMPL(i, int const);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long const &&);
  TEST_IMPL(d, double const);
  TEST_IMPL(b, bool const &);
  TEST_IMPL(v, std::vector<short> const);
  TEST_IMPL(f, float const &&);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::Field::type<data const &>>(); \
  } while (false)

  TEST_IMPL(i, int const &);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long const &);
  TEST_IMPL(d, double const &);
  TEST_IMPL(b, bool const &);
  TEST_IMPL(v, std::vector<short> const &);
  TEST_IMPL(f, float const &);

# undef TEST_IMPL
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::Field::type<data const &&>>(); \
  } while (false)

  TEST_IMPL(i, int const &&);
  TEST_IMPL(s, std::string const &);
  TEST_IMPL(l, long const &&);
  TEST_IMPL(d, double const &&);
  TEST_IMPL(b, bool const &);
  TEST_IMPL(v, std::vector<short> const &&);
  TEST_IMPL(f, float const &&);

# undef TEST_IMPL
}

TEST(data_member_getter, get_ptr) {
  int i = 99;
  std::string const s("hello, world!");
  long l = 27;
  double const d = 5.6;
  bool b = true;
  std::vector<short> v{1, 2, 3, 4, 5, 6};
  float const f = 7.2;

  data_member_getter_test::data x(i, s, std::move(l), d, b, v, std::move(f));
  auto const &y = x;

  ASSERT_EQ(i, x.i);
  ASSERT_EQ(s, x.s);
  ASSERT_EQ(l, x.l);
  ASSERT_EQ(d, x.d);
  ASSERT_EQ(b, x.b);
  ASSERT_EQ(v, x.v);
  ASSERT_EQ(f, x.f);

# define TEST_IMPL(Data, Field) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    \
    EXPECT_EQ(Field, getter::Field::ref(Data)); \
    EXPECT_EQ(Field, getter::Field::ref_getter()(Data)); \
    \
    EXPECT_EQ(Data.Field, getter::Field::ref(Data)); \
    EXPECT_EQ(Data.Field, getter::Field::ref_getter()(Data)); \
    \
    EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(getter::Field::ref(Data)) \
    ); \
    \
    EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(getter::Field::ref_getter()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(getter::Field::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(getter::Field::ref_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, s);
  TEST_IMPL(x, l);
  TEST_IMPL(x, d);
  TEST_IMPL(x, b);
  TEST_IMPL(x, v);
  TEST_IMPL(x, f);

  TEST_IMPL(y, i);
  TEST_IMPL(y, s);
  TEST_IMPL(y, l);
  TEST_IMPL(y, d);
  TEST_IMPL(y, b);
  TEST_IMPL(y, v);
  TEST_IMPL(y, f);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    \
    EXPECT_EQ(Field, getter::Field::ref(std::move(Data))); \
    EXPECT_EQ(Field, getter::Field::ref_getter()(std::move(Data))); \
    \
    EXPECT_EQ(std::move(Data).Field, getter::Field::ref(std::move(Data))); \
    EXPECT_EQ( \
      std::move(Data).Field, \
      getter::Field::ref_getter()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(getter::Field::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(getter::Field::ref_getter()(std::move(Data))) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, s);
  TEST_IMPL(x, l);
  TEST_IMPL(x, d);
  TEST_IMPL(x, b);
  TEST_IMPL(x, v);
  TEST_IMPL(x, f);

  TEST_IMPL(y, i);
  TEST_IMPL(y, s);
  TEST_IMPL(y, l);
  TEST_IMPL(y, d);
  TEST_IMPL(y, b);
  TEST_IMPL(y, v);
  TEST_IMPL(y, f);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    \
    EXPECT_EQ(std::addressof(Data.Field), getter::Field::ptr(Data)); \
    EXPECT_EQ(std::addressof(Data.Field), getter::Field::ptr_getter()(Data)); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(getter::Field::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(getter::Field::ptr_getter()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, s);
  TEST_IMPL(x, l);
  TEST_IMPL(x, d);
  TEST_IMPL(x, b);
  TEST_IMPL(x, v);
  TEST_IMPL(x, f);

  TEST_IMPL(y, i);
  TEST_IMPL(y, s);
  TEST_IMPL(y, l);
  TEST_IMPL(y, d);
  TEST_IMPL(y, b);
  TEST_IMPL(y, v);
  TEST_IMPL(y, f);

# undef TEST_IMPL
}

} // namespace fatal {
