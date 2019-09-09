/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/runtime_array.h>

#include <fatal/test/ref_counter.h>

#include <fatal/test/driver.h>

#include <string>

namespace fatal {

FATAL_TEST(runtime_array, ctor) {
  using refc = ref_counter<>;
  refc::guard guard;

  {
    runtime_array<refc, 10> v1(10);
    FATAL_ASSERT_EQ(10, refc::alive());
    FATAL_ASSERT_EQ(10, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  {
    runtime_array<refc, 5> v2(5);
    FATAL_ASSERT_EQ(5, refc::alive());
    FATAL_ASSERT_EQ(5, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  {
    runtime_array<refc, 0> v1(10);
    FATAL_ASSERT_EQ(10, refc::alive());
    FATAL_ASSERT_EQ(10, refc::valid());
  }

  FATAL_ASSERT_EQ(0, refc::alive());
  FATAL_ASSERT_EQ(0, refc::valid());

  {
    runtime_array<refc, 0> v2(5);
    FATAL_ASSERT_EQ(5, refc::alive());
    FATAL_ASSERT_EQ(5, refc::valid());
  }
}

#define TEST_IMPL(SmallBuffer, Size) \
  do { \
    { \
      using type = runtime_array<std::size_t, SmallBuffer>; \
      \
      type v1(Size); \
      for (auto i = v1.size(); i--; ) { \
        v1[i] = i * 100; \
      } \
      \
      type v2(v1); \
      for (auto i = v1.size(); i--; ) { \
        auto expected = i * 100; \
        FATAL_EXPECT_EQ(expected, v1[i]); \
        FATAL_EXPECT_EQ(expected, v2[i]); \
      } \
    } \
    \
    { \
      using type = runtime_array<refc, SmallBuffer>; \
      \
      type v1(Size); \
      FATAL_ASSERT_EQ(Size, refc::alive()); \
      FATAL_ASSERT_EQ(Size, refc::valid()); \
      \
      type v2(v1); \
      FATAL_ASSERT_EQ(2 * Size, refc::alive()); \
      FATAL_ASSERT_EQ(2 * Size, refc::valid()); \
    } \
    \
    FATAL_ASSERT_EQ(0, refc::alive()); \
    FATAL_ASSERT_EQ(0, refc::valid()); \
  } while (false)

FATAL_TEST(runtime_array, copy_ctor) {
  using refc = ref_counter<>;
  refc::guard guard;

  TEST_IMPL(10, 10);
  TEST_IMPL(5, 5);
  TEST_IMPL(0, 10);
  TEST_IMPL(0, 5);
}

#undef TEST_IMPL

#define TEST_IMPL(SmallBuffer, Size) \
  do { \
    { \
      using type = runtime_array<std::size_t, SmallBuffer>; \
      \
      type v1(Size); \
      for (auto i = v1.size(); i--; ) { \
        v1[i] = i * 100; \
      } \
      \
      type v2(std::move(v1)); \
      for (auto i = v1.size(); i--; ) { \
        auto expected = i * 100; \
        FATAL_EXPECT_EQ(expected, v2[i]); \
      } \
    } \
    \
    { \
      using type = runtime_array<refc, SmallBuffer>; \
      \
      type v1(Size); \
      FATAL_ASSERT_EQ(Size, refc::alive()); \
      FATAL_ASSERT_EQ(Size, refc::valid()); \
      \
      type v2(std::move(v1)); \
      FATAL_ASSERT_EQ(Size <= SmallBuffer ? 2 * Size : Size, refc::alive()); \
      FATAL_ASSERT_EQ(Size, refc::valid()); \
    } \
    \
    FATAL_ASSERT_EQ(0, refc::alive()); \
    FATAL_ASSERT_EQ(0, refc::valid()); \
  } while (false)

FATAL_TEST(runtime_array, move_ctor) {
  using refc = ref_counter<>;
  refc::guard guard;

  TEST_IMPL(10, 10);
  TEST_IMPL(5, 5);
  TEST_IMPL(0, 10);
  TEST_IMPL(0, 5);
}

#undef TEST_IMPL

#define TEST_IMPL(Const, Fn, CheckOverflow, UseSmallBuffer) \
  do { \
    using type = runtime_array<int, UseSmallBuffer ? 10 : 0>; \
    \
    { \
      type Const v(10); \
      for (std::size_t i = 0; i < v.size(); ++i) { \
        auto const i_ = signed_cast(i); \
        FATAL_EXPECT_EQ(std::next(v.data(), i_), std::addressof(v.Fn(i))); \
        FATAL_EXPECT_EQ(*std::next(v.data(), i_), v.Fn(i)); \
      } \
      \
      if (CheckOverflow) { \
        FATAL_EXPECT_THROW(std::out_of_range) { v.Fn(v.size()); }; \
      } \
    } \
    { \
      type Const v(10); \
      for (std::size_t i = 0; i < v.size(); ++i) { \
        auto const i_ = signed_cast(i); \
        FATAL_EXPECT_EQ(std::next(v.data(), i_), std::addressof(v.Fn(i))); \
        FATAL_EXPECT_EQ(*std::next(v.data(), i_), v.Fn(i)); \
      } \
      \
      if (CheckOverflow) { \
        FATAL_EXPECT_THROW(std::out_of_range) { v.Fn(v.size()); }; \
      } \
    } \
  } while (false)

FATAL_TEST(runtime_array, at) {
  TEST_IMPL(, at, true, false);
  TEST_IMPL(const, at, true, false);
  TEST_IMPL(, at, true, true);
  TEST_IMPL(const, at, true, true);
}

FATAL_TEST(runtime_array, operator []) {
  TEST_IMPL(, operator [], false, false);
  TEST_IMPL(const, operator [], false, false);
  TEST_IMPL(, operator [], false, true);
  TEST_IMPL(const, operator [], false, true);
}

#undef TEST_IMPL

template <typename T, std::size_t Size>
void set(runtime_array<T, Size> &v, std::size_t i, T value) {
  v[i] = value;
}

template <typename T, std::size_t Size>
void set(runtime_array<T, Size> const &, std::size_t, T) {}

#define TEST_IMPL(Const, Begin, End, UseSmallBuffer) \
  do { \
    using type = runtime_array<int, UseSmallBuffer ? 10 : 0>; \
    \
    { \
      type Const v(0); \
      FATAL_EXPECT_EQ(v.Begin(), v.End()); \
    } \
    { \
      type Const v(10); \
      auto i = v.Begin(); \
      for (std::size_t n = v.size(); n--; ) { \
        set(v, n, static_cast<int>(n * 100)); \
      } \
      for (std::size_t index = 0; index < v.size(); ++index, ++i) { \
        auto const index_ = signed_cast(index); \
        FATAL_ASSERT_NE(v.Begin(), v.End()); \
        FATAL_EXPECT_EQ(std::next(v.data(), index_), i); \
        FATAL_EXPECT_EQ(*std::next(v.data(), index_), *i); \
        FATAL_EXPECT_EQ(v[index], *i); \
      } \
      FATAL_EXPECT_EQ(i, v.End()); \
    } \
  } while (false)

FATAL_TEST(runtime_array, cbegin/cend) {
  TEST_IMPL(, cbegin, cend, false);
  TEST_IMPL(const, cbegin, cend, false);
  TEST_IMPL(, cbegin, cend, true);
  TEST_IMPL(const, cbegin, cend, true);
}

FATAL_TEST(runtime_array, begin/end) {
  TEST_IMPL(, begin, end, false);
  TEST_IMPL(const, begin, end, false);
  TEST_IMPL(, begin, end, true);
  TEST_IMPL(const, begin, end, true);
}

#undef TEST_IMPL

FATAL_TEST(runtime_array, size) {
  runtime_array<int, 10> v1(10);
  FATAL_EXPECT_EQ(10, v1.size());

  runtime_array<int, 0> v2(10);
  FATAL_EXPECT_EQ(10, v2.size());

  runtime_array<int, 5> v3(5);
  FATAL_EXPECT_EQ(5, v3.size());

  runtime_array<int, 0> v4(5);
  FATAL_EXPECT_EQ(5, v4.size());

  runtime_array<int, 10> v5(0);
  FATAL_EXPECT_EQ(0, v5.size());

  runtime_array<int, 0> v6(0);
  FATAL_EXPECT_EQ(0, v6.size());

  runtime_array<int, 5> v7(0);
  FATAL_EXPECT_EQ(0, v7.size());

  runtime_array<int, 0> v8(0);
  FATAL_EXPECT_EQ(0, v8.size());
}

FATAL_TEST(runtime_array, empty) {
  runtime_array<int, 10> v1(10);
  FATAL_EXPECT_FALSE(v1.empty());

  runtime_array<int, 0> v2(10);
  FATAL_EXPECT_FALSE(v2.empty());

  runtime_array<int, 5> v3(5);
  FATAL_EXPECT_FALSE(v3.empty());

  runtime_array<int, 0> v4(5);
  FATAL_EXPECT_FALSE(v4.empty());

  runtime_array<int, 10> v5(0);
  FATAL_EXPECT_TRUE(v5.empty());

  runtime_array<int, 0> v6(0);
  FATAL_EXPECT_TRUE(v6.empty());

  runtime_array<int, 5> v7(0);
  FATAL_EXPECT_TRUE(v7.empty());

  runtime_array<int, 0> v8(0);
  FATAL_EXPECT_TRUE(v8.empty());
}

} // namespace fatal {
