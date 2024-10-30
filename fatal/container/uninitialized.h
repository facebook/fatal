/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/container/unitary_union.h>

#include <memory>
#include <utility>

namespace fatal {

///////////////////
// uninitialized //
///////////////////

// TODO: DOCUMENT AND TEST
template <typename T, bool AutoDestroy>
struct uninitialized {
  using type = T;
  using const_reference = type const &;
  using reference = type &;
  using const_pointer = type const *;
  using pointer = type *;

  uninitialized() = default;
  uninitialized(uninitialized const &) = delete;
  uninitialized(uninitialized &&) = delete;

  template <typename... Args>
  reference construct(Args &&...args)
    noexcept(noexcept(type(std::forward<Args>(args)...)))
  {
    return *new (ptr()) type(std::forward<Args>(args)...);
  }

  pointer destroy() noexcept {
    ref().~type();
    return ptr();
  }

  void steal(uninitialized &&other)
    noexcept(
      noexcept(
        std::declval<uninitialized>().construct(std::move(other.data_.value))
      )
    )
  {
    construct(std::move(other.data_.value));
    other.destroy();
  }

  const_reference cref() const noexcept { return data_.value; }
  const_reference ref() const noexcept { return data_.value; }
  reference ref() noexcept { return data_.value; }

  const_pointer cptr() const noexcept { return std::addressof(data_.value); }
  const_pointer ptr() const noexcept { return std::addressof(data_.value); }
  pointer ptr() noexcept { return std::addressof(data_.value); }

  const_reference operator *() const noexcept { return data_.value; }

  reference operator *() noexcept { return data_.value; }

  const_pointer operator ->() const noexcept {
    return std::addressof(data_.value);
  }

  pointer operator ->() noexcept { return std::addressof(data_.value); }

private:
  using union_type = unitary_union<type, AutoDestroy>;

  union_type data_;
};

} // namespace fatal {
