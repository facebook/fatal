/*
 *  Copyright (c) 2016, Facebook, Inc.
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

template <typename T, T Result>
struct return_constant:
  public std::integral_constant<T, Result>
{
  template <typename... Args>
  constexpr T operator ()(Args &&...) const { return Result; }
};

using fallback = return_constant<std::size_t, 54321>;

FATAL_CALL_TRAITS(test_fn_traits, test_fn);
FATAL_CALL_TRAITS(no_fn_traits, no_fn);

/////////////////////
// member_function //
/////////////////////

struct member_fn {
  constexpr member_fn(): d_(3.1415926) {}

  [[noreturn]] void test_fn(long, double, int, char const *) const {
    throw std::exception();
  }

  void test_fn() { d_ = 5.6; }

  constexpr double test_fn() const { return d_; }

  constexpr int test_fn(int x) const { return x; }

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

  constexpr long test_fn(int x, bool b) const { return b ? x : -x; }

  constexpr int test_fn(int a, int b, int c) const { return a + b + c; }

private:
  double d_;
};

using member_fn_traits = test_fn_traits::member_function;

/////////////////////////////
// member_function -> call //
/////////////////////////////

FATAL_TEST(member_function, call_const_this_ref) {
  member_fn const f;

  FATAL_EXPECT_THROW(std::exception) {
    member_fn_traits::call(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, member_fn_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, member_fn_traits::call(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    member_fn_traits::call(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, member_fn_traits::call(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, member_fn_traits::call(f, 57, true));
  FATAL_EXPECT_EQ(-57, member_fn_traits::call(f, 57, false));
}

FATAL_TEST(member_function, call_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  FATAL_EXPECT_THROW(std::exception) {
    member_fn_traits::call(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, member_fn_traits::call(cf));
  member_fn_traits::call(f);
  FATAL_EXPECT_EQ(5.6, member_fn_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, member_fn_traits::call(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    member_fn_traits::call(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, member_fn_traits::call(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, member_fn_traits::call(f, 57, true));
  FATAL_EXPECT_EQ(-57, member_fn_traits::call(f, 57, false));
}

////////////////////////////////////////
// member_function -> function_object //
////////////////////////////////////////

FATAL_TEST(member_function, function_object_const_this_ref) {
  member_fn const f;

  member_fn_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, member_fn_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(f, 57, true));
  FATAL_EXPECT_EQ(-57, function_object(f, 57, false));
}

FATAL_TEST(member_function, function_object_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  member_fn_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, member_fn_traits::call(cf));
  member_fn_traits::call(f);
  FATAL_EXPECT_EQ(5.6, member_fn_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(f, 57, true));
  FATAL_EXPECT_EQ(-57, function_object(f, 57, false));
}

/////////////////////////////////
// member_function -> supports //
/////////////////////////////////

struct dummy_ctmfs {
  void test_fn() const {}
  int test_fn(int &&) { return 0; }
};

FATAL_TEST(member_function, supports) {
# define CHECK_MEMBER_FUNCTION_SUPPORTS(Expected, ...) \
  do { \
    FATAL_EXPECT_EQ( \
      Expected, \
      (member_fn_traits::template supports<__VA_ARGS__>::value) \
    ); \
  } while (false)

  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, int);
  CHECK_MEMBER_FUNCTION_SUPPORTS(true, dummy_ctmfs, int const);
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

///////////////////
// static member //
///////////////////

struct static_fn {
  [[noreturn]] static void test_fn(long, double, int, char const *) {
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

using static_fn_traits = test_fn_traits::static_member::bind<static_fn>;

///////////////////////////
// static_member -> call //
///////////////////////////

FATAL_TEST(static_member, call_static_member) {
  FATAL_EXPECT_THROW(std::exception) {
    static_fn_traits::call(0l, 0.0, 0, "");
  };

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, static_fn_traits::call(17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    static_fn_traits::call(s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, static_fn_traits::call(2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, static_fn_traits::call(57, true));
  FATAL_EXPECT_EQ(-57, static_fn_traits::call(57, false));
}

//////////////////////////////////////
// static_member -> function_object //
//////////////////////////////////////

FATAL_TEST(static_member, function_object_static_member) {
  static_fn_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(0l, 0.0, 0, "");
  };

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(57, true));
  FATAL_EXPECT_EQ(-57, function_object(57, false));
}

///////////////////////////////
// static_member -> supports //
///////////////////////////////

struct dummy_ctsms {
  static void test_fn() {}
  static int test_fn(int &&) { return 0; }
};

FATAL_TEST(static_member, supports) {
# define CHECK_STATIC_MEMBER_SUPPORTS(Expected, ...) \
  do { \
    FATAL_EXPECT_EQ( \
      Expected, \
      (test_fn_traits::static_member::template supports<__VA_ARGS__>::value) \
    ); \
  } while (false)

  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, int);
  CHECK_STATIC_MEMBER_SUPPORTS(true, dummy_ctsms, int const);
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

///////////////////
// free function //
///////////////////

[[noreturn]] void test_fn(long, double, int, char const *) {
  throw std::exception();
}

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

FATAL_FREE_FUNCTION_CALL_TRAITS(free_fn_traits, test_fn);

///////////////////////////
// free_function -> call //
///////////////////////////

FATAL_TEST(free_function, call_free_function) {
  FATAL_EXPECT_THROW(std::exception) {
    free_fn_traits::call(0l, 0.0, 0, "");
  };

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, free_fn_traits::call(17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    free_fn_traits::call(s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, free_fn_traits::call(2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, free_fn_traits::call(57, true));
  FATAL_EXPECT_EQ(-57, free_fn_traits::call(57, false));
}

//////////////////////////////////////
// free_function -> function_object //
//////////////////////////////////////

FATAL_TEST(free_function, function_object_free_function) {
  free_fn_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(0l, 0.0, 0, "");
  };

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    function_object(s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(57, true));
  FATAL_EXPECT_EQ(-57, function_object(57, false));
}

//////////////////////////
// call_operator_traits //
//////////////////////////

struct member_op {
  constexpr member_op(): d_(3.1415926) {}

  [[noreturn]] void operator ()(long, double, int, char const *) const {
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

//////////////////////////////////
// call_operator_traits -> call //
//////////////////////////////////

FATAL_TEST(call_operator_traits, const_this_ref) {
  member_op const f;

  FATAL_EXPECT_THROW(std::exception) {
    call_operator_traits::call(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, call_operator_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, call_operator_traits::call(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    call_operator_traits::call(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, call_operator_traits::call(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, call_operator_traits::call(f, 57, true));
  FATAL_EXPECT_EQ(-57, call_operator_traits::call(f, 57, false));
}

FATAL_TEST(call_operator_traits, const_this_ref_function_object) {
  member_op const f;

  call_operator_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, call_operator_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (function_object(f, s1, s2, s3, s4, out))
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(f, 57, true));
  FATAL_EXPECT_EQ(-57, function_object(f, 57, false));
}

FATAL_TEST(call_operator_traits, non_const_this_ref) {
  member_op f;
  member_op const &cf = f;

  FATAL_EXPECT_THROW(std::exception) {
    call_operator_traits::call(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, call_operator_traits::call(cf));
  call_operator_traits::call(f);
  FATAL_EXPECT_EQ(5.6, call_operator_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, call_operator_traits::call(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    call_operator_traits::call(f, s1, s2, s3, s4, out)
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, call_operator_traits::call(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, call_operator_traits::call(f, 57, true));
  FATAL_EXPECT_EQ(-57, call_operator_traits::call(f, 57, false));
}

/////////////////////////////////////////////
// call_operator_traits -> function_object //
/////////////////////////////////////////////

FATAL_TEST(call_operator_traits, non_const_this_ref_function_object) {
  member_op f;
  member_op const &cf = f;

  call_operator_traits function_object;

  FATAL_EXPECT_THROW(std::exception) {
    function_object(f, 0l, 0.0, 0, "");
  };

  FATAL_EXPECT_EQ(3.1415926, call_operator_traits::call(cf));
  call_operator_traits::call(f);
  FATAL_EXPECT_EQ(5.6, call_operator_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  FATAL_EXPECT_EQ(17, function_object(f, 17));

  FATAL_EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (function_object(f, s1, s2, s3, s4, out))
  );
  FATAL_EXPECT_EQ("hello, world!", out);

  using c = std::integral_constant<int, function_object(f, 2, 3, 5)>;

  FATAL_EXPECT_EQ(2 + 3 + 5, c::value);

  FATAL_EXPECT_EQ(57, function_object(f, 57, true));
  FATAL_EXPECT_EQ(-57, function_object(f, 57, false));
}

//////////////////////////////////////
// call_operator_traits -> supports //
//////////////////////////////////////

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

using foonction = void(*)();
using foonction_is = void(*)(int, std::string);

FATAL_TEST(call_operator_traits, supports) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  FATAL_EXPECT_TRUE((call_operator_traits::supports<foonctor>::value));
  FATAL_EXPECT_FALSE((call_operator_traits::supports<foonctor, int>::value));
  FATAL_EXPECT_FALSE(
    (call_operator_traits::supports<foonctor, int, double>::value)
  );
  FATAL_EXPECT_TRUE((
    call_operator_traits::supports<foonctor, int, std::string>::value
  ));

  FATAL_EXPECT_TRUE((call_operator_traits::supports<decltype(lambda)>::value));
  FATAL_EXPECT_FALSE(
    (call_operator_traits::supports<decltype(lambda), int>::value)
  );
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda), int, double>::value
  ));
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda), int, std::string>::value
  ));

  FATAL_EXPECT_FALSE(
    (call_operator_traits::supports<decltype(lambda_is)>::value)
  );
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda_is), int>::value
  ));
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<decltype(lambda_is), int, double>::value
  ));
  FATAL_EXPECT_TRUE((
    call_operator_traits::supports<
      decltype(lambda_is), int, std::string
    >::value
  ));

  FATAL_EXPECT_TRUE((call_operator_traits::supports<foonction>::value));
  FATAL_EXPECT_FALSE((call_operator_traits::supports<foonction, int>::value));
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<foonction, int, double>::value
  ));
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<foonction, int, std::string>::value
  ));

  FATAL_EXPECT_FALSE((call_operator_traits::supports<foonction_is>::value));
  FATAL_EXPECT_FALSE(
    (call_operator_traits::supports<foonction_is, int>::value)
  );
  FATAL_EXPECT_FALSE((
    call_operator_traits::supports<foonction_is, int, double>::value
  ));
  FATAL_EXPECT_TRUE((
    call_operator_traits::supports<foonction_is, int, std::string>::value
  ));
}

/////////////
// call_if //
/////////////

FATAL_TEST(call_if, member_function) {
  member_fn f;

  FATAL_EXPECT_SAME<
    void,
    decltype(call_if<member_fn_traits, fallback, member_fn_traits::supports>(f))
  >();

  FATAL_EXPECT_EQ(
    98989,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(f, 98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(f, "xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, 137, false, "test"
    ))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, 137, false
    ))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::member_function, fallback,
      no_fn_traits::member_function::supports
    >(f))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::member_function, fallback,
      no_fn_traits::member_function::supports
    >(f, 98989))
  );
}

FATAL_TEST(call_if, member_function_const) {
  member_fn const f;

  FATAL_EXPECT_SAME<
    double,
    decltype(call_if<member_fn_traits, fallback, member_fn_traits::supports>(f))
  >();

  FATAL_EXPECT_EQ(
    98989,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(f, 98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(f, "xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, 137, false, "test"
    ))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, 137, false
    ))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if<member_fn_traits, fallback, member_fn_traits::supports>(
      f, "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::member_function, fallback,
      no_fn_traits::member_function::supports
    >(f))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::member_function, fallback,
      no_fn_traits::member_function::supports
    >(f, 98989))
  );
}

FATAL_TEST(call_if, static_member) {
  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>())
  );

  FATAL_EXPECT_EQ(
    98989,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>(98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>("xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>(
      137, false, "test"
    ))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>(
      137, false
    ))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if<static_fn_traits, fallback, static_fn_traits::supports>(
      "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::static_member::bind<static_fn>, fallback,
      no_fn_traits::static_member::bind<static_fn>::supports
    >())
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::static_member::bind<static_fn>, fallback,
      no_fn_traits::static_member::bind<static_fn>::supports
    >(98989))
  );
}

FATAL_TEST(call_if, static_member_unbound) {
  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >())
  );

  FATAL_EXPECT_EQ(
    98989,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >(98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >("xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >(137, false, "test"))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >(137, false))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if<
      test_fn_traits::static_member, fallback,
      test_fn_traits::static_member::supports, static_fn
    >("1234", "56", "789", s1, out))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::static_member, fallback,
      no_fn_traits::static_member::supports, static_fn
    >())
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if<
      no_fn_traits::static_member, fallback,
      no_fn_traits::static_member::supports, static_fn
    >(98989))
  );
}

///////////////////////
// call_if_supported //
///////////////////////

FATAL_TEST(call_if_supported, member_function) {
  member_fn f;

  FATAL_EXPECT_SAME<
    void,
    decltype(call_if_supported<member_fn_traits, fallback>(f))
  >();

  FATAL_EXPECT_EQ(
    98989,
    (call_if_supported<member_fn_traits, fallback>(f, 98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<member_fn_traits, fallback>(f, "xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<member_fn_traits, fallback>(f, 137, false, "test"))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if_supported<member_fn_traits, fallback>(f, 137, false))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if_supported<member_fn_traits, fallback>(
      f, "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::member_function, fallback>(f))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::member_function, fallback>(f, 98989))
  );
}

FATAL_TEST(call_if_supported, member_function_const) {
  member_fn const f;

  FATAL_EXPECT_SAME<
    double,
    decltype(call_if_supported<member_fn_traits, fallback>(f))
  >();

  FATAL_EXPECT_EQ(
    98989,
    (call_if_supported<member_fn_traits, fallback>(f, 98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<member_fn_traits, fallback>(f, "xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<member_fn_traits, fallback>(f, 137, false, "test"))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if_supported<member_fn_traits, fallback>(f, 137, false))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if_supported<member_fn_traits, fallback>(
      f, "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::member_function, fallback>(f))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::member_function, fallback>(f, 98989))
  );
}

FATAL_TEST(call_if_supported, static_member) {
  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<static_fn_traits, fallback>())
  );

  FATAL_EXPECT_EQ(
    98989,
    (call_if_supported<static_fn_traits, fallback>(98989))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<static_fn_traits, fallback>("xx"))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<static_fn_traits, fallback>(137, false, "test"))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if_supported<static_fn_traits, fallback>(137, false))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if_supported<static_fn_traits, fallback>(
      "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::static_member::bind<static_fn>, fallback>(
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::static_member::bind<static_fn>, fallback>(
      98989
    ))
  );
}

FATAL_TEST(call_if_supported, static_member_unbound) {
  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>())
  );

  FATAL_EXPECT_EQ(
    98989,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>(
      98989
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>(
      "xx"
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>(
      137, false, "test"
    ))
  );

  FATAL_EXPECT_EQ(
    -137,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>(
      137, false
    ))
  );

  std::string s1("012345");
  std::string out;

  FATAL_EXPECT_EQ(
    15,
    (call_if_supported<test_fn_traits::static_member, fallback, static_fn>(
      "1234", "56", "789", s1, out
    ))
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::static_member, fallback, static_fn>())
  );

  FATAL_EXPECT_EQ(
    fallback::value,
    (call_if_supported<no_fn_traits::static_member, fallback, static_fn>(98989))
  );
}

//////////
// name //
//////////

struct str {
  FATAL_S(test_fn, "test_fn");
  FATAL_S(no_fn, "no_fn");
};

FATAL_TEST(name, member_traits) {
#define TEST_IMPL(Name, Traits) \
  do { \
    FATAL_EXPECT_SAME<str::Name, Traits::name>(); \
    FATAL_EXPECT_SAME<str::Name, Traits::member_function::name>(); \
    FATAL_EXPECT_SAME<str::Name, Traits::static_member::name>(); \
  } while (false)

  TEST_IMPL(test_fn, test_fn_traits);
  TEST_IMPL(no_fn, no_fn_traits);

# undef TEST_IMPL
}

FATAL_TEST(name, free_function) {
#define TEST_IMPL(Name, Traits) \
  do { \
    FATAL_EXPECT_SAME<str::Name, Traits::name>(); \
  } while (false)

  TEST_IMPL(test_fn, free_fn_traits);

# undef TEST_IMPL
}

} // namespace fatal {
