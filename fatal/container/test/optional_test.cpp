/*
 *  Copyright (c) 2015, Facebook, Inc.
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
    EXPECT_TRUE(o.empty()); \
  } while (false)

TEST(optional, default_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_DEFAULT_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    optional<refc> rc2;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

//////////////////
// emplace_ctor //
//////////////////

#define FATAL_IMPL_EMPLACE_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    EXPECT_FALSE(o.empty()); \
  } while (false)

TEST(optional, emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPLACE_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    optional<refc> rc2(5.6);
    EXPECT_EQ(2, ref_counter::count());
    EXPECT_EQ(2, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

////////////////////////////////
// const_dereference_operator //
////////////////////////////////

#define FATAL_IMPL_CONST_DEREFERENCE_OPERATOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(*o)>(); \
    EXPECT_EQ(Value1, *o); \
  } while (false)

TEST(optional, const_dereference_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_DEREFERENCE_OPERATOR);
}

//////////////////////////
// dereference_operator //
//////////////////////////

#define FATAL_IMPL_DEREFERENCE_OPERATOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type &, decltype(*o)>(); \
    EXPECT_EQ(Value1, *o); \
  } while (false)

TEST(optional, dereference_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_DEREFERENCE_OPERATOR);
}

//////////////////////////
// const_arrow_operator //
//////////////////////////

#define FATAL_IMPL_CONST_ARROW_OPERATOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.operator ->())>(); \
    ASSERT_NE(nullptr, o.operator ->()); \
    EXPECT_EQ(Value1, *(o.operator ->())); \
  } while (false)

TEST(optional, const_arrow_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_ARROW_OPERATOR);
}

////////////////////
// arrow_operator //
////////////////////

#define FATAL_IMPL_ARROW_OPERATOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type *, decltype(o.operator ->())>(); \
    ASSERT_NE(nullptr, o.operator ->()); \
    EXPECT_EQ(Value1, *(o.operator ->())); \
  } while (false)

TEST(optional, arrow_operator) {
  FATAL_IMPL_CALL(FATAL_IMPL_ARROW_OPERATOR);
}

////////////////
// const_cref //
////////////////

#define FATAL_IMPL_CONST_CREF(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.cref())>(); \
    EXPECT_EQ(Value1, o.cref()); \
  } while (false)

TEST(optional, const_cref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_CREF);
}

//////////
// cref //
//////////

#define FATAL_IMPL_CREF(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.cref())>(); \
    EXPECT_EQ(Value1, o.cref()); \
  } while (false)

TEST(optional, cref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CREF);
}

///////////////
// const_ref //
///////////////

#define FATAL_IMPL_CONST_REF(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const &, decltype(o.ref())>(); \
    EXPECT_EQ(Value1, o.ref()); \
  } while (false)

TEST(optional, const_ref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_REF);
}

/////////
// ref //
/////////

#define FATAL_IMPL_REF(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type &, decltype(o.ref())>(); \
    EXPECT_EQ(Value1, o.ref()); \
  } while (false)

TEST(optional, ref) {
  FATAL_IMPL_CALL(FATAL_IMPL_REF);
}

////////////////
// const_cptr //
////////////////

#define FATAL_IMPL_CONST_CPTR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.cptr())>(); \
    ASSERT_NE(nullptr, o.cptr()); \
    EXPECT_EQ(Value1, *o.cptr()); \
  } while (false)

TEST(optional, const_cptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_CPTR);
}

//////////
// cptr //
//////////

#define FATAL_IMPL_CPTR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.cptr())>(); \
    ASSERT_NE(nullptr, o.cptr()); \
    EXPECT_EQ(Value1, *o.cptr()); \
  } while (false)

TEST(optional, cptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CPTR);
}

///////////////
// const_ptr //
///////////////

#define FATAL_IMPL_CONST_PTR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> const o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type const *, decltype(o.ptr())>(); \
    ASSERT_NE(nullptr, o.ptr()); \
    EXPECT_EQ(Value1, *o.ptr()); \
  } while (false)

TEST(optional, const_ptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CONST_PTR);
}

/////////
// ptr //
/////////

#define FATAL_IMPL_PTR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    FATAL_EXPECT_SAME<Type *, decltype(o.ptr())>(); \
    ASSERT_NE(nullptr, o.ptr()); \
    EXPECT_EQ(Value1, *o.ptr()); \
  } while (false)

TEST(optional, ptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_PTR);
}

/////////////////////
// empty_copy_ctor //
/////////////////////

#define FATAL_IMPL_EMPTY_COPY_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    optional<Type> copy(o); \
    EXPECT_TRUE(o.empty()); \
    EXPECT_TRUE(copy.empty()); \
  } while (false)

TEST(optional, empty_copy_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_COPY_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    optional<refc> rc2(rc1);
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////
// copy_ctor //
///////////////

#define FATAL_IMPL_COPY_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    optional<Type> copy(o); \
    ASSERT_FALSE(o.empty()); \
    ASSERT_FALSE(copy.empty()); \
    EXPECT_EQ(Value1, *o); \
    EXPECT_EQ(Value1, *copy); \
  } while (false)

TEST(optional, copy_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    optional<refc> rc2(rc1);
    EXPECT_EQ(2, ref_counter::count());
    EXPECT_EQ(2, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////////////
// empty_move_ctor //
/////////////////////

#define FATAL_IMPL_EMPTY_MOVE_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    optional<Type> thief(std::move(o)); \
    EXPECT_TRUE(o.empty()); \
    EXPECT_TRUE(thief.empty()); \
  } while (false)

TEST(optional, empty_move_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_MOVE_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    optional<refc> rc2(std::move(rc1));
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////
// move_ctor //
///////////////

#define FATAL_IMPL_MOVE_CTOR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    optional<Type> thief(std::move(o)); \
    EXPECT_TRUE(o.empty()); \
    ASSERT_FALSE(thief.empty()); \
    EXPECT_EQ(Value1, *thief); \
  } while (false)

TEST(optional, move_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    optional<refc> rc2(std::move(rc1));
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////////////
// copy_emplace_ctor //
///////////////////////

#define FATAL_IMPL_COPY_EMPLACE_CTOR(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o(value); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    EXPECT_EQ(Value1, value); \
  } while (false)

TEST(optional, copy_emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_EMPLACE_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    auto n = 10;
    optional<refc> rc(n);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////////////
// move_emplace_ctor //
///////////////////////

#define FATAL_IMPL_MOVE_EMPLACE_CTOR(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o(std::move(value)); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    if (!std::is_scalar<Type>::value) { \
      EXPECT_EQ(Type(), value); \
    } \
  } while (false)

TEST(optional, move_emplace_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_EMPLACE_CTOR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    auto n = 10;
    optional<refc> rc(std::move(n));
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

//////////////////
// copy_emplace //
//////////////////

#define FATAL_IMPL_COPY_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    o.emplace(value); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    EXPECT_EQ(Value1, value); \
  } while (false)

TEST(optional, copy_emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_EMPLACE);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    auto n = 10;
    optional<refc> rc;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc.emplace(n);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

//////////////////
// move_emplace //
//////////////////

#define FATAL_IMPL_MOVE_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    Type value(Value1); \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    o.emplace(std::move(value)); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    if (!std::is_scalar<Type>::value) { \
      EXPECT_EQ(Type(), value); \
    } \
  } while (false)

TEST(optional, move_emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_EMPLACE);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    auto n = 10;
    optional<refc> rc;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc.emplace(std::move(n));
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////
// emplace //
/////////////

#define FATAL_IMPL_EMPLACE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    o.emplace(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
  } while (false)

TEST(optional, emplace) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPLACE);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc.emplace(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////////
// empty_clear //
/////////////////

#define FATAL_IMPL_EMPTY_CLEAR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    o.clear(); \
    EXPECT_TRUE(o.empty()); \
  } while (false)

TEST(optional, empty_clear) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_CLEAR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc.clear();
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////
// clear //
///////////

#define FATAL_IMPL_CLEAR(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    EXPECT_FALSE(o.empty()); \
    o.clear(); \
    EXPECT_TRUE(o.empty()); \
    o.clear(); \
    EXPECT_TRUE(o.empty()); \
  } while (false)

TEST(optional, clear) {
  FATAL_IMPL_CALL(FATAL_IMPL_CLEAR);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    rc.clear();
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////////////////
// empty_copy_assignment //
///////////////////////////

#define FATAL_IMPL_EMPTY_COPY_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o; \
    EXPECT_TRUE(o.empty()); \
    { \
      optional<Type> copy; \
      EXPECT_TRUE(copy.empty()); \
      copy = o; \
      EXPECT_TRUE(copy.empty()); \
      EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> copy(Value2); \
      ASSERT_FALSE(copy.empty()); \
      EXPECT_EQ(Value2, *copy); \
      copy = o; \
      EXPECT_TRUE(copy.empty()); \
      EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

TEST(optional, empty_copy_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_COPY_ASSIGNMENT);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());

    {
      optional<refc> rc2;
      EXPECT_EQ(0, ref_counter::count());
      EXPECT_EQ(0, ref_counter::valid());

      rc2 = rc1;
      EXPECT_EQ(0, ref_counter::count());
      EXPECT_EQ(0, ref_counter::valid());
    }

    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());

    {
      optional<refc> rc2(5.6);
      EXPECT_EQ(1, ref_counter::count());
      EXPECT_EQ(1, ref_counter::valid());

      rc2 = rc1;
      EXPECT_EQ(0, ref_counter::count());
      EXPECT_EQ(0, ref_counter::valid());
    }

    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////////////
// copy_assignment //
/////////////////////

#define FATAL_IMPL_COPY_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    { \
      optional<Type> copy; \
      EXPECT_TRUE(copy.empty()); \
      copy = o; \
      ASSERT_FALSE(copy.empty()); \
      EXPECT_EQ(Value1, *copy); \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
    } \
    { \
      optional<Type> copy(Value2); \
      ASSERT_FALSE(copy.empty()); \
      EXPECT_EQ(Value2, *copy); \
      copy = o; \
      ASSERT_FALSE(copy.empty()); \
      EXPECT_EQ(Value1, *copy); \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
    } \
  } while (false)

TEST(optional, copy_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_COPY_ASSIGNMENT);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());

    {
      optional<refc> rc2;
      EXPECT_EQ(1, ref_counter::count());
      EXPECT_EQ(1, ref_counter::valid());

      rc2 = rc1;
      EXPECT_EQ(2, ref_counter::count());
      EXPECT_EQ(2, ref_counter::valid());
    }

    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());

    {
      optional<refc> rc2(5.6);
      EXPECT_EQ(2, ref_counter::count());
      EXPECT_EQ(2, ref_counter::valid());

      rc2 = rc1;
      EXPECT_EQ(2, ref_counter::count());
      EXPECT_EQ(2, ref_counter::valid());
    }

    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

///////////////////////////
// empty_move_assignment //
///////////////////////////

#define FATAL_IMPL_EMPTY_MOVE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o; \
      EXPECT_TRUE(o.empty()); \
      optional<Type> thief; \
      EXPECT_TRUE(thief.empty()); \
      thief = std::move(o); \
      EXPECT_TRUE(thief.empty()); \
      EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> o; \
      EXPECT_TRUE(o.empty()); \
      optional<Type> thief(Value2); \
      ASSERT_FALSE(thief.empty()); \
      EXPECT_EQ(Value2, *thief); \
      thief = std::move(o); \
      EXPECT_TRUE(thief.empty()); \
      EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

TEST(optional, empty_move_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_EMPTY_MOVE_ASSIGNMENT);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    optional<refc> rc2;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc2 = std::move(rc1);
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());

  {
    optional<refc> rc1;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    optional<refc> rc2(5.6);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    rc2 = std::move(rc1);
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////////////
// move_assignment //
/////////////////////

#define FATAL_IMPL_MOVE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o(Value1); \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
      optional<Type> thief; \
      EXPECT_TRUE(thief.empty()); \
      thief = std::move(o); \
      ASSERT_FALSE(thief.empty()); \
      EXPECT_EQ(Value1, *thief); \
      EXPECT_TRUE(o.empty()); \
    } \
    { \
      optional<Type> o(Value1); \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
      optional<Type> thief(Value2); \
      ASSERT_FALSE(thief.empty()); \
      EXPECT_EQ(Value2, *thief); \
      thief = std::move(o); \
      ASSERT_FALSE(thief.empty()); \
      EXPECT_EQ(Value1, *thief); \
      EXPECT_TRUE(o.empty()); \
    } \
  } while (false)

TEST(optional, move_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_MOVE_ASSIGNMENT);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    optional<refc> rc2;
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    rc2 = std::move(rc1);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());

  {
    optional<refc> rc1(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    optional<refc> rc2(5.6);
    EXPECT_EQ(2, ref_counter::count());
    EXPECT_EQ(2, ref_counter::valid());
    rc2 = std::move(rc1);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

//////////////////////
// value_assignment //
//////////////////////

#define FATAL_IMPL_VALUE_ASSIGNMENT(Type, Value1, Value2, ...) \
  do { \
    { \
      optional<Type> o; \
      EXPECT_TRUE(o.empty()); \
      o = Value1; \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
      o = Value2; \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value2, *o); \
    } \
    { \
      optional<Type> o(Value1); \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value1, *o); \
      o = Value2; \
      ASSERT_FALSE(o.empty()); \
      EXPECT_EQ(Value2, *o); \
    } \
  } while (false)

TEST(optional, value_assignment) {
  FATAL_IMPL_CALL(FATAL_IMPL_VALUE_ASSIGNMENT);

  ref_counter::reset();
  using refc = ref_counter::global<>;

  {
    optional<refc> rc;
    EXPECT_EQ(0, ref_counter::count());
    EXPECT_EQ(0, ref_counter::valid());
    rc = 5.6;
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());

  {
    optional<refc> rc(10);
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
    rc = 5.6;
    EXPECT_EQ(1, ref_counter::count());
    EXPECT_EQ(1, ref_counter::valid());
  }

  EXPECT_EQ(0, ref_counter::count());
  EXPECT_EQ(0, ref_counter::valid());
}

/////////////////////
// operator_equals //
/////////////////////

#define FATAL_IMPL_OPERATOR_EQUALS(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o1(Value1); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    EXPECT_FALSE(empty == o1); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    EXPECT_FALSE(o1 == empty); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    optional<Type> o2(Value2); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_FALSE(empty == o2); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_FALSE(o2 == empty); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_FALSE(o1 == o2); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_FALSE(o2 == o1); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    optional<Type> o11(Value1); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    EXPECT_TRUE(o1 == o11); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    EXPECT_FALSE(o2 == o11); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    optional<Type> o22(Value2); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
    \
    EXPECT_FALSE(o1 == o22); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
    \
    EXPECT_TRUE(o2 == o22); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
  } while (false)

TEST(optional, operator_equals) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_EQUALS);
}

///////////////////////////
// operator_equals_value //
///////////////////////////

#define FATAL_IMPL_OPERATOR_EQUALS_VALUE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    EXPECT_TRUE(empty.empty()); \
    EXPECT_FALSE(empty == Value1); \
    EXPECT_FALSE(empty == Value2); \
    \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    EXPECT_TRUE(o == Value1); \
    EXPECT_FALSE(o == Value2); \
  } while (false)

TEST(optional, operator_equals_value) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_EQUALS_VALUE);
}

/////////////////////////
// operator_not_equals //
/////////////////////////

#define FATAL_IMPL_OPERATOR_NOT_EQUALS(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    EXPECT_TRUE(empty.empty()); \
    \
    optional<Type> o1(Value1); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    EXPECT_TRUE(empty != o1); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    EXPECT_TRUE(o1 != empty); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    \
    optional<Type> o2(Value2); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_TRUE(empty != o2); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_TRUE(o2 != empty); \
    EXPECT_TRUE(empty.empty()); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_TRUE(o1 != o2); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    EXPECT_TRUE(o2 != o1); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    \
    optional<Type> o11(Value1); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    EXPECT_FALSE(o1 != o11); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    EXPECT_TRUE(o2 != o11); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    ASSERT_FALSE(o11.empty()); \
    EXPECT_EQ(Value1, *o11); \
    \
    optional<Type> o22(Value2); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
    \
    EXPECT_TRUE(o1 != o22); \
    ASSERT_FALSE(o1.empty()); \
    EXPECT_EQ(Value1, *o1); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
    \
    EXPECT_FALSE(o2 != o22); \
    ASSERT_FALSE(o2.empty()); \
    EXPECT_EQ(Value2, *o2); \
    ASSERT_FALSE(o22.empty()); \
    EXPECT_EQ(Value2, *o22); \
  } while (false)

TEST(optional, operator_not_equals) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_NOT_EQUALS);
}

///////////////////////////////
// operator_not_equals_value //
///////////////////////////////

#define FATAL_IMPL_OPERATOR_NOT_EQUALS_VALUE(Type, Value1, Value2, ...) \
  do { \
    optional<Type> empty; \
    EXPECT_TRUE(empty.empty()); \
    EXPECT_TRUE(empty != Value1); \
    EXPECT_TRUE(empty != Value2); \
    \
    optional<Type> o(Value1); \
    ASSERT_FALSE(o.empty()); \
    EXPECT_EQ(Value1, *o); \
    EXPECT_FALSE(o != Value1); \
    EXPECT_TRUE(o != Value2); \
  } while (false)

TEST(optional, operator_not_equals_value) {
  FATAL_IMPL_CALL(FATAL_IMPL_OPERATOR_NOT_EQUALS_VALUE);
}

} // namespace fatal {
