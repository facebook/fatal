/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/time/time.h>

#include <fatal/test/driver.h>

#include <sstream>

namespace fatal {
namespace time {

FATAL_TEST(time, pretty_print) {
# define TEST_IMPL(Expected, Value) \
  do { \
    std::ostringstream out; \
    pretty_print(out, Value); \
    FATAL_EXPECT_EQ(Expected, out.str()); \
  } while (false)

  TEST_IMPL("1h", std::chrono::hours(1));
  TEST_IMPL("1min", std::chrono::minutes(1));
  TEST_IMPL("1s", std::chrono::seconds(1));
  TEST_IMPL("1ms", std::chrono::milliseconds(1));
  TEST_IMPL("1us", std::chrono::microseconds(1));
  TEST_IMPL("1ns", std::chrono::nanoseconds(1));
  TEST_IMPL("1s", std::chrono::milliseconds(1000));

  TEST_IMPL(
    "1h 1min 1s 1ms 1us 1ns",
    std::chrono::hours(1)
      + std::chrono::minutes(1)
      + std::chrono::seconds(1)
      + std::chrono::milliseconds(1)
      + std::chrono::microseconds(1)
      + std::chrono::nanoseconds(1)
  );

# undef TEST_IMPL
}

} // namespace time {
} // namespace fatal {
