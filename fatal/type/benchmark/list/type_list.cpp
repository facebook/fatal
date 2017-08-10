/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/benchmark/cases.h>
#include <fatal/type/deprecated/type_list.h>

template <int> struct foo {};

int main() {
  using namespace fatal;

# define WRAP(Value) foo<Value>
# define OUTER(...) \
    do { \
      prevent_optimization(type_list<__VA_ARGS__>()); \
    } while (false)

  FATAL_BENCHMARK_CASES_CSV_OUTER_0_500(OUTER, WRAP);

  return 0;
}
