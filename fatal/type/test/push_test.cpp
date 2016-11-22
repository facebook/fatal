/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/push.h>

#include <fatal/type/list.h>
#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

namespace fatal {

using seq = index_sequence<0, 1, 2, 3, 4, 5, 6, 7>;
using lst = list<int, double, bool, void, float, char, unsigned>;

FATAL_TEST(push, list front) {
  FATAL_EXPECT_SAME<lst, push<lst>::front<>>();

  FATAL_EXPECT_SAME<
    list<void *, int, double, bool, void, float, char, unsigned>,
    push<lst>::front<void *>
  >();

  FATAL_EXPECT_SAME<
    list<void *, int *, char *, int, double, bool, void, float, char, unsigned>,
    push<lst>::front<void *, int *, char *>
  >();

  FATAL_EXPECT_SAME<lst, push_front<lst>>();

  FATAL_EXPECT_SAME<
    list<void *, int, double, bool, void, float, char, unsigned>,
    push_front<lst, void *>
  >();

  FATAL_EXPECT_SAME<
    list<void *, int *, char *, int, double, bool, void, float, char, unsigned>,
    push_front<lst, void *, int *, char *>
  >();
}

FATAL_TEST(push, sequence front) {
  FATAL_EXPECT_SAME<seq, push<seq>::front<>>();

  FATAL_EXPECT_SAME<
    index_sequence<9, 0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front<9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front<9, 10, 11>
  >();
}

FATAL_TEST(push, list back) {
  FATAL_EXPECT_SAME<lst, push<lst>::back<>>();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *>,
    push<lst>::back<void *>
  >();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *, int *, char *>,
    push<lst>::back<void *, int *, char *>
  >();

  FATAL_EXPECT_SAME<lst, push_back<lst>>();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *>,
    push_back<lst, void *>
  >();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *, int *, char *>,
    push_back<lst, void *, int *, char *>
  >();
}

FATAL_TEST(push, sequence back) {
  FATAL_EXPECT_SAME<seq, push<seq>::back<>>();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 9>,
    push<seq>::back<9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11>,
    push<seq>::back<9, 10, 11>
  >();
}

FATAL_TEST(push, list front_if true) {
  FATAL_EXPECT_SAME<lst, push<lst>::front_if<true>>();

  FATAL_EXPECT_SAME<
    list<void *, int, double, bool, void, float, char, unsigned>,
    push<lst>::front_if<true, void *>
  >();

  FATAL_EXPECT_SAME<
    list<void *, int *, char *, int, double, bool, void, float, char, unsigned>,
    push<lst>::front_if<true, void *, int *, char *>
  >();

  FATAL_EXPECT_SAME<lst, push_front_if<true, lst>>();

  FATAL_EXPECT_SAME<
    list<void *, int, double, bool, void, float, char, unsigned>,
    push_front_if<true, lst, void *>
  >();

  FATAL_EXPECT_SAME<
    list<void *, int *, char *, int, double, bool, void, float, char, unsigned>,
    push_front_if<true, lst, void *, int *, char *>
  >();
}

FATAL_TEST(push, sequence front_if true) {
/*
  FATAL_EXPECT_SAME<seq, push<seq>::front_if<true>>();

  FATAL_EXPECT_SAME<
    index_sequence<9, 0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front_if<true, 9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front_if<true, 9, 10, 11>
  >();
  */
}

FATAL_TEST(push, list back_if true) {
  FATAL_EXPECT_SAME<lst, push<lst>::back_if<true>>();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *>,
    push<lst>::back_if<true, void *>
  >();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *, int *, char *>,
    push<lst>::back_if<true, void *, int *, char *>
  >();

  FATAL_EXPECT_SAME<lst, push_back_if<true, lst>>();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *>,
    push_back_if<true, lst, void *>
  >();

  FATAL_EXPECT_SAME<
    list<int, double, bool, void, float, char, unsigned, void *, int *, char *>,
    push_back_if<true, lst, void *, int *, char *>
  >();
}

FATAL_TEST(push, sequence back_if true) {
/*
  FATAL_EXPECT_SAME<seq, push<seq>::back_if<true>>();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 9>,
    push<seq>::back_if<true, 9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11>,
    push<seq>::back_if<true, 9, 10, 11>
  >();
  */
}

FATAL_TEST(push, list front_if false) {
  FATAL_EXPECT_SAME<lst, push<lst>::front_if<false>>();
  FATAL_EXPECT_SAME<lst, push<lst>::front_if<false, void *>>();
  FATAL_EXPECT_SAME<lst, push<lst>::front_if<false, void *, int *, char *>>();

  FATAL_EXPECT_SAME<lst, push_front_if<false, lst>>();
  FATAL_EXPECT_SAME<lst, push_front_if<false, lst, void *>>();
  FATAL_EXPECT_SAME<lst, push_front_if<false, lst, void *, int *, char *>>();
}

FATAL_TEST(push, sequence front_if false) {
/*
  FATAL_EXPECT_SAME<seq, push<seq>::front_if<false>>();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front_if<false, 9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::front_if<false, 9, 10, 11>
  >();
  */
}

FATAL_TEST(push, list back_if false) {
  FATAL_EXPECT_SAME<lst, push<lst>::back_if<false>>();
  FATAL_EXPECT_SAME<lst, push<lst>::back_if<false, void *>>();
  FATAL_EXPECT_SAME<lst, push<lst>::back_if<false, void *, int *, char *>>();

  FATAL_EXPECT_SAME<lst, push_back_if<false, lst>>();
  FATAL_EXPECT_SAME<lst, push_back_if<false, lst, void *>>();
  FATAL_EXPECT_SAME<lst, push_back_if<false, lst, void *, int *, char *>>();
}

FATAL_TEST(push, sequence back_if false) {
/*
  FATAL_EXPECT_SAME<seq, push<seq>::back_if<false>>();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::back_if<false, 9>
  >();

  FATAL_EXPECT_SAME<
    index_sequence<0, 1, 2, 3, 4, 5, 6, 7>,
    push<seq>::back_if<false, 9, 10, 11>
  >();
  */
}

} // namespace fatal {
