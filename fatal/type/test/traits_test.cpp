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

#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace fatal {

template <std::size_t> struct T {};
template <std::size_t> struct S {};

/////////////////
// is_complete //
/////////////////

struct complete_type {};
struct incomplete_type;

TEST(traits, is_complete) {
  EXPECT_TRUE((is_complete<int>::value));
  EXPECT_TRUE((is_complete<std::string>::value));
  EXPECT_TRUE((is_complete<complete_type>::value));
  EXPECT_FALSE((is_complete<incomplete_type>::value));
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

TEST(traits, is_template) {
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

////////////////////////
// safe_ctor_overload //
////////////////////////

struct Base {};
struct Derived: public Base {};
struct Foo {};
enum class ctor { def, copy, move, universal };

TEST(traits, safe_ctor_overload) {
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
    typename = typename std::enable_if<
      safe_ctor_overload<overloading_test, T>::value, void
    >::type
  >
  explicit overloading_test(T &&): type(ctor::universal) {}

  ctor type;
};

TEST(traits, safe_ctor_overload_nonvariadic) {
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
      safe_ctor_overload<variadic_overloading_test, Args...>::value, void
    >::type
  >
  explicit variadic_overloading_test(Args &&...): type(ctor::universal) {}

  ctor type;
};

TEST(traits, safe_ctor_overload_variadic) {
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
    typename = safe_ctor_overload_t<overloading_test_t, T>
  >
  explicit overloading_test_t(T &&): type(ctor::universal) {}
  ctor type;
};

TEST(traits, safe_ctor_overload_nonvariadic_t) {
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
    typename = safe_ctor_overload_t<variadic_overloading_test_t, Args...>
  >
  explicit variadic_overloading_test_t(Args &&...): type(ctor::universal) {}
  ctor type;
};

TEST(traits, safe_ctor_overload_variadic_t) {
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

////////////////
// is_functor //
////////////////

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

typedef void(*foonction)();
typedef void(*foonction_is)(int, std::string);

TEST(traits, is_functor) {
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

} // namespace fatal {
