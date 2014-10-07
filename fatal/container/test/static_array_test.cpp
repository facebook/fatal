/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/static_array.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

struct xyz { int x; int y; int z; };

template <typename... TValue>
struct check_static_array {
  template <
    typename TFactory, template <typename...> class TVisitor, typename TList
  >
  static void test() {
    using array = typename static_array_from<TFactory, TValue...>
      ::template list<TList>;

    static_assert(TList::size == array::get.size(), "size mismatch");
    TList::foreach(TVisitor<array>());
  }

private:
  struct int_factory {
    template <typename T>
    static constexpr int create() {
      return static_cast<int>(T::type::value);
    }
  };

  template <typename TArray>
  struct check_int_array_visitor {
    template <typename T, std::size_t Index>
    void operator ()(indexed_type_tag<T, Index>) const {
      ASSERT_LT(Index, TArray::get.size());
      EXPECT_EQ(T::value, TArray::get[Index]);
    }
  };

public:
  template <int... Values>
  static void test_int() {
    test<
      int_factory,
      check_int_array_visitor,
      type_list<std::integral_constant<int, Values>...>
    >();
  }

private:
  struct xyz_factory {
    template <typename T>
    static constexpr xyz create() {
      return xyz{
        T::type::first::first::value,
        T::type::first::second::value,
        T::type::second::value
      };
    }
  };

  template <typename TArray>
  struct check_xyz_array_visitor {
    template <typename T, std::size_t Index>
    void operator ()(indexed_type_tag<T, Index>) const {
      ASSERT_LT(Index, TArray::get.size());
      EXPECT_EQ(T::first::first::value, TArray::get[Index].x);
      EXPECT_EQ(T::first::second::value, TArray::get[Index].y);
      EXPECT_EQ(T::second::value, TArray::get[Index].z);
    }
  };

public:
  template <int... Values>
  static void test_xyz() {
    using values = type_list<std::integral_constant<int, Values>...>;
    using x = typename values::template unzip<3, 0>;
    using y = typename values::template unzip<3, 1>;
    using z = typename values::template unzip<3, 2>;

    test<
      xyz_factory,
      check_xyz_array_visitor,
      typename x::template combine<>::template list<y>
        ::template combine<>::template list<z>
    >();
  }
};

TEST(static_array, int) {
  check_static_array<int>::test_int<>();

  check_static_array<int>::test_int<0>();
  check_static_array<int>::test_int<1>();
  check_static_array<int>::test_int<2>();
  check_static_array<int>::test_int<100>();
  check_static_array<int>::test_int<0, 1, 2>();
  check_static_array<int>::test_int<1, 2, 3>();
  check_static_array<int>::test_int<2, 3, 4>();
  check_static_array<int>::test_int<25, 26, 27>();
  check_static_array<int>::test_int<178, 849, 9, 11, 0>();

  check_static_array<>::test_int<0>();
  check_static_array<>::test_int<1>();
  check_static_array<>::test_int<2>();
  check_static_array<>::test_int<100>();
  check_static_array<>::test_int<0, 1, 2>();
  check_static_array<>::test_int<1, 2, 3>();
  check_static_array<>::test_int<2, 3, 4>();
  check_static_array<>::test_int<25, 26, 27>();
  check_static_array<>::test_int<178, 849, 9, 11, 0>();
}

TEST(static_array, xyz) {
  check_static_array<xyz>::test_xyz<>();

  check_static_array<xyz>::test_xyz<0, 0, 0>();
  check_static_array<xyz>::test_xyz<0, 1, 2>();
  check_static_array<xyz>::test_xyz<99, 56, 43>();

  check_static_array<xyz>::test_xyz<0, 0, 0, 0, 1, 2>();

  check_static_array<xyz>::test_xyz<0, 0, 0, 0, 1, 2, 99, 56, 43>();
  check_static_array<xyz>::test_xyz<0, 1, 2, 3, 4, 5, 6, 7, 8>();

  check_static_array<xyz>::test_xyz<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>();
  check_static_array<xyz>::test_xyz<99, 15, 62, 3, 8, 12, 0, 46, 85, 5, 1, 7>();



  check_static_array<>::test_xyz<0, 0, 0>();
  check_static_array<>::test_xyz<0, 1, 2>();
  check_static_array<>::test_xyz<99, 56, 43>();

  check_static_array<>::test_xyz<0, 0, 0, 0, 1, 2>();

  check_static_array<>::test_xyz<0, 0, 0, 0, 1, 2, 99, 56, 43>();
  check_static_array<>::test_xyz<0, 1, 2, 3, 4, 5, 6, 7, 8>();

  check_static_array<>::test_xyz<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>();
  check_static_array<>::test_xyz<99, 15, 62, 3, 8, 12, 0, 46, 85, 5, 1, 7>();
}

} // namespace fatal {
