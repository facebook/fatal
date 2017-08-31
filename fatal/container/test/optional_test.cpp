/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/optional.h>

#include <fatal/test/driver.h>
#include <fatal/test/ref_counter.h>

#include <string>
#include <type_traits>
#include <utility>

namespace fatal {

#define FATAL_IMPL_CALL(Fn) \
  do { \
    Fn(int, 10, 99); \
    Fn(double, 5.6, 9.1); \
    Fn(bool, true, false); \
    Fn(std::string, "hello, world!", "this is a test"); \
  } while (false)

//////////////////
// default_ctor //
//////////////////

#define FATAL_IMPL_DEFAULT_CTOR(Type, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
  } while (false)

FATAL_TEST(optional, default_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_DEFAULT_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    optional<refc> rc2;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

//////////////////
// emplace_ctor //
//////////////////

#define FATAL_IMPL_EMPLACE_CTOR(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_EXPECT_FALSE(!o); \
    FATAL_EXPECT_TRUE(o); \
    FATAL_EXPECT_FALSE(o.empty()); \
  } while (false)

FATAL_TEST(optional, emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPLACE_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    optional<refc> rc2(5.6);
    FATAL_EXPECT_EQ(2, refc::alive());
    FATAL_EXPECT_EQ(2, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

////////////////////////////////
// const_dereference_operator //
////////////////////////////////

#define FATAL_IMPL_CONST_DEREFERENCE_OPERATOR(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(*o)>(); \
    FATAL_EXPECT_EQ(Value1, *o); \
  } while (false)

FATAL_TEST(optional, const_dereference_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_DEREFERENCE_OPERATOR);
}

//////////////////////////
// dereference_operator //
//////////////////////////

#define FATAL_IMPL_DEREFERENCE_OPERATOR(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type &, decltype(*o)>(); \
    FATAL_EXPECT_EQ(Value1, *o); \
  } while (false)

FATAL_TEST(optional, dereference_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_DEREFERENCE_OPERATOR);
}

//////////////////////////
// const_arrow_operator //
//////////////////////////

#define FATAL_IMPL_CONST_ARROW_OPERATOR(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.operator ->())>(); \
    FATAL_ASSERT_NOT_NULL(o.operator ->()); \
    FATAL_EXPECT_EQ(Value1, *(o.operator ->())); \
  } while (false)

FATAL_TEST(optional, const_arrow_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_ARROW_OPERATOR);
}

////////////////////
// arrow_operator //
////////////////////

#define FATAL_IMPL_ARROW_OPERATOR(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type *, decltype(o.operator ->())>(); \
    FATAL_ASSERT_NOT_NULL(o.operator ->()); \
    FATAL_EXPECT_EQ(Value1, *(o.operator ->())); \
  } while (false)

FATAL_TEST(optional, arrow_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_ARROW_OPERATOR);
}

////////////////
// const_cref //
////////////////

#define FATAL_IMPL_CONST_CREF(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.cref())>(); \
    FATAL_EXPECT_EQ(Value1, o.cref()); \
  } while (false)

FATAL_TEST(optional, const_cref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_CREF);
}

//////////
// cref //
//////////

#define FATAL_IMPL_CREF(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.cref())>(); \
    FATAL_EXPECT_EQ(Value1, o.cref()); \
  } while (false)

FATAL_TEST(optional, cref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CREF);
}

///////////////
// const_ref //
///////////////

#define FATAL_IMPL_CONST_REF(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.ref())>(); \
    FATAL_EXPECT_EQ(Value1, o.ref()); \
  } while (false)

FATAL_TEST(optional, const_ref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_REF);
}

/////////
// ref //
/////////

#define FATAL_IMPL_REF(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type &, decltype(o.ref())>(); \
    FATAL_EXPECT_EQ(Value1, o.ref()); \
  } while (false)

FATAL_TEST(optional, ref) {
  FATAL_IMPL_CALL(FATAL_IMPL_REF);
}

////////////////
// const_cptr //
////////////////

#define FATAL_IMPL_CONST_CPTR(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.cptr())>(); \
    FATAL_ASSERT_NOT_NULL(o.cptr()); \
    FATAL_EXPECT_EQ(Value1, *o.cptr()); \
  } while (false)

FATAL_TEST(optional, const_cptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_CPTR);
}

//////////
// cptr //
//////////

#define FATAL_IMPL_CPTR(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.cptr())>(); \
    FATAL_ASSERT_NOT_NULL(o.cptr()); \
    FATAL_EXPECT_EQ(Value1, *o.cptr()); \
  } while (false)

FATAL_TEST(optional, cptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CPTR);
}

///////////////
// const_ptr //
///////////////

#define FATAL_IMPL_CONST_PTR(Type, Value1, ...) \
  do { \
    optional<Type> const o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.ptr())>(); \
    FATAL_ASSERT_NOT_NULL(o.ptr()); \
    FATAL_EXPECT_EQ(Value1, *o.ptr()); \
  } while (false)

FATAL_TEST(optional, const_ptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_PTR);
}

/////////
// ptr //
/////////

#define FATAL_IMPL_PTR(Type, Value1, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type *, decltype(o.ptr())>(); \
    FATAL_ASSERT_NOT_NULL(o.ptr()); \
    FATAL_EXPECT_EQ(Value1, *o.ptr()); \
  } while (false)

FATAL_TEST(optional, ptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_PTR);
}

///////////////////
// const_try_get //
///////////////////

#define FATAL_IMPL_CONST_TRY_GET(Type, Value1, ...) \
  do { \
    optional<Type> const e; \
    FATAL_EXPECT_NULL(e.try_get()); \
    optional<Type> const o(Value1); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.try_get())>(); \
    FATAL_ASSERT_NOT_NULL(o.try_get()); \
    FATAL_EXPECT_EQ(Value1, *o.try_get()); \
  } while (false)

FATAL_TEST(optional, const_try_get) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_TRY_GET);
}

/////////////
// try_get //
/////////////

#define FATAL_IMPL_TRY_GET(Type, Value1, ...) \
  do { \
    optional<Type> e; \
    FATAL_EXPECT_NULL(e.try_get()); \
    optional<Type> o(Value1); \
    FATAL_EXPECT_SAME<Type *, decltype(o.try_get())>(); \
    FATAL_ASSERT_NOT_NULL(o.try_get()); \
    FATAL_EXPECT_EQ(Value1, *o.try_get()); \
  } while (false)

FATAL_TEST(optional, try_get) {
  FATAL_IMPL_CALL(FATAL_IMPL_TRY_GET);
}

/////////////////////
// empty_copy_ctor //
/////////////////////

#define FATAL_IMPL_EMPTY_COPY_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    optional<Type> copy(o); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    FATAL_EXPECT_TRUE(!copy); \
    FATAL_EXPECT_FALSE(copy); \
    FATAL_EXPECT_TRUE(copy.empty()); \
  } while (false)

FATAL_TEST(optional, empty_copy_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_COPY_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    optional<refc> rc2(rc1);
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////
// copy_ctor //
///////////////

#define FATAL_IMPL_COPY_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    optional<Type> copy(o); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_ASSERT_FALSE(copy.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    FATAL_EXPECT_EQ(Value1, *copy); \
  } while (false)

FATAL_TEST(optional, copy_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    optional<refc> rc2(rc1);
    FATAL_EXPECT_EQ(2, refc::alive());
    FATAL_EXPECT_EQ(2, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////////////
// empty_move_ctor //
/////////////////////

#define FATAL_IMPL_EMPTY_MOVE_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    optional<Type> thief(std::move(o)); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    FATAL_EXPECT_TRUE(!thief); \
    FATAL_EXPECT_FALSE(thief); \
    FATAL_EXPECT_TRUE(thief.empty()); \
  } while (false)

FATAL_TEST(optional, empty_move_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_MOVE_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    optional<refc> rc2(std::move(rc1));
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////
// move_ctor //
///////////////

#define FATAL_IMPL_MOVE_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    optional<Type> thief(std::move(o)); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    FATAL_ASSERT_FALSE(thief.empty()); \
    FATAL_EXPECT_EQ(Value1, *thief); \
  } while (false)

FATAL_TEST(optional, move_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    optional<refc> rc2(std::move(rc1));
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////////////
// copy_emplace_ctor //
///////////////////////

#define FATAL_IMPL_COPY_EMPLACE_CTOR(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o(value); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    FATAL_EXPECT_EQ(Value1, value); \
  } while (false)

FATAL_TEST(optional, copy_emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_EMPLACE_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    auto n = 10;
    optional<refc> rc(n);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////////////
// move_emplace_ctor //
///////////////////////

#define FATAL_IMPL_MOVE_EMPLACE_CTOR(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o(std::move(value)); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    if (!std::is_scalar<Type>::value) { \
      FATAL_EXPECT_EQ(Type(), value); \
    } \
  } while (false)

FATAL_TEST(optional, move_emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_EMPLACE_CTOR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    auto n = 10;
    optional<refc> rc(std::move(n));
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

//////////////////
// copy_emplace //
//////////////////

#define FATAL_IMPL_COPY_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    o.emplace(value); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    FATAL_EXPECT_EQ(Value1, value); \
  } while (false)

FATAL_TEST(optional, copy_emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_EMPLACE);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    auto n = 10;
    optional<refc> rc;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc.emplace(n);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

//////////////////
// move_emplace //
//////////////////

#define FATAL_IMPL_MOVE_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    o.emplace(std::move(value)); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    if (!std::is_scalar<Type>::value) { \
      FATAL_EXPECT_EQ(Type(), value); \
    } \
  } while (false)

FATAL_TEST(optional, move_emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_EMPLACE);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    auto n = 10;
    optional<refc> rc;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc.emplace(std::move(n));
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////
// emplace //
/////////////

#define FATAL_IMPL_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    o.emplace(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
  } while (false)

FATAL_TEST(optional, emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPLACE);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc.emplace(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////////
// empty_clear //
/////////////////

#define FATAL_IMPL_EMPTY_CLEAR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    o.clear(); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
  } while (false)

FATAL_TEST(optional, empty_clear) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_CLEAR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc.clear();
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////
// clear //
///////////

#define FATAL_IMPL_CLEAR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_EXPECT_FALSE(!o); \
    FATAL_EXPECT_TRUE(o); \
    FATAL_EXPECT_FALSE(o.empty()); \
    o.clear(); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    o.clear(); \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
  } while (false)

FATAL_TEST(optional, clear) {
  FATAL_IMPL_CALL(FATAL_IMPL_CLEAR);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    rc.clear();
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////////////////
// empty_copy_assignment //
///////////////////////////

#define FATAL_IMPL_EMPTY_COPY_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    FATAL_EXPECT_TRUE(!o); \
    FATAL_EXPECT_FALSE(o); \
    FATAL_EXPECT_TRUE(o.empty()); \
    { \
      optional<Type> copy; \
      FATAL_EXPECT_TRUE(!copy); \
      FATAL_EXPECT_FALSE(copy); \
      FATAL_EXPECT_TRUE(copy.empty()); \
      copy = o; \
      FATAL_EXPECT_TRUE(!copy); \
      FATAL_EXPECT_FALSE(copy); \
      FATAL_EXPECT_TRUE(copy.empty()); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> copy(Value2); \
      FATAL_ASSERT_FALSE(copy.empty()); \
      FATAL_EXPECT_EQ(Value2, *copy); \
      copy = o; \
      FATAL_EXPECT_TRUE(!copy); \
      FATAL_EXPECT_FALSE(copy); \
      FATAL_EXPECT_TRUE(copy.empty()); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

FATAL_TEST(optional, empty_copy_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_COPY_ASSIGNMENT);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());

    {
      optional<refc> rc2;
      FATAL_EXPECT_EQ(0, refc::alive());
      FATAL_EXPECT_EQ(0, refc::valid());

      rc2 = rc1;
      FATAL_EXPECT_EQ(0, refc::alive());
      FATAL_EXPECT_EQ(0, refc::valid());
    }

    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());

    {
      optional<refc> rc2(5.6);
      FATAL_EXPECT_EQ(1, refc::alive());
      FATAL_EXPECT_EQ(1, refc::valid());

      rc2 = rc1;
      FATAL_EXPECT_EQ(0, refc::alive());
      FATAL_EXPECT_EQ(0, refc::valid());
    }

    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////////////
// copy_assignment //
/////////////////////

#define FATAL_IMPL_COPY_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    { \
      optional<Type> copy; \
      FATAL_EXPECT_TRUE(!copy); \
      FATAL_EXPECT_FALSE(copy); \
      FATAL_EXPECT_TRUE(copy.empty()); \
      copy = o; \
      FATAL_ASSERT_FALSE(copy.empty()); \
      FATAL_EXPECT_EQ(Value1, *copy); \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
    } \
    { \
      optional<Type> copy(Value2); \
      FATAL_ASSERT_FALSE(copy.empty()); \
      FATAL_EXPECT_EQ(Value2, *copy); \
      copy = o; \
      FATAL_ASSERT_FALSE(copy.empty()); \
      FATAL_EXPECT_EQ(Value1, *copy); \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
    } \
  } while (false)

FATAL_TEST(optional, copy_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_ASSIGNMENT);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());

    {
      optional<refc> rc2;
      FATAL_EXPECT_EQ(1, refc::alive());
      FATAL_EXPECT_EQ(1, refc::valid());

      rc2 = rc1;
      FATAL_EXPECT_EQ(2, refc::alive());
      FATAL_EXPECT_EQ(2, refc::valid());
    }

    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());

    {
      optional<refc> rc2(5.6);
      FATAL_EXPECT_EQ(2, refc::alive());
      FATAL_EXPECT_EQ(2, refc::valid());

      rc2 = rc1;
      FATAL_EXPECT_EQ(2, refc::alive());
      FATAL_EXPECT_EQ(2, refc::valid());
    }

    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

///////////////////////////
// empty_move_assignment //
///////////////////////////

#define FATAL_IMPL_EMPTY_MOVE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o; \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
      optional<Type> thief; \
      FATAL_EXPECT_TRUE(!thief); \
      FATAL_EXPECT_FALSE(thief); \
      FATAL_EXPECT_TRUE(thief.empty()); \
      thief = std::move(o); \
      FATAL_EXPECT_TRUE(!thief); \
      FATAL_EXPECT_FALSE(thief); \
      FATAL_EXPECT_TRUE(thief.empty()); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> o; \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
      optional<Type> thief(Value2); \
      FATAL_ASSERT_FALSE(thief.empty()); \
      FATAL_EXPECT_EQ(Value2, *thief); \
      thief = std::move(o); \
      FATAL_EXPECT_TRUE(!thief); \
      FATAL_EXPECT_FALSE(thief); \
      FATAL_EXPECT_TRUE(thief.empty()); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

FATAL_TEST(optional, empty_move_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_MOVE_ASSIGNMENT);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    optional<refc> rc2;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc2 = std::move(rc1);
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());

  {
    optional<refc> rc1;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    optional<refc> rc2(5.6);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    rc2 = std::move(rc1);
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////////////
// move_assignment //
/////////////////////

#define FATAL_IMPL_MOVE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o(Value1); \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
      optional<Type> thief; \
      FATAL_EXPECT_TRUE(!thief); \
      FATAL_EXPECT_FALSE(thief); \
      FATAL_EXPECT_TRUE(thief.empty()); \
      thief = std::move(o); \
      FATAL_ASSERT_FALSE(thief.empty()); \
      FATAL_EXPECT_EQ(Value1, *thief); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> o(Value1); \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
      optional<Type> thief(Value2); \
      FATAL_ASSERT_FALSE(thief.empty()); \
      FATAL_EXPECT_EQ(Value2, *thief); \
      thief = std::move(o); \
      FATAL_ASSERT_FALSE(thief.empty()); \
      FATAL_EXPECT_EQ(Value1, *thief); \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

FATAL_TEST(optional, move_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_ASSIGNMENT);

  using refc = ref_counter<>;

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    optional<refc> rc2;
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    rc2 = std::move(rc1);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());

  {
    optional<refc> rc1(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    optional<refc> rc2(5.6);
    FATAL_EXPECT_EQ(2, refc::alive());
    FATAL_EXPECT_EQ(2, refc::valid());
    rc2 = std::move(rc1);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

//////////////////////
// value_assignment //
//////////////////////

#define FATAL_IMPL_VALUE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o; \
      FATAL_EXPECT_TRUE(!o); \
      FATAL_EXPECT_FALSE(o); \
      FATAL_EXPECT_TRUE(o.empty()); \
      o = Value1; \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
      o = Value2; \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value2, *o); \
    } \
    { \
      optional<Type> o(Value1); \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value1, *o); \
      o = Value2; \
      FATAL_ASSERT_FALSE(o.empty()); \
      FATAL_EXPECT_EQ(Value2, *o); \
    } \
  } while (false)

FATAL_TEST(optional, value_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_VALUE_ASSIGNMENT);

  using refc = ref_counter<>;
  refc::guard guard;

  {
    optional<refc> rc;
    FATAL_EXPECT_EQ(0, refc::alive());
    FATAL_EXPECT_EQ(0, refc::valid());
    rc = 5.6;
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());

  {
    optional<refc> rc(10);
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
    rc = 5.6;
    FATAL_EXPECT_EQ(1, refc::alive());
    FATAL_EXPECT_EQ(1, refc::valid());
  }

  FATAL_EXPECT_EQ(0, refc::alive());
  FATAL_EXPECT_EQ(0, refc::valid());
}

/////////////////////
// operator_equals //
/////////////////////

#define FATAL_IMPL_OPERATOR_EQUALS(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o1(Value1); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    FATAL_EXPECT_FALSE(o1 == empty); \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    optional<Type> o2(Value2); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_FALSE(o2 == empty); \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_FALSE(o1 == o2); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_FALSE(o2 == o1); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    optional<Type> o11(Value1); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    FATAL_EXPECT_TRUE(o1 == o11); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    FATAL_EXPECT_FALSE(o2 == o11); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    optional<Type> o22(Value2); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
    \
    FATAL_EXPECT_FALSE(o1 == o22); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
    \
    FATAL_EXPECT_TRUE(o2 == o22); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
  } while (false)

FATAL_TEST(optional, operator_equals) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_EQUALS);
}

///////////////////////////
// operator_equals_value //
///////////////////////////

#define FATAL_IMPL_OPERATOR_EQUALS_VALUE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    FATAL_EXPECT_TRUE(*o == Value1); \
    FATAL_EXPECT_FALSE(*o == Value2); \
  } while (false)

FATAL_TEST(optional, operator_equals_value) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_EQUALS_VALUE);
}

/////////////////////////
// operator_not_equals //
/////////////////////////

#define FATAL_IMPL_OPERATOR_NOT_EQUALS(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o1(Value1); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    FATAL_EXPECT_TRUE(o1 != empty); \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    \
    optional<Type> o2(Value2); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_TRUE(o2 != empty); \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_TRUE(o1 != o2); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    FATAL_EXPECT_TRUE(o2 != o1); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    \
    optional<Type> o11(Value1); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    FATAL_EXPECT_FALSE(o1 != o11); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    FATAL_EXPECT_TRUE(o2 != o11); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    FATAL_ASSERT_FALSE(o11.empty()); \
    FATAL_EXPECT_EQ(Value1, *o11); \
    \
    optional<Type> o22(Value2); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
    \
    FATAL_EXPECT_TRUE(o1 != o22); \
    FATAL_ASSERT_FALSE(o1.empty()); \
    FATAL_EXPECT_EQ(Value1, *o1); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
    \
    FATAL_EXPECT_FALSE(o2 != o22); \
    FATAL_ASSERT_FALSE(o2.empty()); \
    FATAL_EXPECT_EQ(Value2, *o2); \
    FATAL_ASSERT_FALSE(o22.empty()); \
    FATAL_EXPECT_EQ(Value2, *o22); \
  } while (false)

FATAL_TEST(optional, operator_not_equals) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_NOT_EQUALS);
}

///////////////////////////////
// operator_not_equals_value //
///////////////////////////////

#define FATAL_IMPL_OPERATOR_NOT_EQUALS_VALUE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    FATAL_EXPECT_TRUE(!empty); \
    FATAL_EXPECT_FALSE(empty); \
    FATAL_EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o(Value1); \
    FATAL_ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_EQ(Value1, *o); \
    FATAL_EXPECT_FALSE(*o != Value1); \
    FATAL_EXPECT_TRUE(*o != Value2); \
  } while (false)

FATAL_TEST(optional, operator_not_equals_value) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_NOT_EQUALS_VALUE);
}

} // namespace fatal {
