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

FATAL_TEST(enums, declare_enum) {
  FATAL_EXPECT_EQ(test_enum::state0, static_cast<test_enum>(0));
  FATAL_EXPECT_EQ(test_enum::state1, static_cast<test_enum>(4));
  FATAL_EXPECT_EQ(test_enum::state2, static_cast<test_enum>(97));
  FATAL_EXPECT_EQ(test_enum::state3, static_cast<test_enum>(98));
}

FATAL_TEST(enums, to_str) {
  FATAL_EXPECT_EQ(nullptr, str_class::to_str(static_cast<test_enum>(-1)));
  FATAL_EXPECT_EQ(FATAL_TO_STR(state0), str_class::to_str(test_enum::state0));
  FATAL_EXPECT_EQ(FATAL_TO_STR(state1), str_class::to_str(test_enum::state1));
  FATAL_EXPECT_EQ(FATAL_TO_STR(state2), str_class::to_str(test_enum::state2));
  FATAL_EXPECT_EQ(FATAL_TO_STR(state3), str_class::to_str(test_enum::state3));
}

FATAL_TEST(enums, parse) {
# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    FATAL_EXPECT_EQ(e::x, str_class::parse(s)); \
    FATAL_EXPECT_EQ(e::x, str_class::parse(s.begin(), s.end())); \
    FATAL_EXPECT_THROW(std::invalid_argument) { \
      str_class::parse(s.begin(), s.begin()); \
    }; \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    FATAL_EXPECT_THROW(std::invalid_argument) { \
      str_class::parse(s.begin(), std::next(s.begin(), s.size() - 1)); \
    }; \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x) "invalid"); \
    FATAL_EXPECT_THROW(std::invalid_argument) { \
      str_class::parse(s); \
    }; \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x) "invalid"); \
    FATAL_EXPECT_THROW(std::invalid_argument) { \
      str_class::parse(s.begin(), s.end()); \
    }; \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

  FATAL_EXPECT_THROW(std::invalid_argument) {
    str_class::parse(std::string());
  };
  FATAL_EXPECT_THROW(std::invalid_argument) {
    str_class::parse(std::string("invalid"));
  };
}

FATAL_TEST(enums, try_parse) {
# define CREATE_TEST(e, x) \
  do { \
    std::string const s(FATAL_TO_STR(x)); \
    e out = static_cast<e>(-1); \
    FATAL_EXPECT_TRUE(str_class::try_parse(out, s)); \
    FATAL_EXPECT_EQ(e::x, out); \
    \
    out = static_cast<e>(-1); \
    FATAL_EXPECT_TRUE(str_class::try_parse(out, s.begin(), s.end())); \
    FATAL_EXPECT_EQ(e::x, out); \
    \
    out = static_cast<e>(-1); \
    FATAL_EXPECT_FALSE(str_class::try_parse(out, s.begin(), s.begin())); \
    FATAL_EXPECT_EQ(static_cast<e>(-1), out); \
    \
    FATAL_EXPECT_FALSE( \
      str_class::try_parse(out, s.begin(), std::next(s.begin(), s.size() - 1)) \
    ); \
    FATAL_EXPECT_EQ(static_cast<e>(-1), out); \
    \
    auto const i = s + "invalid"; \
    FATAL_EXPECT_FALSE((str_class::try_parse(out, i))); \
    FATAL_EXPECT_FALSE((str_class::try_parse(out, i.begin(), i.end()))); \
  } while (false)
  CREATE_TEST(test_enum, state0);
  CREATE_TEST(test_enum, state1);
  CREATE_TEST(test_enum, state2);
  CREATE_TEST(test_enum, state3);
# undef CREATE_TEST

  do {
    test_enum out;
    FATAL_EXPECT_FALSE(str_class::try_parse(out, std::string()));
    FATAL_EXPECT_FALSE(str_class::try_parse(out, std::string("invalid")));
  } while (false);
}

} // namespace fatal {
