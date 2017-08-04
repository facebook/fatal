/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/benchmark/cases.h>
#include <fatal/type/deprecated/constant_sequence.h>

int main() {
  using namespace fatal;

# define OUTER(Size) \
  prevent_optimization(constant_range<std::size_t, 0, Size>())

  FATAL_BENCHMARK_CASES_MONOTONIC_OUTER_0_100(OUTER);

  return 0;
}
