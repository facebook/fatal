/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_ephemeral_rope_h
#define FATAL_INCLUDE_fatal_string_ephemeral_rope_h

#include <fatal/container/uninitialized.h>
#include <fatal/math/hash.h>
#include <fatal/string/string_ref.h>
#include <fatal/type/map.h>
#include <fatal/type/tag.h>
#include <fatal/type/traits.h>

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <cassert>

namespace fatal {
namespace detail {
namespace ephemeral_rope_impl {

/////////////
// variant //
/////////////

// optimized for ephemeral_rope
template <typename TData>
class variant {
  enum class id_type: unsigned char { string, reference, character };

public:
  using size_type = std::size_t;
  using payload_type = TData;

  template <typename T>
  using id = typename build_type_map<
    std::string, std::integral_constant<id_type, id_type::string>,
    string_ref, std::integral_constant<id_type, id_type::reference>,
    char, std::integral_constant<id_type, id_type::character>
  >::template get<T>;

  variant(variant const &rhs) = delete;

  variant(variant &&rhs)
    noexcept(noexcept(std::string(std::declval<std::string &&>()))):
    payload_(std::move(rhs.payload_)),
    which_(rhs.which_)
  {
    switch (which_) {
      case id_type::string:
        new (std::addressof(value_.s)) std::string(std::move(rhs.value_.s));
        break;

      case id_type::reference:
        value_.ref = rhs.value_.ref;
        break;

      case id_type::character:
        value_.c = rhs.value_.c;
        break;
    }
  }

  template <typename... Args>
  explicit variant(std::string &&s, Args &&...args):
    payload_(std::forward<Args>(args)...),
    value_(std::move(s)),
    which_(id_type::string)
  {}

  template <typename... Args>
  explicit variant(string_ref s, Args &&...args):
    payload_(std::forward<Args>(args)...),
    value_(s),
    which_(id_type::reference)
  {}

  template <typename... Args>
  explicit variant(char c, Args &&...args):
    payload_(std::forward<Args>(args)...),
    value_(c),
    which_(id_type::character)
  {}

  ~variant() {
    switch (which_) {
      case id_type::string:
        using string_t = std::string;
        value_.s.~string_t();
        break;

      default:
        break;
    }
  }

  template <typename T> bool is() const { return which_ == id<T>::value; }

  template <typename T>
  T const &get() const {
    assert(which_ == id<T>::value);
    return get(type_tag<T>());
  }

  template <typename T>
  T const *try_get() const {
    return which_ == id<T>::value ? try_get(type_tag<T>()) : nullptr;
  }

  string_ref ref() const {
    switch (which_) {
      case id_type::string:
        return string_ref(value_.s);

      case id_type::reference:
        break;

      case id_type::character:
        return string_ref(value_.c);
    }

    assert(which_ == id_type::reference);
    return value_.ref;
  }

  char const *data() const {
    switch (which_) {
      case id_type::string:
        return value_.s.data();

      case id_type::reference:
        break;

      case id_type::character:
        return std::addressof(value_.c);
    }

    assert(which_ == id_type::reference);
    return value_.ref.data();
  }

  size_type size() const {
    switch (which_) {
      case id_type::string:
        return value_.s.size();

      case id_type::reference:
        break;

      case id_type::character:
        return 1;
    }

    assert(which_ == id_type::reference);
    return value_.ref.size();
  }

  bool empty() const {
    switch (which_) {
      case id_type::string:
        return value_.s.empty();

      case id_type::reference:
        break;

      case id_type::character:
        return false;
    }

    assert(which_ == id_type::reference);
    return value_.ref.empty();
  }

  id_type type() const { return which_; }

  fast_pass<payload_type> payload() const { return payload_; }

private:
  union union_t {
    union_t() {}
    union_t(union_t const &) = delete;
    union_t(union_t &&) = delete;

    explicit union_t(std::string &&s): s(std::move(s)) {}
    explicit union_t(string_ref s): ref(s) {}
    explicit union_t(char c): c(c) {}

    ~union_t() {}

    std::string s;
    string_ref ref;
    char c;
  };

  std::string const &get(type_tag<std::string>) const { return value_.s; }
  std::string const *try_get(type_tag<std::string>) const {
    return std::addressof(value_.s);
  }

  string_ref const &get(type_tag<string_ref>) const { return value_.ref; }
  string_ref const *try_get(type_tag<string_ref>) const {
    return std::addressof(value_.ref);
  }

  char const &get(type_tag<char>) const { return value_.c; }
  char const *try_get(type_tag<char>) const { return std::addressof(value_.c); }

  payload_type payload_;
  union_t value_;
  id_type which_;
};

////////////
// vector //
////////////

// optimized for ephemeral_rope
template <typename T, std::size_t SmallBufferSize = 8>
struct vector {
  using value_type = T;
  using const_reference = value_type const &;
  using rvalue_reference = value_type &&;
  using const_pointer = value_type const *;
  using size_type = std::size_t;
  using small_buffer_size = std::integral_constant<size_type, SmallBufferSize>;

  vector() = default;
  vector(vector const &) = delete;
  vector(vector &&rhs):
    size_(std::move(rhs.size_)),
    buffer_(std::move(rhs.buffer_))
  {
    for (auto i = std::min(size_, rhs.small_.size()); i--; ) {
      small_[i].steal(std::move(rhs.small_[i]));
    }
  }

  ~vector() { clear(); }

  template <typename... Args>
  void emplace_back(Args &&...args) {
    if (size_ < small_.size()) {
      small_[size_].construct(std::forward<Args>(args)...);
    } else {
      if (size_ == small_.size()) {
        buffer_.reserve(small_.size());
      }
      buffer_.emplace_back(std::forward<Args>(args)...);
    }

    ++size_;
  }

  void clear() {
    buffer_.clear();

    for (auto i = std::min(size_, small_.size()); i--; ) {
      small_[i].destroy();
    }

    size_ = 0;
  }

  void reserve(size_type size) {
    if (size > small_.size()) {
      buffer_.reserve(size - small_.size());
    }
  }

  size_type capacity() const { return small_.size() + buffer_.capacity(); }

  size_type size() const { return size_; }

  bool empty() const { return !size_; }

  const_reference operator [](size_type i) const {
    assert(i < size_);
    return i < small_.size()
      ? *small_[i]
      : buffer_[i - small_.size()];
  }

  rvalue_reference move(size_type i) {
    assert(i < size_);
    if (i < small_.size()) {
      return std::move(*small_[i]);
    }

    return std::move(buffer_[i - small_.size()]);
  }

private:
  size_type size_ = 0;
  std::array<uninitialized<value_type, false>, small_buffer_size::value> small_;
  // TODO: use something other than std::vector ??
  std::vector<value_type> buffer_;
};

} // namespace ephemeral_rope_impl {
} // namespace detail {

////////////////////
// ephemeral_rope //
////////////////////

template <std::size_t SmallBufferSize = 8>
struct ephemeral_rope {
  // TODO: REPLACE char FOR value_type EVERYWHERE
  using value_type = char;
  using size_type = std::size_t;
  using difference_type = typename std::make_signed<size_type>::type;

private:
  using piece_type = detail::ephemeral_rope_impl::variant<size_type>;
  using container_type = detail::ephemeral_rope_impl::vector<
    piece_type, SmallBufferSize
  >;
  using small_buffer_size = typename container_type::small_buffer_size;

public:
  ephemeral_rope() = default;
  ephemeral_rope(ephemeral_rope const &) = delete;
  ephemeral_rope(ephemeral_rope &&) = default;

  template <
    typename... Args,
    typename = safe_overload_t<ephemeral_rope, Args...>
  >
  explicit ephemeral_rope(Args &&...args) {
    multi_append(std::forward<Args>(args)...);
  }

  using piece_index = typename container_type::size_type;

  ////////////////////
  // const_iterator //
  ////////////////////

  struct const_iterator:
    public std::iterator<
      std::bidirectional_iterator_tag,
      value_type,
      difference_type
    >
  {
    explicit const_iterator(
      ephemeral_rope const *rope,
      piece_type const *piece,
      piece_index index,
      size_type offset
    ):
      rope_(rope),
      piece_(piece),
      index_(index),
      offset_(offset)
    {
      assert(!piece_ || offset_ < piece_->size());
    }

    piece_type const *piece() const { return piece_; }
    piece_index index() const { return index_; }
    size_type offset() const { return offset_; }

    size_type absolute() const {
      return offset_ + (piece_ ? piece_->payload() : rope_->size());
    }

    string_ref ref() const {
      assert(piece_);
      return piece_->ref() + offset_;
    }

    char operator *() const {
      assert(piece_);
      assert(offset_ < piece_->size());
      return piece_->data()[offset_];
    }

    size_type operator +(size_type rhs) const { return absolute() + rhs; }
    size_type operator -(size_type rhs) const { return absolute() - rhs; }

    const_iterator &operator ++() {
      assert(piece_);
      assert(offset_ < piece_->size());

      if (++offset_ == piece_->size()) {
        assert(index_ < rope_->pieces_.size());
        piece_ = ++index_ < rope_->pieces_.size()
          ? std::addressof(rope_->pieces_[index_])
          : nullptr;
        offset_ = 0;
      }

      return *this;
    }

    const_iterator &operator --() {
      if (offset_) {
        assert(offset_ > 0);
        --offset_;
      } else {
        assert(index_ >= 0);
        assert(index_ <= rope_->pieces_.size());
        if (index_) {
          piece_ = std::addressof(rope_->pieces_[--index_]);
          assert(piece_);
          assert(piece_->size() > 0);
          offset_ = piece_->size() - 1;
        } else {
          piece_ = nullptr;
        }
      }

      return *this;
    }

    const_iterator operator ++(int) {
      auto copy(*this);
      ++*this;
      return copy;
    }
    const_iterator operator --(int) {
      auto copy(*this);
      --*this;
      return copy;
    }

    const_iterator &operator +=(size_type rhs) {
      assert(piece_);
      assert(offset_ < piece_->size());

      if (rhs < piece_->size() - offset_) {
        offset_ += rhs;
        assert(offset_ < piece_->size());
      } else {
        *this = rope_->pinpoint(piece_->payload() + offset_ + rhs);
      }

      return *this;
    }

    const_iterator &operator -=(size_type rhs) {
      if (!piece_) {
        assert(rhs <= rope_->size_ + offset_);
        *this = rope_->pinpoint(rope_->size_ + offset_ - rhs);
      } else if (rhs <= offset_) {
        assert(offset_ < piece_->size());
        offset_ -= rhs;
      } else {
        assert(rhs < piece_->payload() + offset_);
        assert(offset_ < piece_->size());
        *this = rope_->pinpoint(piece_->payload() + offset_ - rhs);
      }

      return *this;
    }

    bool operator ==(size_type rhs) const { return rhs == absolute(); }
    bool operator !=(size_type rhs) const { return rhs != absolute(); }
    bool operator <(size_type rhs) const { return rhs < absolute(); }
    bool operator <=(size_type rhs) const { return rhs <= absolute(); }
    bool operator >(size_type rhs) const { return rhs > absolute(); }
    bool operator >=(size_type rhs) const { return rhs >= absolute(); }

    bool operator ==(const_iterator const &rhs) const {
      return offset_ == rhs.offset_
        && index_ == rhs.index_
        && piece_ == rhs.piece_
        && rope_ == rhs.rope_;
    }

    bool operator !=(const_iterator const &rhs) const {
      return !(*this == rhs);
    }

    bool operator <(const_iterator const &rhs) const {
      assert(rope_ == rhs.rope_);
      assert(piece_ == rhs.piece_ || index_ != rhs.index_);
      return index_ < rhs.index_
        || (index_ == rhs.index_ && offset_ < rhs.offset_);
    }

    bool operator <=(const_iterator const &rhs) const { return !(rhs < *this); }
    bool operator >(const_iterator const &rhs) const { return rhs < *this; }
    bool operator >=(const_iterator const &rhs) const { return !(*this < rhs); }

    explicit operator size_type() const { return absolute(); }

  private:
    ephemeral_rope const *rope_;

    // piece
    piece_type const *piece_;

    // piece index
    piece_index index_;

    // relative offset
    size_type offset_;
  };

  // TODO: add a remark saying that iterating over every character using
  // const_iterator is very inneficient. One should use piece()/pieces()
  // instead.

  ///////////////
  // iterators //
  ///////////////

  const_iterator cbegin() const {
    return const_iterator(
      this, pieces_.empty() ? nullptr : std::addressof(pieces_[0]), 0, 0
    );
  }

  const_iterator begin() const { return cbegin(); }

  const_iterator cend() const {
    return const_iterator(this, nullptr, pieces_.size(), 0);
  }

  const_iterator end() const { return cend(); }

  ///////////
  // mimic //
  ///////////

  // TODO: ADD REMARKS ON WHY NOT COPY CONSTRUCTIBLE
  // TODO: BIKE-SHED
  ephemeral_rope mimic() const {
    ephemeral_rope result;

    auto const pieces = pieces_.size();

    result.reserve(pieces, true);

    for (piece_index i = 0; i < pieces; ++i) {
      result.append(pieces_[i].ref());
    }

    return result;
  }

  ///////////////
  // push_back //
  ///////////////

  void push_back(char c) {
    pieces_.emplace_back(c, size_);
    ++size_;
  }

  ////////////
  // append //
  ////////////

  void append(std::string &&s) {
    auto const size = s.size();

    if (!size) {
      return;
    }

    pieces_.emplace_back(std::move(s), size_);
    size_ += size;
  }

  void append(string_ref s) {
    auto const size = s.size();

    if (!size) {
      return;
    }

    pieces_.emplace_back(s, size_);
    size_ += size;
  }

  void append(char c) {
    push_back(c);
  }

  template <
    typename... Args,
    typename = safe_overload_t<ephemeral_rope, Args...>
  >
  void append(Args &&...args) {
    auto s = string_ref(std::forward<Args>(args)...);
    auto const size = s.size();

    if (!size) {
      return;
    }

    pieces_.emplace_back(s, size_);
    size_ += size;
  }

  //////////////////
  // multi_append //
  //////////////////

  template <typename... Args>
  void multi_append(Args &&...args) {
    reserve(sizeof...(args), true);
    multi_append_impl(std::forward<Args>(args)...);
  }

  ////////////
  // concat //
  ////////////

  void concat(ephemeral_rope const &rhs) {
    auto const pieces = rhs.pieces_.size();

    reserve(pieces, true);

    for (piece_index i = 0; i < pieces; ++i) {
      append(rhs.pieces_[i].ref());
    }
  }

  void concat(ephemeral_rope &&rhs) {
    auto const pieces = rhs.pieces_.size();

    if (this == std::addressof(rhs)) {
      throw std::invalid_argument(
        "cannot concatenate r-value reference to self"
      );
    }

    reserve(pieces, true);

    for (piece_index i = 0; i < pieces; ++i) {
      pieces_.emplace_back(rhs.pieces_.move(i));
    }

    size_ += rhs.size_;

    rhs.clear();
  }

  ///////////////
  // accessors //
  ///////////////

  char front() const {
    assert(!pieces_.empty());
    return *pieces_[0].data();
  }

  char back() const {
    assert(!pieces_.empty());
    auto const &last = pieces_[pieces_.size() - 1];

    assert(last.size());
    return last.data()[last.size() - 1];
  }

  char at(size_type i) const {
    auto offset = pinpoint(i);

    if (!offset.piece()) {
      throw std::out_of_range("at(): index out of bounds");
    }

    return *offset;
  }

  char operator [](size_type i) const {
    assert(i < size_);
    return *pinpoint(i);
  }

  string_ref piece(piece_index i) const {
    assert(i < pieces_.size());
    return pieces_[i].ref();
  }

  piece_index pieces() const {
    assert(size_ || pieces_.empty());
    return pieces_.size();
  }

  //////////
  // copy //
  //////////

  /**
   * Copies as much as possible of this rope's contents to the output
   * range `[begin, end)`.
   *
   * Returns the pointer `e`, which is one element past the last one
   * written, such that the written range lies within `[begin, e)`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  char *copy(char *begin, char *end) const {
    for (piece_index i = 0, pieces = pieces_.size();
      i < pieces && begin < end;
      ++i
    ) {
      auto piece = pieces_[i].ref();

      piece.limit(std::distance(begin, end));
      assert(piece);

      auto const e = std::copy(piece.begin(), piece.end(), begin);
      assert(
        e == end
          || static_cast<size_type>(std::distance(begin, e)) == piece.size()
      );

      begin = e;
      assert(begin <= end);
    }

    return begin;
  }

  /**
   * Copies as much as possible of this rope's contents, starting at
   * `offset`, to the output range `[begin, end)`.
   *
   * Returns the pointer `e`, which is one element past the last one
   * written, such that the written range lies within `[begin, e)`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  char *copy(char *begin, char *end, size_type offset) const {
    return copy(begin, end, pinpoint(offset));
  }

  /**
   * Copies as much as possible of this rope's contents, starting at
   * `offset`, to the output range `[begin, end)`.
   *
   * Returns the pointer `e`, which is one element past the last one
   * written, such that the written range lies within `[begin, e)`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  char *copy(char *begin, char *end, const_iterator offset) const {
    if (offset == cend() || begin == end) {
      return begin;
    }

    auto const pieces = pieces_.size();
    auto i = offset.index();
    assert(i < pieces);
    auto piece = offset.ref();

    for (;;) {
      assert(begin < end);
      piece.limit(std::distance(begin, end));
      assert(piece);

      auto const e = std::copy(piece.begin(), piece.end(), begin);

      if (e == end) {
        return end;
      }

      assert(std::distance(begin, e) == piece.size());
      begin = e;
      assert(begin <= end);

      if (++i < pieces) {
        piece = pieces_[i].ref();
      } else {
        break;
      }
    };

    return begin;
  }

  ////////////
  // string //
  ////////////

  template <
    typename Traits = std::char_traits<char>,
    typename Allocator = std::allocator<char>
  >
  std::basic_string<char, Traits, Allocator> to_string(
    Allocator const &allocator = Allocator()
  ) const {
    std::string s(allocator);
    append_to(s);
    return s;
  }

  template <typename String>
  String &append_to(String &out) const {
    out.reserve(out.size() + size_);

    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      auto piece = pieces_[i].ref();
      out.append(piece.begin(), piece.end());
    }

    return out;
  }

  /////////////
  // reserve //
  /////////////

  void reserve(piece_index pieces, bool additional = false) {
    pieces_.reserve(additional ? pieces_.size() + pieces : pieces);
  }

  //////////////
  // capacity //
  //////////////

  size_type capacity() const { return pieces_.capacity(); }

  //////////
  // size //
  //////////

  size_type size() const { return size_; }

  ///////////
  // empty //
  ///////////

  bool empty() const { return !size_; }

  ///////////
  // clear //
  ///////////

  void clear() {
    pieces_.clear();
    size_ = 0;
  }

  /////////////
  // compare //
  /////////////

  int compare(char const *rhs) const {
    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      if (!*rhs) {
        return 1;
      }

      auto piece = pieces_[i].ref();
      assert(piece);

      // TODO: optimize this, either with a duff's device or some function
      for (; *rhs && piece; ++rhs, ++piece) {
        if (auto const result = static_cast<int>(*piece) - *rhs) {
          return result;
        }
      }

      if (piece) {
        assert(!*rhs);
        return 1;
      }
    }

    return -static_cast<bool>(*rhs);
  }

  int compare(string_ref rhs) const {
    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      if (!rhs) {
        return 1;
      }

      auto piece = pieces_[i].ref();
      assert(piece);

      auto const length = std::min(rhs.size(), piece.size());
      assert(length > 0);

      if (auto const result = std::strncmp(piece.data(), rhs.data(), length)) {
        return result;
      }

      if (length == rhs.size()) {
        return length != piece.size();
      }

      assert(length == piece.size());
      rhs += length;
    }

    return -!rhs.empty();
  }

  int compare(ephemeral_rope const &rhs) const {
    if (!size_) {
      return rhs.size_ ? -1 : 0;
    }

    if (!rhs.size_) {
      return 1;
    }

    assert(!pieces_.empty());
    assert(!rhs.pieces_.empty());

    auto const lpieces = pieces_.size();
    auto const rpieces = rhs.pieces_.size();
    piece_index lpiece = 0;
    piece_index rpiece = 0;

    for (auto left = pieces_[0].ref(), right = rhs.pieces_[0].ref();;) {
      assert(!left.empty());
      assert(!right.empty());

      auto const length = std::min(left.size(), right.size());
      assert(length > 0);

      if (auto const result = std::strncmp(left.data(), right.data(), length)) {
        return result;
      }

      left += length;
      if (!left) {
        if (++lpiece == lpieces) {
          return -(right + length || rpiece + 1 < rpieces);
        }

        left = pieces_[lpiece].ref();
      }

      right += length;
      if (!right) {
        if (++rpiece == rpieces) {
          break;
        }

        right = rhs.pieces_[rpiece].ref();
      }
    }

    return 1;
  }

  template <typename T, typename = safe_overload_t<ephemeral_rope, T>>
  int compare(T &&rhs) const {
    return compare(string_ref(std::forward<T>(rhs)));
  }

  //////////
  // find //
  //////////

  const_iterator find(char c) const {
    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      auto &piece = pieces_[i];
      auto const ref = piece.ref();

      auto const j = std::find(ref.begin(), ref.end(), c);

      if (j != ref.end()) {
        return const_iterator(
          this, std::addressof(piece), i, std::distance(ref.begin(), j)
        );
      }
    }

    return cend();
  }

  const_iterator find(char c, size_type offset) const {
    return find(c, pinpoint(offset));
  }

  const_iterator find(char c, const_iterator offset) const {
    if (offset == cend()) {
      return offset;
    }

    auto const pieces = pieces_.size();
    auto piece_offset = offset.offset();

    for (auto i = offset.index(); i < pieces; ++i, piece_offset = 0) {
      auto const &piece = pieces_[i];
      auto const ref = piece.ref();
      auto const j = std::find(
        std::next(ref.begin(), piece_offset),
        ref.end(),
        c
      );

      if (j != ref.end()) {
        assert(i < pieces);
        return const_iterator(
          this, std::addressof(pieces_[i]), i, std::distance(ref.begin(), j)
        );
      }
    };

    return cend();
  }

/* TODO: IMPLEMENT
  const_iterator find(string_ref s) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find(string_ref s, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find(string_ref s, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
  */

  ///////////
  // rfind //
  ///////////

/*
  const_iterator rfind(string_ref s) const {
    return rfind(s, size_);
  }

  const_iterator rfind(string_ref s, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator rfind(string_ref s, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator rfind(char c) const {
    return rfind(c, size_);
  }

  const_iterator rfind(char c, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator rfind(char c, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
*/

  ///////////////////
  // find_first_of //
  ///////////////////

/*
  const_iterator find_first_of(string_ref s, size_type offset = 0) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_first_of(string_ref s, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
*/

  ///////////////////////
  // find_first_not_of //
  ///////////////////////

/*
  const_iterator find_first_not_of(string_ref s, size_type offset = 0) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_first_not_of(string_ref s, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
  const_iterator find_first_not_of(char c, size_type offset = 0) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_first_not_of(char c, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
  */

  //////////////////
  // find_last_of //
  //////////////////

/*
  const_iterator find_last_of(string_ref s) const {
    return find_last_of(s, size_);
  }

  const_iterator find_last_of(string_ref s, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_last_of(string_ref s, size_type const_iterator) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
  */

  //////////////////////
  // find_last_not_of //
  //////////////////////

/*
  const_iterator find_last_not_of(string_ref s) const {
    return find_last_not_of(s, size_);
  }

  const_iterator find_last_not_of(string_ref s, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_last_not_of(string_ref s, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
  const_iterator find_last_not_of(char c) const {
    return find_last_not_of(c, size_);
  }

  const_iterator find_last_not_of(char c, size_type offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }

  const_iterator find_last_not_of(char c, const_iterator offset) const {
    // TODO: implement
    throw std::runtime_error("not implemented");
  }
*/

  /////////////////
  // operator == //
  /////////////////

  bool operator ==(char const *rhs) const {
    //* TODO: REMOVE WHEN compare() GETS OPTIMIZED
    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      if (!*rhs) {
        return false;
      }

      auto piece = pieces_[i].ref();
      assert(piece);

      if (std::strncmp(rhs, piece.data(), piece.size())) {
        return false;
      }

      assert(piece.size() <= std::strlen(rhs));
      std::advance(rhs, piece.size());
    }

    return !*rhs;
    /*/
    return !compare(rhs);
    //*/
  }

  template <std::size_t Size>
  bool operator ==(char const (&rhs)[Size]) const {
    return size_ < Size
      ?  (*this == static_cast<char const *>(rhs))
      : false;
  }

  bool operator ==(string_ref rhs) const {
    return size_ == rhs.size() && !compare(rhs);
  }

  bool operator ==(ephemeral_rope const &rhs) const {
    if (size_ != rhs.size_) {
      return false;
    }

    return !compare(rhs);
  }

  template <typename T, typename = safe_overload_t<ephemeral_rope, T>>
  bool operator ==(T &&rhs) const {
    return *this == string_ref(std::forward<T>(rhs));
  }

  ////////////////
  // operator < //
  ////////////////

  template <typename T>
  bool operator <(T &&rhs) const {
    return compare(std::forward<T>(rhs)) < 0;
  }

  ////////////////
  // operator > //
  ////////////////

  template <typename T>
  bool operator >(T &&rhs) const {
    return compare(std::forward<T>(rhs)) > 0;
  }

private:
  ////////////////////////////
  // IMPLEMENTATION DETAILS //
  ////////////////////////////

  const_iterator make_iterator(piece_index index, size_type offset) const {
    return const_iterator(this, std::addressof(pieces_[index]), index, offset);
  }

  const_iterator pinpoint(size_type i) const {
    auto const pieces = pieces_.size();

    if (i < size_) {
      piece_index begin = 0;
      auto end = pieces;

      while ((end - begin) > 1) {
        auto const pivot = begin + ((end - begin) / 2);
        auto const &piece = pieces_[pivot];

        assert(pivot >= begin);
        assert(pivot < end);

        auto const offset = piece.payload();

        if (i < offset) {
          end = pivot;
        } else if (offset < i) {
          begin = pivot;
        } else {
          assert(i >= offset);
          return const_iterator(this, std::addressof(piece), pivot, i - offset);
        }
      }

      if (begin != end) {
        assert(begin + 1 == end);
        auto const &piece = pieces_[begin];
        auto const offset = piece.payload();

        if (i >= offset) {
          return const_iterator(this, std::addressof(piece), begin, i - offset);
        }
      }
    }

    return const_iterator(this, nullptr, pieces, i - size_);
  }

  template <typename T, typename... Args>
  void multi_append_impl(T &&s, Args &&...args) {
    append(std::forward<T>(s));
    multi_append_impl(std::forward<Args>(args)...);
  }

  void multi_append_impl() {}

  container_type pieces_;
  size_type size_ = 0;
};

/////////////////
// operator == //
/////////////////

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator ==(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return rope == other;
}

////////////////
// operator < //
////////////////

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator <(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return rope > other;
}

////////////////
// operator > //
////////////////

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator >(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return rope < other;
}

/////////////////
// operator != //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator !=(ephemeral_rope<SmallBufferSize> const &rope, T const &other) {
  return !(rope == other);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator !=(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return !(rope == other);
}

/////////////////
// operator <= //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator <=(ephemeral_rope<SmallBufferSize> const &rope, T const &other) {
  return !(rope > other);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator <=(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return !(rope < other);
}

/////////////////
// operator >= //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator >=(ephemeral_rope<SmallBufferSize> const &rope, T const &other) {
  return !(rope < other);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload_t<ephemeral_rope<SmallBufferSize>, T>
>
bool operator >=(T const &other, ephemeral_rope<SmallBufferSize> const &rope) {
  return !(rope > other);
}

///////////////////////////////
// operator <<(std::ostream) //
///////////////////////////////

template <std::size_t SmallBufferSize>
std::ostream &operator <<(
  std::ostream &out,
  ephemeral_rope<SmallBufferSize> const &rope
) {
  using piece_index = typename ephemeral_rope<SmallBufferSize>::piece_index;

  for (piece_index i = 0, pieces = rope.pieces(); i < pieces; ++i) {
    auto piece = rope.piece(i);

    out.write(piece.data(), piece.size());
  }

  return out;
}

} // namespace fatal {

///////////////
// std::hash //
///////////////

namespace std {

template <std::size_t SmallBufferSize>
struct hash<fatal::ephemeral_rope<SmallBufferSize>> {
  using argument = fatal::ephemeral_rope<SmallBufferSize>;
  using result_type = std::size_t;

  result_type operator ()(
    fatal::ephemeral_rope<SmallBufferSize> const &rope
  ) const {
    fatal::bytes_hasher<result_type> hasher;

    using piece_index = typename argument::piece_index;

    for (piece_index i = 0, pieces = rope.pieces(); i < pieces; ++i) {
      auto piece = rope.piece(i);

      hasher(piece.begin(), piece.end());
    }

    return *hasher;
  }
};

} // namespace std {

#endif // FATAL_INCLUDE_fatal_string_ephemeral_rope_h
