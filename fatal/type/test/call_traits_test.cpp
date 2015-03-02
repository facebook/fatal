/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/call_traits.h>

#include <fatal/test/driver.h>

#include <exception>
#include <string>
#include <type_traits>
#include <utility>

#include <cstring>

namespace fatal {

struct dummy_static_member { static void test_fn(int); };
struct dummy_member_function { void test_fn(int); };
struct dummy_none {};
struct dummy_member_type { using test_fn = void; };

struct member_fn {
  constexpr member_fn(): d_(3.1415926) {}

  void test_fn(long, double, int, char const *) const {
    throw std::exception();
  }

  void test_fn() { d_ = 5.6; }

  double test_fn() const { return d_; }

  int test_fn(int x) const { return x; }

  std::size_t test_fn(
    std::string s1,
    char const *s2,
    std::string const &s3,
    std::string &s4,
    std::string &out
  ) const {
    out = std::move(s1);
    out.append(s2);
    out.append(s3);
    out.append(s4);
    return out.size();
  }

  long test_fn(int x, bool b) const { return b ? x : -x; }

  constexpr int test_fn(int a, int b, int c) const { return a + b + c; }

private:
  double d_;
};

struct static_fn {
  static void test_fn(long, double, int, char const *) {
    throw std::exception();
  }

  static int test_fn(int x) { return x; }

  static std::size_t test_fn(
    std::string s1,
    char const *s2,
    std::string const &s3,
    std::string &s4,
    std::string &out
  ) {
    out = std::move(s1);
    out.append(s2);
    out.append(s3);
    out.append(s4);
    return out.size();
  }

  static long test_fn(int x, bool b) { return b ? x : -x; }

  static constexpr int test_fn(int a, int b, int c) { return a + b + c; }
};

void test_fn(long, double, int, char const *) { throw std::exception(); }

int test_fn(int x) { return x; }

std::size_t test_fn(
  std::string s1,
  char const *s2,
  std::string const &s3,
  std::string &s4,
  std::string &out
) {
  out = std::move(s1);
  out.append(s2);
  out.append(s3);
  out.append(s4);
  return out.size();
}

long test_fn(int x, bool b) { return b ? x : -x; }

constexpr int test_fn(int a, int b, int c) { return a + b + c; }

FATAL_CALL_TRAITS(test_fn_traits, test_fn);

////////////////////////////////////////
// call_traits::member_function::call //
////////////////////////////////////////

TEST(member_function, call_const_this_ref) {
  member_fn const f;

  EXPECT_THROW(
    test_fn_traits::member_function::call(f, 0l, 0.0, 0, ""),
    std::exception
  );

  EXPECT_EQ(3.1415926, test_fn_traits::member_function::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, test_fn_traits::member_function::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    test_fn_traits::member_function::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, test_fn_traits::member_function::call(f, 2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, test_fn_traits::member_function::call(f, 57, true));
  EXPECT_EQ(-57, test_fn_traits::member_function::call(f, 57, false));
}

TEST(member_function, call_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  EXPECT_THROW(
    test_fn_traits::member_function::call(f, 0l, 0.0, 0, ""),
    std::exception
  );

  EXPECT_EQ(3.1415926, test_fn_traits::member_function::call(cf));
  test_fn_traits::member_function::call(f);
  EXPECT_EQ(5.6, test_fn_traits::member_function::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, test_fn_traits::member_function::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    test_fn_traits::member_function::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, test_fn_traits::member_function::call(f, 2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, test_fn_traits::member_function::call(f, 57, true));
  EXPECT_EQ(-57, test_fn_traits::member_function::call(f, 57, false));
}

///////////////////////////////////////////////////
// call_traits::member_function::function_object //
///////////////////////////////////////////////////

TEST(member_function, function_object_const_this_ref) {
  member_fn const f;

  test_fn_traits::member_function function_object;

  EXPECT_THROW(function_object(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, test_fn_traits::member_function::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(f, 57, true));
  EXPECT_EQ(-57, function_object(f, 57, false));
}

TEST(member_function, function_object_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  test_fn_traits::member_function function_object;

  EXPECT_THROW(function_object(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, test_fn_traits::member_function::call(cf));
  test_fn_traits::member_function::call(f);
  EXPECT_EQ(5.6, test_fn_traits::member_function::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(f, 57, true));
  EXPECT_EQ(-57, function_object(f, 57, false));
}

////////////////////////////////////////////
// call_traits::member_function::supports //
////////////////////////////////////////////

struct dummy_ctmfs {
  void test_fn() const {}
  int test_fn(int &&) { return 0; }
};

TEST(member_function, supports) {
# define CHECK_MEMBER_FUNCTION_SUPPORTS(Expected, ...) \
  do { \
    EXPECT_EQ( \
      Expected, \
      (test_fn_traits::member_function::template supports<__VA_ARGS__>::value) \
    ); \
  } while (false)

  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, int const); // TODO: CHECK
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs, int &);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs, int const &);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, int &&);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs, int const &&);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs const);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int const);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int &);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int const &);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int &&);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, int const &&);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_ctmfs const, std::string);

  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_static_member, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_static_member);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_static_member, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_static_member, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_static_member, int, double);

  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_member_function, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_function);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_member_function, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_function, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_function, int, double);

  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_none, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_none);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_none, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_none, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_none, int, double);

  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_type, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_type);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_type, double);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_type, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTS(false, dummy_member_type, int, double);

# undef CHECK_MEMBER_FUNCTION_SUPPORTS
}

//////////////////////////////////////
// call_traits::static_member::call //
//////////////////////////////////////

TEST(static_member, call_static_member) {
  EXPECT_THROW(
    test_fn_traits::static_member::bind<static_fn>::call(0l, 0.0, 0, ""),
    std::exception
  );

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, test_fn_traits::static_member::bind<static_fn>::call(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    test_fn_traits::static_member::bind<static_fn>::call(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, test_fn_traits::static_member::bind<static_fn>::call(2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, test_fn_traits::static_member::bind<static_fn>::call(57, true));
  EXPECT_EQ(-57, test_fn_traits::static_member::bind<static_fn>::call(57, false));
}

/////////////////////////////////////////////////
// call_traits::static_member::function_object //
/////////////////////////////////////////////////

TEST(static_member, function_object_static_member) {
  test_fn_traits::static_member::bind<static_fn> function_object;

  EXPECT_THROW(function_object(0l, 0.0, 0, ""), std::exception);

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(57, true));
  EXPECT_EQ(-57, function_object(57, false));
}

//////////////////////////////////////////
// call_traits::static_member::supports //
//////////////////////////////////////////

struct dummy_ctsms {
  static void test_fn() {}
  static int test_fn(int &&) { return 0; }
};

TEST(static_member, supports) {
# define CHECK_STATIC_MEMBER_SUPPORTS(Expected, ...) \
  do { \
    EXPECT_EQ( \
      Expected, \
      (test_fn_traits::static_member::template supports<__VA_ARGS__>::value) \
    ); \
  } while (false)

  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, int);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, int const); // TODO: CHECK
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_ctsms, int &);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_ctsms, int const &);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, int &&);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_ctsms, int const &&);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, double);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_ctsms, std::string);

  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_static_member, int);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_static_member);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_static_member, double);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_static_member, std::string);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_static_member, int, double);

  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_function, int);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_function);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_function, double);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_function, std::string);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_function, int, double);

  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_none, int);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_none);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_none, double);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_none, std::string);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_none, int, double);

  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_type, int);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_type);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_type, double);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_type, std::string);
  CHECK_STATIC_MEMBER_SUPPORTS(false, dummy_member_type, int, double);

# undef CHECK_STATIC_MEMBER_SUPPORTS
}

//////////////////////////////////////
// call_traits::free_function::call //
//////////////////////////////////////

TEST(free_function, call_free_function) {
  EXPECT_THROW(
    test_fn_traits::free_function::call(0l, 0.0, 0, ""),
    std::exception
  );

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, test_fn_traits::free_function::call(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    test_fn_traits::free_function::call(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, test_fn_traits::free_function::call(2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, test_fn_traits::free_function::call(57, true));
  EXPECT_EQ(-57, test_fn_traits::free_function::call(57, false));
}

/////////////////////////////////////////////////
// call_traits::free_function::function_object //
/////////////////////////////////////////////////

TEST(free_function, function_object_free_function) {
  test_fn_traits::free_function function_object;

  EXPECT_THROW(function_object(0l, 0.0, 0, ""), std::exception);

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(57, true));
  EXPECT_EQ(-57, function_object(57, false));
}

/////////////////////////
// call_operator_traits //
/////////////////////////

struct member_op {
  constexpr member_op(): d_(3.1415926) {}

  void operator ()(long, double, int, char const *) const {
    throw std::exception();
  }

  void operator ()() { d_ = 5.6; }

  double operator ()() const { return d_; }

  int operator ()(int x) const { return x; }

  std::size_t operator ()(
    std::string s1,
    char const *s2,
    std::string const &s3,
    std::string &s4,
    std::string &out
  ) const {
    out = std::move(s1);
    out.append(s2);
    out.append(s3);
    out.append(s4);
    return out.size();
  }

  long operator ()(int x, bool b) const { return b ? x : -x; }

  constexpr int operator ()(int a, int b, int c) const { return a + b + c; }

private:
  double d_;
};

TEST(call_operator_traits, const_this_ref) {
  member_op const f;

  EXPECT_THROW(call_operator_traits::call(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, call_operator_traits::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    call_operator_traits::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, call_operator_traits::call(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, call_operator_traits::call(f, 57, true));
  EXPECT_EQ(-57, call_operator_traits::call(f, 57, false));
}

TEST(call_operator_traits, const_this_ref_function_object) {
  member_op const f;

  call_operator_traits function_object;

  EXPECT_THROW(function_object(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (function_object(f, s1, s2, s3, s4, out))
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(f, 57, true));
  EXPECT_EQ(-57, function_object(f, 57, false));
}

TEST(call_operator_traits, non_const_this_ref) {
  member_op f;
  member_op const &cf = f;

  EXPECT_THROW(call_operator_traits::call(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(cf));
  call_operator_traits::call(f);
  EXPECT_EQ(5.6, call_operator_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, call_operator_traits::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    call_operator_traits::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, call_operator_traits::call(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, call_operator_traits::call(f, 57, true));
  EXPECT_EQ(-57, call_operator_traits::call(f, 57, false));
}

TEST(call_operator_traits, non_const_this_ref_function_object) {
  member_op f;
  member_op const &cf = f;

  call_operator_traits function_object;

  EXPECT_THROW(function_object(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(cf));
  call_operator_traits::call(f);
  EXPECT_EQ(5.6, call_operator_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, function_object(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (function_object(f, s1, s2, s3, s4, out))
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, function_object(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, function_object(f, 57, true));
  EXPECT_EQ(-57, function_object(f, 57, false));
}

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

using foonction = void(*)();
using foonction_is = void(*)(int, std::string);

TEST(call_operator_traits, supports) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  EXPECT_TRUE((call_operator_traits::supports<foonctor>::value));
  EXPECT_FALSE((call_operator_traits::supports<foonctor, int>::value));
  EXPECT_FALSE((call_operator_traits::supports<foonctor, int, double>::value));
  EXPECT_TRUE((
    call_operator_traits::supports<foonctor, int, std::string>::value
  ));

  EXPECT_TRUE((call_operator_traits::supports<decltype(lambda)>::value));
  EXPECT_FALSE((call_operator_traits::supports<decltype(lambda), int>::value));
  EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda), int, double>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda), int, std::string>::value
  ));

  EXPECT_FALSE((call_operator_traits::supports<decltype(lambda_is)>::value));
  EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda_is), int>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda_is), int, double>::value
  ));
  EXPECT_TRUE((
    call_operator_traits::supports<
      decltype(lambda_is), int, std::string
    >::value
  ));

  EXPECT_TRUE((call_operator_traits::supports<foonction>::value));
  EXPECT_FALSE((call_operator_traits::supports<foonction, int>::value));
  EXPECT_FALSE((
    call_operator_traits::supports<foonction, int, double>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supports<foonction, int, std::string>::value
  ));

  EXPECT_FALSE((call_operator_traits::supports<foonction_is>::value));
  EXPECT_FALSE((call_operator_traits::supports<foonction_is, int>::value));
  EXPECT_FALSE((
    call_operator_traits::supports<foonction_is, int, double>::value
  ));
  EXPECT_TRUE((
    call_operator_traits::supports<foonction_is, int, std::string>::value
  ));
}

} // namespace fatal {
