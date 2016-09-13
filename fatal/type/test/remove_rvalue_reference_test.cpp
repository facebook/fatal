/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/remove_rvalue_reference.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(traits, remove_rvalue_reference) {
# define TEST_IMPL(Type, Expected) \
  do { \
    FATAL_EXPECT_SAME<Expected, remove_rvalue_reference<Type>::type>(); \
  } while (false)

  TEST_IMPL(int &&, int);
  TEST_IMPL(int &, int &);
  TEST_IMPL(int, int);
  TEST_IMPL(int *&&, int *);
  TEST_IMPL(int *&, int *&);
  TEST_IMPL(int *, int*);
  TEST_IMPL(int const &&, int const);
  TEST_IMPL(int const &, int const &);
  TEST_IMPL(int const, int const);
  TEST_IMPL(int const *&&, int const *);
  TEST_IMPL(int const *&, int const *&);
  TEST_IMPL(int const *, int const *);

# undef TEST_IMPL
}

} // namespace fatal {
