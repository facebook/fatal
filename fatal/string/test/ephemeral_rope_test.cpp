/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/string/ephemeral_rope.h>

#include <fatal/test/driver.h>

#include <fatal/utility/timed_iterations.h>

#include <functional>
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

  ephemeral_rope<> rope(
    std::string("hello"), ", ", world, '!',
    " with", ' ', std::string("some"), extra, " ", 's', std::string("trings")
  );

  std::string const expected("hello, world! with some extra strings");

  FATAL_EXPECT_EQ(expected, rope);

  auto const r = rope.to_string();
  FATAL_EXPECT_EQ(expected, r);

  std::ostringstream ss;
  ss << rope;
  FATAL_EXPECT_EQ(expected, ss.str());

  ephemeral_rope<> unitary;
  unitary.append(expected);

  auto const u = unitary.to_string();

  FATAL_EXPECT_EQ(expected, u);
  FATAL_EXPECT_EQ(r, u);

  FATAL_EXPECT_EQ(expected, unitary);
  FATAL_EXPECT_EQ(rope, unitary);
  FATAL_EXPECT_EQ(unitary, rope);

  std::hash<ephemeral_rope<>> hasher;

  auto const hr1 = hasher(rope);
  auto const hu1 = hasher(unitary);
  auto const hr2 = std::hash<ephemeral_rope<>>()(rope);
  auto const hu2 = std::hash<ephemeral_rope<>>()(unitary);

  FATAL_EXPECT_EQ(hr1, hr2);
  FATAL_EXPECT_EQ(hu1, hu2);

  FATAL_EXPECT_EQ(hr1, hu1);
}

//////////
// size //
//////////

FATAL_TEST(sanity_check, size) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_EQ(0, rope.size());

  rope.push_back('0');
  FATAL_EXPECT_EQ(1, rope.size());

  rope.append("12");
  FATAL_EXPECT_EQ(3, rope.size());

  rope.append(std::string("456"));
  FATAL_EXPECT_EQ(6, rope.size());

  rope.clear();
  FATAL_EXPECT_EQ(0, rope.size());
}

///////////
// empty //
///////////

FATAL_TEST(sanity_check, empty) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  rope.push_back('0');
  FATAL_EXPECT_FALSE(rope.empty());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());

  rope.append("12");
  FATAL_EXPECT_FALSE(rope.empty());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());

  rope.append(std::string("456"));
  FATAL_EXPECT_FALSE(rope.empty());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
}

////////////////
// comparison //
////////////////

FATAL_TEST(comparison, equal) {
# define TEST_IMPL_COMPARE(rope, other) \
  do { \
    FATAL_EXPECT_EQ(0, rope.compare(other)); \
    \
    FATAL_EXPECT_TRUE(rope == other); \
    FATAL_EXPECT_TRUE(other == rope); \
    \
    FATAL_EXPECT_FALSE(rope != other); \
    FATAL_EXPECT_FALSE(other != rope); \
    \
    FATAL_EXPECT_FALSE(rope < other); \
    FATAL_EXPECT_FALSE(other < rope); \
    \
    FATAL_EXPECT_TRUE(rope <= other); \
    FATAL_EXPECT_TRUE(other <= rope); \
    \
    FATAL_EXPECT_FALSE(rope > other); \
    FATAL_EXPECT_FALSE(other > rope); \
    \
    FATAL_EXPECT_TRUE(rope >= other); \
    FATAL_EXPECT_TRUE(other >= rope); \
  } while (false)

# define TEST_IMPL(...) \
  do { \
    auto const str = to_string(__VA_ARGS__); \
    \
    { \
      ephemeral_rope<> r1(__VA_ARGS__); \
      auto cr1 = r1.mimic(); \
      ephemeral_rope<> r2(__VA_ARGS__); \
      auto cr2 = r2.mimic(); \
      string_ref ref(str); \
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
    } \
    { \
      ephemeral_rope<> const r1(__VA_ARGS__); \
      auto const cr1 = r1.mimic(); \
      ephemeral_rope<> const r2(__VA_ARGS__); \
      auto const cr2 = r2.mimic(); \
      string_ref const ref(str); \
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
    } \
    { \
      ephemeral_rope<> r1(__VA_ARGS__); \
      auto cr1 = r1.mimic(); \
      ephemeral_rope<> r2(__VA_ARGS__); \
      auto cr2 = r2.mimic(); \
      string_ref ref(str); \
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
    } \
    { \
      ephemeral_rope<> r1(__VA_ARGS__); \
      auto cr1 = r1.mimic(); \
      std::vector<char> v(str.cbegin(), str.cend()); \
      v.push_back('\0'); \
      \
      TEST_IMPL_COMPARE(r1, ephemeral_rope<>(__VA_ARGS__)); \
      TEST_IMPL_COMPARE(cr1, ephemeral_rope<>(__VA_ARGS__)); \
      TEST_IMPL_COMPARE(r1, ephemeral_rope<>(__VA_ARGS__).mimic()); \
      TEST_IMPL_COMPARE(cr1, ephemeral_rope<>(__VA_ARGS__).mimic()); \
      TEST_IMPL_COMPARE(r1, string_ref(str)); \
      TEST_IMPL_COMPARE(r1, str.c_str()); \
      TEST_IMPL_COMPARE(r1, v.data()); \
      TEST_IMPL_COMPARE(r1, std::string(str)); \
      TEST_IMPL_COMPARE(cr1, string_ref(str)); \
      TEST_IMPL_COMPARE(cr1, str.c_str()); \
      TEST_IMPL_COMPARE(cr1, v.data()); \
      TEST_IMPL_COMPARE(cr1, std::string(str)); \
    } \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
# undef TEST_IMPL_COMPARE
}

FATAL_TEST(comparison, not_equal) {
# define TEST_IMPL_COMPARE(rope, other, expected_less) \
  do { \
    if (expected_less) { \
      FATAL_EXPECT_LT(rope.compare(other), 0); \
    } else { \
      FATAL_EXPECT_GT(rope.compare(other), 0); \
    } \
    \
    FATAL_EXPECT_FALSE(rope == other); \
    FATAL_EXPECT_FALSE(other == rope); \
    \
    FATAL_EXPECT_TRUE(rope != other); \
    FATAL_EXPECT_TRUE(other != rope); \
    \
    FATAL_EXPECT_EQ(expected_less, rope < other); \
    FATAL_EXPECT_NE(expected_less, other < rope); \
    \
    FATAL_EXPECT_EQ(expected_less, rope <= other); \
    FATAL_EXPECT_NE(expected_less, other <= rope); \
    \
    FATAL_EXPECT_NE(expected_less, rope > other); \
    FATAL_EXPECT_EQ(expected_less, other > rope); \
    \
    FATAL_EXPECT_NE(expected_less, rope >= other); \
    FATAL_EXPECT_EQ(expected_less, other >= rope); \
  } while (false)

  std::vector<ephemeral_rope<>> v;
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
        ephemeral_rope<> &lhs,
        ephemeral_rope<> &rhs,
        bool expected_less
      ) {
        auto const lstr = lhs.to_string();
        auto const rstr = rhs.to_string();
        {
          auto &r1 = lhs;
          auto &r2 = rhs;
          string_ref ref(rstr);
          auto cstr = rstr.c_str();
          std::vector<char> v(rstr.cbegin(), rstr.cend());
          v.push_back('\0');
          auto c = v.data();
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
          string_ref const ref(rstr);
          auto const cstr = rstr.c_str();
          std::vector<char> v(rstr.cbegin(), rstr.cend());
          v.push_back('\0');
          auto const c = v.data();
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
          string_ref ref(rstr);
          auto cstr = rstr.c_str();
          std::vector<char> v(rstr.cbegin(), rstr.cend());
          v.push_back('\0');
          auto c = v.data();
          std::string s(rstr);

          TEST_IMPL_COMPARE(std::move(r1), std::move(r2), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(ref), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(cstr), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(c), expected_less);
          TEST_IMPL_COMPARE(std::move(r1), std::move(s), expected_less);
        }
        {
          auto &r1 = lhs;
          std::vector<char> v(rstr.cbegin(), rstr.cend());
          v.push_back('\0');

          TEST_IMPL_COMPARE(r1, string_ref(rstr), expected_less);
          TEST_IMPL_COMPARE(r1, rstr.c_str(), expected_less);
          TEST_IMPL_COMPARE(r1, v.data(), expected_less);
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
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);

  rope.push_back('1');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1", rope);

  char space = ' ';
  rope.push_back(space);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1 ", rope);

  char two = '2';
  rope.push_back(std::move(two));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1 2", rope);

  rope.push_back(char(' '));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1 2 ", rope);

  char const three = '3';
  rope.push_back(three);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1 2 3", rope);

  auto end = "!";
  rope.push_back(end[0]);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("1 2 3!", rope);
}

////////////
// append //
////////////

FATAL_TEST(append, append) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append("");
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append("one");
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.append(' ');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.append(std::string("two"));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  rope.append(string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two ", rope);
  FATAL_EXPECT_EQ(4, rope.pieces());

  std::string three("three");
  rope.append(three);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three", rope);
  FATAL_EXPECT_EQ(5, rope.pieces());

  std::string f(" f");
  rope.append(std::move(f));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three f", rope);
  FATAL_EXPECT_EQ(6, rope.pieces());

  std::string const o("o");
  rope.append(o);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three fo", rope);
  FATAL_EXPECT_EQ(7, rope.pieces());

  string_ref const ur("ur");
  rope.append(ur);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four", rope);
  FATAL_EXPECT_EQ(8, rope.pieces());

  string_ref end("!");
  rope.append(end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four!", rope);
  FATAL_EXPECT_EQ(9, rope.pieces());
}

//////////////////
// multi_append //
//////////////////

FATAL_TEST(multi_append, none) {
  ephemeral_rope<> rope;

  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("");
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
}

FATAL_TEST(multi_append, one) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one");
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.multi_append(' ');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.multi_append(std::string("two"));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  rope.multi_append(string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two ", rope);
  FATAL_EXPECT_EQ(4, rope.pieces());

  std::string three("three");
  rope.multi_append(three);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three", rope);
  FATAL_EXPECT_EQ(5, rope.pieces());

  std::string f(" f");
  rope.multi_append(std::move(f));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three f", rope);
  FATAL_EXPECT_EQ(6, rope.pieces());

  std::string const o("o");
  rope.multi_append(o);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three fo", rope);
  FATAL_EXPECT_EQ(7, rope.pieces());

  string_ref const ur("ur");
  rope.multi_append(ur);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four", rope);
  FATAL_EXPECT_EQ(8, rope.pieces());

  string_ref end("!");
  rope.multi_append(end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four!", rope);
  FATAL_EXPECT_EQ(9, rope.pieces());
}

FATAL_TEST(multi_append, two) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.multi_append(std::string("two"), string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two ", rope);
  FATAL_EXPECT_EQ(4, rope.pieces());

  std::string three("three");
  string_ref space(" ");
  rope.multi_append(three, space);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three ", rope);
  FATAL_EXPECT_EQ(6, rope.pieces());

  std::string f("f");
  std::string const o("o");
  rope.multi_append(std::move(f), o);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three fo", rope);
  FATAL_EXPECT_EQ(8, rope.pieces());

  string_ref const ur("ur");
  string_ref end("!");
  rope.multi_append(ur, end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four!", rope);
  FATAL_EXPECT_EQ(10, rope.pieces());
}

FATAL_TEST(multi_append, three) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ', std::string("two"));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  std::string three("three");
  std::string f(" f");

  rope.multi_append(string_ref(" "), three, std::move(f));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three f", rope);
  FATAL_EXPECT_EQ(6, rope.pieces());

  std::string const o("o");
  string_ref const ur("ur");
  string_ref end("!");

  rope.multi_append(o, ur, end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four!", rope);
  FATAL_EXPECT_EQ(9, rope.pieces());
}

FATAL_TEST(multi_append, many) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ', std::string("two"), string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two ", rope);
  FATAL_EXPECT_EQ(4, rope.pieces());

  std::string three("three");
  std::string const ou("ou");
  string_ref end("!");

  rope.multi_append(three, " ", 'f', ou, std::string("r"), end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two three four!", rope);
  FATAL_EXPECT_EQ(10, rope.pieces());
}

////////////
// concat //
////////////

FATAL_TEST(concat, empty) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  ephemeral_rope<> other;
  rope.concat(other);
  FATAL_EXPECT_TRUE(rope.empty());

  rope.concat(ephemeral_rope<>());
  FATAL_EXPECT_TRUE(rope.empty());

  rope.concat(rope);
  FATAL_EXPECT_TRUE(rope.empty());
}

FATAL_TEST(concat, one_piece) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  std::string const s1("hello");
  ephemeral_rope<> r1((std::string(s1)));
  FATAL_EXPECT_FALSE(r1.empty());
  rope.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1, rope.to_string());
  FATAL_EXPECT_EQ(s1, rope);

  std::string const s2(", ");
  ephemeral_rope<> r2(s2);
  rope.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2, rope);

  std::string const s3("world");
  rope.concat(ephemeral_rope<>(s3));
  FATAL_EXPECT_EQ(s1 + s2 + s3, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3, rope);

  std::string const s4("!");
  ephemeral_rope<> const r4(s4);
  rope.concat(r4);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, rope);

  std::string const s5(" test");
  ephemeral_rope<> const r5(s5);
  rope.concat(r5);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5, rope);

  std::string const s6(" string");
  ephemeral_rope<> const r6(s6);
  rope.concat(r6);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope);

  rope.concat(rope);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { rope.concat(std::move(rope)); };
}

FATAL_TEST(concat, two_pieces) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  std::string const s1("hello");
  std::string const s2(", ");

  ephemeral_rope<> r1(std::string(s1), s2);
  FATAL_EXPECT_FALSE(r1.empty());
  rope.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2, rope);

  std::string const s3("world");
  std::string const s4("!");

  ephemeral_rope<> const r2(s3, s4);
  rope.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4, rope);

  std::string const s5(" test");
  std::string const s6(" string");

  ephemeral_rope<> const r3(s5, s6);
  rope.concat(r3);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope);

  rope.concat(rope);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { rope.concat(std::move(rope)); };
}

FATAL_TEST(concat, three_pieces) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  std::string const s1("hello");
  std::string const s2(", ");
  std::string const s3("world");

  ephemeral_rope<> r1(std::string(s1), s2, s3);
  FATAL_EXPECT_FALSE(r1.empty());
  rope.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2 + s3, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3, rope);

  std::string const s4("!");
  std::string const s5(" test");
  std::string const s6(" string");

  ephemeral_rope<> const r2(s4, s5, s6);
  rope.concat(r2);
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope);

  rope.concat(rope);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { rope.concat(std::move(rope)); };
}

FATAL_TEST(concat, many_pieces) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());

  std::string const s1("hello");
  std::string const s2(", ");
  std::string const s3("world");
  std::string const s4("!");
  std::string const s5(" test");
  std::string const s6(" string");

  ephemeral_rope<> r1(std::string(s1), s2, s3, s4, s5, s6);
  FATAL_EXPECT_FALSE(r1.empty());
  rope.concat(std::move(r1));
  FATAL_EXPECT_TRUE(r1.empty());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope.to_string());
  FATAL_EXPECT_EQ(s1 + s2 + s3 + s4 + s5 + s6, rope);

  ephemeral_rope<> r2(std::string(s1), s2, s3, s4, s5, s6);
  rope.concat(r2);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope
  );

  rope.concat(rope);
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope.to_string()
  );
  FATAL_EXPECT_EQ(
    s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6
      + s1 + s2 + s3 + s4 + s5 + s6,
    rope
  );

  FATAL_EXPECT_THROW(std::invalid_argument) { rope.concat(std::move(rope)); };
}

///////////////
// accessors //
///////////////

FATAL_TEST(accessors, front_back) {
  ephemeral_rope<> rope;

  rope.append('a');
  FATAL_EXPECT_EQ('a', rope.front());
  FATAL_EXPECT_EQ('a', rope.back());

  rope.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', rope.front());
  FATAL_EXPECT_EQ('e', rope.back());

  std::string f_g_h_i("_f_g_h_i");
  rope.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', rope.front());
  FATAL_EXPECT_EQ('i', rope.back());

  char const underscore = '_';
  rope.append(underscore);
  FATAL_EXPECT_EQ('a', rope.front());
  FATAL_EXPECT_EQ('_', rope.back());

  rope.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', rope.front());
  FATAL_EXPECT_EQ('m', rope.back());
}

//////////////////////
// at / operator [] //
//////////////////////

FATAL_TEST(accessors, at) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(0); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(2); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(4); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(18); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size() - 1); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };

  rope.append('a');
  FATAL_EXPECT_EQ('a', rope.at(0));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(2); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(4); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(18); };
  FATAL_EXPECT_EQ('a', rope.at(rope.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };

  rope.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', rope.at(0));
  FATAL_EXPECT_EQ('b', rope.at(2));
  FATAL_EXPECT_EQ('c', rope.at(4));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(14); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(18); };
  FATAL_EXPECT_EQ('e', rope.at(rope.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };

  std::string f_g_h_i("_f_g_h_i");
  rope.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', rope.at(0));
  FATAL_EXPECT_EQ('b', rope.at(2));
  FATAL_EXPECT_EQ('c', rope.at(4));
  FATAL_EXPECT_EQ('h', rope.at(14));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(17); };
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(18); };
  FATAL_EXPECT_EQ('i', rope.at(rope.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };

  char const underscore = '_';
  rope.append(underscore);
  FATAL_EXPECT_EQ('a', rope.at(0));
  FATAL_EXPECT_EQ('b', rope.at(2));
  FATAL_EXPECT_EQ('c', rope.at(4));
  FATAL_EXPECT_EQ('h', rope.at(14));
  FATAL_EXPECT_EQ('_', rope.at(17));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(18); };
  FATAL_EXPECT_EQ('_', rope.at(rope.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };

  rope.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', rope.at(0));
  FATAL_EXPECT_EQ('b', rope.at(2));
  FATAL_EXPECT_EQ('c', rope.at(4));
  FATAL_EXPECT_EQ('h', rope.at(14));
  FATAL_EXPECT_EQ('_', rope.at(17));
  FATAL_EXPECT_EQ('j', rope.at(18));
  FATAL_EXPECT_EQ('m', rope.at(rope.size() - 1));
  FATAL_EXPECT_THROW(std::out_of_range) { rope.at(rope.size()); };
}

FATAL_TEST(accessors, operator []) {
  ephemeral_rope<> rope;

  rope.append('a');
  FATAL_EXPECT_EQ('a', rope[0]);
  FATAL_EXPECT_EQ('a', rope[rope.size() - 1]);

  rope.append("_b_c_d_e");
  FATAL_EXPECT_EQ('a', rope[0]);
  FATAL_EXPECT_EQ('b', rope[2]);
  FATAL_EXPECT_EQ('c', rope[4]);
  FATAL_EXPECT_EQ('e', rope[rope.size() - 1]);

  std::string f_g_h_i("_f_g_h_i");
  rope.append(f_g_h_i);
  FATAL_EXPECT_EQ('a', rope[0]);
  FATAL_EXPECT_EQ('b', rope[2]);
  FATAL_EXPECT_EQ('c', rope[4]);
  FATAL_EXPECT_EQ('h', rope[14]);
  FATAL_EXPECT_EQ('i', rope[rope.size() - 1]);

  char const underscore = '_';
  rope.append(underscore);
  FATAL_EXPECT_EQ('a', rope[0]);
  FATAL_EXPECT_EQ('b', rope[2]);
  FATAL_EXPECT_EQ('c', rope[4]);
  FATAL_EXPECT_EQ('h', rope[14]);
  FATAL_EXPECT_EQ('_', rope[17]);
  FATAL_EXPECT_EQ('_', rope[rope.size() - 1]);

  rope.append(std::string("j_k_l_m"));
  FATAL_EXPECT_EQ('a', rope[0]);
  FATAL_EXPECT_EQ('b', rope[2]);
  FATAL_EXPECT_EQ('c', rope[4]);
  FATAL_EXPECT_EQ('h', rope[14]);
  FATAL_EXPECT_EQ('_', rope[17]);
  FATAL_EXPECT_EQ('j', rope[18]);
  FATAL_EXPECT_EQ('m', rope[rope.size() - 1]);
}

//////////
// copy //
//////////

// TODO: TEST COPY WITH OFFSET

FATAL_TEST(copy, copy) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> rope(__VA_ARGS__); \
    \
    std::vector<char> buffer(rope.size()); \
    \
    auto const end = rope.copy( \
      buffer.data(), \
      std::next(buffer.data(), buffer.size()) \
    ); \
    \
    FATAL_EXPECT_EQ(rope.size(), std::distance(buffer.data(), end)); \
    FATAL_EXPECT_EQ(buffer.size(), std::distance(buffer.data(), end)); \
    \
    FATAL_EXPECT_EQ(rope, buffer); \
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
    ephemeral_rope<> rope(__VA_ARGS__); \
    FATAL_EXPECT_EQ(to_string(__VA_ARGS__), rope.to_string()); \
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
    ephemeral_rope<> rope(__VA_ARGS__); \
    std::string s; \
    FATAL_EXPECT_EQ(to_string(__VA_ARGS__), rope.append_to(s)); \
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
    ephemeral_rope<SmallBufferSize> rope; \
    FATAL_EXPECT_EQ(SmallBufferSize, rope.capacity()); \
    for (auto i = SmallBufferSize; i--; ) { \
      rope.append(' '); \
    } \
    FATAL_EXPECT_EQ(SmallBufferSize, rope.capacity()); \
    FATAL_EXPECT_EQ(rope.size(), rope.capacity()); \
    rope.append(' '); \
    FATAL_EXPECT_NE(SmallBufferSize, rope.capacity()); \
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

FATAL_TEST(capacity, reserve (absolute)) {
# define TEST_IMPL(SmallBufferSize) \
  do { \
    auto const capacity = SmallBufferSize * 2 + 1; \
    do { \
      ephemeral_rope<SmallBufferSize> rope; \
      FATAL_EXPECT_EQ(SmallBufferSize, rope.capacity()); \
      rope.reserve(capacity); \
      FATAL_EXPECT_EQ(capacity, rope.capacity()); \
      rope.reserve(SmallBufferSize); \
      FATAL_EXPECT_EQ(capacity, rope.capacity()); \
    } while (false); \
    do { \
      ephemeral_rope<SmallBufferSize> rope; \
      FATAL_EXPECT_EQ(SmallBufferSize, rope.capacity()); \
      rope.reserve(capacity, false); \
      FATAL_EXPECT_EQ(capacity, rope.capacity()); \
      rope.reserve(SmallBufferSize, false); \
      FATAL_EXPECT_EQ(capacity, rope.capacity()); \
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

FATAL_TEST(capacity, reserve (relative)) {
# define TEST_IMPL(SmallBufferSize) \
  do { \
    ephemeral_rope<SmallBufferSize> rope; \
    FATAL_EXPECT_EQ(SmallBufferSize, rope.capacity()); \
    auto const additional = SmallBufferSize + 1; \
    auto const capacity = SmallBufferSize + additional; \
    rope.reserve(capacity, true); \
    FATAL_EXPECT_EQ(capacity, rope.capacity()); \
    rope.reserve(SmallBufferSize, true); \
    FATAL_EXPECT_EQ(capacity, rope.capacity()); \
    rope.reserve(additional, true); \
    FATAL_EXPECT_EQ(capacity, rope.capacity()); \
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
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
}

FATAL_TEST(clear, one) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append("one");
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append(' ');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ(" ", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append(std::string("two"));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("two", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.append(string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ(" ", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string three("three");
  rope.append(three);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("three", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string f(" f");
  rope.append(std::move(f));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ(" f", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string const o("o");
  rope.append(o);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("o", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  string_ref const ur("ur");
  rope.append(ur);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("ur", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  string_ref end("!");
  rope.append(end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("!", rope);
  FATAL_EXPECT_EQ(1, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
}

FATAL_TEST(clear, two) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ');
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append(std::string("two"), string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("two ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string three("three");
  string_ref space(" ");
  rope.multi_append(three, space);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("three ", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string f(" f");
  std::string const o("o");
  rope.multi_append(std::move(f), o);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ(" fo", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  string_ref const ur("ur");
  string_ref end("!");
  rope.multi_append(ur, end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("ur!", rope);
  FATAL_EXPECT_EQ(2, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
}

FATAL_TEST(clear, three) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ', std::string("two"));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string three("three");
  std::string f(" f");

  rope.multi_append(string_ref(" "), three, std::move(f));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ(" three f", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string const o("o");
  string_ref const ur("ur");
  string_ref end("!");

  rope.multi_append(o, ur, end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("our!", rope);
  FATAL_EXPECT_EQ(3, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
}

FATAL_TEST(clear, many) {
  ephemeral_rope<> rope;
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  rope.multi_append("one", ' ', std::string("two"), string_ref(" "));
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("one two ", rope);
  FATAL_EXPECT_EQ(4, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());

  std::string three("three");
  std::string const ou("ou");
  string_ref end("!");

  rope.multi_append(three, " ", 'f', ou, std::string("r"), end);
  FATAL_EXPECT_FALSE(rope.empty());
  FATAL_EXPECT_EQ("three four!", rope);
  FATAL_EXPECT_EQ(6, rope.pieces());

  rope.clear();
  FATAL_EXPECT_TRUE(rope.empty());
  FATAL_EXPECT_EQ("", rope);
  FATAL_EXPECT_EQ(0, rope.pieces());
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
    auto const end = std::next(begin, s.size());

    for (timed_iterations<> i(time, minimum_iterations); i.next(); ) {
      ephemeral_rope<> rope;
      auto const pieces = rdg.chop(
        begin, end,
        [&](char const *b, char const *e) { rope.append(b, e); }
      );
      FATAL_EXPECT_EQ(pieces, rope.pieces());
      FATAL_EXPECT_EQ(s, rope);

      fn(s, rope, alphabet);
    }
  }
}

FATAL_TEST(find, char) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      ephemeral_rope<> const &rope,
      std::string const &alphabet
    ) {
      for (auto c: alphabet) {
        auto expected = s.find(c);
        if (expected == std::string::npos) {
          expected = s.size();
        } else {
          assert(expected != s.size());
        }

        auto const actual = rope.find(c);
        FATAL_EXPECT_EQ(expected, actual.absolute());
      }
    }
  );
}

FATAL_TEST(find, char size_type) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      ephemeral_rope<> const &rope,
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

          auto const actual = rope.find(c, offset);
          FATAL_EXPECT_EQ(expected, actual.absolute());
        }
      }
    }
  );
}

FATAL_TEST(find, char const_iterator) {
  find_char_test(
    std::chrono::milliseconds(100), 1000,
    [](
      std::string const &s,
      ephemeral_rope<> const &rope,
      std::string const &alphabet
    ) {
      for (auto offset = s.size() + 1; offset--; ) {
        auto const rope_offset = std::next(rope.begin(), offset);
        FATAL_EXPECT_EQ(offset, rope_offset.absolute());

        for (auto c: alphabet) {
          auto expected = s.find(c, offset);
          if (expected == std::string::npos) {
            expected = s.size();
          } else {
            assert(expected != s.size());
          }

          auto const actual = rope.find(c, rope_offset);
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
  ephemeral_rope<> rope;

  FATAL_EXPECT_EQ(rope.cbegin(), rope.begin());
  FATAL_EXPECT_EQ(rope.cend(), rope.end());
  FATAL_EXPECT_EQ(rope.cbegin(), rope.cend());
  FATAL_EXPECT_EQ(rope.begin(), rope.end());
}

FATAL_TEST(const_iterator, foreach) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (prefix ++)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (postfix ++)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (prefix --)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (postfix --)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (+= 1)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(const_iterator, foreach (-= 1)) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r(__VA_ARGS__); \
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

FATAL_TEST(std, hash) {
# define TEST_IMPL(...) \
  do { \
    ephemeral_rope<> r1(__VA_ARGS__); \
    ephemeral_rope<> r2(to_string(__VA_ARGS__)); \
    using hasher = std::hash<ephemeral_rope<>>; \
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
    ephemeral_rope<> r(__VA_ARGS__); \
    std::ostringstream ss; \
    ss << r; \
    auto const expected = to_string(__VA_ARGS__); \
    FATAL_EXPECT_EQ(expected, ss.str()); \
  } while (false)

  TEST_IMPL_SINGLE_STRING(TEST_IMPL);

# undef TEST_IMPL
}

} // namespace fatal {
