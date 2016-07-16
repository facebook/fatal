/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/reflect_function.h>

#include <fatal/test/driver.h>

namespace fatal {

void foo();
int bar(long);
double ampersand(bool &, float &&);

FATAL_TEST(reflect_function, free_functions) {
# define TEST_IMPL(Name, Result, ...) \
  do { \
    using actual = reflect_function<decltype(Name)>; \
    FATAL_EXPECT_SAME<Result, actual::result>(); \
    FATAL_EXPECT_SAME<type_list<__VA_ARGS__>, actual::args>(); \
    FATAL_EXPECT_SAME<decltype(Name), actual::signature>(); \
    FATAL_EXPECT_SAME<decltype(&Name), actual::pointer>(); \
    FATAL_EXPECT_SAME< \
      type_list<__VA_ARGS__>::push_front<Result>, \
      actual::types \
    >(); \
  } while (false)

  TEST_IMPL(foo, void);
  TEST_IMPL(bar, int, long);
  TEST_IMPL(ampersand, double, bool &, float &&);

# undef TEST_IMPL
}

struct static_fn {
  static void foo();
  static int bar(long);
  static double ampersand(bool &, float &&);
};

FATAL_TEST(reflect_function, static_functions) {
# define TEST_IMPL(Name, Result, ...) \
  do { \
    using actual = reflect_function<decltype(static_fn::Name)>; \
    FATAL_EXPECT_SAME<Result, actual::result>(); \
    FATAL_EXPECT_SAME<type_list<__VA_ARGS__>, actual::args>(); \
    FATAL_EXPECT_SAME<decltype(static_fn::Name), actual::signature>(); \
    FATAL_EXPECT_SAME<decltype(&static_fn::Name), actual::pointer>(); \
    FATAL_EXPECT_SAME< \
      type_list<__VA_ARGS__>::push_front<Result>, \
      actual::types \
    >(); \
  } while (false)

  TEST_IMPL(foo, void);
  TEST_IMPL(bar, int, long);
  TEST_IMPL(ampersand, double, bool &, float &&);

# undef TEST_IMPL
}

} // namespace fatal {
