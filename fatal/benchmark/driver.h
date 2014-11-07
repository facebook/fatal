/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_benchmark_driver_h
#define FATAL_INCLUDE_fatal_benchmark_driver_h

#include <folly/Benchmark.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

////////////
// DRIVER //
////////////

int main(int argc, char **argv) {
  google::InitGoogleLogging(argc > 0 ? argv[0] : "unknown");
  google::ParseCommandLineFlags(&argc, &argv, true);

  folly::runBenchmarks();

  return 0;
}

#endif // FATAL_INCLUDE_fatal_benchmark_driver_h
