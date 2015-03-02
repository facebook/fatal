/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/enum.h>

#include <fatal/test/driver.h>

#include <cstring>

namespace fatal {

FATAL_RICH_ENUM(
  str_class, test_enum,
  state0,
  (state1, 4),
  (state2, 97),
  state3
);

///////////
// enums //
///////////

TEST(enums, declare_enum) {
  EXPECT_EQ(test_enum::state0, static_cast<test_enum>(0));
  EXPECT_EQ(test_enum::state1, static_cast<test_enum>(4));
  EXPECT_EQ(test_enum::state2, static_cast<test_enum>(97));
  EXPECT_EQ(test_enum::state3, static_cast<test_enum>(98));
}

TEST(enums, to_str) {
  EXPECT_EQ(nullptr, str_class::to_str(static_cast<test_enum>(-1)));
  EXPECT_EQ(FATAL_TO_STR(state0), str_class::to_str(test_enum::state0));
  EXPECT_EQ(FATAL_TO_STR(state1), str_class::to_str(test_enum::state1));
  EXPECT_EQ(FATAL_TO_STR(state2), str_class::to_str(test_enum::state2));
  EXPECT_EQ(FATAL_TO_STR(state3), str_class::to_str(test_enum::state3));
}

TEST(enums, parse) {
# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    EXPECT_EQ(e::x, str_class::parse(s)); \
    EXPECT_EQ(e::x, str_class::parse(s.begin(), s.end())); \
    EXPECT_THROW( \
      str_class::parse(s.begin(), s.begin()), \
      std::invalid_argument \
    ); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    EXPECT_THROW( \
      (str_class::parse(s.begin(), std::next(s.begin(), s.size() - 1))), \
      std::invalid_argument \
    ); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x) "invalid"); \
    EXPECT_THROW(str_class::parse(s), std::invalid_argument); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x) "invalid"); \
    EXPECT_THROW(str_class::parse(s.begin(), s.end()), std::invalid_argument); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

  EXPECT_THROW(str_class::parse(std::string()), std::invalid_argument);
  EXPECT_THROW(str_class::parse(std::string("invalid")), std::invalid_argument);
}

TEST(enums, try_parse) {
# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    e out = static_cast<e>(-1); \
    EXPECT_TRUE(str_class::try_parse(out, s)); \
    EXPECT_EQ(e::x, out); \
    \
    out = static_cast<e>(-1); \
    EXPECT_TRUE(str_class::try_parse(out, s.begin(), s.end())); \
    EXPECT_EQ(e::x, out); \
    \
    out = static_cast<e>(-1); \
    EXPECT_FALSE(str_class::try_parse(out, s.begin(), s.begin())); \
    EXPECT_EQ(static_cast<e>(-1), out); \
    \
    EXPECT_FALSE( \
      str_class::try_parse(out, s.begin(), std::next(s.begin(), s.size() - 1)) \
    ); \
    EXPECT_EQ(static_cast<e>(-1), out); \
    \
    auto const i = s + "invalid"; \
    EXPECT_FALSE((str_class::try_parse(out, i))); \
    EXPECT_FALSE((str_class::try_parse(out, i.begin(), i.end()))); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

  do {
    test_enum out;
    EXPECT_FALSE(str_class::try_parse(out, std::string()));
    EXPECT_FALSE(str_class::try_parse(out, std::string("invalid")));
  } while (false);
}

} // namespace fatal {
