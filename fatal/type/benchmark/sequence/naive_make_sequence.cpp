/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/benchmark/cases.h>

#include <type_traits>

template <std::size_t... Values>
struct seq {};

template <std::size_t Size, std::size_t... Values>
struct build_impl {
  using type = typename build_impl<Size - 1, Size - 1, Values...>::type;
};

template <std::size_t... Values>
struct build_impl<0, Values...> {
  using type = seq<Values...>;
};

template <std::size_t Size>
using build = typename build_impl<Size>::type;

static_assert(std::is_same<seq<0, 1, 2, 3, 4>, build<5>>::value, "");

int main() {
  using namespace fatal;

# define OUTER(Size) \
  prevent_optimization(build<Size>())

  FATAL_BENCHMARK_CASES_MONOTONIC_OUTER_0_500(OUTER);

  return 0;
}
