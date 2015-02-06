/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/preprocessor.h>

#include <fatal/test/driver.h>

namespace fatal {

TEST(preprocessor, fatal_empty) {
  int FATAL_EMPTY() x = 1;
  EXPECT_EQ(1, x);

  ++ FATAL_EMPTY() x;
  EXPECT_EQ(2, x);

  EXPECT_EQ(2, x FATAL_EMPTY());
  EXPECT_EQ(3, x + FATAL_EMPTY() 1);
}

TEST(preprocessor, fatal_ignore) {
  int FATAL_IGNORE(~) x = 1;
  EXPECT_EQ(1, x);

  FATAL_IGNORE(++x);
  EXPECT_EQ(1, x);
  ++x;
  EXPECT_EQ(2, x);

  EXPECT_EQ(2, x FATAL_IGNORE(+ 1));
  EXPECT_EQ(3, x + 1);
}

TEST(preprocessor, fatal_identity) {
  int x = FATAL_IDENTITY(1);
  EXPECT_EQ(1, x);

  FATAL_IDENTITY(++x);
  EXPECT_EQ(2, x);

  EXPECT_EQ(3, x FATAL_IDENTITY(+ 1));
}

TEST(preprocessor, fatal_cat) {
  int a = 1;
  EXPECT_EQ(a, FATAL_CAT(a, ));
  EXPECT_EQ(a, FATAL_CAT(, a));

  int b = 2;
  EXPECT_EQ(b, FATAL_CAT(b, ));
  EXPECT_EQ(b, FATAL_CAT(, b));

  int c = 3;
  EXPECT_EQ(c, FATAL_CAT(c, ));
  EXPECT_EQ(c, FATAL_CAT(, c));

  int ab = 12;
  EXPECT_EQ(ab, FATAL_CAT(a, b));

  int ac = 13;
  EXPECT_EQ(ac, FATAL_CAT(a, c));

  int bc = 23;
  EXPECT_EQ(bc, FATAL_CAT(b, c));

  int abc = 123;
  EXPECT_EQ(abc, FATAL_CAT(a, FATAL_CAT(b, c)));
  EXPECT_EQ(abc, FATAL_CAT(FATAL_CAT(a, b), c));
}

TEST(preprocessor, fatal_not) {
  EXPECT_EQ(1, FATAL_NOT(0));
  EXPECT_EQ(0, FATAL_NOT(1));
  EXPECT_EQ(0, FATAL_NOT(56));
  EXPECT_EQ(0, FATAL_NOT(arg));
}

TEST(preprocessor, fatal_bool) {
  EXPECT_EQ(0, FATAL_BOOL(0));
  EXPECT_EQ(1, FATAL_BOOL(1));
  EXPECT_EQ(1, FATAL_BOOL(56));
  EXPECT_EQ(1, FATAL_BOOL(arg));
}

TEST(preprocessor, fatal_arg_n) {
  EXPECT_EQ(1, FATAL_ARG_1(1, 2, 3, 4, 5));
  EXPECT_EQ(2, FATAL_ARG_2(1, 2, 3, 4, 5));
  EXPECT_EQ(3, FATAL_ARG_3(1, 2, 3, 4, 5));
  EXPECT_EQ(4, FATAL_ARG_4(1, 2, 3, 4, 5));
  EXPECT_EQ(5, FATAL_ARG_5(1, 2, 3, 4, 5));
}

TEST(preprocessor, fatal_has_args) {
  EXPECT_EQ(0, FATAL_HAS_ARGS());
  EXPECT_EQ(0, FATAL_HAS_ARGS( ));
  EXPECT_EQ(0, FATAL_HAS_ARGS(,));
  EXPECT_EQ(1, FATAL_HAS_ARGS(0));
  EXPECT_EQ(1, FATAL_HAS_ARGS(1));
  EXPECT_EQ(1, FATAL_HAS_ARGS(56));
  EXPECT_EQ(1, FATAL_HAS_ARGS(arg));
}

TEST(preprocessor, fatal_conditional) {
  EXPECT_EQ(20, FATAL_CONDITIONAL(0)(10)(20));
  EXPECT_EQ(10, FATAL_CONDITIONAL(1)(10)(20));
  EXPECT_EQ(10, FATAL_CONDITIONAL(56)(10)(20));
  EXPECT_EQ(10, FATAL_CONDITIONAL(arg)(10)(20));
}

#define TEST_IMPL(...) |__VA_ARGS__|

TEST(preprocessor, fatal_defer_1) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_1(TEST_IMPL)(56)));
  EXPECT_EQ("TEST_IMPL (56)", FATAL_TO_STR(FATAL_DEFER_1(TEST_IMPL)(56)));
  EXPECT_EQ("|56|", FATAL_TO_STR(FATAL_IDENTITY(FATAL_DEFER_1(TEST_IMPL)(56))));
}

TEST(preprocessor, fatal_defer_2) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_2(TEST_IMPL)(56)));
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56))));
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56)))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_2(TEST_IMPL)(56))))
  );
}

TEST(preprocessor, fatal_defer_3) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_3(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_3(TEST_IMPL)(56))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_3(TEST_IMPL)(56))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_3(TEST_IMPL)(56)
    ))))
  );
}

TEST(preprocessor, fatal_defer_4) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_4(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_4(TEST_IMPL)(56)
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_DEFER_4(TEST_IMPL)(56)
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_4(TEST_IMPL)(56))
    ))))
  );
}

TEST(preprocessor, fatal_defer_5) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_5(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56))
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56))
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_5(TEST_IMPL)(56)))
    ))))
  );
}

TEST(preprocessor, fatal_defer_6) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_6(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_6(TEST_IMPL)(56)))
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_6(TEST_IMPL)(56)))
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_6(TEST_IMPL)(56)
      )))
    ))))
  );
}

TEST(preprocessor, fatal_defer_7) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_7(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      )))
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      )))
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_7(TEST_IMPL)(56)
      ))))
    ))))
  );
}

TEST(preprocessor, fatal_defer_8) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_8(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_8(TEST_IMPL)(56)
      ))))
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_DEFER_8(TEST_IMPL)(56)
      ))))
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_8(TEST_IMPL)(56))
      ))))
    ))))
  );
}

TEST(preprocessor, fatal_defer_9) {
  EXPECT_NE("|56|", FATAL_TO_STR(FATAL_DEFER_9(TEST_IMPL)(56)));
  EXPECT_NE(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56))
      ))))
    ))))
  );
  EXPECT_EQ(
    "TEST_IMPL (56)",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56))
      ))))
    ))))
  );
  EXPECT_EQ(
    "|56|",
    FATAL_TO_STR(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
      FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(FATAL_IDENTITY(
        FATAL_IDENTITY(FATAL_IDENTITY(FATAL_DEFER_9(TEST_IMPL)(56)))
      ))))
    ))))
  );
}

# undef TEST_IMPL

TEST(preprocessor, fatal_eval) {
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

  EXPECT_EQ(56, FATAL_EVAL(TEST_IMPL(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_1(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_2(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_3(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_4(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_5(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_6(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_7(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_8(TEST_IMPL)(56)));
  EXPECT_EQ(56, FATAL_EVAL(FATAL_DEFER_9(TEST_IMPL)(56)));

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

TEST(preprocessor, fatal_simple_map) {
# define TEST_IMPL(x) + x
  EXPECT_EQ(15, 0 FATAL_EVAL(FATAL_SIMPLE_MAP(TEST_IMPL, 1, 2, 3, 4, 5)));
# undef TEST_IMPL

  EXPECT_EQ(
    15,
    FATAL_EVAL(FATAL_SIMPLE_MAP(FATAL_IDENTITY, 1, +, 2, +, 3, +, 4, +, 5))
  );

  EXPECT_EQ(
    "1 2 3 4 5",
    FATAL_TO_STR(FATAL_EVAL(FATAL_SIMPLE_MAP(FATAL_IDENTITY, 1, 2, 3, 4, 5)))
  );
}

TEST(preprocessor, fatal_map) {
# define TEST_IMPL(arg, is_first, index, x) + x
  EXPECT_EQ(15, 0 FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5)));
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) x
  EXPECT_EQ(
    15,
    FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, +, 2, +, 3, +, 4, +, 5))
  );
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) x
  EXPECT_EQ(
    "1 2 3 4 5",
    FATAL_TO_STR(FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5)))
  );
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) \
  FATAL_CONDITIONAL(is_first)(x)(+ x)
  EXPECT_EQ(15, FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5)));
# undef TEST_IMPL

# define TEST_IMPL(arg, is_first, index, x) / arg is_first x |
  EXPECT_EQ(
    "/ ~ 1 1 | / ~ 0 2 | / ~ 0 3 | / ~ 0 4 | / ~ 0 5 |",
    FATAL_TO_STR(FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5)))
  );
# undef TEST_IMPL

  std::array<int, 5> a;
# define TEST_IMPL(arg, is_first, index, x) \
    do { \
      ASSERT_LT(index, a.size()); \
      a[index] = x; \
    } while (false);

  FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5));
# undef TEST_IMPL

  std::vector<int> v;
# define TEST_IMPL(arg, is_first, index, x) \
    do { \
      v.push_back(x); \
    } while (false);

  FATAL_EVAL(FATAL_MAP(TEST_IMPL, ~, 1, 2, 3, 4, 5));
# undef TEST_IMPL

  ASSERT_EQ(a.size(), v.size());

  for (auto i = a.size(); i--; ) {
    EXPECT_EQ(i + 1, a[i]);
    EXPECT_EQ(i + 1, v[i]);
  }
}

TEST(preprocessor, fatal_to_str) {
  EXPECT_EQ(FATAL_TO_STR(x), "x");
  EXPECT_EQ(FATAL_TO_STR(xyz), "xyz");
  EXPECT_EQ(FATAL_TO_STR(x y z), "x y z");
}

struct uid_tester {
  uid_tester(int &x): x_(x) { ++x_; }
  ~uid_tester() { --x_; }

private:
  int &x_;
};

TEST(preprocessor, fatal_uid) {
  int x = 0;
  {
    uid_tester FATAL_UID(prefix)(x);
    EXPECT_EQ(1, x);
    {
      uid_tester FATAL_UID(prefix)(x);
      EXPECT_EQ(2, x);
      {
        uid_tester FATAL_UID(prefix)(x);
        EXPECT_EQ(3, x);
        {
          uid_tester FATAL_UID(prefix)(x);
          EXPECT_EQ(4, x);
        }
        EXPECT_EQ(3, x);
      }
      EXPECT_EQ(2, x);
    }
    EXPECT_EQ(1, x);
  }
  EXPECT_EQ(0, x);
}

} // namespace fatal {
