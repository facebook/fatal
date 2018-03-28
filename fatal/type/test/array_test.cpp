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

#include <random>

#include <fatal/test/driver.h>

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

FATAL_TEST(array, z_array from sequence / implicit type) {
  using array = z_array<seq::all>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array from sequence / explicit type) {
  using array = z_array<seq::all, const char *>;
  FATAL_EXPECT_EQ(size<seq::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array from list / implicit type) {
  using array = z_array<lst::all>;
  FATAL_EXPECT_EQ(size<lst::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, z_array from list / explicit type) {
  using array = z_array<lst::all, const char *>;
  FATAL_EXPECT_EQ(size<lst::all>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, string_view_array from sequence) {
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

FATAL_TEST(array, string_view_array from list) {
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
