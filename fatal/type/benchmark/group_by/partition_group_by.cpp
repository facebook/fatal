/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/benchmark/cases.h>
#include <fatal/type/group_by.h>
#include <fatal/type/list.h>
#include <fatal/type/sort.h>

#include <type_traits>

template <std::size_t Threshold>
struct flt {
  template <typename T>
  using apply = std::integral_constant<bool, (T::value < Threshold)>;
};

template <std::size_t Groups>
struct key {
  template <typename T>
  using apply = std::integral_constant<std::size_t, T::value % (Groups + 1)>;
};

int main() {
  using namespace fatal;

# define INNER(Inner) \
  prevent_optimization( \
    group_by<second<partition<input, flt<Inner>>>, key<Inner>>() \
  )

# define OUTER(...) \
    do { \
      using input = index_list<__VA_ARGS__>; \
      FATAL_BENCHMARK_CASES_MONOTONIC_INNER_0_10(INNER); \
    } while (false)

  FATAL_BENCHMARK_CASES_CSV_OUTER_0_500(OUTER);

  return 0;
}
