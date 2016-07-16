/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/maybe.h>

#include <fatal/test/driver.h>

namespace fatal {

template <typename... Args>
struct args {};

FATAL_TEST(maybe, nullary) {
  using actual = maybe<>;
  FATAL_EXPECT_SAME<void, actual::first<void>>();
  FATAL_EXPECT_SAME<void, actual::second<void>>();
  FATAL_EXPECT_SAME<void, actual::third<void>>();
  FATAL_EXPECT_SAME<args<>, actual::all<args>>();
}

FATAL_TEST(maybe, unary) {
  using actual = maybe<int>;
  FATAL_EXPECT_SAME<int, actual::first<void>>();
  FATAL_EXPECT_SAME<void, actual::second<void>>();
  FATAL_EXPECT_SAME<void, actual::third<void>>();
  FATAL_EXPECT_SAME<args<int>, actual::all<args>>();
}

FATAL_TEST(maybe, binary) {
  using actual = maybe<int, bool>;
  FATAL_EXPECT_SAME<int, actual::first<void>>();
  FATAL_EXPECT_SAME<bool, actual::second<void>>();
  FATAL_EXPECT_SAME<void, actual::third<void>>();
  FATAL_EXPECT_SAME<args<int, bool>, actual::all<args>>();
}

FATAL_TEST(maybe, ternary) {
  using actual = maybe<int, bool, double>;
  FATAL_EXPECT_SAME<int, actual::first<void>>();
  FATAL_EXPECT_SAME<bool, actual::second<void>>();
  FATAL_EXPECT_SAME<double, actual::third<void>>();
  FATAL_EXPECT_SAME<args<int, bool, double>, actual::all<args>>();
}

} // namespace fatal {
