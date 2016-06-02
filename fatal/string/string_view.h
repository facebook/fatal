/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_string_view_h
#define FATAL_INCLUDE_fatal_string_string_view_h

#include <fatal/math/hash.h>
#include <fatal/type/call_traits.h>
#include <fatal/type/traits.h>

#include <algorithm>
#include <iterator>

#include <cassert>
#include <cstring>

namespace fatal {
namespace detail {

template <typename U>
using convertible = std::integral_constant<
  bool,
  std::is_pointer<U>::value
    || std::is_array<U>::value
    || std::is_integral<typename std::decay<U>::type>::value
    || (
      call_traits::data::member_function::supports<U>::value
        && call_traits::size::member_function::supports<U>::value
    )
>;

template <typename U>
using convertible_t = typename std::enable_if<convertible<U>::value>::type;

template <typename T, typename U>
using safe_overload_t = typename std::enable_if<
  is_safe_overload<T, U>::value && convertible<U>::value
>::type;

} // namespace detail {

struct string_view {
  using value_type = char;
  using size_type = std::size_t;
  using const_iterator = char const *;

  constexpr string_view(): begin_(nullptr), end_(nullptr) {}

  string_view(const_iterator begin, const_iterator end):
    begin_(begin),
    end_(end)
  {
    assert(begin_ <= end_);
  }

  string_view(const_iterator s, std::size_t size):
    begin_(s),
    end_(std::next(s, size))
  {
    assert(begin_ <= end_);
  }

  explicit string_view(value_type *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  explicit string_view(value_type const *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  template <std::size_t N>
  explicit string_view(value_type const (&s)[N]):
    begin_(s),
    end_(std::strlen(s))
  {
    assert(begin_ <= end_);
  }

  explicit string_view(value_type const &c):
    begin_(std::addressof(c)),
    end_(std::next(std::addressof(c)))
  {
    assert(begin_ <= end_);
  }

  template <typename U, typename = safe_overload_t<string_view, U>>
  explicit string_view(U &&s):
    begin_(s.data()),
    end_(std::next(s.data(), s.size()))
  {
    assert(begin_ <= end_);
  }

  string_view slice(size_type offset, size_type end) const {
    assert(offset <= size());
    assert(end <= size());
    return string_view(std::next(begin_, offset), std::next(begin_, end));
  }

  const_iterator find(value_type needle) const {
    return find(needle, begin_);
  }

  const_iterator find(value_type needle, const_iterator offset) const {
    assert(begin_ <= offset);
    assert(offset <= end_);
    return std::find(offset, end_, needle);
  }

  template <typename U>
  string_view split_step(U &&delimiter) {
    string_view result(begin_, find(std::forward<U>(delimiter)));
    begin_ = result.end() == end_ ? end_ : std::next(result.end());
    assert(begin_ <= end_);
    return result;
  }

  void reset(const_iterator begin) {
    assert(begin_ <= begin);
    begin_ = begin;
    assert(begin_ <= end_);
  }

  void reset(const_iterator begin, const_iterator end) {
    begin_ = begin;
    end_ = end;
    assert(begin_ <= end_);
  }

  void reset(const_iterator begin, size_type size) {
    begin_ = begin;
    end_ = std::next(begin, size);
    assert(begin_ <= end_);
  }

  void limit(size_type size) {
    assert(size >= 0);

    if (size < this->size()) {
      end_ = std::next(begin_, size);
    }

    assert(begin_ <= end_);
  }

  const_iterator data() const { return begin_; }

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

  // TODO: DOCUMENT AND TEST
  template <typename T>
  T to() const {
    return T(data(), size());
  }

  string_view &operator +=(size_type i) {
    assert(i <= size());
    std::advance(begin_, i);
    assert(begin_ <= end_);
    return *this;
  }

  string_view &operator -=(size_type i) {
    begin_ -= i;
    assert(begin_ <= end_);
    return *this;
  }

  string_view operator +(size_type i) const {
    auto copy(*this);
    copy += i;
    return copy;
  }

  string_view operator -(size_type i) const {
    auto copy(*this);
    copy -= i;
    return copy;
  }

  string_view &operator ++() {
    assert(begin_ < end_);
    std::advance(begin_, 1);
    assert(begin_ <= end_);
    return *this;
  }

  string_view &operator --() {
    --begin_;
    assert(begin_ <= end_);
    return *this;
  }

  string_view operator ++(int) {
    auto copy(*this);
    ++*this;
    return copy;
  }

  string_view operator --(int) {
    auto copy(*this);
    --*this;
    return copy;
  }

  value_type operator *() const {
    assert(begin_ <= end_);
    return *begin_;
  }

  bool operator ==(string_view rhs) const {
    return size() == rhs.size() && !std::strncmp(begin_, rhs.begin_, size());
  }

  template <
    typename U,
    typename = detail::safe_overload_t<
      string_view,
      typename std::decay<U>::type
    >
  >
  bool operator ==(U &&rhs) const {
    return *this == string_view(std::forward<U>(rhs));
  }

  template <
    typename U,
    typename = detail::safe_overload_t<
      string_view,
      typename std::decay<U>::type
    >
  >
  bool operator !=(U &&rhs) const { return !(*this == std::forward<U>(rhs)); }

  bool operator <(string_view rhs) const {
    auto const result = std::strncmp(
      begin_, rhs.begin_, std::min(size(), rhs.size())
    );

    return result < 0 || (!result && size() < rhs.size());
  }

  template <
    typename U,
    typename = detail::safe_overload_t<
      string_view,
      typename std::decay<U>::type
    >
  >
  bool operator <(U &&rhs) const {
    return *this < string_view(std::forward<U>(rhs));
  }

  bool operator >(string_view rhs) const { return rhs < *this; }

  template <
    typename U,
    typename = detail::safe_overload_t<
      string_view,
      typename std::decay<U>::type
    >
  >
  bool operator >(U &&rhs) const {
    return *this > string_view(std::forward<U>(rhs));
  }

  explicit operator bool() const { return begin_ < end_; }

  bool operator !() const { return empty(); }

  struct hasher {
    using argument = string_view;
    using result_type = std::size_t;

    result_type operator ()(string_view s) const {
      return *bytes_hasher<result_type>()(s.data(), s.size());
    }
  };

private:
  const_iterator begin_;
  const_iterator end_;
};

// TODO: DOCUMENT, TEST AND BIKE-SHED
template <typename String>
struct string_view_from_type {
  string_view operator ()() const {
    return string_view(String::data(), String::size);
  }
};

/////////////////
// operator == //
/////////////////

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator ==(T const &lhs, string_view rhs) { return rhs == lhs; }

////////////////
// operator < //
////////////////

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator <(T const &lhs, string_view rhs) { return rhs > lhs; }

////////////////
// operator > //
////////////////

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator >(T const &lhs, string_view rhs) { return rhs < lhs; }

/////////////////
// operator != //
/////////////////

template <typename T, typename = detail::convertible_t<T>>
bool operator !=(string_view lhs, T const &rhs) { return !(lhs == rhs); }

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator !=(T const &lhs, string_view rhs) { return !(rhs == lhs); }

/////////////////
// operator <= //
/////////////////

template <typename T, typename = detail::convertible_t<T>>
bool operator <=(string_view lhs, T const &rhs) { return !(lhs > rhs); }

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator <=(T const &lhs, string_view rhs) { return !(rhs < lhs); }

/////////////////
// operator >= //
/////////////////

template <typename T, typename = detail::convertible_t<T>>
bool operator >=(string_view lhs, T const &rhs) { return !(lhs < rhs); }

template <typename T, typename = detail::safe_overload_t<string_view, T>>
bool operator >=(T const &lhs, string_view rhs) { return !(rhs > lhs); }

/////////////////////////////////////
// operator <<(std::basic_ostream) //
/////////////////////////////////////

template <typename C, typename T>
std::ostream &operator <<(std::basic_ostream<C, T> &out, string_view rhs) {
  return out.write(rhs.data(), rhs.size());
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_string_string_view_h
