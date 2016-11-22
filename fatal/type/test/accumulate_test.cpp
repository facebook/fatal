/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/accumulate.h>

#include <fatal/type/arithmetic.h>
#include <fatal/type/list.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(accumulate, sanity check) {
  FATAL_EXPECT_SAME<
    std::integral_constant<int, 0>,
    accumulate<list<>, std::integral_constant<int, 0>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 5>,
    accumulate<int_list<5>, std::integral_constant<int, 0>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 6>,
    accumulate<int_list<5>, std::integral_constant<int, 1>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 5>,
    accumulate<int_list<1, 1, 1, 1, 1>,std::integral_constant<int, 0>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 12>,
    accumulate<int_list<2, 2, 2, 2, 2>, std::integral_constant<int, 2>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 15>,
    accumulate<int_list<1, 2, 3, 4, 5>, std::integral_constant<int, 0>, add>
  >();

  FATAL_EXPECT_SAME<
    std::integral_constant<int, 32>,
    accumulate<int_list<2, 4, 6, 8, 10>, std::integral_constant<int, 2>, add>
  >();
}

} // namespace fatal {
