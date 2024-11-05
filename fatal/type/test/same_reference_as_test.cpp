/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/same_reference_as.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(traits, same_reference_as) {
# define TEST_IMPL(From, T, ...) \
  FATAL_EXPECT_SAME<__VA_ARGS__, same_reference_as<T, From>::type>()

  TEST_IMPL(int &&, int &&, int &&);
  TEST_IMPL(int &&, int &, int &&);
  TEST_IMPL(int &&, int, int &&);
  TEST_IMPL(int &&, int *&&, int *&&);
  TEST_IMPL(int &&, int *&, int *&&);
  TEST_IMPL(int &&, int *, int *&&);
  TEST_IMPL(int &&, int *const &&, int *const &&);
  TEST_IMPL(int &&, int *const &, int *const &&);
  TEST_IMPL(int &&, int *const, int *const &&);
  TEST_IMPL(int &&, int const &&, int const &&);
  TEST_IMPL(int &&, int const &, int const &&);
  TEST_IMPL(int &&, int const, int const &&);
  TEST_IMPL(int &&, int const *&&, int const *&&);
  TEST_IMPL(int &&, int const *&, int const *&&);
  TEST_IMPL(int &&, int const *, int const *&&);
  TEST_IMPL(int &&, int const *const &&, int const *const &&);
  TEST_IMPL(int &&, int const *const &, int const *const &&);
  TEST_IMPL(int &&, int const *const, int const *const &&);

  TEST_IMPL(int &, int &&, int &);
  TEST_IMPL(int &, int &, int &);
  TEST_IMPL(int &, int, int &);
  TEST_IMPL(int &, int *&&, int *&);
  TEST_IMPL(int &, int *&, int *&);
  TEST_IMPL(int &, int *, int *&);
  TEST_IMPL(int &, int *const &&, int *const &);
  TEST_IMPL(int &, int *const &, int *const &);
  TEST_IMPL(int &, int *const, int *const &);
  TEST_IMPL(int &, int const &&, int const &);
  TEST_IMPL(int &, int const &, int const &);
  TEST_IMPL(int &, int const, int const &);
  TEST_IMPL(int &, int const *&&, int const *&);
  TEST_IMPL(int &, int const *&, int const *&);
  TEST_IMPL(int &, int const *, int const *&);
  TEST_IMPL(int &, int const *const &&, int const *const &);
  TEST_IMPL(int &, int const *const &, int const *const &);
  TEST_IMPL(int &, int const *const, int const *const &);

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

# undef TEST_IMPL
}

} // namespace fatal {
