/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/container/uninitialized.h>
#include <fatal/type/traits.h>

#include <utility>

#include <cassert>

namespace fatal {

// TODO: DOCUMENT
template <typename T>
class optional {
  using data = uninitialized<T, false>;

public:
  using type = typename data::type;
  using const_reference = typename data::const_reference;
  using reference = typename data::reference;
  using const_pointer = typename data::const_pointer;
  using pointer = typename data::pointer;

  optional() noexcept: empty_(true){}

  template <typename... Args, typename = safe_overload<optional, Args...>>
  explicit optional(Args &&...args)
    noexcept(
      noexcept(
        std::declval<data>().construct(std::forward<Args>(args)...)
      )
    ):
    empty_(false)
  {
    data_.construct(std::forward<Args>(args)...);
  }

  optional(optional const &rhs)
    noexcept(
      noexcept(std::declval<data>().construct(rhs.data_.ref()))
    ):
    empty_(rhs.empty_)
  {
    if (!rhs.empty_) {
      data_.construct(rhs.data_.ref());
    }
  }

  optional(optional &&rhs)
    noexcept(
      noexcept(
        std::declval<data>().construct(std::move(rhs.data_.ref()))
      )
    ):
    empty_(rhs.empty_)
  {
    if (!rhs.empty_) {
      data_.construct(std::move(rhs.data_.ref()));
      rhs.clear();
    }
  }

  ~optional() noexcept { clear(); }

  template <typename... Args>
  type &emplace(Args &&...args)
    noexcept(
      noexcept(
        std::declval<data>().construct(std::forward<Args>(args)...)
      )
    )
  {
    clear();

    auto &result = data_.construct(std::forward<Args>(args)...);
    empty_ = false;

    return result;
  }

  void clear() noexcept {
    if (!empty_) {
      data_.destroy();
      empty_ = true;
    }

    assert(empty_);
  }

  bool empty() const noexcept { return empty_; }

  const_reference cref() const noexcept {
    assert(!empty_);
    return data_.cref();
  }

  const_reference ref() const noexcept {
    assert(!empty_);
    return data_.cref();
  }

  reference ref() noexcept {
    assert(!empty_);
    return data_.ref();
  }

  const_pointer cptr() const noexcept {
    assert(!empty_);
    return data_.cptr();
  }

  const_pointer ptr() const noexcept {
    assert(!empty_);
    return data_.cptr();
  }

  pointer ptr() noexcept {
    assert(!empty_);
    return data_.ptr();
  }

  const_pointer try_get() const noexcept {
    return empty_ ? nullptr : data_.ptr();
  }

  pointer try_get() noexcept {
    return empty_ ? nullptr : data_.ptr();
  }

  const_reference operator *() const noexcept {
    assert(!empty_);
    return data_.cref();
  }

  reference operator *() noexcept {
    assert(!empty_);
    return data_.ref();
  }

  const_pointer operator ->() const noexcept {
    assert(!empty_);
    return data_.cptr();
  }

  pointer operator ->() noexcept {
    assert(!empty_);
    return data_.ptr();
  }

  optional &operator =(optional const &rhs)
    noexcept(
      noexcept(std::declval<data>().construct(rhs.data_.ref()))
      && noexcept(std::declval<data>().ref() = rhs.data_.ref())
    )
  {
    if (rhs.empty_) {
      clear();
      return *this;
    }

    if (empty_) {
      data_.construct(rhs.data_.ref());
      empty_ = false;
    } else {
      data_.ref() = rhs.data_.ref();
    }

    assert(!empty_);
    return *this;
  }

  optional &operator =(optional &&rhs)
    noexcept(
      noexcept(
        std::declval<data>().construct(std::move(rhs.data_.ref()))
      ) && noexcept(std::declval<data>().ref() = std::move(rhs.data_.ref()))
    )
  {
    if (rhs.empty_) {
      clear();
      return *this;
    }

    if (empty_) {
      data_.construct(std::move(rhs.data_.ref()));
      empty_ = false;
    } else {
      data_.ref() = std::move(rhs.data_.ref());
    }

    rhs.clear();
    assert(!empty_);
    return *this;
  }

  template <typename U, typename = safe_overload<optional, U>>
  optional &operator =(U &&value)
    noexcept(
      noexcept(std::declval<data>().construct(std::forward<U>(value)))
      && noexcept(std::declval<data>().ref() = std::forward<U>(value))
    )
  {
    if (empty_) {
      data_.construct(std::forward<U>(value));
      empty_ = false;
    } else {
      data_.ref() = std::forward<U>(value);
    }

    return *this;
  }

  bool operator ==(optional const &rhs) const {
    return empty_
      ? rhs.empty_
      : !rhs.empty_ && data_.ref() == rhs.data_.ref();
  }

  bool operator !=(optional const &rhs) const { return !(*this == rhs); }

  explicit operator bool() const noexcept { return !empty_; }
  bool operator !() const noexcept { return empty_; }

private:
  data data_;
  bool empty_;
};

} // namespace fatal {
