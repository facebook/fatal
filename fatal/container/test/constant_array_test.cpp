/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/constant_array.h>

#include <fatal/test/driver.h>

#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {

template <typename T>
struct seq {
  template <T... Values>
  using array = constant_array<T, Values...>;

  template <T... Values>
  using list = type_list<std::integral_constant<T, Values>...>;
};

#define CHECK_CONSTANT_ARRAY(Type, ...) \
  do { \
    using type = Type; \
    using values = seq<type>::list<__VA_ARGS__>; \
    \
    FATAL_EXPECT_SAME<type, typename actual::value_type>(); \
    FATAL_EXPECT_EQ(values::size, actual::size); \
    FATAL_EXPECT_EQ(values::size == 0, actual::empty); \
    \
    using array = std::array<type, values::size>; \
    \
    FATAL_EXPECT_SAME<array, typename actual::type>(); \
    \
    array const expected{{__VA_ARGS__}}; \
    \
    FATAL_EXPECT_EQ(expected, actual::get); \
    FATAL_EXPECT_EQ(actual::get.data(), actual::data()); \
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

FATAL_TEST(constant_array, constant_array) {
# define TEST_IMPL(Type, ...) \
  do { \
    using actual = seq<Type>::array<__VA_ARGS__>; \
    CHECK_CONSTANT_ARRAY(Type, __VA_ARGS__); \
  } while (false)

  TEST_CASES(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(constant_array_from, args) {
# define TEST_IMPL(Type, ...) \
  do { \
    using list = seq<Type>::list<__VA_ARGS__>; \
    using actual = list::apply<constant_array_from<Type>::args>; \
    CHECK_CONSTANT_ARRAY(Type, __VA_ARGS__); \
  } while (false)

  TEST_CASES(TEST_IMPL);

# undef TEST_IMPL
}

FATAL_TEST(constant_array_from, list) {
# define TEST_IMPL(Type, ...) \
  do { \
    using list = seq<Type>::list<__VA_ARGS__>; \
    using actual = constant_array_from<Type>::list<list>; \
    CHECK_CONSTANT_ARRAY(Type, __VA_ARGS__); \
  } while (false)

  TEST_CASES(TEST_IMPL);

# undef TEST_IMPL
}

} // namespace fatal {
