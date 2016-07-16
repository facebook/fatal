/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/string/tokenizer.h>

#include <fatal/test/driver.h>

#include <utility>
#include <vector>

namespace fatal {

template <typename Tokenizer, typename T>
void tokenizer_test(T &&data, std::vector<char const *> const &expected) {
  auto i = expected.begin();

  for (auto const &token: Tokenizer(std::forward<T>(data))) {
    FATAL_ASSERT_NE(expected.end(), i);
    FATAL_EXPECT_EQ(*i, token);
    ++i;
  }

  FATAL_EXPECT_EQ(expected.end(), i);
}

FATAL_TEST(tokenizer, colon) {
  tokenizer_test<colon_tokenizer>(
    "1:2:3:4:5::6: 7:8 :9",
    {"1", "2", "3", "4", "5", "", "6", " 7", "8 ", "9"}
  );
}

FATAL_TEST(tokenizer, comma) {
  tokenizer_test<comma_tokenizer>(
    "1,2,3,4,5,,6, 7,8 ,9",
    {"1", "2", "3", "4", "5", "", "6", " 7", "8 ", "9"}
  );
}

FATAL_TEST(tokenizer, line) {
  tokenizer_test<line_tokenizer>(
    "1\n2\n3\n4\n5\n\n6\n 7\n8 \n9",
    {"1", "2", "3", "4", "5", "", "6", " 7", "8 ", "9"}
  );
}

FATAL_TEST(tokenizer, semicolon) {
  tokenizer_test<semicolon_tokenizer>(
    "1;2;3;4;5;;6; 7;8 ;9",
    {"1", "2", "3", "4", "5", "", "6", " 7", "8 ", "9"}
  );
}

FATAL_TEST(tokenizer, space) {
  tokenizer_test<space_tokenizer>(
    "1 2 3 4 5  6 7 8 9",
    {"1", "2", "3", "4", "5", "", "6", "7", "8", "9"}
  );
}

template <typename T>
void csv_tokenizer_test(
  T &&data,
  std::vector<std::vector<char const *>> const &expected
) {
  auto i = expected.begin();
  for (auto const &line: csv_tokenizer(std::forward<T>(data))) {
    FATAL_ASSERT_NE(expected.end(), i);
    auto j = i->begin();
    for (auto const &token: line) {
      FATAL_ASSERT_NE(i->end(), j);
      FATAL_EXPECT_EQ(*j, token);
      ++j;
    }
    FATAL_EXPECT_EQ(i->end(), j);
    ++i;
  }
  FATAL_EXPECT_EQ(expected.end(), i);
}

FATAL_TEST(tokenizer, csv) {
  csv_tokenizer_test(
    "1,2,3,4,5,,6\n\n 7,8 ,9",
    {
      {"1", "2", "3", "4", "5", "", "6"},
      {},
      {" 7", "8 ", "9"}
    }
  );
}

} // namespace fatal {
