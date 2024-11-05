/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/constify.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(traits, constify) {
# define TEST_IMPL(T, ...) \
  FATAL_EXPECT_SAME<__VA_ARGS__, constify<T>::type>()

  TEST_IMPL(int &&, int const &&);
  TEST_IMPL(int &, int const &);
  TEST_IMPL(int, int const);
  TEST_IMPL(int *&&, int *const &&);
  TEST_IMPL(int *&, int *const &);
  TEST_IMPL(int *, int *const);
  TEST_IMPL(int *const &&, int *const &&);
  TEST_IMPL(int *const &, int *const &);
  TEST_IMPL(int *const, int *const);

  TEST_IMPL(int const &&, int const &&);
  TEST_IMPL(int const &, int const &);
  TEST_IMPL(int const, int const);
  TEST_IMPL(int const *&&, int const *const &&);
  TEST_IMPL(int const *&, int const *const &);
  TEST_IMPL(int const *, int const *const);
  TEST_IMPL(int const *const &&, int const *const &&);
  TEST_IMPL(int const *const &, int const *const &);
  TEST_IMPL(int const *const, int const *const);

# undef TEST_IMPL
}

} // namespace fatal {
