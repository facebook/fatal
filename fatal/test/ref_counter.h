/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_ref_counter_h
#define FATAL_INCLUDE_fatal_test_ref_counter_h

#include <cstdint>

namespace fatal {

struct ref_counter {
  template <typename...>
  struct global {
    global():
      valid_(true)
    {
      ++count();
      ++valid();
    }

    global(global const &rhs):
      valid_(rhs.valid_)
    {
      ++count();

      if (valid_) {
        ++valid();
      }
    }

    global(global &&rhs) noexcept:
      valid_(rhs.valid_)
    {
      ++count();
      rhs.valid_ = false;
    }

    template <typename... Args>
    explicit global(Args &&...):
      valid_(true)
    {
      ++count();
      ++valid();
    }

    global &operator =(global const &rhs) {
      if (!valid_ && rhs.valid_) {
        ++valid();
      }

      valid_ = rhs.valid_;

      return *this;
    }

    global &operator =(global &&rhs) {
      if (valid_) {
        --valid();
      }

      valid_ = rhs.valid_;
      rhs.valid_ = false;

      return *this;
    }

    template <typename T>
    global &operator =(T &&) {
      if (!valid_) {
        valid_ = true;
        ++valid();
      }

      return *this;
    }

    ~global() {
      --count();

      if (valid_) {
        --valid();
      }
    }

  private:
    bool valid_;
  };

  static void reset() {
    count() = 0;
    valid() = 0;
  }

  static std::intmax_t &count() {
    static std::intmax_t n = 0;
    return n;
  }

  static std::intmax_t &valid() {
    static std::intmax_t n = 0;
    return n;
  }
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_ref_counter_h
