/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/test/lib.h>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

////////////
// DRIVER //
////////////

int main(int argc, char **argv) {
  google::InitGoogleLogging(argc > 0 ? argv[0] : "unknown");
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}
