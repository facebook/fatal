/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_driver_h
#define FATAL_INCLUDE_fatal_test_driver_h

#include <fatal/test/lib.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

////////////
// DRIVER //
////////////

int main(int argc, char **argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argc > 0 ? argv[0] : "unknown");
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}

#endif // FATAL_INCLUDE_fatal_test_driver_h
