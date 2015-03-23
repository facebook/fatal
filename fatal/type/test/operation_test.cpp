/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/operation.h>

#include <fatal/test/driver.h>

#include <fatal/preprocessor.h>

namespace fatal {

template <typename, typename> struct dummy_pair {};
template <typename...> struct dummy_list {};

///////////////////////
// cartesian_product //
///////////////////////

FATAL_TEST(operation, cartesian_product) {
# define TEST_IMPL(List, Pair, LHS, RHS, ...) \
  FATAL_EXPECT_SAME< \
    List<__VA_ARGS__>, \
    fatal::cartesian_product< \
      List, Pair, \
      FATAL_UNPARENTHESIZE(LHS), \
      FATAL_UNPARENTHESIZE(RHS) \
    > \
  >()

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<>),
    (dummy_list<>)
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<int>),
    (dummy_list<>)
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<>),
    (dummy_list<int>)
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<int>),
    (dummy_list<bool>),
    dummy_pair<int, bool>
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<int, void>),
    (dummy_list<double, bool>),
    dummy_pair<int, double>,
    dummy_pair<int, bool>,
    dummy_pair<void, double>,
    dummy_pair<void, bool>
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_pair<int, void>),
    (dummy_pair<double, bool>),
    dummy_pair<int, double>,
    dummy_pair<int, bool>,
    dummy_pair<void, double>,
    dummy_pair<void, bool>
  );

  TEST_IMPL(
    dummy_list, dummy_list,
    (dummy_list<int, void>),
    (dummy_list<double, bool>),
    dummy_list<int, double>,
    dummy_list<int, bool>,
    dummy_list<void, double>,
    dummy_list<void, bool>
  );

  TEST_IMPL(
    dummy_list, dummy_pair,
    (dummy_list<int, void, float>),
    (dummy_list<double, bool, short>),
    dummy_pair<int, double>,
    dummy_pair<int, bool>,
    dummy_pair<int, short>,
    dummy_pair<void, double>,
    dummy_pair<void, bool>,
    dummy_pair<void, short>,
    dummy_pair<float, double>,
    dummy_pair<float, bool>,
    dummy_pair<float, short>
  );

# undef TEST_IMPL
}

/////////////
// flatten //
/////////////

FATAL_TEST(operation, flatten) {
  FATAL_EXPECT_SAME<
    dummy_list<
      int,
      double,
      float,
      dummy_list<short>,
      bool
    >,
    flatten<
      dummy_list,
      dummy_list,
      int,
      dummy_list<
        double,
        float,
        dummy_list<short>
      >,
      bool
    >
  >();

  FATAL_EXPECT_SAME<
    std::tuple<
      int,
      double,
      float,
      dummy_list<short>,
      bool
    >,
    flatten<
      std::tuple,
      dummy_list,
      int,
      dummy_list<
        double,
        float,
        dummy_list<short>
      >,
      bool
    >
  >();

  // TODO: ADD MORE TESTS
}

} // namespace fatal {
