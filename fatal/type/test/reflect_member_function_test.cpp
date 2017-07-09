/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/reflect_member_function.h>

#include <fatal/test/driver.h>

namespace fatal {

struct foo {
  void noncv();
  void c() const;
  void v() volatile;
  void cv() const volatile;
  void vc() volatile const;
};

template <int> struct R {};
template <int> struct A {};

struct bar {
  R<1> fn(A<10>, A<11> &&);
  R<2> fn_c(A<20> const &, A<21> &) const;
  R<3> fn_v(A<30> &&) volatile;
  R<4> fn_cv(A<40> *, A<41> const *) const volatile;
  R<5> fn_vc(A<50>, A<51> const *const &) volatile const;

  R<1> fn_lr(A<10>, A<11> &&) &;
  R<2> fn_c_lr(A<20> const &, A<21> &) const &;
  R<3> fn_v_lr(A<30> &&) volatile &;
  R<4> fn_cv_lr(A<40> *, A<41> const *) const volatile &;
  R<5> fn_vc_lr(A<50>, A<51> const *const &) volatile const &;

  R<1> fn_rr(A<10>, A<11> &&) &&;
  R<2> fn_c_rr(A<20> const &, A<21> &) const &&;
  R<3> fn_v_rr(A<30> &&) volatile &&;
  R<4> fn_cv_rr(A<40> *, A<41> const *) const volatile &&;
  R<5> fn_vc_rr(A<50>, A<51> const *const &) volatile const &&;

  foo &fn_foo();

  bool operator ==(bar const &) const;
};

template <typename T>
struct gaz {
  int fn(bool, T const &, double *) const;
};

# define CHECK_REFLECT(Class, Fn, Result, CV, CVQ, Ref, RefQ, ...) \
    do { \
      using reflected = reflect_member_function<decltype(&Class::Fn)>; \
      using expected = Result(Class::*)(__VA_ARGS__) CVQ RefQ; \
      FATAL_EXPECT_SAME<Class, reflected::owner>(); \
      FATAL_EXPECT_SAME<Result, reflected::result>(); \
      FATAL_EXPECT_SAME<expected, reflected::pointer>(); \
      FATAL_EXPECT_EQ(ref_qualifier::Ref, reflected::ref::value); \
      FATAL_EXPECT_EQ(cv_qualifier::CV, reflected::cv::value); \
      FATAL_EXPECT_SAME<type_list<__VA_ARGS__>, reflected::args>(); \
    } while (false)

FATAL_TEST(reflect_member_function, reflect_member_function) {
  CHECK_REFLECT(foo, noncv, void, none, , none, );
  CHECK_REFLECT(foo, c, void, c, const, none, );
  CHECK_REFLECT(foo, v, void, v, volatile, none, );
  CHECK_REFLECT(foo, cv, void, cv, const volatile, none, );
  CHECK_REFLECT(foo, vc, void, cv, const volatile, none, );

  CHECK_REFLECT(bar, fn, R<1>, none, , none, , A<10>, A<11> &&);
  CHECK_REFLECT(bar, fn_c, R<2>, c, const, none, , A<20> const &, A<21> &);
  CHECK_REFLECT(bar, fn_v, R<3>, v, volatile, none, , A<30> &&);
  CHECK_REFLECT(
    bar, fn_cv, R<4>, cv, const volatile, none, , A<40> *, A<41> const *
  );
  CHECK_REFLECT(
    bar, fn_vc, R<5>, cv, const volatile, none, , A<50>, A<51> const *const &
  );

  CHECK_REFLECT(bar, fn_lr, R<1>, none, , lvalue, &, A<10>, A<11> &&);
  CHECK_REFLECT(
    bar, fn_c_lr, R<2>, c, const, lvalue, &, A<20> const &, A<21> &
  );
  CHECK_REFLECT(bar, fn_v_lr, R<3>, v, volatile, lvalue, &, A<30> &&);
  CHECK_REFLECT(
    bar, fn_cv_lr, R<4>, cv, const volatile, lvalue, &, A<40> *, A<41> const *
  );
  CHECK_REFLECT(
    bar, fn_vc_lr, R<5>,
    cv, const volatile, lvalue, &, A<50>, A<51> const *const &
  );

  CHECK_REFLECT(bar, fn_rr, R<1>, none, , rvalue, &&, A<10>, A<11> &&);
  CHECK_REFLECT(
    bar, fn_c_rr, R<2>, c, const, rvalue, &&, A<20> const &, A<21> &
  );
  CHECK_REFLECT(bar, fn_v_rr, R<3>, v, volatile, rvalue, &&, A<30> &&);
  CHECK_REFLECT(
    bar, fn_cv_rr, R<4>, cv, const volatile, rvalue, &&, A<40> *, A<41> const *
  );
  CHECK_REFLECT(
    bar, fn_vc_rr, R<5>,
    cv, const volatile, rvalue, &&, A<50>, A<51> const *const &
  );

  CHECK_REFLECT(bar, fn_foo, foo &, none, , none, );
  CHECK_REFLECT(bar, operator ==, bool, c, const, none, , bar const &);

  CHECK_REFLECT(
    gaz<long>, fn, int, c, const, none, , bool, long const &, double *
  );
}

#undef CHECK_REFLECT

} // namespace fatal {
