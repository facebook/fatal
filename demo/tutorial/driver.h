/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_demo_tutorial_driver_h
#define FATAL_INCLUDE_demo_tutorial_driver_h

#include <fatal/test/test.h>

#define FATAL_TUTORIAL(...) \
  FATAL_TEST(fatal tutorial, __VA_ARGS__)

#include <fatal/log/log.h>
#include <fatal/test/string.h>
#include <fatal/test/test.h>
#include <fatal/test/type.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fatal {
namespace tutorial {

template <typename TOut>
struct printer {
  struct writer {
    explicit writer(TOut *out) noexcept: out_(out) {}

    writer(writer const &) = delete;
    writer(writer &&rhs) noexcept: out_(rhs.out_) { rhs.out_ = nullptr; }

    template <typename T>
    writer &operator <<(T &&value) & {
      if (out_) {
        *out_ << std::forward<T>(value);
      }

      return *this;
    }

    template <typename T>
    writer &&operator <<(T &&value) && {
      if (out_) {
        *out_ << std::forward<T>(value);
      }

      return std::move(*this);
    }

    ~writer() {
      if (out_) {
        *out_ << '\n';
      }
    }

  private:
    TOut *out_;
  };

  printer(TOut *out, source_info source) noexcept:
    writer_(out),
    source_(source)
  {}

  printer(printer const &) = delete;
  printer(printer &&rhs) = default;

  template <typename T>
  writer operator <<(T &&value) {
    writer_ << source_.file() << ':' << source_.line() << "| "
      << std::forward<T>(value);

    return std::move(writer_);
  }

private:
  writer writer_;
  source_info source_;
};

} // namespace tutorial {
} // namespace fatal {

#define PRINT \
  ::fatal::tutorial::printer<::std::ostream>( \
    ::std::addressof(::std::cout), \
    FATAL_SOURCE_INFO() \
  )

#define NEW_LINE PRINT << ""

using fatal::type_str;

////////////
// DRIVER //
////////////

int main() {
  return fatal::test::run(std::cout);
}

#endif // FATAL_INCLUDE_demo_tutorial_driver_h
