/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/functional/functional.h>

#include <fatal/container/uninitialized.h>
#include <fatal/preprocessor.h>
#include <fatal/test/ref_counter.h>

#include <fatal/test/driver.h>

#include <algorithm>
#include <array>
#include <vector>

#include <cstdint>
#include <cstring>

namespace fatal {
namespace fn {

static auto const faceb00c = int(0xfaceb00c);

FATAL_TEST(functional, no_op) {
  no_op f;

  int x = 10;
  f(x);
  FATAL_EXPECT_EQ(10, x);
  FATAL_EXPECT_SAME<void, decltype(f(x))>();

  std::string z("hello, world");
  f(z);
  FATAL_EXPECT_EQ("hello, world", z);
  FATAL_EXPECT_SAME<void, decltype(f(z))>();
}

FATAL_TEST(functional, identity) {
  identity f;

  int x = 10;
  FATAL_EXPECT_EQ(10, f(x));
  FATAL_EXPECT_EQ(10, x);

  std::string z("hello, world");
  FATAL_EXPECT_EQ("hello, world", f(z));
  FATAL_EXPECT_EQ("hello, world", z);
}

FATAL_TEST(functional, string) {
  default_constructed<std::string> f;

  int x = 10;
  FATAL_EXPECT_EQ(std::string(), f(x));
  FATAL_EXPECT_EQ(10, x);

  std::string z("hello, world");
  FATAL_EXPECT_EQ(std::string(), f(z));
  FATAL_EXPECT_EQ("hello, world", z);
}

FATAL_TEST(functional, constant) {
  using w = std::integral_constant<unsigned, 56>;
  constant<w> f;

  int x = 10;
  FATAL_EXPECT_EQ(w::value, f(x));
  FATAL_EXPECT_SAME<w::value_type, decltype(f(x))>();
  FATAL_EXPECT_EQ(10, x);

  std::string z("hello, world");
  FATAL_EXPECT_EQ(w::value, f(z));
  FATAL_EXPECT_SAME<w::value_type, decltype(f(z))>();
  FATAL_EXPECT_EQ("hello, world", z);
}

FATAL_TEST(functional, std_get) {
  std_get<0> g0;
  std_get<1> g1;
  std_get<2> g2;

  auto x = std::make_tuple(10, 20, 30);
  FATAL_EXPECT_EQ(10, g0(x));
  FATAL_EXPECT_EQ(20, g1(x));
  FATAL_EXPECT_EQ(30, g2(x));

  auto y = std::make_tuple(40, 50, 60);
  FATAL_EXPECT_EQ(40, g0(y));
  FATAL_EXPECT_EQ(50, g1(y));
  FATAL_EXPECT_EQ(60, g2(y));
}

FATAL_TEST(functional, static_caster) {
  static_caster<unsigned> f;

  FATAL_EXPECT_EQ(10, f(10));
  FATAL_EXPECT_SAME<unsigned, decltype(f(10))>();

  FATAL_EXPECT_EQ(5, f(5.6));
  FATAL_EXPECT_SAME<unsigned, decltype(f(5.6))>();
}

FATAL_TEST(functional, reinterpret_caster) {
  reinterpret_caster<char const *> f;

  constexpr std::array<char, 8> str = {"fatal::"};
  FATAL_EXPECT_EQ(str.size(), std::strlen(str.data()) + 1); // sanity

  std::int64_t x;
  static_assert(sizeof(x) == str.size() * sizeof(str[0]), "mismatched sizes");
  std::memcpy(&x, str.data(), str.size() * sizeof(str[0]));
  FATAL_EXPECT_EQ(reinterpret_cast<char const *>(&x), f(&x));

  double y;
  static_assert(sizeof(y) == str.size() * sizeof(str[0]), "mismatched sizes");
  std::memcpy(&y, str.data(), str.size() * sizeof(str[0]));
  FATAL_EXPECT_EQ(reinterpret_cast<char const *>(&y), f(&y));
}

struct base { virtual ~base() {} };
struct derived: public base {};
struct another: public base {};

FATAL_TEST(functional, dynamic_caster) {
  dynamic_caster<derived *> f;

  base b;
  base *p = &b;
  FATAL_EXPECT_NULL(f(p));

  derived d;
  p = &d;
  FATAL_EXPECT_EQ(&d, f(p));

  another a;
  p = &a;
  FATAL_EXPECT_NULL(f(p));
}

FATAL_TEST(functional, dereferencer) {
  dereferencer f;

  int x = 10;
  FATAL_EXPECT_EQ(x, f(&x));

  double y = 5.6;
  FATAL_EXPECT_EQ(y, f(&y));

  std::string s("hello, world");
  FATAL_EXPECT_EQ(s, f(&s));
}

FATAL_TEST(functional, address_of) {
  address_of f;

  int x = 10;
  FATAL_EXPECT_EQ(&x, f(x));

  double y = 5.6;
  FATAL_EXPECT_EQ(&y, f(y));

  std::string s("hello, world");
  FATAL_EXPECT_EQ(&s, f(s));
}

FATAL_TEST(functional, is_positive) {
  is_positive f;

  FATAL_EXPECT_TRUE(f(10));
  FATAL_EXPECT_FALSE(f(0));
  FATAL_EXPECT_FALSE(f(-10));

  FATAL_EXPECT_TRUE(f(10u));
  FATAL_EXPECT_FALSE(f(0u));
}

FATAL_TEST(functional, not_positive) {
  not_positive f;

  FATAL_EXPECT_FALSE(f(10));
  FATAL_EXPECT_TRUE(f(0));
  FATAL_EXPECT_TRUE(f(-10));

  FATAL_EXPECT_FALSE(f(10u));
  FATAL_EXPECT_TRUE(f(0u));
}

FATAL_TEST(functional, is_negative) {
  is_negative f;

  FATAL_EXPECT_FALSE(f(10));
  FATAL_EXPECT_FALSE(f(0));
  FATAL_EXPECT_TRUE(f(-10));

  FATAL_EXPECT_FALSE(f(10u));
  FATAL_EXPECT_FALSE(f(0u));
}

FATAL_TEST(functional, not_negative) {
  not_negative f;

  FATAL_EXPECT_TRUE(f(10));
  FATAL_EXPECT_TRUE(f(0));
  FATAL_EXPECT_FALSE(f(-10));

  FATAL_EXPECT_TRUE(f(10u));
  FATAL_EXPECT_TRUE(f(0u));
}

FATAL_TEST(functional, equal) {
  equal f;

  FATAL_EXPECT_TRUE(f(10, 10));
  FATAL_EXPECT_FALSE(f(10, 20));

  FATAL_EXPECT_TRUE(f(std::string(), std::string()));
  FATAL_EXPECT_TRUE(f("test", std::string("test")));
  FATAL_EXPECT_TRUE(f(std::string("test"), "test"));
  FATAL_EXPECT_FALSE(f(std::string(), std::string("test")));
}

FATAL_TEST(functional, not_equal) {
  not_equal f;

  FATAL_EXPECT_FALSE(f(10, 10));
  FATAL_EXPECT_TRUE(f(10, 20));

  FATAL_EXPECT_FALSE(f(std::string(), std::string()));
  FATAL_EXPECT_FALSE(f("test", std::string("test")));
  FATAL_EXPECT_FALSE(f(std::string("test"), "test"));
  FATAL_EXPECT_TRUE(f(std::string(), std::string("test")));
}

FATAL_TEST(functional, less) {
  less f;

  FATAL_EXPECT_FALSE(f(10, 10));
  FATAL_EXPECT_TRUE(f(10, 20));
  FATAL_EXPECT_FALSE(f(20, 10));

  FATAL_EXPECT_FALSE(f(5.6, 5.6));
  FATAL_EXPECT_TRUE(f(5.6, 7.2));
  FATAL_EXPECT_FALSE(f(7.2, 5.6));

  FATAL_EXPECT_FALSE(f(std::string(), std::string()));
  FATAL_EXPECT_FALSE(f("test", std::string("test")));
  FATAL_EXPECT_FALSE(f(std::string("test"), "test"));
  FATAL_EXPECT_TRUE(f(std::string(), std::string("test")));
  FATAL_EXPECT_FALSE(f(std::string("test"), std::string()));
}

FATAL_TEST(functional, less_equal) {
  less_equal f;

  FATAL_EXPECT_TRUE(f(10, 10));
  FATAL_EXPECT_TRUE(f(10, 20));
  FATAL_EXPECT_FALSE(f(20, 10));

  FATAL_EXPECT_TRUE(f(5.6, 5.6));
  FATAL_EXPECT_TRUE(f(5.6, 7.2));
  FATAL_EXPECT_FALSE(f(7.2, 5.6));

  FATAL_EXPECT_TRUE(f(std::string(), std::string()));
  FATAL_EXPECT_TRUE(f("test", std::string("test")));
  FATAL_EXPECT_TRUE(f(std::string("test"), "test"));
  FATAL_EXPECT_TRUE(f(std::string(), std::string("test")));
  FATAL_EXPECT_FALSE(f(std::string("test"), std::string()));
}

FATAL_TEST(functional, greater) {
  greater f;

  FATAL_EXPECT_FALSE(f(10, 10));
  FATAL_EXPECT_FALSE(f(10, 20));
  FATAL_EXPECT_TRUE(f(20, 10));

  FATAL_EXPECT_FALSE(f(5.6, 5.6));
  FATAL_EXPECT_FALSE(f(5.6, 7.2));
  FATAL_EXPECT_TRUE(f(7.2, 5.6));

  FATAL_EXPECT_FALSE(f(std::string(), std::string()));
  FATAL_EXPECT_FALSE(f("test", std::string("test")));
  FATAL_EXPECT_FALSE(f(std::string("test"), "test"));
  FATAL_EXPECT_FALSE(f(std::string(), std::string("test")));
  FATAL_EXPECT_TRUE(f(std::string("test"), std::string()));
}

FATAL_TEST(functional, greater_equal) {
  greater_equal f;

  FATAL_EXPECT_TRUE(f(10, 10));
  FATAL_EXPECT_FALSE(f(10, 20));
  FATAL_EXPECT_TRUE(f(20, 10));

  FATAL_EXPECT_TRUE(f(5.6, 5.6));
  FATAL_EXPECT_FALSE(f(5.6, 7.2));
  FATAL_EXPECT_TRUE(f(7.2, 5.6));

  FATAL_EXPECT_TRUE(f(std::string(), std::string()));
  FATAL_EXPECT_TRUE(f("test", std::string("test")));
  FATAL_EXPECT_TRUE(f(std::string("test"), "test"));
  FATAL_EXPECT_FALSE(f(std::string(), std::string("test")));
  FATAL_EXPECT_TRUE(f(std::string("test"), std::string()));
}

FATAL_TEST(functional, negate) {
  negate f;

  FATAL_EXPECT_FALSE(f(true));
  FATAL_EXPECT_TRUE(f(false));

  FATAL_EXPECT_FALSE(f(10));
  FATAL_EXPECT_TRUE(f(0));

  int *p = nullptr;
  FATAL_EXPECT_FALSE(f(&p));
  FATAL_EXPECT_TRUE(f(p));
}

FATAL_TEST(functional, logical_and) {
  logical_and f;

  FATAL_EXPECT_TRUE(f());

  FATAL_EXPECT_TRUE(f(true));
  FATAL_EXPECT_FALSE(f(false));

  FATAL_EXPECT_TRUE(f(true, true));
  FATAL_EXPECT_FALSE(f(true, false));
  FATAL_EXPECT_FALSE(f(false, true));
  FATAL_EXPECT_FALSE(f(false, false));

  FATAL_EXPECT_TRUE(f(true, true, true));
  FATAL_EXPECT_FALSE(f(true, true, false));
  FATAL_EXPECT_FALSE(f(true, false, true));
  FATAL_EXPECT_FALSE(f(true, false, false));
  FATAL_EXPECT_FALSE(f(false, true, true));
  FATAL_EXPECT_FALSE(f(false, true, false));
  FATAL_EXPECT_FALSE(f(false, false, true));
  FATAL_EXPECT_FALSE(f(false, false, false));

  FATAL_EXPECT_TRUE(f(true, true, true, true));
  FATAL_EXPECT_FALSE(f(true, true, true, false));
  FATAL_EXPECT_FALSE(f(true, true, false, true));
  FATAL_EXPECT_FALSE(f(true, true, false, false));
  FATAL_EXPECT_FALSE(f(true, false, true, true));
  FATAL_EXPECT_FALSE(f(true, false, true, false));
  FATAL_EXPECT_FALSE(f(true, false, false, true));
  FATAL_EXPECT_FALSE(f(true, false, false, false));
  FATAL_EXPECT_FALSE(f(false, true, true, true));
  FATAL_EXPECT_FALSE(f(false, true, true, false));
  FATAL_EXPECT_FALSE(f(false, true, false, true));
  FATAL_EXPECT_FALSE(f(false, true, false, false));
  FATAL_EXPECT_FALSE(f(false, false, true, true));
  FATAL_EXPECT_FALSE(f(false, false, true, false));
  FATAL_EXPECT_FALSE(f(false, false, false, true));
  FATAL_EXPECT_FALSE(f(false, false, false, false));
}

FATAL_TEST(functional, logical_or) {
  logical_or f;

  FATAL_EXPECT_FALSE(f());

  FATAL_EXPECT_TRUE(f(true));
  FATAL_EXPECT_FALSE(f(false));

  FATAL_EXPECT_TRUE(f(true, true));
  FATAL_EXPECT_TRUE(f(true, false));
  FATAL_EXPECT_TRUE(f(false, true));
  FATAL_EXPECT_FALSE(f(false, false));

  FATAL_EXPECT_TRUE(f(true, true, true));
  FATAL_EXPECT_TRUE(f(true, true, false));
  FATAL_EXPECT_TRUE(f(true, false, true));
  FATAL_EXPECT_TRUE(f(true, false, false));
  FATAL_EXPECT_TRUE(f(false, true, true));
  FATAL_EXPECT_TRUE(f(false, true, false));
  FATAL_EXPECT_TRUE(f(false, false, true));
  FATAL_EXPECT_FALSE(f(false, false, false));

  FATAL_EXPECT_TRUE(f(true, true, true, true));
  FATAL_EXPECT_TRUE(f(true, true, true, false));
  FATAL_EXPECT_TRUE(f(true, true, false, true));
  FATAL_EXPECT_TRUE(f(true, true, false, false));
  FATAL_EXPECT_TRUE(f(true, false, true, true));
  FATAL_EXPECT_TRUE(f(true, false, true, false));
  FATAL_EXPECT_TRUE(f(true, false, false, true));
  FATAL_EXPECT_TRUE(f(true, false, false, false));
  FATAL_EXPECT_TRUE(f(false, true, true, true));
  FATAL_EXPECT_TRUE(f(false, true, true, false));
  FATAL_EXPECT_TRUE(f(false, true, false, true));
  FATAL_EXPECT_TRUE(f(false, true, false, false));
  FATAL_EXPECT_TRUE(f(false, false, true, true));
  FATAL_EXPECT_TRUE(f(false, false, true, false));
  FATAL_EXPECT_TRUE(f(false, false, false, true));
  FATAL_EXPECT_FALSE(f(false, false, false, false));
}

FATAL_TEST(functional, ternary) {
  ternary f;
  FATAL_EXPECT_EQ(10, f(true, 10, 20));
  FATAL_EXPECT_EQ(20, f(false, 10, 20));
}

FATAL_TEST(functional, complement) {
# define TEST_IMPL(...) \
  FATAL_EXPECT_EQ(~__VA_ARGS__, f(__VA_ARGS__))

  complement f;

  TEST_IMPL(0);
  TEST_IMPL(1);
  TEST_IMPL(999);
  TEST_IMPL(faceb00c);

#undef TEST_IMPL
}

FATAL_TEST(functional, bitwise_and) {
# define TEST_IMPL(Expected, ...) \
  FATAL_EXPECT_EQ(Expected, f(__VA_ARGS__))

  bitwise_and f;

  TEST_IMPL(0, 0);
  TEST_IMPL(1, 1);
  TEST_IMPL(999, 999);
  TEST_IMPL(faceb00c, faceb00c);

  TEST_IMPL((0 & 0), 0, 0);
  TEST_IMPL((0 & 1), 0, 1);
  TEST_IMPL((0 & 999), 0, 999);
  TEST_IMPL((0 & faceb00c), 0, faceb00c);
  TEST_IMPL((1 & 0), 1, 0);
  TEST_IMPL((1 & 1), 1, 1);
  TEST_IMPL((1 & 999), 1, 999);
  TEST_IMPL((1 & faceb00c), 1, faceb00c);
  TEST_IMPL((999 & 0), 999, 0);
  TEST_IMPL((999 & 1), 999, 1);
  TEST_IMPL((999 & 999), 999, 999);
  TEST_IMPL((999 & faceb00c), 999, faceb00c);
  TEST_IMPL((faceb00c & 0), faceb00c, 0);
  TEST_IMPL((faceb00c & 1), faceb00c, 1);
  TEST_IMPL((faceb00c & 999), faceb00c, 999);
  TEST_IMPL((faceb00c & faceb00c), faceb00c, faceb00c);

  TEST_IMPL((0 & 0 & 0), 0, 0, 0);
  TEST_IMPL((0 & 0 & 1), 0, 0, 1);
  TEST_IMPL((0 & 0 & 999), 0, 0, 999);
  TEST_IMPL((0 & 0 & faceb00c), 0, 0, faceb00c);
  TEST_IMPL((0 & 1 & 0), 0, 1, 0);
  TEST_IMPL((0 & 1 & 1), 0, 1, 1);
  TEST_IMPL((0 & 1 & 999), 0, 1, 999);
  TEST_IMPL((0 & 1 & faceb00c), 0, 1, faceb00c);
  TEST_IMPL((0 & 999 & 0), 0, 999, 0);
  TEST_IMPL((0 & 999 & 1), 0, 999, 1);
  TEST_IMPL((0 & 999 & 999), 0, 999, 999);
  TEST_IMPL((0 & 999 & faceb00c), 0, 999, faceb00c);
  TEST_IMPL((0 & faceb00c & 0), 0, faceb00c, 0);
  TEST_IMPL((0 & faceb00c & 1), 0, faceb00c, 1);
  TEST_IMPL((0 & faceb00c & 999), 0, faceb00c, 999);
  TEST_IMPL((0 & faceb00c & faceb00c), 0, faceb00c, faceb00c);
  TEST_IMPL((1 & 0 & 0), 1, 0, 0);
  TEST_IMPL((1 & 0 & 1), 1, 0, 1);
  TEST_IMPL((1 & 0 & 999), 1, 0, 999);
  TEST_IMPL((1 & 0 & faceb00c), 1, 0, faceb00c);
  TEST_IMPL((1 & 1 & 0), 1, 1, 0);
  TEST_IMPL((1 & 1 & 1), 1, 1, 1);
  TEST_IMPL((1 & 1 & 999), 1, 1, 999);
  TEST_IMPL((1 & 1 & faceb00c), 1, 1, faceb00c);
  TEST_IMPL((1 & 999 & 0), 1, 999, 0);
  TEST_IMPL((1 & 999 & 1), 1, 999, 1);
  TEST_IMPL((1 & 999 & 999), 1, 999, 999);
  TEST_IMPL((1 & 999 & faceb00c), 1, 999, faceb00c);
  TEST_IMPL((1 & faceb00c & 0), 1, faceb00c, 0);
  TEST_IMPL((1 & faceb00c & 1), 1, faceb00c, 1);
  TEST_IMPL((1 & faceb00c & 999), 1, faceb00c, 999);
  TEST_IMPL((1 & faceb00c & faceb00c), 1, faceb00c, faceb00c);
  TEST_IMPL((999 & 0 & 0), 999, 0, 0);
  TEST_IMPL((999 & 0 & 1), 999, 0, 1);
  TEST_IMPL((999 & 0 & 999), 999, 0, 999);
  TEST_IMPL((999 & 0 & faceb00c), 999, 0, faceb00c);
  TEST_IMPL((999 & 1 & 0), 999, 1, 0);
  TEST_IMPL((999 & 1 & 1), 999, 1, 1);
  TEST_IMPL((999 & 1 & 999), 999, 1, 999);
  TEST_IMPL((999 & 1 & faceb00c), 999, 1, faceb00c);
  TEST_IMPL((999 & 999 & 0), 999, 999, 0);
  TEST_IMPL((999 & 999 & 1), 999, 999, 1);
  TEST_IMPL((999 & 999 & 999), 999, 999, 999);
  TEST_IMPL((999 & 999 & faceb00c), 999, 999, faceb00c);
  TEST_IMPL((999 & faceb00c & 0), 999, faceb00c, 0);
  TEST_IMPL((999 & faceb00c & 1), 999, faceb00c, 1);
  TEST_IMPL((999 & faceb00c & 999), 999, faceb00c, 999);
  TEST_IMPL((999 & faceb00c & faceb00c), 999, faceb00c, faceb00c);
  TEST_IMPL((faceb00c & 0 & 0), faceb00c, 0, 0);
  TEST_IMPL((faceb00c & 0 & 1), faceb00c, 0, 1);
  TEST_IMPL((faceb00c & 0 & 999), faceb00c, 0, 999);
  TEST_IMPL((faceb00c & 0 & faceb00c), faceb00c, 0, faceb00c);
  TEST_IMPL((faceb00c & 1 & 0), faceb00c, 1, 0);
  TEST_IMPL((faceb00c & 1 & 1), faceb00c, 1, 1);
  TEST_IMPL((faceb00c & 1 & 999), faceb00c, 1, 999);
  TEST_IMPL((faceb00c & 1 & faceb00c), faceb00c, 1, faceb00c);
  TEST_IMPL((faceb00c & 999 & 0), faceb00c, 999, 0);
  TEST_IMPL((faceb00c & 999 & 1), faceb00c, 999, 1);
  TEST_IMPL((faceb00c & 999 & 999), faceb00c, 999, 999);
  TEST_IMPL((faceb00c & 999 & faceb00c), faceb00c, 999, faceb00c);
  TEST_IMPL((faceb00c & faceb00c & 0), faceb00c, faceb00c, 0);
  TEST_IMPL((faceb00c & faceb00c & 1), faceb00c, faceb00c, 1);
  TEST_IMPL((faceb00c & faceb00c & 999), faceb00c, faceb00c, 999);
  TEST_IMPL(
    (faceb00c & faceb00c & faceb00c),
    faceb00c, faceb00c, faceb00c
  );

#undef TEST_IMPL
}

FATAL_TEST(functional, bitwise_or) {
# define TEST_IMPL(Expected, ...) \
  FATAL_EXPECT_EQ(Expected, f(__VA_ARGS__))

  bitwise_or f;

  TEST_IMPL(0, 0);
  TEST_IMPL(1, 1);
  TEST_IMPL(999, 999);
  TEST_IMPL(faceb00c, faceb00c);

  TEST_IMPL((0 | 0), 0, 0);
  TEST_IMPL((0 | 1), 0, 1);
  TEST_IMPL((0 | 999), 0, 999);
  TEST_IMPL((0 | faceb00c), 0, faceb00c);
  TEST_IMPL((1 | 0), 1, 0);
  TEST_IMPL((1 | 1), 1, 1);
  TEST_IMPL((1 | 999), 1, 999);
  TEST_IMPL((1 | faceb00c), 1, faceb00c);
  TEST_IMPL((999 | 0), 999, 0);
  TEST_IMPL((999 | 1), 999, 1);
  TEST_IMPL((999 | 999), 999, 999);
  TEST_IMPL((999 | faceb00c), 999, faceb00c);
  TEST_IMPL((faceb00c | 0), faceb00c, 0);
  TEST_IMPL((faceb00c | 1), faceb00c, 1);
  TEST_IMPL((faceb00c | 999), faceb00c, 999);
  TEST_IMPL((faceb00c | faceb00c), faceb00c, faceb00c);

  TEST_IMPL((0 | 0 | 0), 0, 0, 0);
  TEST_IMPL((0 | 0 | 1), 0, 0, 1);
  TEST_IMPL((0 | 0 | 999), 0, 0, 999);
  TEST_IMPL((0 | 0 | faceb00c), 0, 0, faceb00c);
  TEST_IMPL((0 | 1 | 0), 0, 1, 0);
  TEST_IMPL((0 | 1 | 1), 0, 1, 1);
  TEST_IMPL((0 | 1 | 999), 0, 1, 999);
  TEST_IMPL((0 | 1 | faceb00c), 0, 1, faceb00c);
  TEST_IMPL((0 | 999 | 0), 0, 999, 0);
  TEST_IMPL((0 | 999 | 1), 0, 999, 1);
  TEST_IMPL((0 | 999 | 999), 0, 999, 999);
  TEST_IMPL((0 | 999 | faceb00c), 0, 999, faceb00c);
  TEST_IMPL((0 | faceb00c | 0), 0, faceb00c, 0);
  TEST_IMPL((0 | faceb00c | 1), 0, faceb00c, 1);
  TEST_IMPL((0 | faceb00c | 999), 0, faceb00c, 999);
  TEST_IMPL((0 | faceb00c | faceb00c), 0, faceb00c, faceb00c);
  TEST_IMPL((1 | 0 | 0), 1, 0, 0);
  TEST_IMPL((1 | 0 | 1), 1, 0, 1);
  TEST_IMPL((1 | 0 | 999), 1, 0, 999);
  TEST_IMPL((1 | 0 | faceb00c), 1, 0, faceb00c);
  TEST_IMPL((1 | 1 | 0), 1, 1, 0);
  TEST_IMPL((1 | 1 | 1), 1, 1, 1);
  TEST_IMPL((1 | 1 | 999), 1, 1, 999);
  TEST_IMPL((1 | 1 | faceb00c), 1, 1, faceb00c);
  TEST_IMPL((1 | 999 | 0), 1, 999, 0);
  TEST_IMPL((1 | 999 | 1), 1, 999, 1);
  TEST_IMPL((1 | 999 | 999), 1, 999, 999);
  TEST_IMPL((1 | 999 | faceb00c), 1, 999, faceb00c);
  TEST_IMPL((1 | faceb00c | 0), 1, faceb00c, 0);
  TEST_IMPL((1 | faceb00c | 1), 1, faceb00c, 1);
  TEST_IMPL((1 | faceb00c | 999), 1, faceb00c, 999);
  TEST_IMPL((1 | faceb00c | faceb00c), 1, faceb00c, faceb00c);
  TEST_IMPL((999 | 0 | 0), 999, 0, 0);
  TEST_IMPL((999 | 0 | 1), 999, 0, 1);
  TEST_IMPL((999 | 0 | 999), 999, 0, 999);
  TEST_IMPL((999 | 0 | faceb00c), 999, 0, faceb00c);
  TEST_IMPL((999 | 1 | 0), 999, 1, 0);
  TEST_IMPL((999 | 1 | 1), 999, 1, 1);
  TEST_IMPL((999 | 1 | 999), 999, 1, 999);
  TEST_IMPL((999 | 1 | faceb00c), 999, 1, faceb00c);
  TEST_IMPL((999 | 999 | 0), 999, 999, 0);
  TEST_IMPL((999 | 999 | 1), 999, 999, 1);
  TEST_IMPL((999 | 999 | 999), 999, 999, 999);
  TEST_IMPL((999 | 999 | faceb00c), 999, 999, faceb00c);
  TEST_IMPL((999 | faceb00c | 0), 999, faceb00c, 0);
  TEST_IMPL((999 | faceb00c | 1), 999, faceb00c, 1);
  TEST_IMPL((999 | faceb00c | 999), 999, faceb00c, 999);
  TEST_IMPL((999 | faceb00c | faceb00c), 999, faceb00c, faceb00c);
  TEST_IMPL((faceb00c | 0 | 0), faceb00c, 0, 0);
  TEST_IMPL((faceb00c | 0 | 1), faceb00c, 0, 1);
  TEST_IMPL((faceb00c | 0 | 999), faceb00c, 0, 999);
  TEST_IMPL((faceb00c | 0 | faceb00c), faceb00c, 0, faceb00c);
  TEST_IMPL((faceb00c | 1 | 0), faceb00c, 1, 0);
  TEST_IMPL((faceb00c | 1 | 1), faceb00c, 1, 1);
  TEST_IMPL((faceb00c | 1 | 999), faceb00c, 1, 999);
  TEST_IMPL((faceb00c | 1 | faceb00c), faceb00c, 1, faceb00c);
  TEST_IMPL((faceb00c | 999 | 0), faceb00c, 999, 0);
  TEST_IMPL((faceb00c | 999 | 1), faceb00c, 999, 1);
  TEST_IMPL((faceb00c | 999 | 999), faceb00c, 999, 999);
  TEST_IMPL((faceb00c | 999 | faceb00c), faceb00c, 999, faceb00c);
  TEST_IMPL((faceb00c | faceb00c | 0), faceb00c, faceb00c, 0);
  TEST_IMPL((faceb00c | faceb00c | 1), faceb00c, faceb00c, 1);
  TEST_IMPL((faceb00c | faceb00c | 999), faceb00c, faceb00c, 999);
  TEST_IMPL(
    (faceb00c | faceb00c | faceb00c),
    faceb00c, faceb00c, faceb00c
  );

#undef TEST_IMPL
}

FATAL_TEST(functional, bitwise_xor) {
# define TEST_IMPL(Expected, ...) \
  FATAL_EXPECT_EQ(Expected, f(__VA_ARGS__))

  bitwise_xor f;

  TEST_IMPL(0, 0);
  TEST_IMPL(1, 1);
  TEST_IMPL(999, 999);
  TEST_IMPL(faceb00c, faceb00c);

  TEST_IMPL((0 ^ 0), 0, 0);
  TEST_IMPL((0 ^ 1), 0, 1);
  TEST_IMPL((0 ^ 999), 0, 999);
  TEST_IMPL((0 ^ faceb00c), 0, faceb00c);
  TEST_IMPL((1 ^ 0), 1, 0);
  TEST_IMPL((1 ^ 1), 1, 1);
  TEST_IMPL((1 ^ 999), 1, 999);
  TEST_IMPL((1 ^ faceb00c), 1, faceb00c);
  TEST_IMPL((999 ^ 0), 999, 0);
  TEST_IMPL((999 ^ 1), 999, 1);
  TEST_IMPL((999 ^ 999), 999, 999);
  TEST_IMPL((999 ^ faceb00c), 999, faceb00c);
  TEST_IMPL((faceb00c ^ 0), faceb00c, 0);
  TEST_IMPL((faceb00c ^ 1), faceb00c, 1);
  TEST_IMPL((faceb00c ^ 999), faceb00c, 999);
  TEST_IMPL((faceb00c ^ faceb00c), faceb00c, faceb00c);

  TEST_IMPL((0 ^ 0 ^ 0), 0, 0, 0);
  TEST_IMPL((0 ^ 0 ^ 1), 0, 0, 1);
  TEST_IMPL((0 ^ 0 ^ 999), 0, 0, 999);
  TEST_IMPL((0 ^ 0 ^ faceb00c), 0, 0, faceb00c);
  TEST_IMPL((0 ^ 1 ^ 0), 0, 1, 0);
  TEST_IMPL((0 ^ 1 ^ 1), 0, 1, 1);
  TEST_IMPL((0 ^ 1 ^ 999), 0, 1, 999);
  TEST_IMPL((0 ^ 1 ^ faceb00c), 0, 1, faceb00c);
  TEST_IMPL((0 ^ 999 ^ 0), 0, 999, 0);
  TEST_IMPL((0 ^ 999 ^ 1), 0, 999, 1);
  TEST_IMPL((0 ^ 999 ^ 999), 0, 999, 999);
  TEST_IMPL((0 ^ 999 ^ faceb00c), 0, 999, faceb00c);
  TEST_IMPL((0 ^ faceb00c ^ 0), 0, faceb00c, 0);
  TEST_IMPL((0 ^ faceb00c ^ 1), 0, faceb00c, 1);
  TEST_IMPL((0 ^ faceb00c ^ 999), 0, faceb00c, 999);
  TEST_IMPL((0 ^ faceb00c ^ faceb00c), 0, faceb00c, faceb00c);
  TEST_IMPL((1 ^ 0 ^ 0), 1, 0, 0);
  TEST_IMPL((1 ^ 0 ^ 1), 1, 0, 1);
  TEST_IMPL((1 ^ 0 ^ 999), 1, 0, 999);
  TEST_IMPL((1 ^ 0 ^ faceb00c), 1, 0, faceb00c);
  TEST_IMPL((1 ^ 1 ^ 0), 1, 1, 0);
  TEST_IMPL((1 ^ 1 ^ 1), 1, 1, 1);
  TEST_IMPL((1 ^ 1 ^ 999), 1, 1, 999);
  TEST_IMPL((1 ^ 1 ^ faceb00c), 1, 1, faceb00c);
  TEST_IMPL((1 ^ 999 ^ 0), 1, 999, 0);
  TEST_IMPL((1 ^ 999 ^ 1), 1, 999, 1);
  TEST_IMPL((1 ^ 999 ^ 999), 1, 999, 999);
  TEST_IMPL((1 ^ 999 ^ faceb00c), 1, 999, faceb00c);
  TEST_IMPL((1 ^ faceb00c ^ 0), 1, faceb00c, 0);
  TEST_IMPL((1 ^ faceb00c ^ 1), 1, faceb00c, 1);
  TEST_IMPL((1 ^ faceb00c ^ 999), 1, faceb00c, 999);
  TEST_IMPL((1 ^ faceb00c ^ faceb00c), 1, faceb00c, faceb00c);
  TEST_IMPL((999 ^ 0 ^ 0), 999, 0, 0);
  TEST_IMPL((999 ^ 0 ^ 1), 999, 0, 1);
  TEST_IMPL((999 ^ 0 ^ 999), 999, 0, 999);
  TEST_IMPL((999 ^ 0 ^ faceb00c), 999, 0, faceb00c);
  TEST_IMPL((999 ^ 1 ^ 0), 999, 1, 0);
  TEST_IMPL((999 ^ 1 ^ 1), 999, 1, 1);
  TEST_IMPL((999 ^ 1 ^ 999), 999, 1, 999);
  TEST_IMPL((999 ^ 1 ^ faceb00c), 999, 1, faceb00c);
  TEST_IMPL((999 ^ 999 ^ 0), 999, 999, 0);
  TEST_IMPL((999 ^ 999 ^ 1), 999, 999, 1);
  TEST_IMPL((999 ^ 999 ^ 999), 999, 999, 999);
  TEST_IMPL((999 ^ 999 ^ faceb00c), 999, 999, faceb00c);
  TEST_IMPL((999 ^ faceb00c ^ 0), 999, faceb00c, 0);
  TEST_IMPL((999 ^ faceb00c ^ 1), 999, faceb00c, 1);
  TEST_IMPL((999 ^ faceb00c ^ 999), 999, faceb00c, 999);
  TEST_IMPL((999 ^ faceb00c ^ faceb00c), 999, faceb00c, faceb00c);
  TEST_IMPL((faceb00c ^ 0 ^ 0), faceb00c, 0, 0);
  TEST_IMPL((faceb00c ^ 0 ^ 1), faceb00c, 0, 1);
  TEST_IMPL((faceb00c ^ 0 ^ 999), faceb00c, 0, 999);
  TEST_IMPL((faceb00c ^ 0 ^ faceb00c), faceb00c, 0, faceb00c);
  TEST_IMPL((faceb00c ^ 1 ^ 0), faceb00c, 1, 0);
  TEST_IMPL((faceb00c ^ 1 ^ 1), faceb00c, 1, 1);
  TEST_IMPL((faceb00c ^ 1 ^ 999), faceb00c, 1, 999);
  TEST_IMPL((faceb00c ^ 1 ^ faceb00c), faceb00c, 1, faceb00c);
  TEST_IMPL((faceb00c ^ 999 ^ 0), faceb00c, 999, 0);
  TEST_IMPL((faceb00c ^ 999 ^ 1), faceb00c, 999, 1);
  TEST_IMPL((faceb00c ^ 999 ^ 999), faceb00c, 999, 999);
  TEST_IMPL((faceb00c ^ 999 ^ faceb00c), faceb00c, 999, faceb00c);
  TEST_IMPL((faceb00c ^ faceb00c ^ 0), faceb00c, faceb00c, 0);
  TEST_IMPL((faceb00c ^ faceb00c ^ 1), faceb00c, faceb00c, 1);
  TEST_IMPL((faceb00c ^ faceb00c ^ 999), faceb00c, faceb00c, 999);
  TEST_IMPL(
    (faceb00c ^ faceb00c ^ faceb00c),
    faceb00c, faceb00c, faceb00c
  );

#undef TEST_IMPL
}

FATAL_TEST(functional, placement_forward) {
  placement_forward f;

  using refc = ref_counter<>;
  refc::guard guard;

  uninitialized<refc, false> u;
  auto p = u.ptr();

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  FATAL_EXPECT_EQ(p, f(p));
  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  u.destroy();
  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  {
    refc r1;
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p, std::move(r1)));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(0, refc::valid());

    {
      refc r2;
      FATAL_ASSERT_EQ(2, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());

      FATAL_EXPECT_EQ(p, f(p, r2));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(2, refc::valid());

      u.destroy();
      FATAL_ASSERT_EQ(2, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());

      FATAL_EXPECT_EQ(p, f(p, std::move(r2)));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());

      u.destroy();
      FATAL_ASSERT_EQ(2, refc::alive());
      FATAL_ASSERT_EQ(0, refc::valid());
    }

    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(0, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());
}

FATAL_TEST(functional, placement_copy) {
  placement_copy f;

  using refc = ref_counter<>;
  refc::guard guard;

  uninitialized<refc, false> u;
  auto p = u.ptr();

  {
    refc r1;
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p, std::move(r1)));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());
}

FATAL_TEST(functional, placement_move) {
  placement_move f;

  using refc = ref_counter<>;
  refc::guard guard;

  uninitialized<refc, false> u;
  auto p = u.ptr();

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  FATAL_EXPECT_EQ(p, f(p));
  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  u.destroy();
  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  {
    refc r1;
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    FATAL_EXPECT_EQ(p, f(p, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    u.destroy();
    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(0, refc::valid());

    {
      refc r2;
      FATAL_ASSERT_EQ(2, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());

      FATAL_EXPECT_EQ(p, f(p, r2));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());

      u.destroy();
      FATAL_ASSERT_EQ(2, refc::alive());
      FATAL_ASSERT_EQ(0, refc::valid());
    }

    FATAL_ASSERT_EQ(1, refc::alive());
    FATAL_ASSERT_EQ(0, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());
}

FATAL_TEST(functional, forward_assigner) {
  forward_assigner f;

  using refc = ref_counter<>;
  refc::guard guard;

  refc r;
  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  {
    refc r1;
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    FATAL_EXPECT_EQ(&r, &f(r, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    FATAL_EXPECT_EQ(&r, &f(r, std::move(r1)));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());

    {
      refc r2;
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(2, refc::valid());

      FATAL_EXPECT_EQ(&r, &f(r, r2));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(2, refc::valid());

      FATAL_EXPECT_EQ(&r, &f(r, std::move(r2)));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(1, refc::valid());
    }

    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());
  }

  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());
}

FATAL_TEST(functional, copy_assigner) {
  copy_assigner f;

  using refc = ref_counter<>;
  refc::guard guard;

  refc r;
  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  {
    refc r1;
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    FATAL_EXPECT_EQ(&r, &f(r, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    FATAL_EXPECT_EQ(&r, &f(r, std::move(r1)));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    {
      refc r2;
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(3, refc::valid());

      FATAL_EXPECT_EQ(&r, &f(r, r2));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(3, refc::valid());

      FATAL_EXPECT_EQ(&r, &f(r, std::move(r2)));
      FATAL_ASSERT_EQ(3, refc::alive());
      FATAL_ASSERT_EQ(3, refc::valid());
    }

    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());
  }

  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());
}

FATAL_TEST(functional, move_assigner) {
  move_assigner f;

  using refc = ref_counter<>;
  refc::guard guard;

  refc r;
  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  {
    refc r1;
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(2, refc::valid());

    FATAL_EXPECT_EQ(&r, &f(r, r1));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());
  }

  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());

  {
    refc r2;
    FATAL_EXPECT_EQ(&r, &f(r, std::move(r2)));
    FATAL_ASSERT_EQ(2, refc::alive());
    FATAL_ASSERT_EQ(1, refc::valid());
  }

  FATAL_ASSERT_EQ(1, refc::alive());
  FATAL_ASSERT_EQ(1, refc::valid());
}

FATAL_TEST(functional, tuple_comparer) {
  using tuple = std::tuple<int, int, int, int>;

# define TEST_IMPL(Indexes, ...) \
  do { \
    auto actual = data; \
    tuple_comparer<FATAL_UNPARENTHESIZE(Indexes)> comparer; \
    std::stable_sort(actual.begin(), actual.end(), comparer); \
    std::vector<tuple> const expected __VA_ARGS__; \
    FATAL_EXPECT_EQ(expected, actual); \
  } while (false)

  std::vector<tuple> const data{
    tuple{ 1, -1, -5,  5},
    tuple{ 4, -1, -3,  1},
    tuple{-5, -5, -5,  2},
    tuple{-3, -4, -3, -1},
    tuple{ 3,  0, -2,  0},
    tuple{ 1, -2,  5, -3},
    tuple{-3,  3, -3, -5},
    tuple{ 1, -5,  1, -3},
    tuple{ 0,  3, -2, -5},
    tuple{-2,  0,  1,  2},
    tuple{ 1,  2, -1,  0},
    tuple{ 4,  3, -1, -1},
    tuple{ 4,  0,  0,  1},
    tuple{ 0, -1,  5,  1},
    tuple{-3, -3, -5,  0},
    tuple{ 5,  1, -2, -2},
    tuple{ 5, -2,  1,  3},
    tuple{ 5, -5,  0,  0},
    tuple{-5, -2,  4, -2},
    tuple{ 3,  0, -1,  4}
  };

  TEST_IMPL(0, {
    tuple{-5, -5, -5,  2},
    tuple{-5, -2,  4, -2},
    tuple{-3, -4, -3, -1},
    tuple{-3,  3, -3, -5},
    tuple{-3, -3, -5,  0},
    tuple{-2,  0,  1,  2},
    tuple{ 0,  3, -2, -5},
    tuple{ 0, -1,  5,  1},
    tuple{ 1, -1, -5,  5},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{ 1,  2, -1,  0},
    tuple{ 3,  0, -2,  0},
    tuple{ 3,  0, -1,  4},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  3, -1, -1},
    tuple{ 4,  0,  0,  1},
    tuple{ 5,  1, -2, -2},
    tuple{ 5, -2,  1,  3},
    tuple{ 5, -5,  0,  0}
  });

  TEST_IMPL(-1, {
    tuple{ 5,  1, -2, -2},
    tuple{ 5, -2,  1,  3},
    tuple{ 5, -5,  0,  0},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  3, -1, -1},
    tuple{ 4,  0,  0,  1},
    tuple{ 3,  0, -2,  0},
    tuple{ 3,  0, -1,  4},
    tuple{ 1, -1, -5,  5},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{ 1,  2, -1,  0},
    tuple{ 0,  3, -2, -5},
    tuple{ 0, -1,  5,  1},
    tuple{-2,  0,  1,  2},
    tuple{-3, -4, -3, -1},
    tuple{-3,  3, -3, -5},
    tuple{-3, -3, -5,  0},
    tuple{-5, -5, -5,  2},
    tuple{-5, -2,  4, -2}
  });

  TEST_IMPL(1, {
    tuple{-5, -5, -5,  2},
    tuple{ 1, -5,  1, -3},
    tuple{ 5, -5,  0,  0},
    tuple{-3, -4, -3, -1},
    tuple{-3, -3, -5,  0},
    tuple{ 1, -2,  5, -3},
    tuple{ 5, -2,  1,  3},
    tuple{-5, -2,  4, -2},
    tuple{ 1, -1, -5,  5},
    tuple{ 4, -1, -3,  1},
    tuple{ 0, -1,  5,  1},
    tuple{ 3,  0, -2,  0},
    tuple{-2,  0,  1,  2},
    tuple{ 4,  0,  0,  1},
    tuple{ 3,  0, -1,  4},
    tuple{ 5,  1, -2, -2},
    tuple{ 1,  2, -1,  0},
    tuple{-3,  3, -3, -5},
    tuple{ 0,  3, -2, -5},
    tuple{ 4,  3, -1, -1}
  });

  TEST_IMPL(-2, {
    tuple{-3,  3, -3, -5},
    tuple{ 0,  3, -2, -5},
    tuple{ 4,  3, -1, -1},
    tuple{ 1,  2, -1,  0},
    tuple{ 5,  1, -2, -2},
    tuple{ 3,  0, -2,  0},
    tuple{-2,  0,  1,  2},
    tuple{ 4,  0,  0,  1},
    tuple{ 3,  0, -1,  4},
    tuple{ 1, -1, -5,  5},
    tuple{ 4, -1, -3,  1},
    tuple{ 0, -1,  5,  1},
    tuple{ 1, -2,  5, -3},
    tuple{ 5, -2,  1,  3},
    tuple{-5, -2,  4, -2},
    tuple{-3, -3, -5,  0},
    tuple{-3, -4, -3, -1},
    tuple{-5, -5, -5,  2},
    tuple{ 1, -5,  1, -3},
    tuple{ 5, -5,  0,  0}
  });

  TEST_IMPL(2, {
    tuple{ 1, -1, -5,  5},
    tuple{-5, -5, -5,  2},
    tuple{-3, -3, -5,  0},
    tuple{ 4, -1, -3,  1},
    tuple{-3, -4, -3, -1},
    tuple{-3,  3, -3, -5},
    tuple{ 3,  0, -2,  0},
    tuple{ 0,  3, -2, -5},
    tuple{ 5,  1, -2, -2},
    tuple{ 1,  2, -1,  0},
    tuple{ 4,  3, -1, -1},
    tuple{ 3,  0, -1,  4},
    tuple{ 4,  0,  0,  1},
    tuple{ 5, -5,  0,  0},
    tuple{ 1, -5,  1, -3},
    tuple{-2,  0,  1,  2},
    tuple{ 5, -2,  1,  3},
    tuple{-5, -2,  4, -2},
    tuple{ 1, -2,  5, -3},
    tuple{ 0, -1,  5,  1}
  });

  TEST_IMPL(-3, {
    tuple{ 1, -2,  5, -3},
    tuple{ 0, -1,  5,  1},
    tuple{-5, -2,  4, -2},
    tuple{ 1, -5,  1, -3},
    tuple{-2,  0,  1,  2},
    tuple{ 5, -2,  1,  3},
    tuple{ 4,  0,  0,  1},
    tuple{ 5, -5,  0,  0},
    tuple{ 1,  2, -1,  0},
    tuple{ 4,  3, -1, -1},
    tuple{ 3,  0, -1,  4},
    tuple{ 3,  0, -2,  0},
    tuple{ 0,  3, -2, -5},
    tuple{ 5,  1, -2, -2},
    tuple{ 4, -1, -3,  1},
    tuple{-3, -4, -3, -1},
    tuple{-3,  3, -3, -5},
    tuple{ 1, -1, -5,  5},
    tuple{-5, -5, -5,  2},
    tuple{-3, -3, -5,  0}
  });

  TEST_IMPL(3, {
    tuple{-3,  3, -3, -5},
    tuple{ 0,  3, -2, -5},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{ 5,  1, -2, -2},
    tuple{-5, -2,  4, -2},
    tuple{-3, -4, -3, -1},
    tuple{ 4,  3, -1, -1},
    tuple{ 3,  0, -2,  0},
    tuple{ 1,  2, -1,  0},
    tuple{-3, -3, -5,  0},
    tuple{ 5, -5,  0,  0},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  0,  0,  1},
    tuple{ 0, -1,  5,  1},
    tuple{-5, -5, -5,  2},
    tuple{-2,  0,  1,  2},
    tuple{ 5, -2,  1,  3},
    tuple{ 3,  0, -1,  4},
    tuple{ 1, -1, -5,  5}
  });

  TEST_IMPL(-4, {
    tuple{ 1, -1, -5,  5},
    tuple{ 3,  0, -1,  4},
    tuple{ 5, -2,  1,  3},
    tuple{-5, -5, -5,  2},
    tuple{-2,  0,  1,  2},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  0,  0,  1},
    tuple{ 0, -1,  5,  1},
    tuple{ 3,  0, -2,  0},
    tuple{ 1,  2, -1,  0},
    tuple{-3, -3, -5,  0},
    tuple{ 5, -5,  0,  0},
    tuple{-3, -4, -3, -1},
    tuple{ 4,  3, -1, -1},
    tuple{ 5,  1, -2, -2},
    tuple{-5, -2,  4, -2},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{-3,  3, -3, -5},
    tuple{ 0,  3, -2, -5}
  });

  TEST_IMPL((0, 1, 2, 3), {
    tuple{-5, -5, -5,  2},
    tuple{-5, -2,  4, -2},
    tuple{-3, -4, -3, -1},
    tuple{-3, -3, -5,  0},
    tuple{-3,  3, -3, -5},
    tuple{-2,  0,  1,  2},
    tuple{ 0, -1,  5,  1},
    tuple{ 0,  3, -2, -5},
    tuple{ 1, -5,  1, -3},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -1, -5,  5},
    tuple{ 1,  2, -1,  0},
    tuple{ 3,  0, -2,  0},
    tuple{ 3,  0, -1,  4},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  0,  0,  1},
    tuple{ 4,  3, -1, -1},
    tuple{ 5, -5,  0,  0},
    tuple{ 5, -2,  1,  3},
    tuple{ 5,  1, -2, -2}
  });

  TEST_IMPL((-1, -2, -3, -4), {
    tuple{ 5,  1, -2, -2},
    tuple{ 5, -2,  1,  3},
    tuple{ 5, -5,  0,  0},
    tuple{ 4,  3, -1, -1},
    tuple{ 4,  0,  0,  1},
    tuple{ 4, -1, -3,  1},
    tuple{ 3,  0, -1,  4},
    tuple{ 3,  0, -2,  0},
    tuple{ 1,  2, -1,  0},
    tuple{ 1, -1, -5,  5},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{ 0,  3, -2, -5},
    tuple{ 0, -1,  5,  1},
    tuple{-2,  0,  1,  2},
    tuple{-3,  3, -3, -5},
    tuple{-3, -3, -5,  0},
    tuple{-3, -4, -3, -1},
    tuple{-5, -2,  4, -2},
    tuple{-5, -5, -5,  2}
  });

  TEST_IMPL((3, 2, 1, 0), {
    tuple{-3,  3, -3, -5},
    tuple{ 0,  3, -2, -5},
    tuple{ 1, -5,  1, -3},
    tuple{ 1, -2,  5, -3},
    tuple{ 5,  1, -2, -2},
    tuple{-5, -2,  4, -2},
    tuple{-3, -4, -3, -1},
    tuple{ 4,  3, -1, -1},
    tuple{-3, -3, -5,  0},
    tuple{ 3,  0, -2,  0},
    tuple{ 1,  2, -1,  0},
    tuple{ 5, -5,  0,  0},
    tuple{ 4, -1, -3,  1},
    tuple{ 4,  0,  0,  1},
    tuple{ 0, -1,  5,  1},
    tuple{-5, -5, -5,  2},
    tuple{-2,  0,  1,  2},
    tuple{ 5, -2,  1,  3},
    tuple{ 3,  0, -1,  4},
    tuple{ 1, -1, -5,  5}
  });

  TEST_IMPL((-4, -3, -2, -1), {
    tuple{ 1, -1, -5,  5},
    tuple{ 3,  0, -1,  4},
    tuple{ 5, -2,  1,  3},
    tuple{-2,  0,  1,  2},
    tuple{-5, -5, -5,  2},
    tuple{ 0, -1,  5,  1},
    tuple{ 4,  0,  0,  1},
    tuple{ 4, -1, -3,  1},
    tuple{ 5, -5,  0,  0},
    tuple{ 1,  2, -1,  0},
    tuple{ 3,  0, -2,  0},
    tuple{-3, -3, -5,  0},
    tuple{ 4,  3, -1, -1},
    tuple{-3, -4, -3, -1},
    tuple{-5, -2,  4, -2},
    tuple{ 5,  1, -2, -2},
    tuple{ 1, -2,  5, -3},
    tuple{ 1, -5,  1, -3},
    tuple{ 0,  3, -2, -5},
    tuple{-3,  3, -3, -5}
  });
# undef TEST_IMPL
}

} // namespace fn {
} // namespace fatal {
