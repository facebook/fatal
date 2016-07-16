/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_lesson_driver_h
#define FATAL_INCLUDE_fatal_lesson_driver_h

#include <fatal/lesson/lesson.h>
#include <fatal/test/string.h>
#include <fatal/test/type.h>

#include <algorithm>
#include <array>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

int main() {
  fatal::lesson::registry::run();
  return 0;
}

#endif // FATAL_INCLUDE_fatal_lesson_driver_h
