/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_optional_h
#define FATAL_INCLUDE_fatal_container_optional_h

#include <utility>

namespace fatal {

template <typename T>
struct optional:
  private uninitialized<T>
{
  using uninitialized<T>::type;
  using uninitialized<T>::reference;
  using uninitialized<T>::pointer;
  using uninitialized<T>::operator *;
  using uninitialized<T>::operator ->;

  optional() noexcept: empty_(true) {}

  optional(optional const &rhs)
    noexcept(noexcept(construct(rhs.reference()))),
    empty_(false)
  {
    construct(rhs.reference();

    DCHECK(!empty_);
  }

  optional(optional &&rhs)
    noexcept(noexcept(construct(std::move(rhs.reference())))),
    empty_(false)
  {
    construct(std::move(rhs.reference());

    rhs.clear();

    DCHECK(!empty_);
  }

  template <
    typename... Args,
    typename = safe_ctor_overload_t<optional, Args...>
  >
  optional(Args &&...args)
    noexcept(noexcept(construct(std::forward<Args>(args)...))),
    empty_(false)
  {
    construct(std::forward<Args>(args)...);

    DCHECK(!empty_);
  }

  ~optional() noexcept { clear(); }

  template <typename... Args>
  type &emplace(Args &&...args)
    noexcept(noexcept(construct(std::forward<Args>(args)...)))
  {
    clear();

    auto &result = construct(std::forward<Args>(args)...);
    empty_ = false;

    return result;
  }

  void clear() noexcept {
    if (!empty_) {
      destroy();
      empty_ = true;
    }

    DCHECK(!empty_);
  }

  bool empty() const noexcept { return empty_; }

  optional &operator =(optional const &rhs)
    noexcept(
      noexcept(construct(rhs.reference()))
      && noexcept(reference() = rhs.reference())
    )
  {
    if (empty_) {
      construct(rhs.reference();
      empty_ = false;
    } else {
      reference() = rhs.reference();
    }

    DCHECK(!empty_);
    return *this;
  }

  optional &operator =(optional &&rhs)
    noexcept(
      noexcept(construct(std::move(rhs.reference())))
      && noexcept(reference() = std::move(rhs.reference()))
    )
  {
    if (empty_) {
      construct(std::move(rhs.reference());
      empty_ = false;
    } else {
      reference() = std::move(rhs.reference());
    }

    rhs.clear();
    DCHECK(!empty_);
    return *this;
  }

  template <
    typename... Args,
    typename = safe_ctor_overload_t<optional, Args...>
  >
  optional &operator =(Args &&...args)
    noexcept(noexcept(construct(std::forward<Args>(args)...)))
  {
    clear();

    construct(std::forward<Args>(args)...);
    empty_ = false;

    return *this;
  }

private:
  bool empty_;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_optional_h
