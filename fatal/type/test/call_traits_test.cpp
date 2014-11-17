/*
 *  Copyright (c) 2014, Facebook, Inc.
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

namespace fatal {

////////////////////////////////////////
// call_traits::member_function::call //
////////////////////////////////////////

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

FATAL_CALL_TRAITS(mf_traits, test_fn);

TEST(call_traits_member_function, call_const_this_ref) {
  member_fn const f;

  EXPECT_THROW(
    mf_traits::member_function::call(f, 0l, 0.0, 0, ""),
    std::exception
  );

  EXPECT_EQ(3.1415926, mf_traits::member_function::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, mf_traits::member_function::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    mf_traits::member_function::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, mf_traits::member_function::call(f, 2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, mf_traits::member_function::call(f, 57, true));
  EXPECT_EQ(-57, mf_traits::member_function::call(f, 57, false));
}

TEST(call_traits_member_function, call_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  EXPECT_THROW(
    mf_traits::member_function::call(f, 0l, 0.0, 0, ""),
    std::exception
  );

  EXPECT_EQ(3.1415926, mf_traits::member_function::call(cf));
  mf_traits::member_function::call(f);
  EXPECT_EQ(5.6, mf_traits::member_function::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, mf_traits::member_function::call(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    mf_traits::member_function::call(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, mf_traits::member_function::call(f, 2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, mf_traits::member_function::call(f, 57, true));
  EXPECT_EQ(-57, mf_traits::member_function::call(f, 57, false));
}

///////////////////////////////////////////
// call_traits::member_function::functor //
///////////////////////////////////////////

TEST(call_traits_member_function, functor_const_this_ref) {
  member_fn const f;

  mf_traits::member_function functor;

  EXPECT_THROW(functor(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, mf_traits::member_function::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    functor(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(f, 57, true));
  EXPECT_EQ(-57, functor(f, 57, false));
}

TEST(call_traits_member_function, functor_non_const_this_ref) {
  member_fn f;
  member_fn const &cf = f;

  mf_traits::member_function functor;

  EXPECT_THROW(functor(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, mf_traits::member_function::call(cf));
  mf_traits::member_function::call(f);
  EXPECT_EQ(5.6, mf_traits::member_function::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    functor(f, s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(f, 57, true));
  EXPECT_EQ(-57, functor(f, 57, false));
}

/////////////////////////////////////////////
// call_traits::member_function::supported //
/////////////////////////////////////////////

struct CTMFS {
  void test_fn() const {}
  int test_fn(int&&) { return 0; }
};

#define CHECK_MEMBER_FUNCTION_SUPPORTED(Expected, ...) \
  do { \
    EXPECT_EQ( \
      Expected, \
      (mf_traits::member_function::template supported<__VA_ARGS__>::value) \
    ); \
  } while (false)

TEST(call_traits_member_function, supported) {
  CHECK_MEMBER_FUNCTION_SUPPORTED(true, CTMFS);
  CHECK_MEMBER_FUNCTION_SUPPORTED(true, CTMFS, int);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS, int const);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS, int &);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS, int const &);
  CHECK_MEMBER_FUNCTION_SUPPORTED(true, CTMFS, int &&);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS, int const &&);
  CHECK_MEMBER_FUNCTION_SUPPORTED(true, CTMFS, double);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS, std::string);
  CHECK_MEMBER_FUNCTION_SUPPORTED(true, CTMFS const);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int const);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int &);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int const &);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int &&);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, int const &&);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, double);
  CHECK_MEMBER_FUNCTION_SUPPORTED(false, CTMFS const, std::string);
}

#undef CHECK_MEMBER_FUNCTION_SUPPORTED

//////////////////////////////////////
// call_traits::static_member::call //
//////////////////////////////////////

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

FATAL_CALL_TRAITS(sm_traits, test_fn);

TEST(call_traits_static_member, call_static_member) {
  EXPECT_THROW(
    sm_traits::static_member<static_fn>::call(0l, 0.0, 0, ""),
    std::exception
  );

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, sm_traits::static_member<static_fn>::call(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    sm_traits::static_member<static_fn>::call(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, sm_traits::static_member<static_fn>::call(2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, sm_traits::static_member<static_fn>::call(57, true));
  EXPECT_EQ(-57, sm_traits::static_member<static_fn>::call(57, false));
}

/////////////////////////////////////////
// call_traits::static_member::functor //
/////////////////////////////////////////

TEST(call_traits_static_member, functor_static_member) {
  sm_traits::static_member<static_fn> functor;

  EXPECT_THROW(functor(0l, 0.0, 0, ""), std::exception);

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    functor(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(57, true));
  EXPECT_EQ(-57, functor(57, false));
}

//////////////////////////////////////
// call_traits::free_function::call //
//////////////////////////////////////

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

FATAL_CALL_TRAITS(ff_traits, test_fn);

TEST(call_traits_free_function, call_free_function) {
  EXPECT_THROW(ff_traits::free_function::call(0l, 0.0, 0, ""), std::exception);

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, ff_traits::free_function::call(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    ff_traits::free_function::call(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<
    int, ff_traits::free_function::call(2, 3, 5)
  > c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, ff_traits::free_function::call(57, true));
  EXPECT_EQ(-57, ff_traits::free_function::call(57, false));
}

/////////////////////////////////////////
// call_traits::free_function::functor //
/////////////////////////////////////////

TEST(call_traits_free_function, functor_free_function) {
  ff_traits::free_function functor;

  EXPECT_THROW(functor(0l, 0.0, 0, ""), std::exception);

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    functor(s1, s2, s3, s4, out)
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(57, true));
  EXPECT_EQ(-57, functor(57, false));
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

TEST(call_operator_traits, const_this_ref_functor) {
  member_op const f;

  call_operator_traits functor;

  EXPECT_THROW(functor(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(f));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (functor(f, s1, s2, s3, s4, out))
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(f, 57, true));
  EXPECT_EQ(-57, functor(f, 57, false));
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

TEST(call_operator_traits, non_const_this_ref_functor) {
  member_op f;
  member_op const &cf = f;

  call_operator_traits functor;

  EXPECT_THROW(functor(f, 0l, 0.0, 0, ""), std::exception);

  EXPECT_EQ(3.1415926, call_operator_traits::call(cf));
  call_operator_traits::call(f);
  EXPECT_EQ(5.6, call_operator_traits::call(cf));

  std::string s1("hello");
  char const *s2 = ", ";
  std::string const s3("world");
  std::string s4("!");
  std::string out("some test string");

  EXPECT_EQ(17, functor(f, 17));

  EXPECT_EQ(
    s1.size() + std::strlen(s2) + s3.size() + s4.size(),
    (functor(f, s1, s2, s3, s4, out))
  );
  EXPECT_EQ("hello, world!", out);

  typedef std::integral_constant<int, functor(f, 2, 3, 5)> c;

  EXPECT_EQ(2 + 3 + 5, c::value);

  EXPECT_EQ(57, functor(f, 57, true));
  EXPECT_EQ(-57, functor(f, 57, false));
}

struct foonctor {
  void operator ()() {}
  void operator ()(int, std::string) {}
};

typedef void(*foonction)();
typedef void(*foonction_is)(int, std::string);

TEST(call_operator_traits, supported) {
  auto const lambda = []() {};
  auto const lambda_is = [](int, std::string) {};

  EXPECT_TRUE((call_operator_traits::supported<foonctor>::value));
  EXPECT_FALSE((call_operator_traits::supported<foonctor, int>::value));
  EXPECT_FALSE((call_operator_traits::supported<foonctor, int, double>::value));
  EXPECT_TRUE((
    call_operator_traits::supported<foonctor, int, std::string>::value
  ));

  EXPECT_TRUE((call_operator_traits::supported<decltype(lambda)>::value));
  EXPECT_FALSE((call_operator_traits::supported<decltype(lambda), int>::value));
  EXPECT_FALSE((
    call_operator_traits::supported<decltype(lambda), int, double>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supported<decltype(lambda), int, std::string>::value
  ));

  EXPECT_FALSE((call_operator_traits::supported<decltype(lambda_is)>::value));
  EXPECT_FALSE((
    call_operator_traits::supported<decltype(lambda_is), int>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supported<decltype(lambda_is), int, double>::value
  ));
  EXPECT_TRUE((
    call_operator_traits::supported<
      decltype(lambda_is), int, std::string
    >::value
  ));

  EXPECT_FALSE((call_operator_traits::supported<foonction>::value));
  EXPECT_FALSE((call_operator_traits::supported<foonction, int>::value));
  EXPECT_FALSE((
    call_operator_traits::supported<foonction, int, double>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supported<foonction, int, std::string>::value
  ));

  EXPECT_FALSE((call_operator_traits::supported<foonction_is>::value));
  EXPECT_FALSE((call_operator_traits::supported<foonction_is, int>::value));
  EXPECT_FALSE((
    call_operator_traits::supported<foonction_is, int, double>::value
  ));
  EXPECT_FALSE((
    call_operator_traits::supported<foonction_is, int, std::string>::value
  ));
}

} // namespace fatal {
