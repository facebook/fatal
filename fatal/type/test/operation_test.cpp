/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/operation.h>

#include <fatal/test/driver.h>

#include <fatal/preprocessor.h>
#include <fatal/type/deprecated/type_map.h>
#include <fatal/type/sequence.h>

#include <tuple>
#include <type_traits>

namespace fatal {

template <typename> struct dummy_unit {};
template <typename, typename> struct dummy_pair {};
template <typename...> struct dummy_list {};
template <typename T, T...> struct dummy_sequence {};
template <int... Values> using int_seq = sequence<int, Values...>;

////////////
// expand //
////////////

FATAL_TEST(operation, expand) {
  FATAL_EXPECT_SAME<
    type_list<void, int, double>,
    expand<type_list, void>::front<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<int, double, void>,
    expand<type_list, void>::back<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<void, int, double>,
    expand<type_list, dummy_unit<void>>::front<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<int, double, void>,
    expand<type_list, dummy_unit<void>>::back<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<void, bool, int, double>,
    expand<type_list, dummy_pair<void, bool>>::front<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<int, double, void, bool>,
    expand<type_list, dummy_pair<void, bool>>::back<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<void, bool, int, double>,
    expand<type_list, dummy_list<void, bool>>::front<int, double>
  >();

  FATAL_EXPECT_SAME<
    type_list<int, double, void, bool>,
    expand<type_list, dummy_list<void, bool>>::back<int, double>
  >();
}

///////////////////////
// cartesian_product //
///////////////////////

FATAL_TEST(operation, cartesian_product) {
# define TEST_IMPL(List, Pair, LHS, RHS, ...) \
  FATAL_EXPECT_SAME< \
    List<__VA_ARGS__>, \
    fatal::bound::cartesian_product<List, Pair>::apply< \
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

//////////////////////
// flatten_sequence //
//////////////////////

FATAL_TEST(operation, flatten_sequence) {
  FATAL_EXPECT_SAME<
    int_seq<>,
    flatten_sequence<int, sequence>::apply<>
  >();

  FATAL_EXPECT_SAME<
    int_seq<>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>,
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<20>,
      int_seq<30>,
      int_seq<40>,
      int_seq<50>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<>,
      int_seq<20>,
      int_seq<>,
      int_seq<30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>,
      int_seq<>,
      int_seq<20>,
      int_seq<>,
      int_seq<30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<>,
      int_seq<20>,
      int_seq<>,
      int_seq<30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50>,
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>,
      int_seq<>,
      int_seq<20>,
      int_seq<>,
      int_seq<30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50>,
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50, 60, 70>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<20, 30>,
      int_seq<40>,
      int_seq<50, 60, 70>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50, 60, 70>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<>,
      int_seq<20, 30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50, 60, 70>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50, 60, 70>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>,
      int_seq<>,
      int_seq<20, 30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50, 60, 70>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50, 60, 70>,
    flatten_sequence<int, sequence>::apply<
      int_seq<10>,
      int_seq<>,
      int_seq<20, 30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50, 60, 70>,
      int_seq<>
    >
  >();

  FATAL_EXPECT_SAME<
    int_seq<10, 20, 30, 40, 50, 60, 70>,
    flatten_sequence<int, sequence>::apply<
      int_seq<>,
      int_seq<10>,
      int_seq<>,
      int_seq<20, 30>,
      int_seq<>,
      int_seq<40>,
      int_seq<>,
      int_seq<50, 60, 70>,
      int_seq<>
    >
  >();
}

//////////////////////////
// expand_recursive_map //
//////////////////////////

template <int Value>
using iv = std::integral_constant<int, Value>;

template <int Value>
struct tr: public std::integral_constant<int, Value> {};

template <typename T>
using to_seq = typename T::template apply_typed_values<int, sequence>;

template <typename T>
using cut_last = typename T::template split<T::size - 1>::template transform<
  to_seq,
  type_get_first
>;

FATAL_TEST(operation, expand_recursive_map) {
  using small_map = build_type_map<
    iv<0>, build_type_map<
      iv<4>, build_type_map<
        iv<3>, build_type_map<
          iv<3>, tr<0>,
          iv<1>, tr<1>
        >,
        iv<2>, build_type_map<
          iv<0>, tr<2>
        >
      >
    >
  >;

  FATAL_EXPECT_SAME<
    type_list<
      dummy_list<iv<0>, iv<4>, iv<3>, iv<3>, tr<0>>,
      dummy_list<iv<0>, iv<4>, iv<3>, iv<1>, tr<1>>,
      dummy_list<iv<0>, iv<4>, iv<2>, iv<0>, tr<2>>
    >,
    expand_recursive_map<type_map, type_list, dummy_list>::apply<small_map>
  >();

  using big_map = build_type_map<
    iv<0>, build_type_map<
      iv<4>, build_type_map<
        iv<3>, build_type_map<
          iv<3>, tr<0>,
          iv<1>, tr<1>
        >,
        iv<2>, build_type_map<
          iv<0>, tr<2>
        >
      >,
      iv<5>, build_type_map<
        iv<1>, build_type_map<
          iv<2>, build_type_map<
            iv<3>, build_type_map<
              iv<4>, build_type_map<
                iv<5>, tr<3>,
                iv<6>, tr<4>
              >,
              iv<7>, build_type_map<
                iv<8>, build_type_map<
                  iv<9>, tr<5>
                >
              >
            >,
            iv<0>, tr<6>
          >,
          iv<1>, tr<7>
        >
      >,
      iv<6>, tr<8>
    >,
    iv<2>, build_type_map<
      iv<7>, tr<9>,
      iv<2>, tr<10>,
      iv<0>, tr<11>
    >,
    iv<3>, build_type_map<
      iv<9>, tr<12>,
      iv<8>, build_type_map<
        iv<0>, tr<13>
      >,
      iv<6>, tr<14>
    >
  >;

  FATAL_EXPECT_SAME<
    dummy_list<
      std::tuple<iv<0>, iv<4>, iv<3>, iv<3>, tr<0>>,
      std::tuple<iv<0>, iv<4>, iv<3>, iv<1>, tr<1>>,
      std::tuple<iv<0>, iv<4>, iv<2>, iv<0>, tr<2>>,
      std::tuple<iv<0>, iv<5>, iv<1>, iv<2>, iv<3>, iv<4>, iv<5>, tr<3>>,
      std::tuple<iv<0>, iv<5>, iv<1>, iv<2>, iv<3>, iv<4>, iv<6>, tr<4>>,
      std::tuple<iv<0>, iv<5>, iv<1>, iv<2>, iv<3>, iv<7>, iv<8>, iv<9>, tr<5>>,
      std::tuple<iv<0>, iv<5>, iv<1>, iv<2>, iv<0>, tr<6>>,
      std::tuple<iv<0>, iv<5>, iv<1>, iv<1>, tr<7>>,
      std::tuple<iv<0>, iv<6>, tr<8>>,
      std::tuple<iv<2>, iv<7>, tr<9>>,
      std::tuple<iv<2>, iv<2>, tr<10>>,
      std::tuple<iv<2>, iv<0>, tr<11>>,
      std::tuple<iv<3>, iv<9>, tr<12>>,
      std::tuple<iv<3>, iv<8>, iv<0>, tr<13>>,
      std::tuple<iv<3>, iv<6>, tr<14>>
    >,
    expand_recursive_map<type_map, dummy_list, std::tuple>::apply<big_map>
  >();

  FATAL_EXPECT_SAME<
    type_list<
      sequence<int, 0, 4, 3, 3, 0>,
      sequence<int, 0, 4, 3, 1, 1>,
      sequence<int, 0, 4, 2, 0, 2>,
      sequence<int, 0, 5, 1, 2, 3, 4, 5, 3>,
      sequence<int, 0, 5, 1, 2, 3, 4, 6, 4>,
      sequence<int, 0, 5, 1, 2, 3, 7, 8, 9, 5>,
      sequence<int, 0, 5, 1, 2, 0, 6>,
      sequence<int, 0, 5, 1, 1, 7>,
      sequence<int, 0, 6, 8>,
      sequence<int, 2, 7, 9>,
      sequence<int, 2, 2, 10>,
      sequence<int, 2, 0, 11>,
      sequence<int, 3, 9, 12>,
      sequence<int, 3, 8, 0, 13>,
      sequence<int, 3, 6, 14>
    >,
    expand_recursive_map<type_map, type_list>::apply<big_map>
      ::transform<to_seq>
  >();

  FATAL_EXPECT_SAME<
    type_map<
      type_pair<sequence<int, 0, 4, 3, 3>, tr<0>>,
      type_pair<sequence<int, 0, 4, 3, 1>, tr<1>>,
      type_pair<sequence<int, 0, 4, 2, 0>, tr<2>>,
      type_pair<sequence<int, 0, 5, 1, 2, 3, 4, 5>, tr<3>>,
      type_pair<sequence<int, 0, 5, 1, 2, 3, 4, 6>, tr<4>>,
      type_pair<sequence<int, 0, 5, 1, 2, 3, 7, 8, 9>, tr<5>>,
      type_pair<sequence<int, 0, 5, 1, 2, 0>, tr<6>>,
      type_pair<sequence<int, 0, 5, 1, 1>, tr<7>>,
      type_pair<sequence<int, 0, 6>, tr<8>>,
      type_pair<sequence<int, 2, 7>, tr<9>>,
      type_pair<sequence<int, 2, 2>, tr<10>>,
      type_pair<sequence<int, 2, 0>, tr<11>>,
      type_pair<sequence<int, 3, 9>, tr<12>>,
      type_pair<sequence<int, 3, 8, 0>, tr<13>>,
      type_pair<sequence<int, 3, 6>, tr<14>>
    >,
    expand_recursive_map<type_map, type_list>::apply<big_map>
      ::transform<cut_last>::apply<type_map>
  >();
}

} // namespace fatal {
