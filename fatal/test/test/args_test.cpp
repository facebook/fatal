/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/test/args.h>

#include <map>
#include <string>

#include <fatal/test/driver.h>

using Args = std::map<std::string, std::string>;

FATAL_TEST(args, parse_args empty) {
  char const * const argv[] = {"prog"};
  int const argc = sizeof(argv) / sizeof(*argv);
  FATAL_EXPECT_EQ(1, argc); // sanity

  auto const args = fatal::test_impl::args::parse_args<Args>(argc, argv);
  FATAL_EXPECT_EQ(Args({}), args);
}

FATAL_TEST(args, parse_args one basic) {
  char const * const argv[] = {"prog", "x"};
  int const argc = sizeof(argv) / sizeof(*argv);
  FATAL_EXPECT_EQ(2, argc); // sanity

  auto const args = fatal::test_impl::args::parse_args<Args>(argc, argv);
  FATAL_EXPECT_EQ(Args({{"x", ""}}), args);
}

FATAL_TEST(args, parse_args one key) {
  char const * const argv[] = {"prog", "y="};
  int const argc = sizeof(argv) / sizeof(*argv);
  FATAL_EXPECT_EQ(2, argc); // sanity

  auto const args = fatal::test_impl::args::parse_args<Args>(argc, argv);
  FATAL_EXPECT_EQ(Args({{"y", ""}}), args);
}

FATAL_TEST(args, parse_args one key-value) {
  char const * const argv[] = {"prog", "z=a"};
  int const argc = sizeof(argv) / sizeof(*argv);
  FATAL_EXPECT_EQ(2, argc); // sanity

  auto const args = fatal::test_impl::args::parse_args<Args>(argc, argv);
  FATAL_EXPECT_EQ(Args({{"z", "a"}}), args);
}

FATAL_TEST(args, parse_args multi) {
  char const * const argv[] = {"prog", "x", "y=", "z=a"};
  int const argc = sizeof(argv) / sizeof(*argv);
  FATAL_EXPECT_EQ(4, argc); // sanity

  auto const args = fatal::test_impl::args::parse_args<Args>(argc, argv);
  FATAL_EXPECT_EQ(Args({{"x", ""}, {"y", ""}, {"z", "a"}}), args);
}
