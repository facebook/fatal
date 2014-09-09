/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/reflect_member_function.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace ftl {

struct foo {
  void noncv();
  void c() const;
  void v() volatile;
  void cv() const volatile;
  void vc() volatile const;
};

TEST(reflect_member_function, cv_qualifier_bitwise_and) {
  EXPECT_FALSE(cv_qualifier::none & cv_qualifier::none);
  EXPECT_FALSE(cv_qualifier::none & cv_qualifier::c);
  EXPECT_FALSE(cv_qualifier::none & cv_qualifier::v);
  EXPECT_FALSE(cv_qualifier::none & cv_qualifier::cv);

  EXPECT_FALSE(cv_qualifier::c & cv_qualifier::none);
  EXPECT_TRUE(cv_qualifier::c & cv_qualifier::c);
  EXPECT_FALSE(cv_qualifier::c & cv_qualifier::v);
  EXPECT_TRUE(cv_qualifier::c & cv_qualifier::cv);

  EXPECT_FALSE(cv_qualifier::v & cv_qualifier::none);
  EXPECT_FALSE(cv_qualifier::v & cv_qualifier::c);
  EXPECT_TRUE(cv_qualifier::v & cv_qualifier::v);
  EXPECT_TRUE(cv_qualifier::v & cv_qualifier::cv);

  EXPECT_FALSE(cv_qualifier::cv & cv_qualifier::none);
  EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::c);
  EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::v);
  EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::cv);
}

#define CHECK_IS_NONCV(Expected, Member) \
  do { \
    if (Expected) { \
      EXPECT_TRUE((is_noncv_member_function<decltype(&Member)>::value)); \
    } else { \
      EXPECT_FALSE((is_noncv_member_function<decltype(&Member)>::value)); \
    } \
  } while (false)

TEST(reflect_member_function, is_noncv_member_function) {
  CHECK_IS_NONCV(true, foo::noncv);
  CHECK_IS_NONCV(false, foo::c);
  CHECK_IS_NONCV(false, foo::v);
  CHECK_IS_NONCV(false, foo::cv);
  CHECK_IS_NONCV(false, foo::vc);
}

#define CHECK_IS_CONST(Expected, Member) \
  do { \
    if (Expected) { \
      EXPECT_TRUE((is_const_member_function<decltype(&Member)>::value)); \
    } else { \
      EXPECT_FALSE((is_const_member_function<decltype(&Member)>::value)); \
    } \
  } while (false)

TEST(reflect_member_function, is_const_member_function) {
  CHECK_IS_CONST(false, foo::noncv);
  CHECK_IS_CONST(true, foo::c);
  CHECK_IS_CONST(false, foo::v);
  CHECK_IS_CONST(false, foo::cv);
  CHECK_IS_CONST(false, foo::vc);
}

#define CHECK_IS_VOLATILE(Expected, Member) \
  do { \
    if (Expected) { \
      EXPECT_TRUE((is_volatile_member_function<decltype(&Member)>::value)); \
    } else { \
      EXPECT_FALSE((is_volatile_member_function<decltype(&Member)>::value)); \
    } \
  } while (false)

TEST(reflect_member_function, is_volatile_member_function) {
  CHECK_IS_VOLATILE(false, foo::noncv);
  CHECK_IS_VOLATILE(false, foo::c);
  CHECK_IS_VOLATILE(true, foo::v);
  CHECK_IS_VOLATILE(false, foo::cv);
  CHECK_IS_VOLATILE(false, foo::vc);
}

#define CHECK_IS_CV(Expected, Member) \
  do { \
    if (Expected) { \
      EXPECT_TRUE((is_cv_member_function<decltype(&Member)>::value)); \
    } else { \
      EXPECT_FALSE((is_cv_member_function<decltype(&Member)>::value)); \
    } \
  } while (false)

TEST(reflect_member_function, is_cv_member_function) {
  CHECK_IS_CV(false, foo::noncv);
  CHECK_IS_CV(false, foo::c);
  CHECK_IS_CV(false, foo::v);
  CHECK_IS_CV(true, foo::cv);
  CHECK_IS_CV(true, foo::vc);
}

template <cv_qualifier Expected, typename T>
void check_qualifier() {
  EXPECT_EQ(Expected, (member_function_qualifier<T>::value));
}

#define CHECK_QUALIFIER(Expected, Member) \
  check_qualifier<cv_qualifier::Expected, decltype(&Member)>();

TEST(reflect_member_function, member_function_qualifier) {
  CHECK_QUALIFIER(none, foo::noncv);
  CHECK_QUALIFIER(c, foo::c);
  CHECK_QUALIFIER(v, foo::v);
  CHECK_QUALIFIER(cv, foo::cv);
  CHECK_QUALIFIER(cv, foo::vc);
}

template <int> struct R {};
template <int> struct A {};

struct bar {
  R<1> fn(A<10>, A<11> &&);
  R<2> fn_c(A<20> const &, A<21> &) const;
  R<3> fn_v(A<30> &&) volatile;
  R<4> fn_cv(A<40> *, A<41> const *) const volatile;
  R<5> fn_vc(A<50>, A<51> const *const &) volatile const;
  foo &fn_foo();
  bool operator ==(bar const &) const;
};

template <typename T>
struct gaz {
  int fn(bool, T const &, double *) const;
};

#define CHECK_REFLECT(Class, Fn, Result, Qualifier, ...) \
  do { \
    typedef reflect_member_function<decltype(&Class::Fn)> reflected; \
    expect_same<Class, reflected::owner>(); \
    expect_same<Result, reflected::result>(); \
    EXPECT_EQ(cv_qualifier::Qualifier, reflected::cv::value); \
    expect_same<type_list<__VA_ARGS__>, reflected::args>(); \
  } while (false)

TEST(reflect_member_function, reflect_member_function) {
  CHECK_REFLECT(foo, noncv, void, none);
  CHECK_REFLECT(foo, c, void, c);
  CHECK_REFLECT(foo, v, void, v);
  CHECK_REFLECT(foo, cv, void, cv);
  CHECK_REFLECT(foo, vc, void, cv);

  CHECK_REFLECT(bar, fn, R<1>, none, A<10>, A<11> &&);
  CHECK_REFLECT(bar, fn_c, R<2>, c, A<20> const &, A<21> &);
  CHECK_REFLECT(bar, fn_v, R<3>, v, A<30> &&);
  CHECK_REFLECT(bar, fn_cv, R<4>, cv, A<40> *, A<41> const *);
  CHECK_REFLECT(bar, fn_vc, R<5>, cv, A<50>, A<51> const *const &);
  CHECK_REFLECT(bar, fn_foo, foo &, none);
  CHECK_REFLECT(bar, operator ==, bool, c, bar const &);

  CHECK_REFLECT(gaz<long>, fn, int, c, bool, long const &, double *);
}

} // namespace ftl {
