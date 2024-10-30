/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/traits.h>

#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

namespace fatal {
namespace detail {

template <typename T, bool>
struct random_access_iterator_traits {
  using type = T;
  using value_type = typename T::value_type;
  using reference = typename T::reference;
  using const_reference = typename T::const_reference;
  using pointer = typename T::pointer;
  using const_pointer = typename T::const_pointer;
  using size_type = typename T::size_type;
};

template <typename T>
struct random_access_iterator_traits<T, true> {
  using type = typename std::add_const<T>::type;
  using value_type = typename std::add_const<typename T::value_type>::type;
  using reference = typename T::const_reference;
  using const_reference = typename T::const_reference;
  using pointer = typename T::const_pointer;
  using const_pointer = typename T::const_pointer;
  using size_type = typename T::size_type;
};

} // namespace detail {

// TODO: document and test
template <typename TContainer, bool IsConst>
class random_access_iterator {
  using traits = detail::random_access_iterator_traits<TContainer, IsConst>;
  using container_type = typename traits::type;

public:
  using value_type = typename traits::value_type;
  using reference = typename traits::reference;
  using const_reference = typename traits::const_reference;
  using pointer = typename traits::pointer;
  using const_pointer = typename traits::const_pointer;
  using size_type = typename traits::size_type;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  random_access_iterator(container_type *container, size_type i):
    container_(container),
    i_(i)
  {}

  random_access_iterator &operator ++() {
    assert(!container_ || i_ < container_->size());
    ++i_;
    return *this;
  }

  random_access_iterator &operator --() {
    assert(i_ > 0);
    --i_;
    return *this;
  }

  random_access_iterator operator ++(int) {
    assert(container_);
    assert(i_ < container_->size());
    return random_access_iterator(*container_, i_++);
  }

  random_access_iterator operator --(int) {
    assert(container_);
    assert(i_ > 0);
    return random_access_iterator(*container_, i_--);
  }

  random_access_iterator &operator +=(fatal::fast_pass<size_type> diff) {
    assert(!container_ || i_ <= container_->size());
    assert(!container_ || (
      std::numeric_limits<size_type>::max() - (container_->size() - i_) <= diff
    ));
    i_ += diff;
    return *this;
  }

  random_access_iterator &operator -=(fatal::fast_pass<size_type> diff) {
    assert(diff <= i_);
    i_ -= diff;
    return *this;
  }

  random_access_iterator operator +(fatal::fast_pass<size_type> diff) const {
    auto copy = &this;
    copy += diff;
    return copy;
  }

  random_access_iterator operator -(fatal::fast_pass<size_type> diff) const {
    auto copy = &this;
    copy -= diff;
    return copy;
  }

  random_access_iterator &operator +=(fatal::fast_pass<difference_type> diff) {
    assert(!container_ || (
      diff < 0
        ? -diff <= i_
        : std::numeric_limits<size_type>::max() - container_->size() <= diff
    ));
    assert(static_cast<size_type>(i_ + diff) == (
      diff < 0
        ? i_ + static_cast<size_type>(-diff)
        : i_ + static_cast<size_type>(diff)
    ));

    i_ += diff;

    return *this;
  }

  random_access_iterator &operator -=(fatal::fast_pass<difference_type> diff) {
    assert(!container_ || (
      diff < 0
        ? std::numeric_limits<size_type>::max() - container_->size() <= -diff
        : diff <= i_
    ));
    assert(static_cast<size_type>(i_ - diff) == (
      diff < 0
        ? i_ - static_cast<size_type>(-diff)
        : i_ - static_cast<size_type>(diff)
    ));

    i_ -= diff;

    return *this;
  }

  random_access_iterator operator +(
    fatal::fast_pass<difference_type> diff
  ) const {
    auto copy = &this;
    copy += diff;
    return copy;
  }

  random_access_iterator operator -(
    fatal::fast_pass<difference_type> diff
  ) const {
    auto copy = &this;
    copy -= diff;
    return copy;
  }

  difference_type operator -(
    fatal::fast_pass<random_access_iterator> rhs
  ) const {
    assert(container_ == rhs.container_);
    assert(rhs.i_ <= i_);

    return i_ < rhs.i_
      ? -static_cast<difference_type>(rhs.i_ - i_)
      : static_cast<difference_type>(i_ - rhs.i_);
  }

  bool operator ==(fatal::fast_pass<random_access_iterator> rhs) const {
    return container_ == rhs.container_ && i_ == rhs.i_;
  }

  bool operator !=(fatal::fast_pass<random_access_iterator> rhs) const {
    return !(*this == rhs);
  }

  const_pointer operator ->() const {
    assert(container_);
    assert(i_ < container_->size());
    return std::addressof((*container_)[i_]);
  }

  pointer operator ->() {
    assert(container_);
    assert(i_ < container_->size());
    return std::addressof((*container_)[i_]);
  }

  const_reference operator *() const {
    assert(container_);
    assert(i_ < container_->size());
    return (*container_)[i_];
  }

  reference operator *() {
    assert(container_);
    assert(i_ < container_->size());
    return (*container_)[i_];
  }

private:
  container_type *container_;
  size_type i_;
};

} // namespace fatal {
