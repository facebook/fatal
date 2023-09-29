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
struct bar { using xyz = int; };
struct baz {};
struct gaz { struct xyz {}; };
struct qux { int xyz; };

FATAL_HAS_TYPE(has_xyz, xyz);

FATAL_TEST(has_type, fatal_has_type) {
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<foo>>();
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<bar>>();
  FATAL_EXPECT_SAME<std::false_type, has_xyz::apply<baz>>();
  FATAL_EXPECT_SAME<std::true_type, has_xyz::apply<gaz>>();
  FATAL_EXPECT_SAME<std::false_type, has_xyz::apply<qux>>();
}

FATAL_HAS_TYPE_NAME(xyz);

FATAL_TEST(has_type, fatal_has_type_name) {
  FATAL_EXPECT_SAME<std::true_type, xyz::apply<foo>>();
  FATAL_EXPECT_SAME<std::true_type, xyz::apply<bar>>();
  FATAL_EXPECT_SAME<std::false_type, xyz::apply<baz>>();
  FATAL_EXPECT_SAME<std::true_type, xyz::apply<gaz>>();
  FATAL_EXPECT_SAME<std::false_type, xyz::apply<qux>>();
}

struct ifoo { using id = int; };
struct ibar { using id = int; };
struct ibaz {};
struct igaz { struct id {}; };
struct iqux { int id; };

FATAL_TEST(has_type, has_type) {
  FATAL_EXPECT_SAME<std::true_type, has_type::id::apply<ifoo>>();
  FATAL_EXPECT_SAME<std::true_type, has_type::id::apply<ibar>>();
  FATAL_EXPECT_SAME<std::false_type, has_type::id::apply<ibaz>>();
  FATAL_EXPECT_SAME<std::true_type, has_type::id::apply<igaz>>();
  FATAL_EXPECT_SAME<std::false_type, has_type::id::apply<iqux>>();
}

} // namespace fatal {
