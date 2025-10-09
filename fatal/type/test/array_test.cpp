/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/array.h>

#include <fatal/string/string_view.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>
#include <fatal/type/size.h>
#include <fatal/type/slice.h>

#include <array>
#include <random>

#include <fatal/test/driver.h>

FATAL_TEST(c_array, overloads_full) {
  using arr_t = fatal::c_array<int, 3>;
  arr_t arr{{'b', 'a', 'r'}};
  FATAL_EXPECT_EQ(size(arr), 3);
  FATAL_EXPECT_EQ(empty(arr), false);
  FATAL_EXPECT_EQ(data(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(begin(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(cbegin(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(end(arr), &arr.data[3]);
  FATAL_EXPECT_EQ(cend(arr), &arr.data[3]);
}

FATAL_TEST(c_array, overloads_empty) {
  using arr_t = fatal::c_array<int, 0>;
  arr_t arr{{}};
  FATAL_EXPECT_EQ(size(arr), 0);
  FATAL_EXPECT_EQ(empty(arr), true);
  FATAL_EXPECT_EQ(data(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(begin(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(cbegin(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(end(arr), &arr.data[0]);
  FATAL_EXPECT_EQ(cend(arr), &arr.data[0]);
}

namespace fatal {
namespace seq {

using foo = char_sequence<'f', 'o', 'o'>;
using bar = char_sequence<'b', 'a', 'r', '1'>;
using baz = char_sequence<'b', 'a', 'z', '$', '_', '2'>;
using gaz = char_sequence<'g', 'a', 'z', '-', '3'>;

using all = list<foo, bar, baz, gaz>;

} // namespace seq {

namespace lst {

using foo = char_list<'f', 'o', 'o'>;
using bar = char_list<'b', 'a', 'r', '1'>;
using baz = char_list<'b', 'a', 'z', '$', '_', '2'>;
using gaz = char_list<'g', 'a', 'z', '-', '3'>;

using all = list<foo, bar, baz, gaz>;

} // namespace lst {

template <typename T, std::size_t S>
using raw = T[S];

template <template <typename, std::size_t> class A, typename T, T... V>
struct static_array {
  static constexpr A<T, sizeof...(V)> value{{V...}};
};
#if FATAL_CPLUSPLUS < 201703L
template <template <typename, std::size_t> class A, typename T, T... V>
constexpr A<T, sizeof...(V)> static_array<A, T, V...>::value;
#endif

template <typename T, T... V>
struct static_array<raw, T, V...> {
  static constexpr raw<T, sizeof...(V)> value{V...}; // NOLINT(facebook-hte-CArray)
};
#if FATAL_CPLUSPLUS < 201703L
template <typename T, T... V>
constexpr raw<T, sizeof...(V)> static_array<raw, T, V...>::value;
#endif

FATAL_TEST(array_to_sequence, raw_array_full) {
  using arr = static_array<raw, char, 'h', 'e', 'l', 'l', 'o'>;
  FATAL_EXPECT_SAME<
    array_to_sequence<raw, char, 5, arr::value>,
    char_sequence<'h', 'e', 'l', 'l', 'o'>
  >();
}

FATAL_TEST(array_to_sequence, c_array_full) {
  using arr = static_array<c_array, char, 'h', 'e', 'l', 'l', 'o'>;
  FATAL_EXPECT_SAME<
    array_to_sequence<c_array, char, 5, arr::value>,
    char_sequence<'h', 'e', 'l', 'l', 'o'>
  >();
}

FATAL_TEST(array_to_sequence, c_array_empty) {
  using arr = static_array<c_array, char>;
  FATAL_EXPECT_SAME<
    array_to_sequence<c_array, char, 0, arr::value>,
    char_sequence<>
  >();
}

FATAL_TEST(array_to_sequence, std_array_full) {
  using arr = static_array<std::array, char, 'h', 'e', 'l', 'l', 'o'>;
  FATAL_EXPECT_SAME<
    array_to_sequence<std::array, char, 5, arr::value>,
    char_sequence<'h', 'e', 'l', 'l', 'o'>
  >();
}

FATAL_TEST(array_to_sequence, std_array_empty) {
  using arr = static_array<std::array, char>;
  FATAL_EXPECT_SAME<
    array_to_sequence<std::array, char, 0, arr::value>,
    char_sequence<>
  >();
}

struct constexpr_factory {
  template <typename T>
  static constexpr std::size_t get() { return size<T>::value; }
};

FATAL_TEST(array, as_array_from) {
  using array = as_array_from<seq::all, constexpr_factory>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(
    array::data[0] == size<first<seq::all>>::value,
    "ensure it is not constexpr"
  );
}

std::size_t non_constexpr() {
  static std::size_t const value = std::default_random_engine()();
  return value;
}

struct non_constexpr_factory {
  template <typename T>
  static std::size_t get() {
    return size<T>::value + non_constexpr();
  }
};

FATAL_TEST(array, as_runtime_array_from) {
  using array = as_runtime_array_from<seq::all, non_constexpr_factory>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  FATAL_EXPECT_EQ(
    size<first<seq::all>>::value + non_constexpr(),
    array::data[0]
  );
}

FATAL_TEST(array, z_array_from_sequence_with_implicit_type) {
  using array = z_array<seq::all>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array_from_sequence_with_explicit_type) {
  using array = z_array<seq::all, const char *>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array_from_list_with_implicit_type) {
  using array = z_array<lst::all>;
  FATAL_EXPECT_EQ(size<lst::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array_from_list_with_explicit_type) {
  using array = z_array<lst::all, const char *>;
  FATAL_EXPECT_EQ(size<lst::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, string_view_array_from_sequence) {
  using array = string_view_array<seq::all, string_view>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
  static_assert(
    array::data[0].size() == size<first<seq::all>>::value,
    "ensure it is constexpr"
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<seq::all, 0>>(), size<at<seq::all, 0>>::value),
    array::data[0]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<seq::all, 1>>(), size<at<seq::all, 1>>::value),
    array::data[1]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<seq::all, 2>>(), size<at<seq::all, 2>>::value),
    array::data[2]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<seq::all, 3>>(), size<at<seq::all, 3>>::value),
    array::data[3]
  );
}

FATAL_TEST(array, string_view_array_from_list) {
  using array = string_view_array<lst::all, string_view>;
  FATAL_EXPECT_EQ(size<lst::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
  static_assert(
    array::data[0].size() == size<first<lst::all>>::value,
    "ensure it is constexpr"
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<lst::all, 0>>(), size<at<lst::all, 0>>::value),
    array::data[0]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<lst::all, 1>>(), size<at<lst::all, 1>>::value),
    array::data[1]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<lst::all, 2>>(), size<at<lst::all, 2>>::value),
    array::data[2]
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<at<lst::all, 3>>(), size<at<lst::all, 3>>::value),
    array::data[3]
  );
}

} // namespace fatal {
