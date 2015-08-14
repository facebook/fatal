/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/constant_array.h>

#include <fatal/test/driver.h>

namespace fatal {

template <typename T, T... Values>
void test_constant_array() {
  using actual = constant_array<T, Values...>;

  FATAL_EXPECT_SAME<T, typename actual::value_type>();
  FATAL_EXPECT_EQ(sizeof...(Values), actual::size);
  FATAL_EXPECT_EQ(sizeof...(Values) == 0, actual::empty);

  using type = std::array<T, sizeof...(Values)>;

  FATAL_EXPECT_SAME<type, typename actual::type>();

  type const expected{{Values...}};

  FATAL_EXPECT_EQ(expected, actual::get);
  FATAL_EXPECT_EQ(actual::get.data(), actual::data());
}

FATAL_TEST(constant_array, constant_array) {
  test_constant_array<int>();
  test_constant_array<int, 99>();
  test_constant_array<int, 0, 1, 2>();
  test_constant_array<int, 10, 20, 30, 40, 50, 60, 70, 80, 90>();

  test_constant_array<char>();
  test_constant_array<char, 'x'>();
  test_constant_array<char, '0', '1', '2'>();
  test_constant_array<char, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'>();
}

} // namespace fatal {
