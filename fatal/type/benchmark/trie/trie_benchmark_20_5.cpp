/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/benchmark/trie/trie_benchmark.h>

#include <fatal/benchmark/driver.h>

namespace fatal {

CREATE_BENCHMARK(n20_len5,
  s5_00, s5_01, s5_02, s5_03, s5_04,
  s5_05, s5_06, s5_07, s5_08, s5_09,
  s5_10, s5_11, s5_12, s5_13, s5_14,
  s5_15, s5_16, s5_17, s5_18, s5_19
)

} // namespace fatal {
