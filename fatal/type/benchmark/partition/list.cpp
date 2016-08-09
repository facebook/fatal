/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/benchmark/cases.h>
#include <fatal/type/list.h>
#include <fatal/type/sort.h>

struct flt {
  template <typename T>
  using apply = std::integral_constant<bool, T::value % 2 == 0>;
};

int main() {
  using namespace fatal;

# define OUTER(...) \
  do { \
    prevent_optimization(partition<int_list<__VA_ARGS__>, flt>()); \
  } while (false)

  FATAL_BENCHMARK_CASES_CSV_OUTER_0_1000_SHUFFLED(OUTER);

  return 0;
}
