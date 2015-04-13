/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_string_ref_h
#define FATAL_INCLUDE_fatal_string_string_ref_h

#include <fatal/math/hash.h>

#include <iterator>

#include <cstring>

namespace fatal {

struct string_ref {
  using size_type = std::size_t;
  using const_iterator = char const *;

  string_ref(char const *begin, char const *end):
    begin_(begin),
    end_(end)
  {
    assert(begin_ <= end_);
  }

  string_ref(char const *s, std::size_t size):
    begin_(s),
    end_(std::next(s, size))
  {
    assert(begin_ <= end_);
  }

  string_ref(char *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  string_ref(char const *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  template <std::size_t N>
  explicit string_ref(char const (&s)[N]):
    begin_(s),
    end_(std::strlen(s))
  {
    assert(begin_ <= end_);
  }

  explicit string_ref(char const &c):
    begin_(std::addressof(c)),
    end_(std::next(std::addressof(c)))
  {
    assert(begin_ <= end_);
  }

  template <typename T, typename = safe_overload_t<string_ref, T>>
  explicit string_ref(T &&s):
    begin_(s.data()),
    end_(std::next(s.data(), s.size()))
  {
    assert(begin_ <= end_);
  }

  string_ref slice(size_type offset, size_type end) const {
    assert(offset <= size());
    assert(end <= size());
    return string_ref(std::next(begin_, offset), std::next(begin_, end));
  }

  void advance_to(const_iterator i) {
    assert(begin_ <= i);
    begin_ = i;
    assert(begin_ <= end_);
  }

  void limit(size_type size) {
    assert(size >= 0);

    if (size < this->size()) {
      end_ = std::next(begin_, size);
    }

    assert(begin_ <= end_);
  }

  char const *data() const { return begin_; }

  void clear() { begin_ = end_; }

  size_type size() const {
    assert(begin_ <= end_);
    return std::distance(begin_, end_);
  }

  bool empty() const {
    assert(begin_ <= end_);
    return begin_ == end_;
  }

  const_iterator cbegin() const { return begin_; }
  const_iterator begin() const { return begin_; }
  const_iterator cend() const { return end_; }
  const_iterator end() const { return end_; }

  string_ref &operator +=(size_type i) {
    assert(i <= size());
    std::advance(begin_, i);
    assert(begin_ <= end_);
    return *this;
  }

  string_ref &operator -=(size_type i) {
    begin_ -= i;
    assert(begin_ <= end_);
    return *this;
  }

  string_ref operator +(size_type i) const {
    auto copy(*this);
    copy += i;
    return copy;
  }

  string_ref operator -(size_type i) const {
    auto copy(*this);
    copy -= i;
    return copy;
  }

  string_ref &operator ++() {
    assert(begin_ < end_);
    std::advance(begin_, 1);
    assert(begin_ <= end_);
    return *this;
  }

  string_ref &operator --() {
    --begin_;
    assert(begin_ <= end_);
    return *this;
  }

  string_ref operator ++(int) {
    auto copy(*this);
    ++*this;
    return copy;
  }

  string_ref operator --(int) {
    auto copy(*this);
    --*this;
    return copy;
  }

  char operator *() const {
    assert(begin_ <= end_);
    return *begin_;
  }

  explicit operator bool() const { return begin_ < end_; }

  bool operator !() const { return empty(); }

private:
  const_iterator begin_;
  const_iterator end_;
};

} // namespace fatal {

#include <functional>

namespace std {

template <>
struct hash<fatal::string_ref> {
  using argument = fatal::string_ref;
  using result_type = std::size_t;

  result_type operator ()(fatal::string_ref s) const {
    return *fatal::bytes_hasher<result_type>()(s.data(), s.size());
  }
};

} // namespace std {

#endif // FATAL_INCLUDE_fatal_string_string_ref_h
