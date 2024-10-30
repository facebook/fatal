/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/container/variadic_union.h>

#include <fatal/portability.h>

#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <cassert>

namespace fatal {
namespace detail {
namespace runtime_array_impl {

using default_small_buffer_size = std::integral_constant<std::size_t, 64>;

template <typename T, std::size_t Bytes = default_small_buffer_size::value>
using size = std::integral_constant<
  std::size_t, Bytes / sizeof(T) ? Bytes / sizeof(T) : 1
>;

} // namespace runtime_array_impl {
} // namespace detail {

template <
  typename T,
  std::size_t SmallBufferSize = detail::runtime_array_impl::size<T>::value
>
struct runtime_array {
  using value_type = T;

  using const_pointer = value_type const *;
  using pointer = value_type *;

  using const_reference = value_type const &;
  using reference = value_type &;

  using const_iterator = const_pointer;
  using iterator = pointer;

  using size_type = std::size_t;

  static_assert(
    SmallBufferSize <= std::numeric_limits<size_type>::max(),
    "small buffer size is too big"
  );

private:
  using small_size = std::integral_constant<size_type, SmallBufferSize>;
  using static_type = std::array<value_type, small_size::value>;
  using dynamic_type = std::unique_ptr<value_type[]>;
  using container_type = variadic_union<static_type, dynamic_type>;

public:

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

  explicit runtime_array(size_type size):
    size_(size)
  {
    if (is_small()) {
      data_ = container_.template construct<static_type>().data();
    } else {
      data_ = container_.template construct<dynamic_type>(
        new value_type[size_]
      ).get();
    }
  }

  runtime_array(runtime_array const &rhs):
    size_(rhs.size_)
  {
    if (rhs.is_small()) {
      data_ = container_.template construct<static_type>(
        rhs.container_.template ref<static_type>()
      ).data();
    } else {
      data_ = container_.template construct<dynamic_type>(
        new value_type[size_]
      ).get();

      auto input = rhs.container_.template ref<dynamic_type>().get();
      auto end = std::copy(input, std::next(input, signed_cast(size_)), data_);
      assert(std::next(data_, signed_cast(size_)) == end);
      (void)end;
    }
  }

FATAL_DIAGNOSTIC_POP

  runtime_array(runtime_array &&rhs):
    size_(rhs.size_)
  {
    if (rhs.is_small()) {
      data_ = container_.template construct<static_type>(
        std::move(rhs.container_.template ref<static_type>())
      ).data();
    } else {
      data_ = container_.template construct<dynamic_type>(
        std::move(rhs.container_.template ref<dynamic_type>())
      ).get();
    }
  }

  ~runtime_array() {
    if (is_small()) {
      container_.template destroy<static_type>();
    } else {
      container_.template destroy<dynamic_type>();
    }
  }

  const_reference at(size_type i) const {
    if (i < size_) {
      return data_[i];
    }

    throw std::out_of_range("index out of bounds");
  }

  reference at(size_type i) {
    if (i < size_) {
      return data_[i];
    }

    throw std::out_of_range("index out of bounds");
  }

  const_reference operator [](size_type i) const {
    assert(i < size_);
    return data_[i];
  }

  reference operator [](size_type i) {
    assert(i < size_);
    return data_[i];
  }

  pointer data() const { return data_; }
  pointer data() { return data_; }

  const_iterator cbegin() const { return data_; }
  const_iterator begin() const { return data_; }
  iterator begin() { return data_; }

  const_iterator cend() const { return std::next(data_, signed_cast(size_)); }
  const_iterator end() const { return std::next(data_, signed_cast(size_)); }
  iterator end() { return std::next(data_, signed_cast(size_)); }

  size_type size() const { return size_; }
  bool empty() const { return !size_; }

private:
  bool is_small() const { return size_ <= small_size::value; }

  size_type const size_;
  pointer data_;
  container_type container_;
};

} // namespace fatal {
