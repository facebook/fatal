/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_benchmark_driver_h
#define FATAL_INCLUDE_fatal_benchmark_driver_h

#include <fatal/benchmark/benchmark.h>

#include <iostream>

////////////
// DRIVER //
////////////

int main() {
  fatal::benchmark::run(std::cout);

  return 0;
}

#endif // FATAL_INCLUDE_fatal_benchmark_driver_h
