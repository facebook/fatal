/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/string/rope.h>

#include <fatal/test/driver.h>

#include <fatal/math/numerics.h>
#include <fatal/utility/timed_iterations.h>

#include <sstream>
#include <string>
#include <utility>

namespace fatal {

#define TEST_IMPL_SINGLE_STRING(Fn) \
  Fn(""); \
  Fn('1'); \
  Fn("1"); \
  Fn(std::string("1")); \
  Fn("12"); \
  Fn(std::string("12")); \
  Fn("123"); \
  Fn(std::string("123")); \
  Fn("hello, world! with some extra strings"); \
  Fn(std::string("hello, world! with some extra strings")); \
  Fn( \
    std::string("hello"), ", ", std::string("world"), '!', " with", \
    ' ', std::string("some"), " extra", " ", 's', std::string("trings") \
  ) \

//////////////////
// sanity_check //
//////////////////

FATAL_TEST(sanity_check, sanity_check) {
  std::string const world("world");
  std::string extra(" extra");

  rope<> s(
    std::string("hello"), ", ", world, '!',
    " with", ' ', std::string("some"), extra, " ", 's', std::string("trings")
  );

  std::string const expected("hello, world! with some extra strings");

  FATAL_EXPECT_EQ(expected, s);

  auto const r = s.to_string();
  FATAL_EXPECT_EQ(expected, r);

  std::ostringstream ss;
  ss << s;
  FATAL_EXPECT_EQ(expected, ss.str());

  rope<> unitary;
  unitary.append(expected);

  auto const u = unitary.to_string();

  FATAL_EXPECT_EQ(expected, u);
  FATAL_EXPECT_EQ(r, u);

  FATAL_EXPECT_EQ(expected, unitary);
  FATAL_EXPECT_EQ(s, unitary);
  FATAL_EXPECT_EQ(unitary, s);

  rope<>::hasher hasher;

  auto const hr1 = hasher(s);
  auto const hu1 = hasher(unitary);
  auto const hr2 = rope<>::hasher()(s);
  auto const hu2 = rope<>::hasher()(unitary);

  FATAL_EXPECT_EQ(hr1, hr2);
  FATAL_EXPECT_EQ(hu1, hu2);

  FATAL_EXPECT_EQ(hr1, hu1);
}

//////////
// size //
//////////

FATAL_TEST(sanity_check, size) {
  rope<> r;
  FATAL_EXPECT_EQ(0, r.size());

  r.push_back('0');
  FATAL_EXPECT_EQ(1, r.size());

  r.append("12");
  FATAL_EXPECT_EQ(3, r.size());

  r.append(std::string("456"));
  FATAL_EXPECT_EQ(6, r.size());

  r.clear();
  FATAL_EXPECT_EQ(0, r.size());
}

///////////
// empty //
///////////

FATAL_TEST(sanity_check, empty) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  r.push_back('0');
  FATAL_EXPECT_FALSE(r.empty());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());

  r.append("12");
  FATAL_EXPECT_FALSE(r.empty());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());

  r.append(std::string("456"));
  FATAL_EXPECT_FALSE(r.empty());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
}

////////////////
// comparison //
////////////////

# define TEST_IMPL_COMPARE(r, other) \
  do { \
    FATAL_EXPECT_EQ(0, r.compare(other)); \
    \
    FATAL_EXPECT_TRUE(r == other); \
    FATAL_EXPECT_TRUE(other == r); \
    \
    FATAL_EXPECT_FALSE(r != other); \
    FATAL_EXPECT_FALSE(other != r); \
    \
    FATAL_EXPECT_FALSE(r < other); \
    FATAL_EXPECT_FALSE(other < r); \
    \
    FATAL_EXPECT_TRUE(r <= other); \
    FATAL_EXPECT_TRUE(other <= r); \
    \
    FATAL_EXPECT_FALSE(r > other); \
    FATAL_EXPECT_FALSE(other > r); \
    \
    FATAL_EXPECT_TRUE(r >= other); \
    FATAL_EXPECT_TRUE(other >= r); \
  } while (false)

FATAL_TEST(comparison, equal) {
# define TEST_IMPL(...) \
  do { \
    auto const str = to_string(__VA_ARGS__); \
    rope<> r1(__VA_ARGS__); \
    auto cr1 = r1.mimic(); \
    rope<> r2(__VA_ARGS__); \
    auto cr2 = r2.mimic(); \
    string_view ref(str); \
    auto cstr = str.c_str(); \
    std::vector<char> v(str.cbegin(), str.cend()); \
    v.push_back('\0'); \
    auto c = v.data(); \
    std::string s(str); \
    \
    TEST_IMPL_COMPARE(r1, r1); \
    TEST_IMPL_COMPARE(cr1, r1); \
    TEST_IMPL_COMPARE(r1, cr1); \
    TEST_IMPL_COMPARE(r1, r2); \
    TEST_IMPL_COMPARE(cr1, r2); \
    TEST_IMPL_COMPARE(r1, cr2); \
    TEST_IMPL_COMPARE(r1, ref); \
    TEST_IMPL_COMPARE(r1, cstr); \
    TEST_IMPL_COMPARE(r1, c); \
    TEST_IMPL_COMPARE(r1, s); \
    TEST_IMPL_COMPARE(cr1, ref); \
    TEST_IMPL_COMPARE(cr1, cstr); \
    TEST_IMPL_COMPARE(cr1, c); \
    TEST_IMPL_COMPARE(cr1, s); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(comparison, equal_const) {
# define TEST_IMPL(...) \
  do { \
    auto const str = to_string(__VA_ARGS__); \
    rope<> const r1(__VA_ARGS__); \
    auto const cr1 = r1.mimic(); \
    rope<> const r2(__VA_ARGS__); \
    auto const cr2 = r2.mimic(); \
    string_view const ref(str); \
    auto const cstr = str.c_str(); \
    std::vector<char> v(str.cbegin(), str.cend()); \
    v.push_back('\0'); \
    auto const c = v.data(); \
    std::string const s(str); \
    \
    TEST_IMPL_COMPARE(r1, r1); \
    TEST_IMPL_COMPARE(cr1, r1); \
    TEST_IMPL_COMPARE(r1, cr1); \
    TEST_IMPL_COMPARE(r1, r2); \
    TEST_IMPL_COMPARE(cr1, r2); \
    TEST_IMPL_COMPARE(r1, cr2); \
    TEST_IMPL_COMPARE(r1, ref); \
    TEST_IMPL_COMPARE(r1, cstr); \
    TEST_IMPL_COMPARE(r1, c); \
    TEST_IMPL_COMPARE(r1, s); \
    TEST_IMPL_COMPARE(cr1, ref); \
    TEST_IMPL_COMPARE(cr1, cstr); \
    TEST_IMPL_COMPARE(cr1, c); \
    TEST_IMPL_COMPARE(cr1, s); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(comparison, equal_move) {
# define TEST_IMPL(...) \
  do { \
    auto const str = to_string(__VA_ARGS__); \
    rope<> r1(__VA_ARGS__); \
    auto cr1 = r1.mimic(); \
    rope<> r2(__VA_ARGS__); \
    auto cr2 = r2.mimic(); \
    string_view ref(str); \
    auto cstr = str.c_str(); \
    std::vector<char> v(str.cbegin(), str.cend()); \
    v.push_back('\0'); \
    auto c = v.data(); \
    std::string s(str); \
    \
    TEST_IMPL_COMPARE(std::move(r1), std::move(r1)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(r1)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(cr1)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(r2)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(r2)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(cr2)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(ref)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(cstr)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(c)); \
    TEST_IMPL_COMPARE(std::move(r1), std::move(s)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(ref)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(cstr)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(c)); \
    TEST_IMPL_COMPARE(std::move(cr1), std::move(s)); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(comparison, equal_other) {
# define TEST_IMPL(...) \
  do { \
    auto const str = to_string(__VA_ARGS__); \
    rope<> r1(__VA_ARGS__); \
    auto cr1 = r1.mimic(); \
    std::vector<char> v(str.cbegin(), str.cend()); \
    v.push_back('\0'); \
    \
    TEST_IMPL_COMPARE(r1, rope<>(__VA_ARGS__)); \
    TEST_IMPL_COMPARE(cr1, rope<>(__VA_ARGS__)); \
    TEST_IMPL_COMPARE(r1, rope<>(__VA_ARGS__).mimic()); \
    TEST_IMPL_COMPARE(cr1, rope<>(__VA_ARGS__).mimic()); \
    TEST_IMPL_COMPARE(r1, string_view(str)); \
    TEST_IMPL_COMPARE(r1, str.c_str()); \
    TEST_IMPL_COMPARE(r1, v.data()); \
    TEST_IMPL_COMPARE(r1, std::string(str)); \
    TEST_IMPL_COMPARE(cr1, string_view(str)); \
    TEST_IMPL_COMPARE(cr1, str.c_str()); \
    TEST_IMPL_COMPARE(cr1, v.data()); \
    TEST_IMPL_COMPARE(cr1, std::string(str)); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

# undef TEST_IMPL_COMPARE

FATAL_TEST(comparison, not_equal) {
# define TEST_IMPL_COMPARE(r, other, expected_less) \
  do { \
    if (expected_less) { \
      FATAL_EXPECT_LT(r.compare(other), 0); \
    } else { \
      FATAL_EXPECT_GT(r.compare(other), 0); \
    } \
    \
    FATAL_EXPECT_FALSE(r == other); \
    FATAL_EXPECT_FALSE(other == r); \
    \
    FATAL_EXPECT_TRUE(r != other); \
    FATAL_EXPECT_TRUE(other != r); \
    \
    FATAL_EXPECT_EQ(expected_less, r < other); \
    FATAL_EXPECT_NE(expected_less, other < r); \
    \
    FATAL_EXPECT_EQ(expected_less, r <= other); \
    FATAL_EXPECT_NE(expected_less, other <= r); \
    \
    FATAL_EXPECT_NE(expected_less, r > other); \
    FATAL_EXPECT_EQ(expected_less, other > r); \
    \
    FATAL_EXPECT_NE(expected_less, r >= other); \
    FATAL_EXPECT_EQ(expected_less, other >= r); \
  } while (false)

  std::vector<rope<>> v;
  TEST_IMPL_SINGLE_STRING(v.emplace_back);

  for (auto l = v.size(); l--; ) {
    for (auto r = v.size(); r--; ) {
      auto ls = v[l].to_string();
      auto rs = v[r].to_string();

      if (l == r || ls == rs) {
        continue;
      }

      // TODO: ADD MIMICS
      auto test = [](
        rope<> &lhs,
        rope<> &rhs,
        bool expected_less
      ) {
        auto const lstr = lhs.to_string();
        auto const rstr = rhs.to_string();
        {
          auto &r1 = lhs;
          auto &r2 = rhs;
          string_view ref(rstr);
          auto cstr = rstr.c_str();
          std::vector<char> vc(rstr.cbegin(), rstr.cend());
          vc.push_back('\0');
          auto c = vc.data();
          std::string s(rstr);

          TEST_IMPL_COMPARE(r1, r2, expected_less);
          TEST_IMPL_COMPARE(r1, ref, expected_less);
          TEST_IMPL_COMPARE(r1, cstr, expected_less);
          TEST_IMPL_COMPARE(r1, c, expected_less);
          TEST_IMPL_COMPARE(r1, s, expected_less);
        }
        {
          auto const &r1 = lhs;
          auto const &r2 = rhs;
          string_view const ref(rstr);
          auto const cstr = rstr.c_str();
          std::vector<char> vc(rstr.cbegin(), rstr.cend());
          vc.push_back('\0');
          auto const c = vc.data();
          std::string const s(rstr);

          TEST_IMPL_COMPARE(r1, r2, expected_less);
          TEST_IMPL_COMPARE(r1, ref, expected_less);
          TEST_IMPL_COMPARE(r1, cstr, expected_less);
          TEST_IMPL_COMPARE(r1, c, expected_less);
          TEST_IMPL_COMPARE(r1, s, expected_less);
        }
        {
          auto &r1 = lhs;
          auto &r2 = rhs;
          string_view ref(rstr);
          auto cstr = rstr.c_str();
          std::vector<char> vc(rstr.cbegin(), rstr.cend());
          vc.push_back('\0');
          auto c = vc.data();
          std::string s(rstr);

          TEST_IMPL_COMPARE(std::move(r1), std::move(r2), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(ref), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(cstr), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(c), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(s), expected_less);
        }
        {
          auto &r1 = lhs;
          std::vector<char> vc(rstr.cbegin(), rstr.cend());
          vc.push_back('\0');

          TEST_IMPL_COMPARE(r1, string_view(rstr), expected_less);
          TEST_IMPL_COMPARE(r1, rstr.c_str(), expected_less);
          TEST_IMPL_COMPARE(r1, vc.data(), expected_less);
          TEST_IMPL_COMPARE(r1, std::string(rstr), expected_less);
        }

        FATAL_ASSERT_EQ(lhs, lstr);
        FATAL_ASSERT_EQ(rhs, rstr);
      };

      test(v[l], v[r], ls < rs);
      test(v[r], v[l], rs < ls);
    }
  }

# undef TEST_IMPL_COMPARE
}

///////////////
// push_back //
///////////////

FATAL_TEST(push_back, char) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);

  r.push_back('1');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1", r);

  char space = ' ';
  r.push_back(space);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1 ", r);

  char two = '2';
  r.push_back(std::move(two));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1 2", r);

  r.push_back(char(' '));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1 2 ", r);

  char const three = '3';
  r.push_back(three);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1 2 3", r);

  auto end = "!";
  r.push_back(end[0]);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("1 2 3!", r);
}

////////////
// append //
////////////

FATAL_TEST(append, append) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append("");
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append("one");
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.append(' ');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.append(std::string("two"));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  r.append(string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two ", r);
  FATAL_EXPECT_EQ(4, r.pieces());

  std::string three("three");
  r.append(three);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three", r);
  FATAL_EXPECT_EQ(5, r.pieces());

  std::string f(" f");
  r.append(std::move(f));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three f", r);
  FATAL_EXPECT_EQ(6, r.pieces());

  std::string const o("o");
  r.append(o);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three fo", r);
  FATAL_EXPECT_EQ(7, r.pieces());

  string_view const ur("ur");
  r.append(ur);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four", r);
  FATAL_EXPECT_EQ(8, r.pieces());

  string_view end("!");
  r.append(end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four!", r);
  FATAL_EXPECT_EQ(9, r.pieces());
}

//////////////////
// multi_append //
//////////////////

FATAL_TEST(multi_append, none) {
  rope<> r;

  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("");
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

FATAL_TEST(multi_append, one) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one");
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.multi_append(' ');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.multi_append(std::string("two"));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  r.multi_append(string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two ", r);
  FATAL_EXPECT_EQ(4, r.pieces());

  std::string three("three");
  r.multi_append(three);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three", r);
  FATAL_EXPECT_EQ(5, r.pieces());

  std::string f(" f");
  r.multi_append(std::move(f));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three f", r);
  FATAL_EXPECT_EQ(6, r.pieces());

  std::string const o("o");
  r.multi_append(o);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three fo", r);
  FATAL_EXPECT_EQ(7, r.pieces());

  string_view const ur("ur");
  r.multi_append(ur);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four", r);
  FATAL_EXPECT_EQ(8, r.pieces());

  string_view end("!");
  r.multi_append(end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four!", r);
  FATAL_EXPECT_EQ(9, r.pieces());
}

FATAL_TEST(multi_append, two) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.multi_append(std::string("two"), string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two ", r);
  FATAL_EXPECT_EQ(4, r.pieces());

  std::string three("three");
  string_view space(" ");
  r.multi_append(three, space);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three ", r);
  FATAL_EXPECT_EQ(6, r.pieces());

  std::string f("f");
  std::string const o("o");
  r.multi_append(std::move(f), o);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three fo", r);
  FATAL_EXPECT_EQ(8, r.pieces());

  string_view const ur("ur");
  string_view end("!");
  r.multi_append(ur, end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four!", r);
  FATAL_EXPECT_EQ(10, r.pieces());
}

FATAL_TEST(multi_append, three) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ', std::string("two"));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  std::string three("three");
  std::string f(" f");

  r.multi_append(string_view(" "), three, std::move(f));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three f", r);
  FATAL_EXPECT_EQ(6, r.pieces());

  std::string const o("o");
  string_view const ur("ur");
  string_view end("!");

  r.multi_append(o, ur, end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four!", r);
  FATAL_EXPECT_EQ(9, r.pieces());
}

FATAL_TEST(multi_append, many) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ', std::string("two"), string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two ", r);
  FATAL_EXPECT_EQ(4, r.pieces());

  std::string three("three");
  std::string const ou("ou");
  string_view end("!");

  r.multi_append(three, " ", 'f', ou, std::string("r"), end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two three four!", r);
  FATAL_EXPECT_EQ(10, r.pieces());
}

////////////
// concat //
////////////

FATAL_TEST(concat, empty) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  rope<> other;
  r.concat(other);
  FATAL_EXPECT_TRUE(r.empty());

  r.concat(rope<>());
  FATAL_EXPECT_TRUE(r.empty());

  r.concat(r);
  FATAL_EXPECT_TRUE(r.empty());
}

FATAL_TEST(concat, one_piece) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  std::string const s1("hello");
  rope<> r1((std::string(s1)));
  FATAL_EXPECT_FALSE(r1.empty());
  r.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1, r.to_string());
  FATAL_EXPECT_EQ(s1, r);

  std::string const s2(", ");
  rope<> r2(s2);
  r.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2, r);

  std::string const s3("world");
  r.concat(rope<>(s3));
  FATAL_EXPECT_EQ(s1 + s2 + s3, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3, r);

  std::string const s4("!");
  rope<> const r4(s4);
  r.concat(r4);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, r);

  std::string const s5(" test");
  rope<> const r5(s5);
  r.concat(r5);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5, r);

  std::string const s6(" string");
  rope<> const r6(s6);
  r.concat(r6);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r);

  r.concat(r);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { r.concat(std::move(r)); };
}

FATAL_TEST(concat, two_pieces) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  std::string const s1("hello");
  std::string const s2(", ");

  rope<> r1(std::string(s1), s2);
  FATAL_EXPECT_FALSE(r1.empty());
  r.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2, r);

  std::string const s3("world");
  std::string const s4("!");

  rope<> const r2(s3, s4);
  r.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, r);

  std::string const s5(" test");
  std::string const s6(" string");

  rope<> const r3(s5, s6);
  r.concat(r3);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r);

  r.concat(r);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { r.concat(std::move(r)); };
}

FATAL_TEST(concat, three_pieces) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  std::string const s1("hello");
  std::string const s2(", ");
  std::string const s3("world");

  rope<> r1(std::string(s1), s2, s3);
  FATAL_EXPECT_FALSE(r1.empty());
  r.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2 + s3, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3, r);

  std::string const s4("!");
  std::string const s5(" test");
  std::string const s6(" string");

  rope<> const r2(s4, s5, s6);
  r.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r);

  r.concat(r);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { r.concat(std::move(r)); };
}

FATAL_TEST(concat, many_pieces) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());

  std::string const s1("hello");
  std::string const s2(", ");
  std::string const s3("world");
  std::string const s4("!");
  std::string const s5(" test");
  std::string const s6(" string");

  rope<> r1(std::string(s1), s2, s3, s4, s5, s6);
  FATAL_EXPECT_FALSE(r1.empty());
  r.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, r);

  rope<> r2(std::string(s1), s2, s3, s4, s5, s6);
  r.concat(r2);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r
  );

  r.concat(r);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    r
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { r.concat(std::move(r)); };
}

///////////////
// accessors //
///////////////

FATAL_TEST(accessors, front_back) {
  rope<> r;

  r.append('a');
  FATAL_EXPECT_EQ('a', r.front());
  FATAL_EXPECT_EQ('a', r.back());

  r.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', r.front());
  FATAL_EXPECT_EQ('e', r.back());

  std::string f_g_h_i("_f_g_h_i");
  r.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', r.front());
  FATAL_EXPECT_EQ('i', r.back());

  char const underscore = '_';
  r.append(underscore);
  FATAL_EXPECT_EQ('a', r.front());
  FATAL_EXPECT_EQ('_', r.back());

  r.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', r.front());
  FATAL_EXPECT_EQ('m', r.back());
}

//////////////////////
// at / operator [] //
//////////////////////

FATAL_TEST(accessors, at) {
  rope<> r;
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(0); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(2); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(4); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(18); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size() - 1); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };

  r.append('a');
  FATAL_EXPECT_EQ('a', r.at(0));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(2); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(4); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(18); };
  FATAL_EXPECT_EQ('a', r.at(r.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };

  r.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', r.at(0));
  FATAL_EXPECT_EQ('b', r.at(2));
  FATAL_EXPECT_EQ('c', r.at(4));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(18); };
  FATAL_EXPECT_EQ('e', r.at(r.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };

  std::string f_g_h_i("_f_g_h_i");
  r.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', r.at(0));
  FATAL_EXPECT_EQ('b', r.at(2));
  FATAL_EXPECT_EQ('c', r.at(4));
  FATAL_EXPECT_EQ('h', r.at(14));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(18); };
  FATAL_EXPECT_EQ('i', r.at(r.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };

  char const underscore = '_';
  r.append(underscore);
  FATAL_EXPECT_EQ('a', r.at(0));
  FATAL_EXPECT_EQ('b', r.at(2));
  FATAL_EXPECT_EQ('c', r.at(4));
  FATAL_EXPECT_EQ('h', r.at(14));
  FATAL_EXPECT_EQ('_', r.at(17));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(18); };
  FATAL_EXPECT_EQ('_', r.at(r.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };

  r.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', r.at(0));
  FATAL_EXPECT_EQ('b', r.at(2));
  FATAL_EXPECT_EQ('c', r.at(4));
  FATAL_EXPECT_EQ('h', r.at(14));
  FATAL_EXPECT_EQ('_', r.at(17));
  FATAL_EXPECT_EQ('j', r.at(18));
  FATAL_EXPECT_EQ('m', r.at(r.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { r.at(r.size()); };
}

FATAL_TEST(accessors, operator_index) {
  rope<> r;

  r.append('a');
  FATAL_EXPECT_EQ('a', r[0]);
  FATAL_EXPECT_EQ('a', r[r.size() - 1]);

  r.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', r[0]);
  FATAL_EXPECT_EQ('b', r[2]);
  FATAL_EXPECT_EQ('c', r[4]);
  FATAL_EXPECT_EQ('e', r[r.size() - 1]);

  std::string f_g_h_i("_f_g_h_i");
  r.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', r[0]);
  FATAL_EXPECT_EQ('b', r[2]);
  FATAL_EXPECT_EQ('c', r[4]);
  FATAL_EXPECT_EQ('h', r[14]);
  FATAL_EXPECT_EQ('i', r[r.size() - 1]);

  char const underscore = '_';
  r.append(underscore);
  FATAL_EXPECT_EQ('a', r[0]);
  FATAL_EXPECT_EQ('b', r[2]);
  FATAL_EXPECT_EQ('c', r[4]);
  FATAL_EXPECT_EQ('h', r[14]);
  FATAL_EXPECT_EQ('_', r[17]);
  FATAL_EXPECT_EQ('_', r[r.size() - 1]);

  r.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', r[0]);
  FATAL_EXPECT_EQ('b', r[2]);
  FATAL_EXPECT_EQ('c', r[4]);
  FATAL_EXPECT_EQ('h', r[14]);
  FATAL_EXPECT_EQ('_', r[17]);
  FATAL_EXPECT_EQ('j', r[18]);
  FATAL_EXPECT_EQ('m', r[r.size() - 1]);
}

//////////
// copy //
//////////

// TODO: TEST COPY WITH OFFSET

FATAL_TEST(copy, copy) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    \
    std::vector<char> buffer(r.size()); \
    \
    auto const end = r.copy( \
      buffer.data(), \
      std::next(buffer.data(), signed_cast(buffer.size())) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      r.size(), \
      unsigned_cast(std::distance(buffer.data(), end)) \
    ); \
    FATAL_EXPECT_EQ( \
      buffer.size(), \
      unsigned_cast(std::distance(buffer.data(), end)) \
    ); \
    \
    FATAL_EXPECT_EQ(r, buffer); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

///////////////
// to_string //
///////////////

FATAL_TEST(string, to_string) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    FATAL_EXPECT_EQ(to_string(__VA_ARGS__), r.to_string()); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

///////////////
// append_to //
///////////////

FATAL_TEST(string, append_to) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    std::string s; \
    FATAL_EXPECT_EQ(to_string(__VA_ARGS__), r.append_to(s)); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

//////////////
// capacity //
//////////////

FATAL_TEST(capacity, empty) {
# define TEST_IMPL(SmallBufferSize) \
  do { \
    rope<SmallBufferSize> r; \
    FATAL_EXPECT_EQ(SmallBufferSize, r.capacity()); \
    for (auto i = SmallBufferSize; i--; ) { \
      r.append(' '); \
    } \
    FATAL_EXPECT_EQ(SmallBufferSize, r.capacity()); \
    FATAL_EXPECT_EQ(r.size(), r.capacity()); \
    r.append(' '); \
    FATAL_EXPECT_NE(SmallBufferSize, r.capacity()); \
  } while (false)

  TEST_IMPL(0);
  TEST_IMPL(1);
  TEST_IMPL(2);
  TEST_IMPL(3);
  TEST_IMPL(4);
  TEST_IMPL(5);
  TEST_IMPL(6);
  TEST_IMPL(7);
  TEST_IMPL(8);
  TEST_IMPL(9);
  TEST_IMPL(10);
  TEST_IMPL(11);
  TEST_IMPL(12);
  TEST_IMPL(13);
  TEST_IMPL(14);
  TEST_IMPL(15);
  TEST_IMPL(16);
  TEST_IMPL(17);
  TEST_IMPL(18);
  TEST_IMPL(19);
  TEST_IMPL(20);
  TEST_IMPL(21);
  TEST_IMPL(22);
  TEST_IMPL(23);
  TEST_IMPL(24);
  TEST_IMPL(25);
  TEST_IMPL(26);
  TEST_IMPL(27);
  TEST_IMPL(28);
  TEST_IMPL(29);
  TEST_IMPL(30);
  TEST_IMPL(31);
  TEST_IMPL(32);

# undef TEST_IMPL
}

FATAL_TEST(capacity, reserve_absolute) {
# define TEST_IMPL(SmallBufferSize) \
  do { \
    auto const capacity = SmallBufferSize * 2 + 1; \
    do { \
      rope<SmallBufferSize> r; \
      FATAL_EXPECT_EQ(SmallBufferSize, r.capacity()); \
      r.reserve(capacity); \
      FATAL_EXPECT_EQ(capacity, r.capacity()); \
      r.reserve(SmallBufferSize); \
      FATAL_EXPECT_EQ(capacity, r.capacity()); \
    } while (false); \
    do { \
      rope<SmallBufferSize> r; \
      FATAL_EXPECT_EQ(SmallBufferSize, r.capacity()); \
      r.reserve(capacity, false); \
      FATAL_EXPECT_EQ(capacity, r.capacity()); \
      r.reserve(SmallBufferSize, false); \
      FATAL_EXPECT_EQ(capacity, r.capacity()); \
    } while (false); \
  } while (false)

  TEST_IMPL(0);
  TEST_IMPL(1);
  TEST_IMPL(2);
  TEST_IMPL(3);
  TEST_IMPL(4);
  TEST_IMPL(5);
  TEST_IMPL(6);
  TEST_IMPL(7);
  TEST_IMPL(8);
  TEST_IMPL(9);
  TEST_IMPL(10);
  TEST_IMPL(11);
  TEST_IMPL(12);
  TEST_IMPL(13);
  TEST_IMPL(14);
  TEST_IMPL(15);
  TEST_IMPL(16);
  TEST_IMPL(17);
  TEST_IMPL(18);
  TEST_IMPL(19);
  TEST_IMPL(20);
  TEST_IMPL(21);
  TEST_IMPL(22);
  TEST_IMPL(23);
  TEST_IMPL(24);
  TEST_IMPL(25);
  TEST_IMPL(26);
  TEST_IMPL(27);
  TEST_IMPL(28);
  TEST_IMPL(29);
  TEST_IMPL(30);
  TEST_IMPL(31);
  TEST_IMPL(32);

# undef TEST_IMPL
}

FATAL_TEST(capacity, reserve_relative) {
# define TEST_IMPL(SmallBufferSize) \
  do { \
    rope<SmallBufferSize> r; \
    FATAL_EXPECT_EQ(SmallBufferSize, r.capacity()); \
    auto const additional = SmallBufferSize + 1; \
    auto const capacity = SmallBufferSize + additional; \
    r.reserve(capacity, true); \
    FATAL_EXPECT_EQ(capacity, r.capacity()); \
    r.reserve(SmallBufferSize, true); \
    FATAL_EXPECT_EQ(capacity, r.capacity()); \
    r.reserve(additional, true); \
    FATAL_EXPECT_EQ(capacity, r.capacity()); \
  } while (false)

  TEST_IMPL(0);
  TEST_IMPL(1);
  TEST_IMPL(2);
  TEST_IMPL(3);
  TEST_IMPL(4);
  TEST_IMPL(5);
  TEST_IMPL(6);
  TEST_IMPL(7);
  TEST_IMPL(8);
  TEST_IMPL(9);
  TEST_IMPL(10);
  TEST_IMPL(11);
  TEST_IMPL(12);
  TEST_IMPL(13);
  TEST_IMPL(14);
  TEST_IMPL(15);
  TEST_IMPL(16);
  TEST_IMPL(17);
  TEST_IMPL(18);
  TEST_IMPL(19);
  TEST_IMPL(20);
  TEST_IMPL(21);
  TEST_IMPL(22);
  TEST_IMPL(23);
  TEST_IMPL(24);
  TEST_IMPL(25);
  TEST_IMPL(26);
  TEST_IMPL(27);
  TEST_IMPL(28);
  TEST_IMPL(29);
  TEST_IMPL(30);
  TEST_IMPL(31);
  TEST_IMPL(32);

# undef TEST_IMPL
}

///////////
// clear //
///////////

FATAL_TEST(clear, empty) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

FATAL_TEST(clear, one) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append("one");
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append(' ');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ(" ", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append(std::string("two"));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("two", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.append(string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ(" ", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string three("three");
  r.append(three);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("three", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string f(" f");
  r.append(std::move(f));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ(" f", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string const o("o");
  r.append(o);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("o", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  string_view const ur("ur");
  r.append(ur);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("ur", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  string_view end("!");
  r.append(end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("!", r);
  FATAL_EXPECT_EQ(1, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

FATAL_TEST(clear, two) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ');
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append(std::string("two"), string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("two ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string three("three");
  string_view space(" ");
  r.multi_append(three, space);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("three ", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string f(" f");
  std::string const o("o");
  r.multi_append(std::move(f), o);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ(" fo", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  string_view const ur("ur");
  string_view end("!");
  r.multi_append(ur, end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("ur!", r);
  FATAL_EXPECT_EQ(2, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

FATAL_TEST(clear, three) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ', std::string("two"));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string three("three");
  std::string f(" f");

  r.multi_append(string_view(" "), three, std::move(f));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ(" three f", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string const o("o");
  string_view const ur("ur");
  string_view end("!");

  r.multi_append(o, ur, end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("our!", r);
  FATAL_EXPECT_EQ(3, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

FATAL_TEST(clear, many) {
  rope<> r;
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  r.multi_append("one", ' ', std::string("two"), string_view(" "));
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("one two ", r);
  FATAL_EXPECT_EQ(4, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());

  std::string three("three");
  std::string const ou("ou");
  string_view end("!");

  r.multi_append(three, " ", 'f', ou, std::string("r"), end);
  FATAL_EXPECT_FALSE(r.empty());
  FATAL_EXPECT_EQ("three four!", r);
  FATAL_EXPECT_EQ(6, r.pieces());

  r.clear();
  FATAL_EXPECT_TRUE(r.empty());
  FATAL_EXPECT_EQ("", r);
  FATAL_EXPECT_EQ(0, r.pieces());
}

//////////
// find //
//////////

template <typename T>
void find_char_test(
  std::chrono::milliseconds time,
  std::size_t minimum_iterations,
  T &&fn
) {
  std::string const alphabet(
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
  );

  random_data rdg;

  for (std::size_t size = 30; size; --size) {
    std::string s(size, '\0');
    rdg.string(s.begin(), s.end(), alphabet.data(), alphabet.size());

    auto const begin = s.data();
    auto const end = std::next(begin, signed_cast(s.size()));

    for (timed_iterations<> i(time, minimum_iterations); i.next(); ) {
      rope<> r;
      auto const pieces = rdg.chop(
        begin, end,
        [&](char const *b, char const *e) { r.append(b, e); }
      );
      FATAL_EXPECT_EQ(pieces, r.pieces());
      FATAL_EXPECT_EQ(s, r);

      fn(s, r, alphabet);
    }
  }
}

FATAL_TEST(find, char) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      rope<> const &r,
      std::string const &alphabet
    ) {
      for (auto c: alphabet) {
        auto expected = s.find(c);
        if (expected == std::string::npos) {
          expected = s.size();
        } else {
          assert(expected != s.size());
        }

        auto const actual = r.find(c);
        FATAL_EXPECT_EQ(expected, actual.absolute());
      }
    }
  );
}

FATAL_TEST(find, char_size_type) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      rope<> const &r,
      std::string const &alphabet
    ) {
      for (auto offset = s.size() + 1; offset--; ) {
        for (auto c: alphabet) {
          auto expected = s.find(c, offset);
          if (expected == std::string::npos) {
            expected = s.size();
          } else {
            assert(expected != s.size());
          }

          auto const actual = r.find(c, offset);
          FATAL_EXPECT_EQ(expected, actual.absolute());
        }
      }
    }
  );
}

FATAL_TEST(find, char_const_iterator) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      rope<> const &r,
      std::string const &alphabet
    ) {
      for (auto offset = s.size() + 1; offset--; ) {
        auto const r_offset = std::next(r.begin(), signed_cast(offset));
        FATAL_EXPECT_EQ(offset, r_offset.absolute());

        for (auto c: alphabet) {
          auto expected = s.find(c, offset);
          if (expected == std::string::npos) {
            expected = s.size();
          } else {
            assert(expected != s.size());
          }

          auto const actual = r.find(c, r_offset);
          FATAL_EXPECT_EQ(expected, actual.absolute());
        }
      }
    }
  );
}

/* TODO: IMPLEMENT
FATAL_TEST(find, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}

///////////
// rfind //
///////////

FATAL_TEST(rfind, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}

///////////////////
// find_first_of //
///////////////////

FATAL_TEST(find_first_of, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}

///////////////////////
// find_first_not_of //
///////////////////////

FATAL_TEST(find_first_not_of, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}

//////////////////
// find_last_of //
//////////////////

FATAL_TEST(find_last_of, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}

//////////////////////
// find_last_not_of //
//////////////////////

FATAL_TEST(find_last_not_of, TODO) {
  // TODO: IMPLEMENT
  FATAL_EXPECT_UNREACHABLE();
}
*/

///////////////
// iterators //
///////////////

FATAL_TEST(const_iterator, empty) {
  rope<> r;

  FATAL_EXPECT_EQ(r.cbegin(), r.begin());
  FATAL_EXPECT_EQ(r.cend(), r.end());
  FATAL_EXPECT_EQ(r.cbegin(), r.cend());
  FATAL_EXPECT_EQ(r.begin(), r.end());
}

FATAL_TEST(const_iterator, foreach) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = begin; \
    std::size_t n = 0; \
    for (auto const i: r) { \
      FATAL_ASSERT_LT(j, end); \
      FATAL_EXPECT_EQ(*j, i); \
      ++j; \
      ++n; \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(j, end); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_prefix_operator_inc) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = begin; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = b; \
    std::size_t n = 0; \
    for (; i != e; ++i, ++j, ++n) { \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(j, end); \
    FATAL_EXPECT_EQ(i, e); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_postfix_operator_inc) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = begin; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = b; \
    std::size_t n = 0; \
    for (; i != e; i++, ++j, ++n) { \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(j, end); \
    FATAL_EXPECT_EQ(i, e); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_prefix_operator_dec) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = end; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = e; \
    std::size_t n = 0; \
    for (; i != b; ++n) { \
      --i; \
      --j; \
      FATAL_ASSERT_GE(j, begin); \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(i, b); \
    FATAL_EXPECT_EQ(j, begin); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_postfix_operator_dec) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = end; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = e; \
    std::size_t n = 0; \
    for (; i != b; ++n) { \
      i--; \
      --j; \
      FATAL_ASSERT_GE(j, begin); \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(i, b); \
    FATAL_EXPECT_EQ(j, begin); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_operator_add_assign) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = begin; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = b; \
    std::size_t n = 0; \
    for (; i != e; i += 1, ++j, ++n) { \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(j, end); \
    FATAL_EXPECT_EQ(i, e); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(const_iterator, foreach_with_operator_sub_assign) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    auto const s = to_string(__VA_ARGS__); \
    auto const begin = s.begin(); \
    auto const end = s.end(); \
    auto j = end; \
    auto const b = r.begin(); \
    auto const e = r.end(); \
    auto i = e; \
    std::size_t n = 0; \
    for (; i != b; ++n) { \
      i -= 1; \
      --j; \
      FATAL_ASSERT_GE(j, begin); \
      FATAL_ASSERT_LT(j, end); \
      FATAL_ASSERT_NE(i, e); \
      FATAL_ASSERT_LT(i, e); \
      FATAL_ASSERT_GE(i, b); \
      FATAL_ASSERT_GT(e, i); \
      FATAL_ASSERT_LE(b, i); \
      FATAL_EXPECT_EQ(*j, *i); \
    } \
    FATAL_EXPECT_EQ(n, s.size()); \
    FATAL_EXPECT_EQ(i, b); \
    FATAL_EXPECT_EQ(j, begin); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

//////////
// hash //
//////////

FATAL_TEST(rope, hasher) {
# define TEST_IMPL(...) \
  do { \
    rope<> r1(__VA_ARGS__); \
    rope<> r2(to_string(__VA_ARGS__)); \
    using hasher = rope<>::hasher; \
    FATAL_EXPECT_EQ(hasher()(r1), hasher()(r2)); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

/////////////
// ostream //
/////////////

FATAL_TEST(std, ostream) {
# define TEST_IMPL(...) \
  do { \
    rope<> r(__VA_ARGS__); \
    std::ostringstream ss; \
    ss << r; \
    auto const expected = to_string(__VA_ARGS__); \
    FATAL_EXPECT_EQ(expected, ss.str()); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

} // namespace fatal {
