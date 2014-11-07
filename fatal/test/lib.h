/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_lib_h
#define FATAL_INCLUDE_fatal_test_lib_h

#include <folly/Demangle.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <type_traits>
#include <string>

namespace fatal {

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
template <typename TExpected, typename TActual>
void expect_same(
  char const *file_name = __FILE__,
  std::size_t line_number = __LINE__
) {
  if (!std::is_same<TExpected, TActual>::value) {
    LOG(ERROR) << "at " << file_name << ':' << line_number << ':'
      << std::endl << std::endl
      << "  expected: '" << type_str<TExpected>() << '\''
      << std::endl << std::endl
      << "  actual:   '" << type_str<TActual>() << '\''
      << std::endl << std::endl;

    EXPECT_TRUE((std::is_same<TExpected, TActual>::value));
  } \
}

namespace detail {

struct expect_same_impl {
  expect_same_impl(char const *file_name, std::size_t line_number):
    file_name_(file_name),
    line_number_(line_number)
  {}

  template <typename TExpected, typename TActual>
  void check() const {
    expect_same<TExpected, TActual>(file_name_, line_number_);
  }

private:
  char const *file_name_;
  std::size_t line_number_;
};

} // namespace detail {

#define FATAL_EXPECT_SAME \
  ::fatal::detail::expect_same_impl{__FILE__, __LINE__}.check

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_lib_h
