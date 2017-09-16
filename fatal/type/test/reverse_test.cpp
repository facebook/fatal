/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/list.h>
#include <fatal/type/pair.h>
#include <fatal/type/reverse.h>

#include <fatal/test/driver.h>

namespace fatal {

template <int> struct t {};
using t0 = t<0>; using t1 = t<1>; using t2 = t<2>; using t3 = t<3>;
using t4 = t<4>; using t5 = t<5>; using t6 = t<6>; using t7 = t<7>;
using t8 = t<8>; using t9 = t<9>;
template <int> struct u {};
using u0 = u<0>; using u1 = u<1>; using u2 = u<2>; using u3 = u<3>;
using u4 = u<4>; using u5 = u<5>; using u6 = u<6>; using u7 = u<7>;
using u8 = u<8>; using u9 = u<9>;

FATAL_TEST(reverse, pair) {
  FATAL_EXPECT_SAME<pair<t0, t1>, reverse<pair<t1, t0>>>();
}

FATAL_TEST(reverse, list) {
  FATAL_EXPECT_SAME<list<>, reverse<list<>>>();
  FATAL_EXPECT_SAME<list<t0>, reverse<list<t0>>>();
  FATAL_EXPECT_SAME<list<t0, t1>, reverse<list<t1, t0>>>();
  FATAL_EXPECT_SAME<list<t0, t1, t2>, reverse<list<t2, t1, t0>>>();
  FATAL_EXPECT_SAME<list<t0, t1, t2, t3>, reverse<list<t3, t2, t1, t0>>>();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4>,
    reverse<list<t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5>,
    reverse<list<t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6>,
    reverse<list<t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7>,
    reverse<list<t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8>,
    reverse<list<t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9>,
    reverse<list<t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0>,
    reverse<list<u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1>,
    reverse<list<u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2>,
    reverse<list<u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3>,
    reverse<list<u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4>,
    reverse<list<u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>>
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4, u5>,
    reverse<
      list<u5, u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>
    >
  >();
  FATAL_EXPECT_SAME<
    list<t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4, u5, u6>,
    reverse<
      list<u6, u5, u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0>
    >
  >();
  FATAL_EXPECT_SAME<
    list<
      t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4, u5, u6, u7
    >,
    reverse<
      list<
        u7, u6, u5, u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0
      >
    >
  >();
  FATAL_EXPECT_SAME<
    list<
      t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4, u5, u6, u7, u8
    >,
    reverse<
      list<
        u8, u7, u6, u5, u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2, t1,
        t0
      >
    >
  >();
  FATAL_EXPECT_SAME<
    list<
      t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, u0, u1, u2, u3, u4, u5, u6, u7,
      u8, u9
    >,
    reverse<
      list<
        u9, u8, u7, u6, u5, u4, u3, u2, u1, u0, t9, t8, t7, t6, t5, t4, t3, t2,
        t1, t0
      >
    >
  >();
}

FATAL_TEST(reverse, sequence) {
  FATAL_EXPECT_SAME<index_sequence<>, reverse<index_sequence<>>>();
  FATAL_EXPECT_SAME<index_sequence<0>, reverse<index_sequence<0>>>();
  FATAL_EXPECT_SAME<index_sequence<0, 1>, reverse<index_sequence<1, 0>>>();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2>,
    reverse<index_sequence<2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3>,
    reverse<index_sequence<3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4>,
    reverse<index_sequence<4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5>,
    reverse<index_sequence<5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6>,
    reverse<index_sequence<6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7>,
    reverse<index_sequence<7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8>,
    reverse<index_sequence<8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    reverse<index_sequence<9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>,
    reverse<index_sequence<10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>,
    reverse<index_sequence<11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12>,
    reverse<index_sequence<12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>,
    reverse<index_sequence<13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>,
    reverse<index_sequence<14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>>
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15>,
    reverse<
      index_sequence<15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>
    >
  >();
  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16>,
    reverse<
      index_sequence<16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>
    >
  >();
  FATAL_EXPECT_SAME<
    index_sequence<
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
    >,
    reverse<
      index_sequence<
        17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
      >
    >
  >();
  FATAL_EXPECT_SAME<
    index_sequence<
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18
    >,
    reverse<
      index_sequence<
        18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
      >
    >
  >();
  FATAL_EXPECT_SAME<
    index_sequence<
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    >,
    reverse<
      index_sequence<
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
      >
    >
  >();
}

} // namespace fatal {
