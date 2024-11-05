/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/constify_from.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(traits, constify_from) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME<__VA_ARGS__, constify_from<T, From>::type>()

  TEST_IMPL(int, int &&, int &&);
  TEST_IMPL(int, int &, int &);
  TEST_IMPL(int, int, int);
  TEST_IMPL(int, int *&&, int *&&);
  TEST_IMPL(int, int *&, int *&);
  TEST_IMPL(int, int *, int *);
  TEST_IMPL(int, int *const &&, int *const &&);
  TEST_IMPL(int, int *const &, int *const &);
  TEST_IMPL(int, int *const, int *const);
  TEST_IMPL(int, int const &&, int const &&);
  TEST_IMPL(int, int const &, int const &);
  TEST_IMPL(int, int const, int const);
  TEST_IMPL(int, int const *&&, int const *&&);
  TEST_IMPL(int, int const *&, int const *&);
  TEST_IMPL(int, int const *, int const *);
  TEST_IMPL(int, int const *const &&, int const *const &&);
  TEST_IMPL(int, int const *const &, int const *const &);
  TEST_IMPL(int, int const *const, int const *const);

  TEST_IMPL(int const, int &&, int const &&);
  TEST_IMPL(int const, int &, int const &);
  TEST_IMPL(int const, int, int const);
  TEST_IMPL(int const, int *&&, int *const &&);
  TEST_IMPL(int const, int *&, int *const &);
  TEST_IMPL(int const, int *, int *const);
  TEST_IMPL(int const, int *const &&, int *const &&);
  TEST_IMPL(int const, int *const &, int *const &);
  TEST_IMPL(int const, int *const, int *const);
  TEST_IMPL(int const, int const &&, int const &&);
  TEST_IMPL(int const, int const &, int const &);
  TEST_IMPL(int const, int const, int const);
  TEST_IMPL(int const, int const *&&, int const *const &&);
  TEST_IMPL(int const, int const *&, int const *const &);
  TEST_IMPL(int const, int const *, int const *const);
  TEST_IMPL(int const, int const *const &&, int const *const &&);
  TEST_IMPL(int const, int const *const &, int const *const &);
  TEST_IMPL(int const, int const *const, int const *const);

# undef TEST_IMPL
}

} // namespace fatal {
