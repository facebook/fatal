/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/array.h>

#include <fatal/type/foreach.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>
#include <fatal/type/slice.h>
#include <fatal/type/type.h>
#include <fatal/type/zip.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

struct abc { int x; int y; int z; };

struct str {
  FATAL_S(hello, "hello");
  FATAL_S(world, "world");
  FATAL_S(test, "test");
};

struct check_abc_visitor {
  template <typename T, std::size_t Index, typename U>
  void operator ()(indexed<T, Index>, U const &array) const {
    FATAL_EXPECT_EQ(first<T>::value, array[Index].x);
    FATAL_EXPECT_EQ(second<T>::value, array[Index].y);
    FATAL_EXPECT_EQ(third<T>::value, array[Index].z);
  }
};

struct abc_factory {
  template <typename T>
  static constexpr abc get() {
    return abc{
      first<T>::value,
      second<T>::value,
      third<T>::value
    };
  }
};

template <typename Expected, typename... T>
void check_abc_array() {
  using array = as_array_from<Expected, abc_factory, T...>;
  static_assert(size<Expected>::value == array::size::value, "size mismatch");
  foreach<Expected>(check_abc_visitor(), array::data);
}

template <typename x, typename y, typename z>
void check_abc() {
  using expected = fatal::zip<list, list, x, y, z>;
  check_abc_array<expected>();
  check_abc_array<expected, abc>();
}

FATAL_TEST(as_array_from, struct) {
  check_abc<int_list<0>, int_list<0>, int_list<0>>();
  check_abc<int_list<0>, int_list<1>, int_list<2>>();
  check_abc<int_list<99>, int_list<56>, int_list<43>>();

  check_abc<int_list<0, 0>, int_list<0, 1>, int_list<0, 2>>();

  check_abc<int_list<0, 0, 99>, int_list<0, 1, 56>, int_list<0, 2, 43>>();
  check_abc<int_list<0, 3, 6>, int_list<1, 4, 7>, int_list<2, 5, 8>>();

  check_abc<
    int_list<0, 3, 6, 9>,
    int_list<1, 4, 7, 10>,
    int_list<2, 8, 5, 11>
  >();
  check_abc<
    int_list<99, 3, 0, 5>,
    int_list<15, 8, 46, 1>,
    int_list<62, 12, 85, 7>
  >();
}

template <typename T, typename... Values>
struct check_sequence_list {
  template <typename... U>
  static void impl() {
    using actual = z_array<list<Values...>, U...>;
    std::array<T, sizeof...(Values)> const expected{{ z_data<Values>()...  }};

    FATAL_EXPECT_SAME<T, value_type_of<actual>>();
    FATAL_EXPECT_EQ(sizeof...(Values), actual::size::value);
    FATAL_EXPECT_TRUE(
      std::equal(expected.begin(), expected.end(), actual::data)
    );
  }

  static void check() {
    impl<>();
    impl<T>();
  }
};

template <typename T>
struct check_sequence_list<T> {
  static void check() {
    using actual = z_array<list<>, T>;

    FATAL_EXPECT_SAME<T, value_type_of<actual>>();
    FATAL_EXPECT_EQ(0, actual::size::value);
  }
};

FATAL_TEST(as_array_from, sequence_list) {
  check_sequence_list<char const *>::check();
  check_sequence_list<char const *, str::hello>::check();
  check_sequence_list<char const *, str::hello, str::world>::check();
  check_sequence_list<
    char const *, str::hello, str::world, str::test
  >::check();
}

} // namespace fatal {
