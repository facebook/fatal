/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/math/compile_time_rng.h>

#include <fatal/test/driver.h>

#include <unordered_set>

namespace fatal {

FATAL_TEST(compile_time_rng, get) {
# define TEST_IMPL(Set, N) \
  do { \
    Set.insert( \
      compile_time_rng<std::size_t>::seed<__LINE__>::get<0>::value \
    ); \
    ++N; \
  } while (false)

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);

  FATAL_EXPECT_GE(s.size(), n * 95 / 100);

# undef TEST_IMPL
}

FATAL_TEST(compile_time_rng, iteration) {
# define TEST_IMPL(Set, N, Offset) \
  do { \
    Set.insert( \
      compile_time_rng<std::size_t>::get<__LINE__ - Offset - 1>::value \
    ); \
    ++N; \
  } while (false)

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  auto const offset = __LINE__;
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);

  FATAL_EXPECT_GE(s.size(), n * 95 / 100);

# undef TEST_IMPL
}

FATAL_TEST(compile_time_rng, fatal_compile_time_rng) {
# define TEST_IMPL(Set, N) \
  do { \
    using rng = FATAL_COMPILE_TIME_RNG<std::size_t>; \
    Set.insert(rng::get<>::value); \
    ++N; \
  } while (false)

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);

  FATAL_EXPECT_GE(s.size(), n * 95 / 100);

# undef TEST_IMPL
}

} // namespace fatal {
