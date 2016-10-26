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
namespace str {

FATAL_S(foo, "foo");
FATAL_S(bar, "bar");
FATAL_S(baz, "baz");
FATAL_S(gaz, "gaz");

using lst = list<foo, bar, baz, gaz>;

} // namespace str {

struct constexpr_factory {
  template <typename T>
  static constexpr std::size_t get() { return size<T>::value; }
};

FATAL_TEST(array, as_array_from) {
  using array = as_array_from<str::lst, constexpr_factory>;
  FATAL_EXPECT_EQ(size<str::lst>::value, array::size::value);
  static_assert(
    array::data[0] == size<first<str::lst>>::value,
    "ensure it is not constexpr"
  );
}

std::size_t non_constexpr() {
  static std::size_t const value = std::default_random_engine()();
  return value;
};

struct non_constexpr_factory {
  template <typename T>
  static std::size_t get() {
    return size<T>::value + non_constexpr();
  }
};

FATAL_TEST(array, as_runtime_array_from) {
  using array = as_runtime_array_from<str::lst, non_constexpr_factory>;
  FATAL_EXPECT_EQ(size<str::lst>::value, array::size::value);
  FATAL_EXPECT_EQ(
    size<first<str::lst>>::value + non_constexpr(),
    array::data[0]
  );
}

FATAL_TEST(array, z_array) {
  using array = z_array<str::lst>;
  FATAL_EXPECT_EQ(size<str::lst>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
}

FATAL_TEST(array, string_view_array) {
  using array = string_view_array<str::lst, string_view>;
  FATAL_EXPECT_EQ(size<str::lst>::value, array::size::value);
  static_assert(array::data[0][0] == 'f', "ensure it is constexpr");
  static_assert(
    array::data[0].size() == size<first<str::lst>>::value,
    "ensure it is constexpr"
  );
  FATAL_EXPECT_EQ(
    string_view(z_data<first<str::lst>>(), size<first<str::lst>>::value),
    array::data[0]
  );
}

} // namespace fatal {
