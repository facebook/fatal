/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/pointer_selector.h>

#include <fatal/test/driver.h>

#include <memory>

namespace fatal {

struct test_deleter {
  template <typename T>
  void operator ()(T *p) const { delete p; }
};

FATAL_TEST(pointer_selector, element_type) {
  FATAL_EXPECT_SAME<
    int,
    pointer_selector<pointer_class::raw, int>::element_type
  >();

  FATAL_EXPECT_SAME<
    int const,
    pointer_selector<pointer_class::raw, int const>::element_type
  >();

  FATAL_EXPECT_SAME<
    int,
    pointer_selector<pointer_class::unique, int>::element_type
  >();

  FATAL_EXPECT_SAME<
    int const,
    pointer_selector<pointer_class::unique, int const>::element_type
  >();

  FATAL_EXPECT_SAME<
    int,
    pointer_selector<pointer_class::unique, int, test_deleter>::element_type
  >();

  FATAL_EXPECT_SAME<
    int const,
    pointer_selector<
      pointer_class::unique, int const, test_deleter
    >::element_type
  >();

  FATAL_EXPECT_SAME<
    int,
    pointer_selector<pointer_class::shared, int>::element_type
  >();

  FATAL_EXPECT_SAME<
    int const,
    pointer_selector<pointer_class::shared, int const>::element_type
  >();
}

FATAL_TEST(pointer_selector, pointer) {
  FATAL_EXPECT_SAME<
    int *,
    pointer_selector<pointer_class::raw, int>::pointer
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector<pointer_class::raw, int const>::pointer
  >();

  FATAL_EXPECT_SAME<
    int *,
    pointer_selector<pointer_class::unique, int>::pointer
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector<pointer_class::unique, int const>::pointer
  >();

  FATAL_EXPECT_SAME<
    int *,
    pointer_selector<pointer_class::unique, int, test_deleter>::pointer
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector<pointer_class::unique, int const, test_deleter>::pointer
  >();

  FATAL_EXPECT_SAME<
    int *,
    pointer_selector<pointer_class::shared, int>::pointer
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector<pointer_class::shared, int const>::pointer
  >();
}

FATAL_TEST(pointer_selector, type) {
  FATAL_EXPECT_SAME<
    int *,
    pointer_selector<pointer_class::raw, int>::type
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector<pointer_class::raw, int const>::type
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int>,
    pointer_selector<pointer_class::unique, int>::type
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int const>,
    pointer_selector<pointer_class::unique, int const>::type
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int, test_deleter>,
    pointer_selector<pointer_class::unique, int, test_deleter>::type
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int const, test_deleter>,
    pointer_selector<pointer_class::unique, int const, test_deleter>::type
  >();

  FATAL_EXPECT_SAME<
    std::shared_ptr<int>,
    pointer_selector<pointer_class::shared, int>::type
  >();

  FATAL_EXPECT_SAME<
    std::shared_ptr<int const>,
    pointer_selector<pointer_class::shared, int const>::type
  >();
}

FATAL_TEST(pointer_selector, managed) {
  FATAL_EXPECT_FALSE((
    pointer_selector<pointer_class::raw, int>::managed::value
  ));
  FATAL_EXPECT_FALSE((
    pointer_selector<pointer_class::raw, int const>::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<pointer_class::unique, int>::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<pointer_class::unique, int const>::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<pointer_class::unique, int, test_deleter>::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<
      pointer_class::unique, int const, test_deleter
    >::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<pointer_class::shared, int>::managed::value
  ));
  FATAL_EXPECT_TRUE((
    pointer_selector<pointer_class::shared, int const>::managed::value
  ));
}

struct pointer_selector_tester {
  pointer_selector_tester(int &v_): v(v_) { v = 99; }
  ~pointer_selector_tester() { v = 55; }

private:
  int &v;
};

FATAL_TEST(pointer_selector, make_get_destroy) {
# define TEST_IMPL(PtrClass, ...) \
  do { \
    value = 11; \
    using selector = pointer_selector<pointer_class::PtrClass, __VA_ARGS__>; \
    { \
      auto p = selector::make(value); \
      FATAL_ASSERT_NE(nullptr, selector::get(p)); \
      FATAL_EXPECT_EQ(99, value); \
      if (!selector::managed::value) { \
        selector::destroy(p); \
      } \
    } \
    FATAL_EXPECT_EQ(55, value); \
    \
    value = 22; \
    auto p = selector::make(value); \
    FATAL_ASSERT_NE(nullptr, selector::get(p)); \
    FATAL_EXPECT_EQ(99, value); \
    selector::destroy(p); \
    if (selector::managed::value) { \
      FATAL_EXPECT_EQ(nullptr, selector::get(p)); \
    } \
    FATAL_EXPECT_EQ(55, value); \
  } while (false)

  int value = 0;

  TEST_IMPL(raw, pointer_selector_tester);
  TEST_IMPL(unique, pointer_selector_tester);
  TEST_IMPL(unique, pointer_selector_tester, test_deleter);
  TEST_IMPL(shared, pointer_selector_tester);

# undef TEST_IMPL
}

FATAL_TEST(pointer_selector_t, sanity_check) {
  FATAL_EXPECT_SAME<
    int *,
    pointer_selector_t<pointer_class::raw, int>
  >();

  FATAL_EXPECT_SAME<
    int const *,
    pointer_selector_t<pointer_class::raw, int const>
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int>,
    pointer_selector_t<pointer_class::unique, int>
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int const>,
    pointer_selector_t<pointer_class::unique, int const>
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int, test_deleter>,
    pointer_selector_t<pointer_class::unique, int, test_deleter>
  >();

  FATAL_EXPECT_SAME<
    std::unique_ptr<int const, test_deleter>,
    pointer_selector_t<pointer_class::unique, int const, test_deleter>
  >();

  FATAL_EXPECT_SAME<
    std::shared_ptr<int>,
    pointer_selector_t<pointer_class::shared, int>
  >();

  FATAL_EXPECT_SAME<
    std::shared_ptr<int const>,
    pointer_selector_t<pointer_class::shared, int const>
  >();
}

FATAL_TEST(make_ptr, sanity_check) {
# define TEST_IMPL(PtrClass, ...) \
  do { \
    value = 12; \
    using selector = pointer_selector<pointer_class::PtrClass, __VA_ARGS__>; \
    { \
      auto p = make_ptr<pointer_class::PtrClass, __VA_ARGS__>(value); \
      FATAL_ASSERT_NE(nullptr, selector::get(p)); \
      FATAL_EXPECT_EQ(99, value); \
      if (!selector::managed::value) { \
        selector::destroy(p); \
      } \
    } \
    FATAL_EXPECT_EQ(55, value); \
    \
    value = 22; \
    auto p = make_ptr<pointer_class::PtrClass, __VA_ARGS__>(value); \
    FATAL_ASSERT_NE(nullptr, selector::get(p)); \
    FATAL_EXPECT_EQ(99, value); \
    selector::destroy(p); \
    if (selector::managed::value) { \
      FATAL_EXPECT_EQ(nullptr, selector::get(p)); \
    } \
    FATAL_EXPECT_EQ(55, value); \
  } while (false)

  int value = 0;

  TEST_IMPL(raw, pointer_selector_tester);
  TEST_IMPL(unique, pointer_selector_tester);
  TEST_IMPL(unique, pointer_selector_tester, test_deleter);
  TEST_IMPL(shared, pointer_selector_tester);

# undef TEST_IMPL
}

} // namespace fatal {
