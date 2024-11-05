/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/flag_set.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

// test data
template <unsigned>
struct x {
  x(): n_(0) {}
  explicit x(unsigned p): n_(1u << p) {}
  /* implicit */ operator unsigned() const { return n_; }
protected:
  unsigned n_;
};

typedef x<0> x0t; x0t const x0(0); x0t const y0;
typedef x<1> x1t; x1t const x1(1); x1t const y1;
typedef x<2> x2t; x2t const x2(2); x2t const y2;
typedef x<3> x3t; x3t const x3(3); x3t const y3(0);
typedef x<4> x4t; x4t const x4(4); x4t const y4(1);
typedef x<5> x5t; x5t const x5(5); x5t const y5(2);
typedef x<6> x6t; x6t const x6; x6t const y6(3);
typedef x<7> x7t; x7t const x7; x7t const y7(4);
typedef x<8> x8t; x8t const x8; x8t const y8(5);

typedef flag_set<x0t, x1t, x2t, x3t, x4t, x5t> fx;
typedef flag_set<x3t, x4t, x5t, x6t, x7t, x8t> fy;
fx const x0s(x0); fx const x1s(x1); fx const x2s(x2);
fx const x3s(x3); fx const x4s(x4); fx const x5s(x5);
fx const x01s(x0, x1); fx const x23s(x2, x3); fx const x45s(x4, x5);
fx const x012345s(x0, x1, x2, x3, x4, x5);
fx const x543210s(x5, x4, x3, x2, x1, x0);
fx const x051423s(x0, x5, x1, x4, x2, x3);

template <typename TExpectedType, typename TActual, typename TExpectedValue>
void check(TExpectedValue &&expected, TActual &&actual) {
  FATAL_EXPECT_SAME<
    typename std::decay<TExpectedType>::type,
    typename std::decay<TActual>::type
  >();
  FATAL_EXPECT_EQ(expected, actual.get());
}

#define X_COPY(suffix) \
  fx x0##suffix(x0s); fx x1##suffix(x1s); fx x2##suffix(x2s); \
  fx x3##suffix(x3s); fx x4##suffix(x4s); fx x5##suffix(x5s); \
  fx x01##suffix(x01s); fx x23##suffix(x23s); fx x45##suffix(x45s); \
  fx x012345##suffix(x012345s); \
  fx x543210##suffix(x543210s); \
  fx x051423##suffix(x051423s); \
  static_assert(true)

//////////
// ctor //
//////////

FATAL_TEST(flag_set, default_ctor) {
  FATAL_EXPECT_EQ(0, (flag_set<>().get()));
  FATAL_EXPECT_EQ(0, (flag_set<std::string>().get()));
  FATAL_EXPECT_EQ(0, (flag_set<int, bool>().get()));
}

FATAL_TEST(flag_set, tags_ctor) {
  FATAL_EXPECT_EQ(x0, x0s.get());
  FATAL_EXPECT_EQ(x1, x1s.get());
  FATAL_EXPECT_EQ(x2, x2s.get());
  FATAL_EXPECT_EQ(x3, x3s.get());
  FATAL_EXPECT_EQ(x4, x4s.get());
  FATAL_EXPECT_EQ(x5, x5s.get());

  FATAL_EXPECT_EQ(x0 | x1, x01s.get());
  FATAL_EXPECT_EQ(x2 | x3, x23s.get());
  FATAL_EXPECT_EQ(x4 | x5, x45s.get());

  FATAL_EXPECT_NE(x0, x01s.get());
  FATAL_EXPECT_NE(x1, x01s.get());
  FATAL_EXPECT_NE(x2, x23s.get());
  FATAL_EXPECT_NE(x3, x23s.get());
  FATAL_EXPECT_NE(x4, x45s.get());
  FATAL_EXPECT_NE(x5, x45s.get());

  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, x012345s.get());
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, x543210s.get());
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, x051423s.get());
}

FATAL_TEST(flag_set, copy_ctor) {
  FATAL_EXPECT_EQ(x0s.get(), fx(x0s).get());
  FATAL_EXPECT_EQ(x1s.get(), fx(x1s).get());
  FATAL_EXPECT_EQ(x2s.get(), fx(x2s).get());
  FATAL_EXPECT_EQ(x3s.get(), fx(x3s).get());
  FATAL_EXPECT_EQ(x4s.get(), fx(x4s).get());
  FATAL_EXPECT_EQ(x5s.get(), fx(x5s).get());

  FATAL_EXPECT_EQ(x01s.get(), fx(x01s).get());
  FATAL_EXPECT_EQ(x23s.get(), fx(x23s).get());
  FATAL_EXPECT_EQ(x45s.get(), fx(x45s).get());

  FATAL_EXPECT_EQ(x012345s.get(), fx(x012345s).get());
  FATAL_EXPECT_EQ(x543210s.get(), fx(x543210s).get());
  FATAL_EXPECT_EQ(x051423s.get(), fx(x051423s).get());
}

FATAL_TEST(flag_set, move_ctor) {
  X_COPY(c);

  FATAL_EXPECT_EQ(x0s.get(), fx(std::move(x0c)).get());
  FATAL_EXPECT_EQ(x1s.get(), fx(std::move(x1c)).get());
  FATAL_EXPECT_EQ(x2s.get(), fx(std::move(x2c)).get());
  FATAL_EXPECT_EQ(x3s.get(), fx(std::move(x3c)).get());
  FATAL_EXPECT_EQ(x4s.get(), fx(std::move(x4c)).get());
  FATAL_EXPECT_EQ(x5s.get(), fx(std::move(x5c)).get());

  FATAL_EXPECT_EQ(x01s.get(), fx(std::move(x01c)).get());
  FATAL_EXPECT_EQ(x23s.get(), fx(std::move(x23c)).get());
  FATAL_EXPECT_EQ(x45s.get(), fx(std::move(x45c)).get());

  FATAL_EXPECT_EQ(x012345s.get(), fx(std::move(x012345c)).get());
  FATAL_EXPECT_EQ(x543210s.get(), fx(std::move(x543210c)).get());
  FATAL_EXPECT_EQ(x051423s.get(), fx(std::move(x051423c)).get());
}

FATAL_TEST(flag_set, foreign_ctor) {
  FATAL_EXPECT_EQ(y0, fy(x0s).get());
  FATAL_EXPECT_EQ(y1, fy(x1s).get());
  FATAL_EXPECT_EQ(y2, fy(x2s).get());
  FATAL_EXPECT_EQ(y3, fy(x3s).get());
  FATAL_EXPECT_EQ(y4, fy(x4s).get());
  FATAL_EXPECT_EQ(y5, fy(x5s).get());

  FATAL_EXPECT_EQ(y0 | y1, fy(x01s).get());
  FATAL_EXPECT_EQ(y2 | y3, fy(x23s).get());
  FATAL_EXPECT_EQ(y4 | y5, fy(x45s).get());

  FATAL_EXPECT_EQ(y0 | y1 | y2 | y3 | y4 | y5, fy(x012345s).get());
  FATAL_EXPECT_EQ(y0 | y1 | y2 | y3 | y4 | y5, fy(x543210s).get());
  FATAL_EXPECT_EQ(y0 | y1 | y2 | y3 | y4 | y5, fy(x051423s).get());
}

/////////
// set //
/////////

FATAL_TEST(flag_set, set_lvalue_tags) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.set(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set(x1, x2);
  FATAL_EXPECT_EQ(x0 | x1 | x2, s.get());

  s.set(x3).set(x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.set(x0).set(x1);
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  r.set(x2).set(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());
}

FATAL_TEST(flag_set, set_rvalue_tags) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).set(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set(x1, x2);
  FATAL_EXPECT_EQ(x0 | x1 | x2, s.get());

  std::move(s).set(x3).set(x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).set(x0).set(x1);
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  std::move(r).set(x2).set(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  FATAL_EXPECT_EQ(x1 | x3 | x5, (fx().set(x1).set(x3, x5).get()));
}

FATAL_TEST(flag_set, set_lvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.set<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  s.set<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  s.set<x1t, x2t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2, s.get());

  s.set<x3t>().set<x4t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.set<x0t>().set<x1t>();
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  r.set<x2t>().set<x3t, x4t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());
}

FATAL_TEST(flag_set, set_rvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).set<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set<x1t, x2t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2, s.get());

  std::move(s).set<x3t>().set<x4t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).set<x0t>().set<x1t>();
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  std::move(r).set<x2t>().set<x3t, x4t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  FATAL_EXPECT_EQ(x1 | x3 | x5, (fx().set<x1t>().set<x3t, x5t>().get()));
}

////////////
// set_if //
////////////

FATAL_TEST(flag_set, set_if_lvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.set_if<x0t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  s.set_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set_if<x0t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set_if<x5t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  s.set_if<x5t>(true);
  FATAL_EXPECT_EQ(x0 | x5, s.get());

  s.set_if<x3t>(false).set_if<x4t>(true);
  FATAL_EXPECT_EQ(x0 | x4 | x5, s.get());

  s.set_if<x3t>(true).set_if<x2t>(true);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());
}

FATAL_TEST(flag_set, set_if_rvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).set_if<x0t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).set_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set_if<x0t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set_if<x5t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).set_if<x5t>(true);
  FATAL_EXPECT_EQ(x0 | x5, s.get());

  std::move(s).set_if<x3t>(false).set_if<x4t>(true);
  FATAL_EXPECT_EQ(x0 | x4 | x5, s.get());

  std::move(s).set_if<x3t>(true).set_if<x2t>(true);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());

  FATAL_EXPECT_EQ(
    x1 | x3 | x5,
    fx().set_if<x0t>(false)
      .set_if<x1t>(true)
      .set_if<x2t>(false)
      .set_if<x3t>(true)
      .set_if<x4t>(false)
      .set_if<x5t>(true)
      .get()
  );
}

///////////
// reset //
///////////

FATAL_TEST(flag_set, reset_lvalue_tags) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  s.reset(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4 | x5, s.get());

  s.reset(x3).reset(x4);
  FATAL_EXPECT_EQ(x5, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.reset(x0).reset(x1);
  FATAL_EXPECT_EQ(0, r.get());

  r.reset(x2).reset(x3, x4);
  FATAL_EXPECT_EQ(0, r.get());
}

FATAL_TEST(flag_set, reset_rvalue_tags) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4 | x5, s.get());

  std::move(s).reset(x3).reset(x4);
  FATAL_EXPECT_EQ(x5, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).reset(x0).reset(x1);
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).reset(x2).reset(x3, x4);
  FATAL_EXPECT_EQ(0, r.get());

  FATAL_EXPECT_EQ(
    x0 | x2 | x4,
    (fx(x012345s).reset(x1).reset(x3, x5).get())
  );
}

FATAL_TEST(flag_set, reset_lvalue) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  s.reset<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset<x1t, x2t>();
  FATAL_EXPECT_EQ(x3 | x4 | x5, s.get());

  s.reset<x3t>().reset<x4t>();
  FATAL_EXPECT_EQ(x5, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.reset<x0t>().reset(x1);
  FATAL_EXPECT_EQ(0, r.get());

  r.reset<x2t>().reset<x3t, x4t>();
  FATAL_EXPECT_EQ(0, r.get());
}

FATAL_TEST(flag_set, reset_rvalue) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset<x1t, x2t>();
  FATAL_EXPECT_EQ(x3 | x4 | x5, s.get());

  std::move(s).reset<x3t>().reset<x4t>();
  FATAL_EXPECT_EQ(x5, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).reset<x0t>().reset(x1);
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).reset<x2t>().reset<x3t, x4t>();
  FATAL_EXPECT_EQ(0, r.get());

  FATAL_EXPECT_EQ(
    x0 | x2 | x4,
    (fx(x012345s).reset<x1t>().reset<x3t, x5t>().get())
  );
}

//////////////
// reset_if //
//////////////

FATAL_TEST(flag_set, reset_if_lvalue) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x0t>(false);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x0t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x0t>(false);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x0t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x5t>(false);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  s.reset_if<x5t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  s.reset_if<x3t>(false).reset_if<x4t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3, s.get());

  s.reset_if<x3t>(true).reset_if<x2t>(true);
  FATAL_EXPECT_EQ(x1, s.get());
}

FATAL_TEST(flag_set, reset_if_rvalue) {
  auto s = x012345s;
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x0t>(false);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x0t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x0t>(false);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x0t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x5t>(false);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4 | x5, s.get());

  std::move(s).reset_if<x5t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  std::move(s).reset_if<x3t>(false).reset_if<x4t>(true);
  FATAL_EXPECT_EQ(x1 | x2 | x3, s.get());

  std::move(s).reset_if<x3t>(true).reset_if<x2t>(true);
  FATAL_EXPECT_EQ(x1, s.get());

  FATAL_EXPECT_EQ(
    x0 | x2 | x4,
    fx(x012345s).reset_if<x0t>(false)
      .reset_if<x1t>(true)
      .reset_if<x2t>(false)
      .reset_if<x3t>(true)
      .reset_if<x4t>(false)
      .reset_if<x5t>(true)
      .get()
  );
}

//////////
// flip //
//////////

FATAL_TEST(flag_set, flip_lvalue_tags) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.flip(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  s.flip(x0);
  FATAL_EXPECT_EQ(0, s.get());

  s.flip(x1, x2);
  FATAL_EXPECT_EQ(x1 | x2, s.get());

  s.flip(x3).flip(x4);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  s.flip(x0);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  s.flip(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  s.flip(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4, s.get());

  s.flip(x3).flip(x4);
  FATAL_EXPECT_EQ(0, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.flip(x0).flip(x1);
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  r.flip(x2).flip(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  r.flip(x5).flip(x3, x1);
  FATAL_EXPECT_EQ(x0 | x2 | x4 | x5, r.get());
}

FATAL_TEST(flag_set, flip_rvalue_tags) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip(x0);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).flip(x0);
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip(x1, x2);
  FATAL_EXPECT_EQ(x1 | x2, s.get());

  std::move(s).flip(x3).flip(x4);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  std::move(s).flip(x0);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  std::move(s).flip(x0);
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  std::move(s).flip(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4, s.get());

  std::move(s).flip(x3).flip(x4);
  FATAL_EXPECT_EQ(0, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).flip(x0).flip(x1);
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  std::move(r).flip(x2).flip(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  std::move(r).flip(x5).flip(x3, x1);
  FATAL_EXPECT_EQ(x0 | x2 | x4 | x5, r.get());

  FATAL_EXPECT_EQ(
    x1 | x3 | x5,
    (fx().flip(x1).flip(x3, x5).get())
  );

  FATAL_EXPECT_EQ(
    x0 | x2 | x4,
    (fx(x012345s).flip(x1).flip(x3, x5).get())
  );
}

FATAL_TEST(flag_set, flip_lvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.flip<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  s.flip<x0t>();
  FATAL_EXPECT_EQ(0, s.get());

  s.flip(x1, x2);
  FATAL_EXPECT_EQ(x1 | x2, s.get());

  s.flip<x3t>().flip<x4t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  s.flip<x0t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  s.flip<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  s.flip(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4, s.get());

  s.flip<x3t>().flip<x4t>();
  FATAL_EXPECT_EQ(0, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  r.flip<x0t>().flip<x1t>();
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  r.flip<x2t>().flip(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  r.flip<x5t>().flip(x3, x1);
  FATAL_EXPECT_EQ(x0 | x2 | x4 | x5, r.get());
}

FATAL_TEST(flag_set, flip_rvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip<x0t>();
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).flip<x0t>();
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip(x1, x2);
  FATAL_EXPECT_EQ(x1 | x2, s.get());

  std::move(s).flip<x3t>().flip<x4t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  std::move(s).flip<x0t>();
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, s.get());

  std::move(s).flip<x0t>();
  FATAL_EXPECT_EQ(x1 | x2 | x3 | x4, s.get());

  std::move(s).flip(x1, x2);
  FATAL_EXPECT_EQ(x3 | x4, s.get());

  std::move(s).flip<x3t>().flip<x4t>();
  FATAL_EXPECT_EQ(0, s.get());

  fx r;
  FATAL_EXPECT_EQ(0, r.get());

  std::move(r).flip<x0t>().flip<x1t>();
  FATAL_EXPECT_EQ(x0 | x1, r.get());

  std::move(r).flip<x2t>().flip(x3, x4);
  FATAL_EXPECT_EQ(x0 | x1 | x2 | x3 | x4, r.get());

  std::move(r).flip<x5t>().flip(x3, x1);
  FATAL_EXPECT_EQ(x0 | x2 | x4 | x5, r.get());

  FATAL_EXPECT_EQ(
    x1 | x3 | x5,
    (fx().flip<x1t>().flip<x3t, x5t>().get())
  );

  FATAL_EXPECT_EQ(
    x0 | x2 | x4,
    (fx(x012345s).flip<x1t>().flip<x3t, x5t>().get())
  );
}

/////////////
// flip_if //
/////////////

FATAL_TEST(flag_set, flip_if_lvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s.flip_if<x0t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  s.flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  s.flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  s.flip_if<x0t>(true);
  FATAL_EXPECT_EQ(0, s.get());

  s.flip_if<x5t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  s.flip_if<x5t>(true);
  FATAL_EXPECT_EQ(x5, s.get());

  s.flip_if<x3t>(false).flip_if<x4t>(true);
  FATAL_EXPECT_EQ(x4 | x5, s.get());

  s.flip_if<x3t>(true).flip_if<x2t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  s.flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  s.flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());

  s.flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());

  s.flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  s.flip_if<x5t>(false);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  s.flip_if<x5t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4, s.get());

  s.flip_if<x3t>(false).flip_if<x4t>(true);
  FATAL_EXPECT_EQ(x2 | x3, s.get());

  s.flip_if<x3t>(true).flip_if<x2t>(true);
  FATAL_EXPECT_EQ(0, s.get());
}

FATAL_TEST(flag_set, flip_if_rvalue) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip_if<x0t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x0, s.get());

  std::move(s).flip_if<x0t>(true);
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip_if<x5t>(false);
  FATAL_EXPECT_EQ(0, s.get());

  std::move(s).flip_if<x5t>(true);
  FATAL_EXPECT_EQ(x5, s.get());

  std::move(s).flip_if<x3t>(false).flip_if<x4t>(true);
  FATAL_EXPECT_EQ(x4 | x5, s.get());

  std::move(s).flip_if<x3t>(true).flip_if<x2t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x0t>(false);
  FATAL_EXPECT_EQ(x0 | x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x0t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x5t>(false);
  FATAL_EXPECT_EQ(x2 | x3 | x4 | x5, s.get());

  std::move(s).flip_if<x5t>(true);
  FATAL_EXPECT_EQ(x2 | x3 | x4, s.get());

  std::move(s).flip_if<x3t>(false).flip_if<x4t>(true);
  FATAL_EXPECT_EQ(x2 | x3, s.get());

  std::move(s).flip_if<x3t>(true).flip_if<x2t>(true);
  FATAL_EXPECT_EQ(0, s.get());

  FATAL_EXPECT_EQ(
    x2 | x3 | x5,
    fx().flip_if<x0t>(false)
      .flip_if<x1t>(true)
      .flip_if<x2t>(false)
      .flip_if<x3t>(true)
      .flip_if<x4t>(false)
      .flip_if<x5t>(true)
      .flip_if<x2t>(true)
      .flip_if<x1t>(true)
      .get()
  );

  FATAL_EXPECT_EQ(
    x0 | x1 | x4,
    fx(x012345s).flip_if<x0t>(false)
      .flip_if<x1t>(true)
      .flip_if<x2t>(false)
      .flip_if<x3t>(true)
      .flip_if<x4t>(false)
      .flip_if<x5t>(true)
      .flip_if<x2t>(true)
      .flip_if<x1t>(true)
      .get()
  );
}

//////////
// test //
//////////

FATAL_TEST(flag_set, test_tags) {
  FATAL_EXPECT_TRUE((x0s.test(x0)));
  FATAL_EXPECT_FALSE((x0s.test(x1)));
  FATAL_EXPECT_FALSE((x0s.test(x2)));
  FATAL_EXPECT_FALSE((x0s.test(x3)));
  FATAL_EXPECT_FALSE((x0s.test(x4)));
  FATAL_EXPECT_FALSE((x0s.test(x5)));

  FATAL_EXPECT_TRUE((x1s.test(x1)));
  FATAL_EXPECT_FALSE((x1s.test(x0)));
  FATAL_EXPECT_FALSE((x1s.test(x2)));
  FATAL_EXPECT_FALSE((x1s.test(x3)));
  FATAL_EXPECT_FALSE((x1s.test(x4)));
  FATAL_EXPECT_FALSE((x1s.test(x5)));

  FATAL_EXPECT_TRUE((x2s.test(x2)));
  FATAL_EXPECT_FALSE((x2s.test(x0)));
  FATAL_EXPECT_FALSE((x2s.test(x1)));
  FATAL_EXPECT_FALSE((x2s.test(x3)));
  FATAL_EXPECT_FALSE((x2s.test(x4)));
  FATAL_EXPECT_FALSE((x2s.test(x5)));

  FATAL_EXPECT_TRUE((x3s.test(x3)));
  FATAL_EXPECT_FALSE((x3s.test(x0)));
  FATAL_EXPECT_FALSE((x3s.test(x1)));
  FATAL_EXPECT_FALSE((x3s.test(x2)));
  FATAL_EXPECT_FALSE((x3s.test(x4)));
  FATAL_EXPECT_FALSE((x3s.test(x5)));

  FATAL_EXPECT_TRUE((x4s.test(x4)));
  FATAL_EXPECT_FALSE((x4s.test(x0)));
  FATAL_EXPECT_FALSE((x4s.test(x1)));
  FATAL_EXPECT_FALSE((x4s.test(x2)));
  FATAL_EXPECT_FALSE((x4s.test(x3)));
  FATAL_EXPECT_FALSE((x4s.test(x5)));

  FATAL_EXPECT_TRUE((x5s.test(x5)));
  FATAL_EXPECT_FALSE((x5s.test(x0)));
  FATAL_EXPECT_FALSE((x5s.test(x1)));
  FATAL_EXPECT_FALSE((x5s.test(x2)));
  FATAL_EXPECT_FALSE((x5s.test(x3)));
  FATAL_EXPECT_FALSE((x5s.test(x4)));

  FATAL_EXPECT_TRUE((x01s.test(x0)));
  FATAL_EXPECT_TRUE((x01s.test(x1)));
  FATAL_EXPECT_FALSE((x01s.test(x2)));
  FATAL_EXPECT_FALSE((x01s.test(x3)));
  FATAL_EXPECT_FALSE((x01s.test(x4)));
  FATAL_EXPECT_FALSE((x01s.test(x5)));
  FATAL_EXPECT_TRUE((x01s.test(x0, x1)));
  FATAL_EXPECT_FALSE((x01s.test(x0, x2)));
  FATAL_EXPECT_FALSE((x01s.test(x0, x3)));
  FATAL_EXPECT_FALSE((x01s.test(x1, x4)));

  FATAL_EXPECT_TRUE((x012345s.test(x0)));
  FATAL_EXPECT_TRUE((x012345s.test(x0, x1)));
  FATAL_EXPECT_TRUE((x012345s.test(x0, x1, x2)));
  FATAL_EXPECT_TRUE((x012345s.test(x0, x1, x2, x3)));
  FATAL_EXPECT_TRUE((x012345s.test(x0, x1, x2, x3, x4)));
  FATAL_EXPECT_TRUE((x012345s.test(x0, x1, x2, x3, x4, x5)));
  FATAL_EXPECT_TRUE((x012345s.test(x5, x4, x3, x2, x1, x0)));
}

FATAL_TEST(flag_set, test) {
  FATAL_EXPECT_TRUE((x0s.test<x0t>()));
  FATAL_EXPECT_FALSE((x0s.test<x1t>()));
  FATAL_EXPECT_FALSE((x0s.test<x2t>()));
  FATAL_EXPECT_FALSE((x0s.test<x3t>()));
  FATAL_EXPECT_FALSE((x0s.test<x4t>()));
  FATAL_EXPECT_FALSE((x0s.test<x5t>()));

  FATAL_EXPECT_TRUE((x1s.test<x1t>()));
  FATAL_EXPECT_FALSE((x1s.test<x0t>()));
  FATAL_EXPECT_FALSE((x1s.test<x2t>()));
  FATAL_EXPECT_FALSE((x1s.test<x3t>()));
  FATAL_EXPECT_FALSE((x1s.test<x4t>()));
  FATAL_EXPECT_FALSE((x1s.test<x5t>()));

  FATAL_EXPECT_TRUE((x2s.test<x2t>()));
  FATAL_EXPECT_FALSE((x2s.test<x0t>()));
  FATAL_EXPECT_FALSE((x2s.test<x1t>()));
  FATAL_EXPECT_FALSE((x2s.test<x3t>()));
  FATAL_EXPECT_FALSE((x2s.test<x4t>()));
  FATAL_EXPECT_FALSE((x2s.test<x5t>()));

  FATAL_EXPECT_TRUE((x3s.test<x3t>()));
  FATAL_EXPECT_FALSE((x3s.test<x0t>()));
  FATAL_EXPECT_FALSE((x3s.test<x1t>()));
  FATAL_EXPECT_FALSE((x3s.test<x2t>()));
  FATAL_EXPECT_FALSE((x3s.test<x4t>()));
  FATAL_EXPECT_FALSE((x3s.test<x5t>()));

  FATAL_EXPECT_TRUE((x4s.test<x4t>()));
  FATAL_EXPECT_FALSE((x4s.test<x0t>()));
  FATAL_EXPECT_FALSE((x4s.test<x1t>()));
  FATAL_EXPECT_FALSE((x4s.test<x2t>()));
  FATAL_EXPECT_FALSE((x4s.test<x3t>()));
  FATAL_EXPECT_FALSE((x4s.test<x5t>()));

  FATAL_EXPECT_TRUE((x5s.test<x5t>()));
  FATAL_EXPECT_FALSE((x5s.test<x0t>()));
  FATAL_EXPECT_FALSE((x5s.test<x1t>()));
  FATAL_EXPECT_FALSE((x5s.test<x2t>()));
  FATAL_EXPECT_FALSE((x5s.test<x3t>()));
  FATAL_EXPECT_FALSE((x5s.test<x4t>()));

  FATAL_EXPECT_TRUE((x01s.test<x0t>()));
  FATAL_EXPECT_TRUE((x01s.test<x1t>()));
  FATAL_EXPECT_FALSE((x01s.test<x2t>()));
  FATAL_EXPECT_FALSE((x01s.test<x3t>()));
  FATAL_EXPECT_FALSE((x01s.test<x4t>()));
  FATAL_EXPECT_FALSE((x01s.test<x5t>()));
  FATAL_EXPECT_TRUE((x01s.test<x0t, x1t>()));
  FATAL_EXPECT_FALSE((x01s.test<x0t, x2t>()));
  FATAL_EXPECT_FALSE((x01s.test<x0t, x3t>()));
  FATAL_EXPECT_FALSE((x01s.test<x1t, x4t>()));

  FATAL_EXPECT_TRUE((x012345s.test<x0t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x0t, x1t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x0t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x0t, x1t, x2t, x3t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x0t, x1t, x2t, x3t, x4t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x0t, x1t, x2t, x3t, x4t, x5t>()));
  FATAL_EXPECT_TRUE((x012345s.test<x5t, x4t, x3t, x2t, x1t, x0t>()));
}

////////////
// expand //
////////////

FATAL_TEST(flag_set, expand) {
  flag_set<> f;

  auto f0 = f.expand<x0t>();
  check<flag_set<x0t>>(0b1, f0);

  auto f01 = f0.expand<x1t>();
  check<flag_set<x0t, x1t>>(0b11, f01);

  auto f011 = f01.expand<x1t>();
  check<flag_set<x0t, x1t>>(0b11, f011);

  auto f013 = f01.expand<x3t>();
  check<flag_set<x0t, x1t, x3t>>(0b111, f013);

  auto f0135 = f013.expand<x5t>();
  check<flag_set<x0t, x1t, x3t, x5t>>(0b1111, f0135);

  check<flag_set<x0t>>(
    0b1,
    flag_set<>().expand<x0t>()
  );

  check<flag_set<x0t, x5t>>(
    0b11,
    flag_set<>().expand<x0t>().expand<x5t>()
  );

  check<flag_set<x0t, x5t>>(
    0b11,
    flag_set<>().expand<x0t>()
      .expand<x5t>()
      .expand<x0t>()
      .expand<x5t>()
  );

  check<flag_set<x1t, x3t, x5t>>(
    0b111,
    flag_set<>().expand<x1t>()
      .expand<x3t>()
      .expand<x5t>()
  );
}

///////////////
// expand_if //
///////////////

FATAL_TEST(flag_set, expand_if) {
  {
    flag_set<> f;

    auto f0 = f.expand_if<x0t>(true);
    check<flag_set<x0t>>(0b1, f0);

    auto f01 = f0.expand_if<x1t>(true);
    check<flag_set<x0t, x1t>>(0b11, f01);

    auto f013 = f01.expand_if<x3t>(true);
    check<flag_set<x0t, x1t, x3t>>(0b111, f013);

    auto f0135 = f013.expand_if<x5t>(true);
    check<flag_set<x0t, x1t, x3t, x5t>>(0b1111, f0135);

    check<flag_set<x0t>>(
      0b1,
      flag_set<>().expand_if<x0t>(true)
    );

    check<flag_set<x0t, x5t>>(
      0b11,
      flag_set<>().expand_if<x0t>(true)
        .expand_if<x5t>(true)
        .expand_if<x0t>(true)
        .expand_if<x5t>(true)
    );

    check<flag_set<x1t, x3t, x5t>>(
      0b111,
      flag_set<>().expand_if<x1t>(true)
        .expand_if<x3t>(true)
        .expand_if<x5t>(true)
    );
  }

  {
    flag_set<> f;

    auto f0 = f.expand_if<x0t>(false);
    check<flag_set<x0t>>(0, f0);

    auto f01 = f0.expand_if<x1t>(false);
    check<flag_set<x0t, x1t>>(0, f01);

    auto f013 = f01.expand_if<x3t>(false);
    check<flag_set<x0t, x1t, x3t>>(0, f013);

    auto f0135 = f013.expand_if<x5t>(false);
    check<flag_set<x0t, x1t, x3t, x5t>>(0, f0135);

    check<flag_set<x0t>>(
      0,
      flag_set<>().expand_if<x0t>(false)
    );

    check<flag_set<x0t, x5t>>(
      0,
      flag_set<>().expand_if<x0t>(false)
        .expand_if<x5t>(false)
        .expand_if<x0t>(false)
        .expand_if<x5t>(false)
    );

    check<flag_set<x1t, x3t, x5t>>(
      0,
      flag_set<>().expand_if<x1t>(false)
        .expand_if<x3t>(false)
        .expand_if<x5t>(false)
    );
  }

  {
    flag_set<> f;

    auto f0 = f.expand_if<x0t>(true);
    check<flag_set<x0t>>(0b1, f0);

    auto f01 = f0.expand_if<x1t>(false);
    check<flag_set<x0t, x1t>>(0b01, f01);

    auto f013 = f01.expand_if<x3t>(true);
    check<flag_set<x0t, x1t, x3t>>(0b101, f013);

    auto f0135 = f013.expand_if<x5t>(false);
    check<flag_set<x0t, x1t, x3t, x5t>>(0b0101, f0135);

    check<flag_set<x0t>>(
      0b1,
      flag_set<>().expand_if<x0t>(false)
        .expand_if<x0t>(true)
    );

    check<flag_set<x0t, x5t>>(
      0b10,
      flag_set<>().expand_if<x0t>(false)
        .expand_if<x5t>(true)
    );

    check<flag_set<x0t, x5t>>(
      0b11,
      flag_set<>().expand_if<x0t>(false)
        .expand_if<x5t>(true)
        .expand_if<x0t>(true)
        .expand_if<x5t>(false)
    );

    check<flag_set<x1t, x3t, x5t>>(
      0b101,
      flag_set<>().expand_if<x1t>(true)
        .expand_if<x3t>(false)
        .expand_if<x5t>(true)
    );
  }
}

FATAL_TEST(flag_set, equals) {
  auto s = fx().set<x1t, x2t, x4t>();

  FATAL_EXPECT_TRUE((s.equals<x1t, x2t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x1t, x4t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x1t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x4t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x1t>()));

  FATAL_EXPECT_TRUE((s.equals<x1t, x2t, x4t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x1t, x4t, x2t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x1t, x4t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x4t, x1t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x1t, x2t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x1t, x1t>()));

  FATAL_EXPECT_TRUE((s.equals<x1t, x2t, x1t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x1t, x4t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x1t, x1t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x4t, x1t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x1t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x1t, x1t>()));

  FATAL_EXPECT_TRUE((s.equals<x1t, x2t, x2t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x1t, x2t, x4t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x2t, x1t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x2t, x2t, x4t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x2t, x1t>()));

  FATAL_EXPECT_TRUE((s.equals<x4t, x1t, x2t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x1t, x4t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x1t, x4t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x2t, x4t, x1t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x4t, x1t, x2t>()));
  FATAL_EXPECT_TRUE((s.equals<x4t, x4t, x2t, x1t>()));

  FATAL_EXPECT_FALSE((s.equals<x1t, x2t, x0t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x2t, x3t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x2t, x5t>()));

  FATAL_EXPECT_FALSE((s.equals<x1t, x0t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x2t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x3t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x4t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t, x5t>()));

  FATAL_EXPECT_FALSE((s.equals<x0t>()));
  FATAL_EXPECT_FALSE((s.equals<x1t>()));
  FATAL_EXPECT_FALSE((s.equals<x2t>()));
  FATAL_EXPECT_FALSE((s.equals<x3t>()));
  FATAL_EXPECT_FALSE((s.equals<x4t>()));
  FATAL_EXPECT_FALSE((s.equals<x5t>()));

  FATAL_EXPECT_FALSE((s.equals<>()));
}

/////////////////////////
// operator assignment //
/////////////////////////

FATAL_TEST(flag_set, operator_assignment) {
  fx s;
  FATAL_EXPECT_EQ(0, s.get());

  s = x0s;
  FATAL_EXPECT_EQ(x0s.get(), s.get());

  s = x1s;
  FATAL_EXPECT_EQ(x1s.get(), s.get());

  s = x2s;
  FATAL_EXPECT_EQ(x2s.get(), s.get());

  s = x3s;
  FATAL_EXPECT_EQ(x3s.get(), s.get());

  s = x4s;
  FATAL_EXPECT_EQ(x4s.get(), s.get());

  s = x5s;
  FATAL_EXPECT_EQ(x5s.get(), s.get());

  s = x01s;
  FATAL_EXPECT_EQ(x01s.get(), s.get());

  s = x23s;
  FATAL_EXPECT_EQ(x23s.get(), s.get());

  s = x45s;
  FATAL_EXPECT_EQ(x45s.get(), s.get());

  s = x012345s;
  FATAL_EXPECT_EQ(x012345s.get(), s.get());

  s = x543210s;
  FATAL_EXPECT_EQ(x543210s.get(), s.get());

  s = x051423s;
  FATAL_EXPECT_EQ(x051423s.get(), s.get());
}

FATAL_TEST(flag_set, operator_assignment_foreign) {
  {
    flag_set<> s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x0s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x1s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x2s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x3s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x4s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x5s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x01s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x23s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x45s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x012345s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x543210s;
    FATAL_EXPECT_EQ(0, s.get());

    s = x051423s;
    FATAL_EXPECT_EQ(0, s.get());
  }

  {
    fx s(x23s);
    FATAL_EXPECT_EQ(x23s.get(), s.get());

    s = flag_set<>();
    FATAL_EXPECT_EQ(0, s.get());
  }

  {
    flag_set<x0t, x1t, x2t, x3t, x4t> s;
    auto e = flag_set<>()
      .expand_if<x1t>(false)
      .expand<x5t>()
      .expand<x4t>()
      .expand<x7t>()
      .expand<x3t>()
      .expand<x0t>();
    check<flag_set<x1t, x5t, x4t, x7t, x3t, x0t>>(0b111110, e);

    s = e;
    check<flag_set<x0t, x1t, x2t, x3t, x4t>>(0b11001, s);
  }
}

///////////
// clear //
///////////

FATAL_TEST(flag_set, clear) {
  X_COPY(c);

# define TEST_CLEAR(s) \
  do { \
    s.clear(); \
    FATAL_EXPECT_EQ(0, s.get()); \
  } while (false)

  TEST_CLEAR(x0c);
  TEST_CLEAR(x1c);
  TEST_CLEAR(x2c);
  TEST_CLEAR(x3c);
  TEST_CLEAR(x4c);
  TEST_CLEAR(x5c);

  TEST_CLEAR(x01c);
  TEST_CLEAR(x23c);
  TEST_CLEAR(x45c);

  TEST_CLEAR(x012345c);
  TEST_CLEAR(x543210c);
  TEST_CLEAR(x051423c);

# undef TEST_CLEAR
}

} // namespace fatal {
