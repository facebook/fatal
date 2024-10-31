/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <utility>

namespace fatal {
namespace fn {

/**
 * An identity operation that receives any parameter and returns it, unaltered.
 *
 * Example:
 *
 *  identity i;
 *
 *  // yields an `int &&` containing `10`
 *  auto result1 = i(10);
 *
 *  // yields a `char const [6] &` pointing to "hello"
 *  auto result2 = i("hello");
 *
 *  // yields a `std::string &&` containing "world"
 *  auto result3 = i(std::string("world"));
 *
 *  std::string s("test");
 *  // yields a `std::string &` containing "test"
 *  auto result4 = i(s);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct identity {
  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }
};

} // namespace fn {
} // namespace fatal {
