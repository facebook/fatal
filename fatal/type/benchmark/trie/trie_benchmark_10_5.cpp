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

CREATE_BENCHMARK(n10_len5,
  s5_00, s5_01, s5_02, s5_03, s5_04,
  s5_05, s5_06, s5_07, s5_08, s5_09
)

} // namespace fatal {
