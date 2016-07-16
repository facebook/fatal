/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/math/hash.h>

#include <fatal/test/driver.h>

#include <iterator>

#include <cstring>

namespace fatal {

FATAL_TEST(bytes_hasher, sanity_check) {
  auto const hello = "hello";
  auto const end = std::next(hello, std::strlen(hello));

  auto const h1 = *bytes_hasher<>()('h')('e')('l')('l')('o');
  auto const h2 = *bytes_hasher<>()(hello, end);
  auto const h3 = *bytes_hasher<>()(hello, std::strlen(hello));

  FATAL_EXPECT_EQ(h1, h2);
  FATAL_EXPECT_EQ(h1, h3);
  FATAL_EXPECT_EQ(h2, h3);
}

FATAL_TEST(bytes_hasher, sanity_check_2) {
  auto const r = *bytes_hasher<>()("hello", 5)(", ", 2)("world", 5)('!')
    (" with", 5)(' ')("some", 4)(" extra", 6)(" ", 1)('s')("trings", 6);
  auto const u = *bytes_hasher<>()("hello, world! with some extra strings", 37);

  FATAL_EXPECT_EQ(r, u);
}

} // namespace fatal {
