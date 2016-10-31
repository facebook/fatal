/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/fold.h>

#include <fatal/type/arithmetic.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(fold, sanity check) {
  FATAL_EXPECT_SAME<
    list<>,
    fold<std::integral_constant<int, 0>, add, list<>>
  >();

  FATAL_EXPECT_SAME<
    int_list<5>,
    fold<std::integral_constant<int, 0>, add, int_list<5>>
  >();

  FATAL_EXPECT_SAME<
    int_list<6>,
    fold<std::integral_constant<int, 1>, add, int_list<5>>
  >();

  FATAL_EXPECT_SAME<
    int_list<1, 2, 3, 4, 5>,
    fold<std::integral_constant<int, 0>, add, int_list<1, 1, 1, 1, 1>>
  >();

  FATAL_EXPECT_SAME<
    int_list<4, 6, 8, 10, 12>,
    fold<std::integral_constant<int, 2>, add, int_list<2, 2, 2, 2, 2>>
  >();

  FATAL_EXPECT_SAME<
    int_list<1, 3, 6, 10, 15>,
    fold<std::integral_constant<int, 0>, add, int_list<1, 2, 3, 4, 5>>
  >();

  FATAL_EXPECT_SAME<
    int_list<4, 8, 14, 22, 32>,
    fold<std::integral_constant<int, 2>, add, int_list<2, 4, 6, 8, 10>>
  >();
}

} // namespace fatal {
