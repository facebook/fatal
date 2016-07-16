/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/qualifier.h>

#include <fatal/test/driver.h>

namespace fatal {

struct foo {
  void noncv();
  void c() const;
  void v() volatile;
  void cv() const volatile;
  void vc() volatile const;
};

FATAL_TEST(reflect_member_function, cv_qualifier_bitwise_and) {
  FATAL_EXPECT_FALSE(cv_qualifier::none & cv_qualifier::none);
  FATAL_EXPECT_FALSE(cv_qualifier::none & cv_qualifier::c);
  FATAL_EXPECT_FALSE(cv_qualifier::none & cv_qualifier::v);
  FATAL_EXPECT_FALSE(cv_qualifier::none & cv_qualifier::cv);

  FATAL_EXPECT_FALSE(cv_qualifier::c & cv_qualifier::none);
  FATAL_EXPECT_TRUE(cv_qualifier::c & cv_qualifier::c);
  FATAL_EXPECT_FALSE(cv_qualifier::c & cv_qualifier::v);
  FATAL_EXPECT_TRUE(cv_qualifier::c & cv_qualifier::cv);

  FATAL_EXPECT_FALSE(cv_qualifier::v & cv_qualifier::none);
  FATAL_EXPECT_FALSE(cv_qualifier::v & cv_qualifier::c);
  FATAL_EXPECT_TRUE(cv_qualifier::v & cv_qualifier::v);
  FATAL_EXPECT_TRUE(cv_qualifier::v & cv_qualifier::cv);

  FATAL_EXPECT_FALSE(cv_qualifier::cv & cv_qualifier::none);
  FATAL_EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::c);
  FATAL_EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::v);
  FATAL_EXPECT_TRUE(cv_qualifier::cv & cv_qualifier::cv);
}

} // namespace fatal {
