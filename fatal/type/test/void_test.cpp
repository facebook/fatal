/*
 *  Copyright (c) 2017, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/void.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

namespace {
template <typename T, typename = void>
struct has_type : std::false_type {};

template <typename T>
struct has_type<T, void_t<typename T::type>> : std::true_type {};
}

FATAL_TEST(void_t, example) {
  FATAL_EXPECT_FALSE(has_type<int>::value);
  FATAL_EXPECT_TRUE(has_type<std::decay<int>>::value);
}

} // namespace fatal {
