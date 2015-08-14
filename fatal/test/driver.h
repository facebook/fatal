/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_driver_h
#define FATAL_INCLUDE_fatal_test_driver_h

#include <fatal/log/log.h>
#include <fatal/test/random_data.h>
#include <fatal/test/string.h>
#include <fatal/test/test.h>
#include <fatal/test/type.h>

#include <iostream>

////////////
// DRIVER //
////////////

int main() {
  return fatal::test::run(std::cerr);
}

#endif // FATAL_INCLUDE_fatal_test_driver_h
