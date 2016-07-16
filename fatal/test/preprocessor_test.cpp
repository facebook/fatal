/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/preprocessor.h>

#include <fatal/test/driver.h>

#include <array>
#include <vector>

namespace fatal {

template <typename T, typename... Args>
std::vector<typename std::decay<T>::type> to_vector(T &&value, Args &&...args) {
  return { std::forward<T>(value), std::forward<Args>(args)... };
}

/////////////////
// FATAL_EMPTY //
/////////////////

FATAL_TEST(preprocessor, fatal_empty) {
  int FATAL_EMPTY() x = 1;
  FATAL_EXPECT_EQ(1, x);

  ++ FATAL_EMPTY() x;
  FATAL_EXPECT_EQ(2, x);

  FATAL_EXPECT_EQ(2, x FATAL_EMPTY());
  FATAL_EXPECT_EQ(3, x + FATAL_EMPTY() 1);
}

//////////////////
// FATAL_IGNORE //
//////////////////

FATAL_TEST(preprocessor, fatal_ignore) {
  int FATAL_IGNORE(~) x = 1;
  FATAL_EXPECT_EQ(1, x);

  FATAL_IGNORE(++x);
  FATAL_EXPECT_EQ(1, x);
  ++x;
  FATAL_EXPECT_EQ(2, x);

  FATAL_EXPECT_EQ(2, x FATAL_IGNORE(+ 1));
  FATAL_EXPECT_EQ(3, x + 1);
}

////////////////////
// FATAL_IDENTITY //
////////////////////

FATAL_TEST(preprocessor, fatal_identity) {
  int x = FATAL_IDENTITY(1);
  FATAL_EXPECT_EQ(1, x);

  FATAL_IDENTITY(++x);
  FATAL_EXPECT_EQ(2, x);

  FATAL_EXPECT_EQ(3, x FATAL_IDENTITY(+ 1));
}

///////////////
// FATAL_CAT //
///////////////

FATAL_TEST(preprocessor, fatal_cat) {
  int a = 1;
  FATAL_EXPECT_EQ(a, FATAL_CAT(a, ));
  FATAL_EXPECT_EQ(a, FATAL_CAT(, a));

  int b = 2;
  FATAL_EXPECT_EQ(b, FATAL_CAT(b, ));
  FATAL_EXPECT_EQ(b, FATAL_CAT(, b));

  int c = 3;
  FATAL_EXPECT_EQ(c, FATAL_CAT(c, ));
  FATAL_EXPECT_EQ(c, FATAL_CAT(, c));

  int ab = 12;
  FATAL_EXPECT_EQ(ab, FATAL_CAT(a, b));

  int ac = 13;
  FATAL_EXPECT_EQ(ac, FATAL_CAT(a, c));

  int bc = 23;
  FATAL_EXPECT_EQ(bc, FATAL_CAT(b, c));

  int abc = 123;
  FATAL_EXPECT_EQ(abc, FATAL_CAT(a, FATAL_CAT(b, c)));
  FATAL_EXPECT_EQ(abc, FATAL_CAT(FATAL_CAT(a, b), c));
}

///////////////
// FATAL_NOT //
///////////////

FATAL_TEST(preprocessor, fatal_not) {
  FATAL_EXPECT_EQ(1, FATAL_NOT());
  FATAL_EXPECT_EQ(1, FATAL_NOT(0));
  FATAL_EXPECT_EQ(0, FATAL_NOT(1));
  FATAL_EXPECT_EQ(0, FATAL_NOT(56));
  FATAL_EXPECT_EQ(0, FATAL_NOT(arg));
}

////////////////
// FATAL_BOOL //
////////////////

FATAL_TEST(preprocessor, fatal_bool) {
  FATAL_EXPECT_EQ(0, FATAL_BOOL(0));
  FATAL_EXPECT_EQ(1, FATAL_BOOL(1));
  FATAL_EXPECT_EQ(1, FATAL_BOOL(56));
  FATAL_EXPECT_EQ(1, FATAL_BOOL(arg));
}

////////////////////////////
// FATAL_IS_PARENTHESIZED //
////////////////////////////

FATAL_TEST(preprocessor, fatal_is_parenthesized) {
# define TEST_IMPL(...) \
    FATAL_IS_PARENTHESIZED(__VA_ARGS__)

  FATAL_EXPECT_EQ(0, TEST_IMPL(0));
  FATAL_EXPECT_EQ(1, TEST_IMPL((0)));
  FATAL_EXPECT_EQ(0, TEST_IMPL(0, 2));
  FATAL_EXPECT_EQ(1, TEST_IMPL((0, 2)));
  FATAL_EXPECT_EQ(1, TEST_IMPL((0), (2)));
  FATAL_EXPECT_EQ(1, TEST_IMPL(((0), (2))));

  FATAL_EXPECT_EQ(0, TEST_IMPL(1));
  FATAL_EXPECT_EQ(1, TEST_IMPL((1)));
  FATAL_EXPECT_EQ(0, TEST_IMPL(1, 2));
  FATAL_EXPECT_EQ(1, TEST_IMPL((1, 2)));
  FATAL_EXPECT_EQ(1, TEST_IMPL((1), (2)));
  FATAL_EXPECT_EQ(1, TEST_IMPL(((1), (2))));

# undef TEST_IMPL
}

//////////////////////////
// FATAL_UNPARENTHESIZE //
//////////////////////////

FATAL_TEST(preprocessor, fatal_unparenthesize) {
# define TEST_IMPL(...) \
    FATAL_TO_STR(FATAL_UNPARENTHESIZE(__VA_ARGS__))

  FATAL_EXPECT_EQ("0", TEST_IMPL(0));
  FATAL_EXPECT_EQ("0", TEST_IMPL((0)));
  FATAL_EXPECT_EQ("0, 2", TEST_IMPL(0, 2));
  FATAL_EXPECT_EQ("0, 2", TEST_IMPL((0, 2)));
  FATAL_EXPECT_EQ("0, (2)", TEST_IMPL((0), (2)));
  FATAL_EXPECT_EQ("(0), (2)", TEST_IMPL(((0), (2))));

  FATAL_EXPECT_EQ("1", TEST_IMPL(1));
  FATAL_EXPECT_EQ("1", TEST_IMPL((1)));
  FATAL_EXPECT_EQ("1, 2", TEST_IMPL(1, 2));
  FATAL_EXPECT_EQ("1, 2", TEST_IMPL((1, 2)));
  FATAL_EXPECT_EQ("1, (2)", TEST_IMPL((1), (2)));
  FATAL_EXPECT_EQ("(1), (2)", TEST_IMPL(((1), (2))));

# undef TEST_IMPL
}

/////////////////
// FATAL_ARG_n //
/////////////////

FATAL_TEST(preprocessor, fatal_arg_n) {
# define TEST_IMPL(Fn) \
  Fn(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)

  FATAL_EXPECT_EQ(1, TEST_IMPL(FATAL_ARG_1));
  FATAL_EXPECT_EQ(2, TEST_IMPL(FATAL_ARG_2));
  FATAL_EXPECT_EQ(3, TEST_IMPL(FATAL_ARG_3));
  FATAL_EXPECT_EQ(4, TEST_IMPL(FATAL_ARG_4));
  FATAL_EXPECT_EQ(5, TEST_IMPL(FATAL_ARG_5));
  FATAL_EXPECT_EQ(6, TEST_IMPL(FATAL_ARG_6));
  FATAL_EXPECT_EQ(7, TEST_IMPL(FATAL_ARG_7));
  FATAL_EXPECT_EQ(8, TEST_IMPL(FATAL_ARG_8));
  FATAL_EXPECT_EQ(9, TEST_IMPL(FATAL_ARG_9));
  FATAL_EXPECT_EQ(10, TEST_IMPL(FATAL_ARG_10));
  FATAL_EXPECT_EQ(11, TEST_IMPL(FATAL_ARG_11));
  FATAL_EXPECT_EQ(12, TEST_IMPL(FATAL_ARG_12));
  FATAL_EXPECT_EQ(13, TEST_IMPL(FATAL_ARG_13));
  FATAL_EXPECT_EQ(14, TEST_IMPL(FATAL_ARG_14));
  FATAL_EXPECT_EQ(15, TEST_IMPL(FATAL_ARG_15));
  FATAL_EXPECT_EQ(16, TEST_IMPL(FATAL_ARG_16));

# undef TEST_IMPL
}

///////////////////////
// FATAL_SKIP_n_ARGS //
///////////////////////

FATAL_TEST(preprocessor, fatal_skip_n_args) {
# define TEST_IMPL(Fn) \
  Fn(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)

  FATAL_EXPECT_EQ(
    "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_1_ARG))
  );
  FATAL_EXPECT_EQ(
    "2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_2_ARGS))
  );
  FATAL_EXPECT_EQ(
    "3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_3_ARGS))
  );
  FATAL_EXPECT_EQ(
    "4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_4_ARGS))
  );
  FATAL_EXPECT_EQ(
    "5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_5_ARGS))
  );
  FATAL_EXPECT_EQ(
    "6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_6_ARGS))
  );
  FATAL_EXPECT_EQ(
    "7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_7_ARGS))
  );
  FATAL_EXPECT_EQ(
    "8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_8_ARGS))
  );
  FATAL_EXPECT_EQ(
    "9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_9_ARGS))
  );
  FATAL_EXPECT_EQ(
    "10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_10_ARGS))
  );
  FATAL_EXPECT_EQ(
    "11, 12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_11_ARGS))
  );
  FATAL_EXPECT_EQ(
    "12, 13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_12_ARGS))
  );
  FATAL_EXPECT_EQ(
    "13, 14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_13_ARGS))
  );
  FATAL_EXPECT_EQ(
    "14, 15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_14_ARGS))
  );
  FATAL_EXPECT_EQ(
    "15, 16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_15_ARGS))
  );
  FATAL_EXPECT_EQ(
    "16, 17, 18, 19, 20",
    FATAL_TO_STR(TEST_IMPL(FATAL_SKIP_16_ARGS))
  );

# undef TEST_IMPL
}

////////////////////
// FATAL_HAS_ARGS //
////////////////////

FATAL_TEST(preprocessor, fatal_has_args) {
  FATAL_EXPECT_EQ(0, FATAL_HAS_ARGS());
  FATAL_EXPECT_EQ(0, FATAL_HAS_ARGS( ));
  FATAL_EXPECT_EQ(0, FATAL_HAS_ARGS(()));
  FATAL_EXPECT_EQ(0, FATAL_HAS_ARGS(,));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS(0));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS(1));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS(56));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS(arg));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS((x)));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS((x), (y)));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS((x, y)));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS((x, y), (z)));
  FATAL_EXPECT_EQ(1, FATAL_HAS_ARGS((x, y), (z, ~)));
}

///////////////////////
// FATAL_CONDITIONAL //
///////////////////////

FATAL_TEST(preprocessor, fatal_conditional) {
  FATAL_EXPECT_EQ(20, FATAL_CONDITIONAL(0)(10)(20));
  FATAL_EXPECT_EQ(10, FATAL_CONDITIONAL(1)(10)(20));
  FATAL_EXPECT_EQ(10, FATAL_CONDITIONAL(56)(10)(20));
  FATAL_EXPECT_EQ(10, FATAL_CONDITIONAL(arg)(10)(20));
}

///////////////////
// FATAL_DEFER_n //
///////////////////

#define TEST_IMPL(...) |__VA_ARGS__|

FATAL_TEST(preprocessor, fatal_defer_1) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_1(TEST_IMPL)(56)));
  std::string result = FATAL_TO_STR(FATAL_DEFER_1(TEST_IMPL)(56));
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_DEFER_1(TEST_IMPL)(56)))
  );
}

FATAL_TEST(preprocessor, fatal_defer_2) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_2(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56)))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_3) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_3(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_3(TEST_IMPL)(56))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_3(TEST_IMPL)(56)))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_3(TEST_IMPL)(56)
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_4) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_4(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_4(TEST_IMPL)(56)
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_4(TEST_IMPL)(56)
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_4(TEST_IMPL)(56))
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_5) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_5(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56))
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56))
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56)))
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_6) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_6(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_6(TEST_IMPL)(56)))
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_6(TEST_IMPL)(56)))
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_6(TEST_IMPL)(56)
      )))
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_7) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_7(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      )))
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      )))
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      ))))
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_8) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_8(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_8(TEST_IMPL)(56)
      ))))
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_8(TEST_IMPL)(56)
      ))))
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_8(TEST_IMPL)(56))
      ))))
    ))))
  );
}

FATAL_TEST(preprocessor, fatal_defer_9) {
  FATAL_EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_9(TEST_IMPL)(56)));
  FATAL_EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56))
      ))))
    ))))
  );
  std::string result = FATAL_TO_STR(
    FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56))
      ))))
    )))
  );
  FATAL_EXPECT_TRUE(result == "TEST_IMPL(56)" || result == "TEST_IMPL (56)");
  FATAL_EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56)))
      ))))
    ))))
  );
}

#undef TEST_IMPL

////////////////
// FATAL_EVAL //
////////////////

FATAL_TEST(preprocessor, fatal_eval) {
# define TEST_IMPL(...) TEST_IMPL_1 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_1(...) TEST_IMPL_2 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_2(...) TEST_IMPL_3 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_3(...) TEST_IMPL_4 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_4(...) TEST_IMPL_5 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_5(...) TEST_IMPL_6 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_6(...) TEST_IMPL_7 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_7(...) TEST_IMPL_8 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_8(...) TEST_IMPL_9 FATAL_EMPTY() (__VA_ARGS__)
# define TEST_IMPL_9(...) FATAL_IDENTITY FATAL_EMPTY() (__VA_ARGS__)

  FATAL_EXPECT_EQ(56, FATAL_EVAL(TEST_IMPL(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_1(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_2(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_3(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_4(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_5(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_6(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_7(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_8(TEST_IMPL)(56)));
  FATAL_EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_9(TEST_IMPL)(56)));

# undef TEST_IMPL_9
# undef TEST_IMPL_8
# undef TEST_IMPL_7
# undef TEST_IMPL_6
# undef TEST_IMPL_5
# undef TEST_IMPL_4
# undef TEST_IMPL_3
# undef TEST_IMPL_2
# undef TEST_IMPL_1
# undef TEST_IMPL
}

//////////////////////
// FATAL_SIMPLE_MAP //
//////////////////////

FATAL_TEST(preprocessor, fatal_simple_map) {
# define TEST_IMPL(x) + x
  FATAL_EXPECT_EQ(15, 0 FATAL_SIMPLE_MAP(TEST_IMPL, 1, 2, 3, 4, 5));
# undef TEST_IMPL

  FATAL_EXPECT_EQ(
    15,
    FATAL_SIMPLE_MAP(FATAL_IDENTITY, 1, +, 2, +, 3, +, 4, +, 5)
  );

  FATAL_EXPECT_EQ(
    "1 2 3 4 5",
    FATAL_TO_STR(FATAL_SIMPLE_MAP(FATAL_IDENTITY, 1, 2, 3, 4, 5))
  );

# define TEST_IMPL(...) / __VA_ARGS__ |
  FATAL_EXPECT_EQ(
    "/ (1, 2, 3) | / (4, 5, 6) | / (7, 8, 9) |",
    FATAL_TO_STR(
      FATAL_SIMPLE_MAP(TEST_IMPL, (1, 2, 3), (4, 5, 6), (7, 8, 9))
    )
  );
# undef TEST_IMPL

# define TEST_IMPL(...)  to_vector __VA_ARGS__,

  FATAL_EXPECT_EQ(
    to_vector(
      to_vector(1, 2, 3), to_vector(4, 5, 6), to_vector(7, 8, 9),
      std::vector<int>{}
    ),
    to_vector(
      FATAL_SIMPLE_MAP(TEST_IMPL, (1, 2, 3), (4, 5, 6), (7, 8, 9))
      std::vector<int>{}
    )
  );

# undef TEST_IMPL
}

///////////////
// FATAL_MAP //
///////////////

FATAL_TEST(preprocessor, fatal_map) {
# define TEST_IMPL(arg, is_first, index, x) + x
  FATAL_EXPECT_EQ(15, 0 FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5));
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) x
  FATAL_EXPECT_EQ(15, FATAL_MAP(TEST_IMPL, ~, 1, +, 2, +, 3, +, 4, +, 5));
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) x
  FATAL_EXPECT_EQ(
    "1 2 3 4 5",
    FATAL_TO_STR(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5))
  );
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) \
  FATAL_CONDITIONAL(is_first)(x)(+ x)
  FATAL_EXPECT_EQ(15, FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5));
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) / arg is_first x |
  FATAL_EXPECT_EQ(
    "/ ~ 1 1 | / ~ 0 2 | / ~ 0 3 | / ~ 0 4 | / ~ 0 5 |",
    FATAL_TO_STR(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5))
  );
# undef TEST_IMPL

  std::array<int, 5> a;
# define TEST_IMPL(arg, is_first, index, x) \
    do { \
      FATAL_ASSERT_LT(index, a.size()); \
      a[index] = x; \
    } while (false);

  FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5);
# undef TEST_IMPL

  std::vector<int> v;
# define TEST_IMPL(arg, is_first, index, x) \
    do { \
      v.push_back(x); \
    } while (false);

  FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5);
# undef TEST_IMPL

  FATAL_ASSERT_EQ(a.size(), v.size());

  for (auto i = a.size(); i--; ) {
    FATAL_EXPECT_EQ(i + 1, a[i]);
    FATAL_EXPECT_EQ(i + 1, v[i]);
  }

# define TEST_IMPL(arg, is_first, index, ...) / __VA_ARGS__ |
  FATAL_EXPECT_EQ(
    "/ (1, 2, 3) | / (4, 5, 6) | / (7, 8, 9) |",
    FATAL_TO_STR(FATAL_MAP(TEST_IMPL, ~, (1, 2, 3), (4, 5, 6), (7, 8, 9)))
  );
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, ...) \
  FATAL_CONDITIONAL(is_first)()(,) \
  to_vector __VA_ARGS__

  FATAL_EXPECT_EQ(
    to_vector(to_vector(1, 2, 3), to_vector(4, 5, 6), to_vector(7, 8, 9)),
    to_vector(FATAL_MAP(TEST_IMPL, ~, (1, 2, 3), (4, 5, 6), (7, 8, 9)))
  );

# undef TEST_IMPL
}

//////////////////
// FATAL_TO_STR //
//////////////////

FATAL_TEST(preprocessor, fatal_to_str) {
  FATAL_EXPECT_EQ(FATAL_TO_STR(x), "x");
  FATAL_EXPECT_EQ(FATAL_TO_STR(xyz), "xyz");
  FATAL_EXPECT_EQ(FATAL_TO_STR(x y z), "x y z");
}

///////////////
// FATAL_UID //
///////////////

struct uid_tester {
  explicit uid_tester(int &x): x_(x) { ++x_; }
  ~uid_tester() { --x_; }

private:
  int &x_;
};

FATAL_TEST(preprocessor, fatal_uid) {
  int x = 0;
  {
    uid_tester FATAL_UID(prefix)(x);
    FATAL_EXPECT_EQ(1, x);
    {
      uid_tester FATAL_UID(prefix)(x);
      FATAL_EXPECT_EQ(2, x);
      {
        uid_tester FATAL_UID(prefix)(x);
        FATAL_EXPECT_EQ(3, x);
        {
          uid_tester FATAL_UID(prefix)(x);
          FATAL_EXPECT_EQ(4, x);
        }
        FATAL_EXPECT_EQ(3, x);
      }
      FATAL_EXPECT_EQ(2, x);
    }
    FATAL_EXPECT_EQ(1, x);
  }
  FATAL_EXPECT_EQ(0, x);
}

} // namespace fatal {
