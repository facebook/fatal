/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/constant.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(constant, example) {
  using get_constant_double = get_constant<double>;
  FATAL_EXPECT_SAME<double, get_constant_double::apply<int>>();
}

} // namespace fatal {
