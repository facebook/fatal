/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/array.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>
#include <fatal/type/size.h>

#include <fatal/test/driver.h>

#include <array>
#include <type_traits>

namespace fatal {

template <typename T>
struct input {
  template <T... Values>
  using sq = sequence<T, Values...>;

  template <T... Values>
  using ls = value_list<T, Values...>;
};

#define CHECK_ARRAY(Type, ...) \
  do { \
    using type = Type; \
    using values = input<type>::ls<__VA_ARGS__>; \
    using array = std::array<type, size<values>::value>; \
    \
    array const expected{{__VA_ARGS__}}; \
    \
    FATAL_ASSERT_EQ(expected.size(), actual::size::value); \
    FATAL_EXPECT_TRUE( \
      std::equal(expected.begin(), \
      expected.end(), \
      actual::data \
    )); \
  } while (false)

#define TEST_CASES(Fn) \
  do { \
    Fn(int); \
    Fn(int, 99); \
    Fn(int, 0, 1, 2); \
    Fn(int, 10, 20, 30, 40, 50, 60, 70, 80, 90); \
    \
    Fn(char); \
    Fn(char, 'x'); \
    Fn(char, '0', '1', '2'); \
    Fn(char, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'); \
  } while (false)

FATAL_TEST(as_array, list) {
# define TEST_IMPL(Type, ...) \
  do { \
    using list = input<Type>::ls<__VA_ARGS__>; \
    using actual = as_array<list, Type>; \
    CHECK_ARRAY(Type, __VA_ARGS__); \
  } while (false)

  TEST_CASES(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(as_array, sequence) {
# define TEST_IMPL(Type, ...) \
  do { \
    using sq = input<Type>::sq<__VA_ARGS__>; \
    using actual = as_array<sq>; \
    CHECK_ARRAY(Type, __VA_ARGS__); \
  } while (false)

  TEST_CASES(TEST_IMPL);

# undef TEST_IMPL
}

} // namespace fatal {
