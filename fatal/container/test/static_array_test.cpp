/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/static_array.h>

#include <fatal/type/deprecated/type_list.h>
#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

struct abc { int x; int y; int z; };

struct str {
  FATAL_STR(hello, "hello");
  FATAL_STR(world, "world");
  FATAL_STR(test, "test");
};

template <typename T>
void check_empty() {
  using array = typename static_array<T>::template value<>;
  FATAL_EXPECT_SAME<T, typename decltype(array::get)::value_type>();
  FATAL_EXPECT_EQ(0, array::get.size());
}

FATAL_TEST(static_array, empty) {
  check_empty<int>();
  check_empty<abc>();
}

struct check_array_visitor {
  template <typename Tag, typename U>
  void operator ()(Tag, U const &array) {
    FATAL_ASSERT_LT(Tag::index, array.size());
    FATAL_EXPECT_EQ(Tag::type::value, array[Tag::index]);
  }
};

template <typename Actual, typename... Expected>
void check_array() {
  FATAL_EXPECT_EQ(sizeof...(Expected), Actual::get.size());
  type_list<Expected...>::foreach(check_array_visitor(), Actual::get);
}

template <int... Values>
void check_int() {
  check_array<
    static_array<>::value<std::integral_constant<int, Values>...>,
    std::integral_constant<int, Values>...
  >();
  check_array<
    static_array<int>::value<std::integral_constant<int, Values>...>,
    std::integral_constant<int, Values>...
  >();
}

FATAL_TEST(static_array, int) {
  check_int<0>();
  check_int<1>();
  check_int<2>();
  check_int<100>();
  check_int<0, 1, 2>();
  check_int<1, 2, 3>();
  check_int<2, 3, 4>();
  check_int<25, 26, 27>();
  check_int<178, 849, 9, 11, 0>();
}

struct check_abc_visitor {
  template <typename T, std::size_t Index, typename U>
  void operator ()(indexed_type_tag<T, Index>, U const &array) const {
    FATAL_ASSERT_LT(Index, array.size());
    FATAL_EXPECT_EQ(T::first::first::value, array[Index].x);
    FATAL_EXPECT_EQ(T::first::second::value, array[Index].y);
    FATAL_EXPECT_EQ(T::second::value, array[Index].z);
  }
};

template <typename Expected, template <typename...> class Factory>
void check_abc_array() {
  using array = typename Expected::template apply<Factory>;
  static_assert(Expected::size == array::get.size(), "size mismatch");
  Expected::foreach(check_abc_visitor(), array::get);
}

template <typename T>
struct abc_factory {
  constexpr abc operator ()() const {
    return abc{
      T::first::first::value,
      T::first::second::value,
      T::second::value
    };
  }
};

template <int... Values>
void check_abc() {
  using values = type_list<std::integral_constant<int, Values>...>;
  using x = typename values::template unzip<3, 0>;
  using y = typename values::template unzip<3, 1>;
  using z = typename values::template unzip<3, 2>;
  using list = typename x::template combine<>::template list<y>
    ::template combine<>::template list<z>;

  check_abc_array<list, static_array<>::with<abc_factory>::apply>();
  check_abc_array<list, static_array<abc>::with<abc_factory>::apply>();
}

FATAL_TEST(static_array, struct) {
  check_abc<0, 0, 0>();
  check_abc<0, 1, 2>();
  check_abc<99, 56, 43>();

  check_abc<0, 0, 0, 0, 1, 2>();

  check_abc<0, 0, 0, 0, 1, 2, 99, 56, 43>();
  check_abc<0, 1, 2, 3, 4, 5, 6, 7, 8>();

  check_abc<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>();
  check_abc<99, 15, 62, 3, 8, 12, 0, 46, 85, 5, 1, 7>();
}

template <typename T, typename... Values>
struct check_constant_sequence_list {
  template <typename Factory>
  static void impl() {
    using expected_type = std::array<T, sizeof...(Values)>;

    {
      using actual = typename Factory::template data<Values...>;
      expected_type const expected{{ Values::data()...  }};

      FATAL_EXPECT_SAME<T, typename actual::value_type>();
      FATAL_EXPECT_SAME<expected_type, typename actual::type>();
      FATAL_EXPECT_EQ(sizeof...(Values), actual::get.size());
      FATAL_EXPECT_EQ(expected, actual::get);
    }

    {
      using actual = typename Factory::template z_data<Values...>;
      expected_type const expected{{ Values::z_data()...  }};

      FATAL_EXPECT_SAME<T, typename actual::value_type>();
      FATAL_EXPECT_SAME<expected_type, typename actual::type>();
      FATAL_EXPECT_EQ(sizeof...(Values), actual::get.size());
      FATAL_EXPECT_EQ(expected, actual::get);
    }
  };

  static void check() {
    impl<static_array<>>();
    impl<static_array<T>>();
  }
};

template <typename T>
struct check_constant_sequence_list<T> {
  static void check() {
    {
      using actual = typename static_array<T>::template data<>;

      FATAL_EXPECT_SAME<T, typename actual::value_type>();
      FATAL_EXPECT_SAME<std::array<T, 0>, typename actual::type>();
      FATAL_EXPECT_EQ(0, actual::get.size());
    }

    {
      using actual = typename static_array<T>::template z_data<>;

      FATAL_EXPECT_SAME<T, typename actual::value_type>();
      FATAL_EXPECT_SAME<std::array<T, 0>, typename actual::type>();
      FATAL_EXPECT_EQ(0, actual::get.size());
    }
  }
};

FATAL_TEST(static_array, constant_sequence list) {
  check_constant_sequence_list<char const *>::check();
  check_constant_sequence_list<char const *, str::hello>::check();
  check_constant_sequence_list<char const *, str::hello, str::world>::check();
  check_constant_sequence_list<
    char const *, str::hello, str::world, str::test
  >::check();
}

} // namespace fatal {
