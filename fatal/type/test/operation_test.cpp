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
#include <fatal/type/test/parse_sequence_input.h>

namespace fatal {

template <typename, typename> struct dummy_pair {};
template <typename...> struct dummy_list {};
template <typename T, T...> struct dummy_sequence {};

///////////////////////
// cartesian_product //
///////////////////////

FATAL_TEST(operation, cartesian_product) {
# define TEST_IMPL(List, Pair, LHS, RHS, ...) \
  FATAL_EXPECT_SAME< \
    List<__VA_ARGS__>, \
    fatal::cartesian_product<List, Pair>::apply< \
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
    flatten<dummy_list, dummy_list>::apply<
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
    flatten<std::tuple, dummy_list>::apply<
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

/////////////////
// to_sequence //
/////////////////

FATAL_TEST(operation, to_sequence) {
# define TEST_IMPL(T, Value, TChar, ...) \
  do { \
    FATAL_EXPECT_SAME< \
      dummy_sequence<TChar, __VA_ARGS__>, \
      to_sequence<T, Value, dummy_sequence, TChar> \
    >(); \
  } while (false)

  FATAL_IMPL_PARSE_SEQUENCE_TEST_CALLS(TEST_IMPL);

# undef TEST_IMPL
}

////////////////////
// parse_sequence //
////////////////////

FATAL_TEST(operation, parse_sequence) {
# define TEST_IMPL(T, Value, TChar, ...) \
  do { \
    using expected = std::integral_constant<T, Value>; \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      parse_sequence<T>::bind<TChar>::apply<__VA_ARGS__> \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      parse_sequence<T>::apply<TChar, __VA_ARGS__> \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      parse_sequence<T>::from<dummy_sequence<TChar, __VA_ARGS__>> \
    >(); \
  } while (false)

  FATAL_IMPL_PARSE_SEQUENCE_TEST_CALLS(TEST_IMPL);

# undef TEST_IMPL
}

} // namespace fatal {
