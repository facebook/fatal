/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/has_type.h>

#include <fatal/test/driver.h>

namespace fatal {

struct foo { using xyz = int; };
struct bar { typedef int xyz; };
struct baz {};
struct gaz { struct xyz {}; };
struct qux { int xyz; };

FATAL_HAS_MEMBER_TYPE(has_xyz, xyz);

FATAL_TEST(has_type, has_type) {
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<foo>>();
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<bar>>();
  FATAL_EXPECT_SAME<std::false_type, has_xyz::apply<baz>>();
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<gaz>>();
  FATAL_EXPECT_SAME<std::false_type, has_xyz::apply<qux>>();
}

} // namespace fatal {
