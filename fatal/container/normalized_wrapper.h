/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_normalized_wrapper_h
#define FATAL_INCLUDE_fatal_container_normalized_wrapper_h

#include <fatal/type/traits.h>

#include <memory>
#include <type_traits>

#include <cassert>

namespace fatal {

template <typename T>
struct normalized_wrapper {
  using type = T;
  using reference = typename std::add_lvalue_reference<type>::type;
  using const_reference = typename std::add_const<
    typename std::remove_reference<type>::type
  >::type &;
  using pointer = typename std::remove_reference<type>::type *;
  using const_pointer = typename std::add_const<
    typename std::remove_reference<type>::type
  >::type *;

  template <
    typename... Args,
    typename = safe_overload<normalized_wrapper, Args...>
  >
  explicit normalized_wrapper(Args &&...args):
    wrapped_(std::forward<Args>(args)...)
  {}

  normalized_wrapper(normalized_wrapper const &rhs): wrapped_(rhs.wrapped_) {}

  normalized_wrapper(normalized_wrapper &&rhs):
    wrapped_(std::forward<type>(rhs.wrapped_))
  {}

  fast_pass<type> get() const { return wrapped_; }
  type &get() { return wrapped_; }

  const_reference ref() const { return wrapped_; }
  reference ref() { return wrapped_; }

  const_pointer ptr() const { return std::addressof(wrapped_); }
  pointer ptr() { return std::addressof(wrapped_); }

  const_reference operator *() const { return ref(); }
  reference operator *() { return ref(); }

  const_pointer operator ->() const { return ptr(); }
  pointer operator ->() { return ptr(); }

  normalized_wrapper &operator =(normalized_wrapper const &rhs) {
    wrapped_ = rhs.wrapped_;
    return *this;
  }

  normalized_wrapper &operator =(normalized_wrapper &&rhs) {
    wrapped_ = std::move(rhs.wrapped_);
    return *this;
  }

  // TODO: HIDE operator == WHEN `T` DOESN'T SUPPORT IT

  bool operator ==(normalized_wrapper const &rhs) const {
    return wrapped_ == rhs.wrapped_;
  }

  bool operator !=(normalized_wrapper const &rhs) const {
    return wrapped_ != rhs.wrapped_;
  }

private:
  type wrapped_;
};

template <typename T>
struct normalized_wrapper<T *> {
  using type = T;
  using reference = type &;
  using const_reference = typename std::add_const<type>::type &;
  using pointer = type *;
  using const_pointer = typename std::add_const<type>::type *;

  explicit normalized_wrapper(type &wrapped):
    wrapped_(std::addressof(wrapped))
  {}

  explicit normalized_wrapper(pointer wrapped) noexcept:
    wrapped_(wrapped)
  {}

  normalized_wrapper(normalized_wrapper const &rhs) noexcept:
    wrapped_(rhs.wrapped_)
  {}

  normalized_wrapper(normalized_wrapper &&rhs) noexcept:
    wrapped_(rhs.wrapped_)
  {
    rhs.wrapped_ = nullptr;
  }

  fast_pass<type> get() noexcept const { return wrapped_; }
  type &get() noexcept { return wrapped_; }

  const_reference cref() noexcept const {
    assert(wrapped_);
    return *wrapped_;
  }

  const_reference ref() noexcept const {
    assert(wrapped_);
    return *wrapped_;
  }

  reference ref() noexcept {
    assert(wrapped_);
    return *wrapped_;
  }

  const_pointer cptr() noexcept const {
    assert(wrapped_);
    return wrapped_;
  }

  const_pointer ptr() noexcept const {
    assert(wrapped_);
    return wrapped_;
  }

  pointer ptr() noexcept {
    assert(wrapped_);
    return wrapped_;
  }

  const_reference operator *() noexcept const { return ref(); }
  reference operator *() noexcept { return ref(); }

  const_pointer operator ->() noexcept const { return ptr(); }
  pointer operator ->() noexcept { return ptr(); }

  normalized_wrapper &operator =(normalized_wrapper const &rhs) noexcept {
    wrapped_ = rhs.wrapped_;
    return *this;
  }

  normalized_wrapper &operator =(normalized_wrapper &&rhs) noexcept {
    wrapped_ = rhs.wrapped_;
    rhs.wrapped_ = nullptr;
    return *this;
  }

  bool operator ==(normalized_wrapper const &rhs) const noexcept {
    return wrapped_ == rhs.wrapped_;
  }

  bool operator !=(normalized_wrapper const &rhs) const noexcept {
    return wrapped_ != rhs.wrapped_;
  }

private:
  pointer wrapped_;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_normalized_wrapper_h
