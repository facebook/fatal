/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/test/tools.h>

#include <fatal/test/driver.h>

namespace fatal {

#define TEST_IMPL(Expected, Actual) \
  do { \
    auto const expected = (Expected); \
    auto const actual = (Actual); \
    if (expected != actual) { \
      std::cout << "expected: " << expected << '\n'; \
      std::cout << "  actual: " << actual << '\n'; \
    } \
    FATAL_EXPECT_EQ(expected, actual); \
  } while (false)

FATAL_TEST(string_as_literal, expanded) {
  TEST_IMPL(R"("")", string_as_literal<literal_style::expanded>(""));
  TEST_IMPL(R"("\x0d")", string_as_literal<literal_style::expanded>("\x0d"));
  TEST_IMPL(R"("\xd0")", string_as_literal<literal_style::expanded>("\xd0"));
  TEST_IMPL(R"("\x00")", string_as_literal<literal_style::expanded>(std::string("", 1)));
  TEST_IMPL(
    R"("\x68\x65\x6c\x6c\x6f\x2c\x20\x77\x6f\x72\x6c\x64")",
    string_as_literal<literal_style::expanded>("hello, world")
  );
  TEST_IMPL(
    R"("\x10\x20\x30")",
    string_as_literal<literal_style::expanded>("\x10\x20\x30")
  );
  TEST_IMPL(
    R"("\x0a\x14\x1e")",
    string_as_literal<literal_style::expanded>("\xa\x14\x1e")
  );
  TEST_IMPL(
    R"("\x0a\x0d\x09\x0b\x07\x08\x0c\xff\x3b\xab\x61\x05)"
    R"(\x20\x09\x27\x22\x13\x10\x0d\x0a")",
    string_as_literal<literal_style::expanded>(
      "\n\r\t\v\a\b\f\xff;\xab""a\x5 \x9'\"\x13""\x10""\xd""\xa"
    )
  );
}

FATAL_TEST(string_as_literal, readable) {
  TEST_IMPL("", string_as_literal<literal_style::readable>(""));
  TEST_IMPL("0d", string_as_literal<literal_style::readable>("\x0d"));
  TEST_IMPL("d0", string_as_literal<literal_style::readable>("\xd0"));
  TEST_IMPL("00", string_as_literal<literal_style::readable>(std::string("", 1)));
  TEST_IMPL(
    "68 65 6c 6c   6f 2c 20 77   6f 72 6c 64",
    string_as_literal<literal_style::readable>("hello, world")
  );
  TEST_IMPL("10 20 30", string_as_literal<literal_style::readable>("\x10\x20\x30"));
  TEST_IMPL("0a 14 1e", string_as_literal<literal_style::readable>("\xa\x14\x1e"));
  TEST_IMPL(
    "0a 0d 09 0b   07 08 0c ff   3b ab 61 05   20 09 27 22   13 10 0d 0a",
    string_as_literal<literal_style::readable>(
      "\n\r\t\v\a\b\f\xff;\xab""a\x5 \x9'\"\x13""\x10""\xd""\xa"
    )
  );
}

FATAL_TEST(string_as_literal, compact) {
  TEST_IMPL(R"("")", string_as_literal<literal_style::compact>(""));
  TEST_IMPL(R"("\r")", string_as_literal<literal_style::compact>("\x0d"));
  TEST_IMPL(R"("\xd0")", string_as_literal<literal_style::compact>("\xd0"));
  TEST_IMPL(R"("\0")", string_as_literal<literal_style::compact>(std::string("", 1)));
  TEST_IMPL(
    R"("hello, world")",
    string_as_literal<literal_style::compact>("hello, world")
  );
  TEST_IMPL(R"("\x10 0")", string_as_literal<literal_style::compact>("\x10\x20\x30"));
  TEST_IMPL(
    R"("\n\x14\x1e")",
    string_as_literal<literal_style::compact>("\xa\x14\x1e")
  );
  TEST_IMPL(
    R"("\n\r\t\v\a\b\f\xff;\xab""a\x5 \t'\"\x13\x10\r\n")",
    string_as_literal<literal_style::compact>(
      "\n\r\t\v\a\b\f\xff;\xab""a\x5 \x9'\"\x13""\x10""\xd""\xa"
    )
  );
}

} // namespace fatal {
