/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <folly/Demangle.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <type_traits>
#include <string>

namespace ftl {

template <typename T>
std::string type_str() {
  auto s = folly::demangle(typeid(T)).toStdString();

  typedef typename std::remove_reference<T>::type type;

  if (std::is_const<type>::value) {
    s.append(" const");
  }

  if (std::is_volatile<type>::value) {
    s.append(" volatile");
  }

  if (std::is_lvalue_reference<T>::value) {
    s.append(" &");
  } else if (std::is_rvalue_reference<T>::value) {
    s.append(" &&");
  }

  return s;
}

/**
 * Unit test helper for checking whether two types are the same.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define EXPECT_SAME(Expected, Actual) \
  do { \
    if (!std::is_same<Expected, Actual>::value) { \
      LOG(ERROR) << "expected: '" << type_str<Expected>() << '\''; \
      LOG(ERROR) << "  actual: '" << type_str<Actual>() << '\''; \
      EXPECT_TRUE((std::is_same<Expected, Actual>::value)); \
    } \
  } while (false)

template <typename TExpected, typename TActual>
void expect_same() { EXPECT_SAME(TExpected, TActual); }

} // namespace ftl {
