/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_rope_h
#define FATAL_INCLUDE_fatal_string_rope_h

#include <fatal/container/uninitialized.h>
#include <fatal/math/hash.h>
#include <fatal/portability.h>
#include <fatal/string/string_view.h>
#include <fatal/type/get.h>
#include <fatal/type/list.h>
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

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

namespace fatal {
namespace detail {
namespace rope_impl {

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

// optimized for rope
template <typename TData>
class variant {
  enum class id_type: unsigned char { string, reference, character };

public:
  using size_type = std::size_t;
  using payload_type = TData;

  template <id_type V>
  using id_constant = std::integral_constant<id_type, V>;

  template <typename T>
  using id = fatal::pair_get<
    fatal::list<
      fatal::pair<std::string, id_constant<id_type::string>>,
      fatal::pair<string_view, id_constant<id_type::reference>>,
      fatal::pair<char, id_constant<id_type::character>>
    >,
    T
  >;

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
  explicit variant(string_view s, Args &&...args):
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
    return get(tag<T>());
  }

  template <typename T>
  T const *try_get() const {
    return which_ == id<T>::value ? try_get(tag<T>()) : nullptr;
  }

  string_view ref() const {
    switch (which_) {
      case id_type::string:
        return string_view(value_.s);

      case id_type::reference:
        break;

      case id_type::character:
        return string_view(value_.c);
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

    explicit union_t(std::string &&s_): s(std::move(s_)) {}
    explicit union_t(string_view s_): ref(s_) {}
    explicit union_t(char c_): c(c_) {}

    ~union_t() {}

    std::string s;
    string_view ref;
    char c;
  };

  std::string const &get(tag<std::string>) const { return value_.s; }
  std::string const *try_get(tag<std::string>) const {
    return std::addressof(value_.s);
  }

  string_view const &get(tag<string_view>) const { return value_.ref; }
  string_view const *try_get(tag<string_view>) const {
    return std::addressof(value_.ref);
  }

  char const &get(tag<char>) const { return value_.c; }
  char const *try_get(tag<char>) const { return std::addressof(value_.c); }

  payload_type payload_;
  union_t value_;
  id_type which_;
};

// optimized for rope
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

} // namespace rope_impl {
} // namespace detail {

//////////
// rope //
//////////

/**
 * A class used to represent a rope, that is, a sequence of string pieces
 * that together represent a string.
 *
 * Provides an interface very similar to that of `std::string`.
 *
 * This is an alternative to regular strings for append-heavy use-cases
 * since appending to a rope doesn't incur copying the contents of the
 * string piece being appended.
 *
 * The downside of this optimization is that some types of pieces are
 * not owned, but rather referenced, by the rope. This means that such
 * pieces must outlive the rope instance for the latter to remain valid.
 *
 * There are three types of pieces that can be stored in a rope:
 *
 * 1. string_view: a reference to a portion of an existing string.
 *    Results from appending a string literal, an lvalue of type
 *    `std::string` or any container that stores elements of type
 *    `char` and provides the member functions `data()` and
 *    `size()`. Examples of such containers are `std::string`,
 *    `std::vector` and `std::array`.
 *
 * 2. std::string: stores a regular string as one piece of this rope.
 *    Results from appending a temporary / rvalue of type `std::string`.
 *    The contents of this piece are owned by the rope.
 *
 * 3. char: stores a single character. Results from appending
 *    a value of type `char`. This is suboptimal when compared
 *    to storing references to existing strings. Avoid calling
 *    append passing a single character as a parameter. The
 *    contents of this piece are owned by the rope.
 *
 * Example 1:
 *
 *  rope<> r;
 *
 *  // the string literal "hello" will be referenced by the rope
 *  r.append("hello");
 *
 *  // the character ',' will be owned by the rope
 *  r.append(',');
 *
 *  // the character ' ' will be owned by the rope
 *  r.push_back(' ');
 *
 *  // the string temporary "world!" will be owned by the rope
 *  r.append(std::string("world!"));
 *
 *  std::string s1(" this is");
 *  // the string `s1` will be referenced by the rope
 *  r.append(s1);
 *
 *  std::string s2(" a test.");
 *  // the contents of `s2` will be owned by the rope
 *  r.append(std::move(s2));
 *
 *  // prints "hello, world! this is a test."
 *  std::cout << r << std::endl;
 *
 * Example 2:
 *
 *  rope<> r;
 *
 *  // the string literal "hello" will be referenced by the rope
 *  r.append("hello");
 *
 *  // the character ',' will be owned by the rope
 *  // the character ' ' will be owned by the rope
 *  // the string temporary "world!" will be owned by the rope
 *  r.multi_append(',', ' ', std::string("world!"));
 *
 *  std::string s1(" this is");
 *  std::string s2(" a test.");
 *  // the string `s1` will be referenced by the rope
 *  // the contents of `s2` will be owned by the rope
 *  r.multi_append(s1, std::move(s2));
 *
 *  // prints "hello, world! this is a test."
 *  std::cout << r << std::endl;
 *
 * Example 3:
 *
 *  std::string s1(" this is");
 *  std::string s2(" a test.");
 *
 *  // the string literal "hello" will be referenced by the rope
 *  // the character ',' will be owned by the rope
 *  // the character ' ' will be owned by the rope
 *  // the string temporary "world!" will be owned by the rope
 *  // the string `s1` will be referenced by the rope
 *  // the contents of `s2` will be owned by the rope
 *
 *  rope<> r("hello", ',', ' ', std::string("world!"), s1, std::move(s2));
 *
 *  // prints "hello, world! this is a test."
 *  std::cout << r << std::endl;
 *
 * @author: Marcelo Juchem
 */

template <std::size_t SmallBufferSize = 8>
struct rope {
  // TODO: switch `char` piece with array+size, taking up the same space as the
  // other pieces

  // TODO: REPLACE char FOR value_type EVERYWHERE

  /**
   * The type used to represent the characters contained
   * in the string represented by this rope.
   *
   * @author: Marcelo Juchem
   */
  using value_type = char;

  /**
   * The type used to represent the number of characters
   * contained in the string represented by this rope.
   *
   * @author: Marcelo Juchem
   */
  using size_type = std::size_t;

  /**
   * The type used to represent the difference between two iterators.
   *
   * @author: Marcelo Juchem
   */
  using difference_type = typename std::make_signed<size_type>::type;

private:
  using piece_type = detail::rope_impl::variant<size_type>;
  using container_type = detail::rope_impl::vector<
    piece_type, SmallBufferSize
  >;
  using small_buffer_size = typename container_type::small_buffer_size;

public:
  /**
   * Default constructor. Constructs an empty rope.
   *
   * @author: Marcelo Juchem
   */
  rope() = default;

  /**
   * There is no copy constructor defined for this rope.
   *
   * The reason there's no copy constructor is to avoid confusion
   * with the semantics of copying a string. Since this rope class
   * doesn't own its pieces most of the time, a copy constructor
   * would silently allow ropes to outlive its pieces.
   *
   * Refer to the `mimic()` function for a way to obtain another
   * rope instance representing the same string.
   *
   * @author: Marcelo Juchem
   */
  rope(rope const &) = delete;

  /**
   * Move constructor. Commandeers the pieces contained in the given rope.
   *
   * @author: Marcelo Juchem
   */
  rope(rope &&) = default;

  /**
   * Constructs a rope ouf of the given pieces.
   *
   * This is equivalent to constructing an empty rope using
   * the default constructor, then calling `append()` on
   * each piece given.
   *
   * Example:
   *
   *  rope<> hello("hello, ", std::string("world"), '!');
   *
   * @author: Marcelo Juchem
   */
  template <typename... Args, typename = safe_overload<rope, Args...>>
  explicit rope(Args &&...args) {
    multi_append(std::forward<Args>(args)...);
  }

  /**
   * The type used to represent the number of pieces stored in this rope.
   *
   * @author: Marcelo Juchem
   */
  using piece_index = typename container_type::size_type;

  ///////////////
  // accessors //
  ///////////////

  /**
   * Returns a reference to the `i-th' piece contained in this rope.
   *
   * Using `piece()` / `pieces()` is the prefered way to iterate over
   * the characters of this rope, as opposed to using iterators, for
   * optimal efficiency.
   *
   * Example:
   *
   *  std::string s;
   *  s.reserve(r.size());
   *
   *  for (decltype(r)::piece_index i = 0; i < r.pieces(); ++i) {
   *    auto piece = r.piece(i);
   *    s.append(piece.begin(), piece.end());
   *  }
   *
   * @author: Marcelo Juchem
   */
  string_view piece(piece_index i) const {
    assert(i < pieces_.size());
    return pieces_[i].ref();
  }

  /**
   * The number of pieces contained in this rope.
   *
   * Using `piece()` / `pieces()` is the prefered way to iterate over
   * the characters of this rope, as opposed to using iterators, for
   * optimal efficiency.
   *
   * Example:
   *
   *  std::string s;
   *  s.reserve(r.size());
   *
   *  for (decltype(r)::piece_index i = 0; i < r.pieces(); ++i) {
   *    auto piece = r.piece(i);
   *    s.append(piece.begin(), piece.end());
   *  }
   *
   * @author: Marcelo Juchem
   */
  piece_index pieces() const {
    assert(size_ || pieces_.empty());
    return pieces_.size();
  }

  /**
   * Returns the first character of the string represented by this rope.
   *
   * No bounds checking is performed. It is up to the user to make sure
   * this rope does not represent an empty string.
   *
   * @author: Marcelo Juchem
   */
  value_type front() const {
    assert(!pieces_.empty());
    return *pieces_[0].data();
  }

  /**
   * Returns the last character of the string represented by this rope.
   *
   * No bounds checking is performed. It is up to the user to make sure
   * this rope does not represent an empty string.
   *
   * @author: Marcelo Juchem
   */
  value_type back() const {
    assert(!pieces_.empty());
    auto const &last = pieces_[pieces_.size() - 1];

    assert(last.size());
    return last.data()[last.size() - 1];
  }

  /**
   * Returns the `i-th` character of the string represented by this rope.
   *
   * Throws `std::out_of_range` if `i` represents an invalid index.
   *
   * Bear in mind that using `piece()` / `pieces()` is the prefered way
   * to iterate over the characters of this rope.
   *
   * @author: Marcelo Juchem
   */
  value_type at(size_type i) const {
    auto offset = pinpoint(i);

    if (!offset.piece()) {
      throw std::out_of_range("at(): index out of bounds");
    }

    return *offset;
  }

  /**
   * Returns the `i-th` character of the string represented by this rope.
   *
   * No bounds checking is performed. It is up to the user to make sure
   * `i` represents a valid index.
   *
   * Bear in mind that using `piece()` / `pieces()` is the prefered way
   * to iterate over the characters of this rope.
   *
   * @author: Marcelo Juchem
   */
  value_type operator [](size_type i) const {
    assert(i < size_);
    return *pinpoint(i);
  }

  ////////////////////
  // const_iterator //
  ////////////////////

  /**
   * The class used to represent an iterator for the characters
   * contained in the string represented by this rope.
   *
   * @author: Marcelo Juchem
   */
  struct const_iterator:
    public std::iterator<
      std::bidirectional_iterator_tag,
      value_type,
      difference_type
    >
  {
    explicit const_iterator(
      rope const *r,
      piece_type const *piece,
      piece_index index,
      size_type offset
    ):
      rope_(r),
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

    string_view ref() const {
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
    rope const *rope_;

    // piece
    piece_type const *piece_;

    // piece index
    piece_index index_;

    // relative offset
    size_type offset_;
  };

  ///////////////
  // iterators //
  ///////////////

  /**
   * An iterator to the first element of this rope.
   *
   * The interval `[cbegin, cend)` contains the sequence of characters
   * represented by this rope.
   *
   * Bear in mind that iterating over every character of a rope
   * using iterators is more inneficient than iterating over the
   * characters of a regular string.
   *
   * It is advised that `piece()` / `pieces()` should be used instead
   * for optimal efficiency.
   *
   * @author: Marcelo Juchem
   */
  const_iterator cbegin() const {
    return const_iterator(
      this, pieces_.empty() ? nullptr : std::addressof(pieces_[0]), 0, 0
    );
  }

  /**
   * An iterator to the first element of this rope.
   *
   * The interval `[begin, end)` contains the sequence of characters
   * represented by this rope.
   *
   * Bear in mind that iterating over every character of a rope
   * using iterators is more inneficient than iterating over the
   * characters of a regular string.
   *
   * It is advised that `piece()` / `pieces()` should be used instead
   * for optimal efficiency.
   *
   * @author: Marcelo Juchem
   */
  const_iterator begin() const { return cbegin(); }

  /**
   * An iterator to the end of this rope, past the last element.
   *
   * The interval `[cbegin, cend)` contains the sequence of characters
   * represented by this rope.
   *
   * Bear in mind that iterating over every character of a rope
   * using iterators is more inneficient than iterating over the
   * characters of a regular string.
   *
   * It is advised that `piece()` / `pieces()` should be used instead
   * for optimal efficiency.
   *
   * @author: Marcelo Juchem
   */
  const_iterator cend() const {
    return const_iterator(this, nullptr, pieces_.size(), 0);
  }

  /**
   * An iterator to the end of this rope, past the last element.
   *
   * The interval `[begin, end)` contains the sequence of characters
   * represented by this rope.
   *
   * Bear in mind that iterating over every character of a rope
   * using iterators is more inneficient than iterating over the
   * characters of a regular string.
   *
   * It is advised that `piece()` / `pieces()` should be used instead
   * for optimal efficiency.
   *
   * @author: Marcelo Juchem
   */
  const_iterator end() const { return cend(); }

  ///////////
  // mimic //
  ///////////

  /**
   * Returns another rope that references the contents
   * of this rope.
   *
   * The returned rope doesn't own any of the pieces.
   *
   * This is what the copy constructor of the rope would look like.
   *
   * The reason there's no copy constructor is to avoid confusion
   * with the semantics of copying a string. Since this rope class
   * doesn't own its pieces most of the time, a copy constructor
   * would silently allow ropes to outlive its pieces.
   *
   * @author: Marcelo Juchem
   *
   * TODO: BIKE-SHED
   */
  rope mimic() const {
    rope result;

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

  /**
   * Appends the given character to the end of this rope.
   *
   * The character will be copied, not referenced.
   *
   * @author: Marcelo Juchem
   */
  void push_back(char c) {
    pieces_.emplace_back(c, size_);
    ++size_;
  }

  ////////////
  // append //
  ////////////

  /**
   * Appends the given string to the end of this rope.
   *
   * Since the string is taken as an r-value reference,
   * it will be moved into and owned by this rope, rather
   * than simply referenced by it.
   *
   * L-value references to a string are handled by another
   * overload of `append()` that simply references the
   * string.
   *
   * @author: Marcelo Juchem
   */
  void append(std::string &&s) {
    auto const size = s.size();

    if (!size) {
      return;
    }

    pieces_.emplace_back(std::move(s), size_);
    size_ += size;
  }

  /**
   * Appends a reference to the string represented by the
   * given `string_view` to the end of this rope.
   *
   * @author: Marcelo Juchem
   */
  void append(string_view s) {
    auto const size = s.size();

    if (!size) {
      return;
    }

    pieces_.emplace_back(s, size_);
    size_ += size;
  }

  /**
   * Appends the given character to the end of this rope.
   *
   * The character will be copied, not referenced.
   *
   * This is equivalent to calling `push_back(c)`.
   *
   * @author: Marcelo Juchem
   */
  void append(char c) {
    push_back(c);
  }

  /**
   * Constructs a `string_view` out of the given arguments, then
   * adds it to the end of this rope.
   *
   * @author: Marcelo Juchem
   */
  template <typename... Args, typename = safe_overload<rope, Args...>>
  void append(Args &&...args) {
    auto s = string_view(std::forward<Args>(args)...);
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

  /**
   * Appends each of the given pieces to the end of this rope.
   *
   * This is the same as calling `append()` for each of the parameters.
   *
   * @author: Marcelo Juchem
   */
  template <typename... Args>
  void multi_append(Args &&...args) {
    reserve(sizeof...(args), true);
    multi_append_impl(std::forward<Args>(args)...);
  }

  ////////////
  // concat //
  ////////////

  /**
   * Concatenates the pieces of the given rope to the end of this rope.
   *
   * The pieces of `rhs` are only referenced by this rope, even those
   * owned by `rhs`.
   *
   * @author: Marcelo Juchem
   */
  void concat(rope const &rhs) {
    auto const pieces = rhs.pieces_.size();

    reserve(pieces, true);

    // TODO: copy pieces which are single characters
    for (piece_index i = 0; i < pieces; ++i) {
      append(rhs.pieces_[i].ref());
    }
  }

  /**
   * Concatenates the pieces of the given rope to the end of this rope.
   *
   * The pieces are moved from `rhs` into this rope.
   *
   * After this function returns, `rhs` will be empty.
   *
   * @author: Marcelo Juchem
   */
  void concat(rope &&rhs) {
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

      piece.limit(static_cast<size_type>(std::distance(begin, end)));
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
    }

    return begin;
  }

  ////////////
  // string //
  ////////////

  /**
   * Returns a copy of this rope's contents as an instance of `std:string`.
   *
   * A custom allocator can be provided to allocate the output string.
   *
   * @author: Marcelo Juchem
   */
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

  /**
   * Appends the contents of this rope to the given string.
   *
   * Uses the member function `append()` from `out` passing
   * a pair of iterators `begin` and `end` to each of the
   * pieces contained in this rope.
   *
   * @author: Marcelo Juchem
   */
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

  /**
   * Pre-allocates space for the given number of `pieces`.
   *
   * The `additional` parameter tells whether the given number of pieces
   * is absolute, or should be allocated in addition to the number of
   * pieces already stored in this rope.
   *
   * @author: Marcelo Juchem
   */
  void reserve(piece_index pieces, bool additional = false) {
    pieces_.reserve(additional ? pieces_.size() + pieces : pieces);
  }

  //////////////
  // capacity //
  //////////////

  /**
   * Returns the capacity, in number of pieces, allocated for this rope.
   *
   * @author: Marcelo Juchem
   */
  size_type capacity() const { return pieces_.capacity(); }

  //////////
  // size //
  //////////

  /**
   * Returns the total number of characters contained
   * in the string represented by this rope.
   *
   * @author: Marcelo Juchem
   */
  size_type size() const { return size_; }

  ///////////
  // empty //
  ///////////

  /**
   * Tells whether this rope is empty or not.
   *
   * @author: Marcelo Juchem
   */
  bool empty() const { return !size_; }

  ///////////
  // clear //
  ///////////

  /**
   * Clears the contents of this rope. This rope
   * will be empty after this function is called.
   *
   * @author: Marcelo Juchem
   */
  void clear() {
    pieces_.clear();
    size_ = 0;
  }

  /////////////
  // compare //
  /////////////

  /**
   * Lexicographically compares the string represented by `rhs` to
   * the string represented by this rope. Returns a negative integer
   * when this rope is lexicographically smaller than `rhs`, a positive
   * integer when this rope is lexicographically greater than `rhs`
   * or 0 when this rope represents the same string as `rhs`.
   *
   * @author: Marcelo Juchem
   */
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

  /**
   * Lexicographically compares the string represented by `rhs` to
   * the string represented by this rope. Returns a negative integer
   * when this rope is lexicographically smaller than `rhs`, a positive
   * integer when this rope is lexicographically greater than `rhs`
   * or 0 when this rope represents the same string as `rhs`.
   *
   * @author: Marcelo Juchem
   */
  int compare(string_view rhs) const {
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

  /**
   * Lexicographically compares the string represented by `rhs` to
   * the string represented by this rope. Returns a negative integer
   * when this rope is lexicographically smaller than `rhs`, a positive
   * integer when this rope is lexicographically greater than `rhs`
   * or 0 when this rope represents the same string as `rhs`.
   *
   * @author: Marcelo Juchem
   */
  int compare(rope const &rhs) const {
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

  /**
   * Lexicographically compares the string represented by `rhs` to
   * the string represented by this rope. Returns a negative integer
   * when this rope is lexicographically smaller than `rhs`, a positive
   * integer when this rope is lexicographically greater than `rhs`
   * or 0 when this rope represents the same string as `rhs`.
   *
   * @author: Marcelo Juchem
   */
  template <typename T, typename = safe_overload<rope, T>>
  int compare(T &&rhs) const {
    return compare(string_view(std::forward<T>(rhs)));
  }

  //////////
  // find //
  //////////

  /**
   * Returns an iterator to the first occurence of the
   * character `c` or `end()` if not found.
   *
   * @author: Marcelo Juchem
   */
  const_iterator find(char c) const {
    for (piece_index i = 0, pieces = pieces_.size(); i < pieces; ++i) {
      auto &piece = pieces_[i];
      auto const ref = piece.ref();

      auto const j = std::find(ref.begin(), ref.end(), c);

      if (j != ref.end()) {
        auto const off = static_cast<size_type>(std::distance(ref.begin(), j));
        return const_iterator(this, std::addressof(piece), i, off);
      }
    }

    return cend();
  }

  /**
   * Returns an iterator to the first occurence of the character `c`
   * or `end()` if not found. Beginst searching at `offset` instead
   * of searching from the beginning of the rope.
   *
   * @author: Marcelo Juchem
   */
  const_iterator find(char c, size_type offset) const {
    return find(c, pinpoint(offset));
  }

  /**
   * Returns an iterator to the first occurence of the character `c`
   * or `end()` if not found. Beginst searching at `offset` instead
   * of searching from the beginning of the rope.
   *
   * @author: Marcelo Juchem
   */
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
        std::next(ref.begin(), static_cast<difference_type>(piece_offset)),
        ref.end(),
        c
      );

      if (j != ref.end()) {
        assert(i < pieces);
        auto const off = static_cast<size_type>(std::distance(ref.begin(), j));
        return const_iterator(this, std::addressof(pieces_[i]), i, off);
      }
    }

    return cend();
  }

  ///////////
  // rfind //
  ///////////

  ///////////////////
  // find_first_of //
  ///////////////////

  ///////////////////////
  // find_first_not_of //
  ///////////////////////

  //////////////////
  // find_last_of //
  //////////////////

  //////////////////////
  // find_last_not_of //
  //////////////////////

  /////////////////
  // operator == //
  /////////////////

  /**
   * Returns true if the string represented by `rhs` is equal to
   * the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
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

  /**
   * Returns true if the string represented by `rhs` is equal to
   * the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  template <std::size_t Size>
  bool operator ==(char const (&rhs)[Size]) const {
    return size_ < Size
      ?  (*this == static_cast<char const *>(rhs))
      : false;
  }

  /**
   * Returns true if the string represented by `rhs` is equal to
   * the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  bool operator ==(string_view rhs) const {
    return size_ == rhs.size() && !compare(rhs);
  }

  /**
   * Returns true if the string represented by `rhs` is equal to
   * the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  bool operator ==(rope const &rhs) const {
    if (size_ != rhs.size_) {
      return false;
    }

    return !compare(rhs);
  }

  /**
   * Returns true if the string represented by `rhs` is equal to
   * the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  template <typename T, typename = safe_overload<rope, T>>
  bool operator ==(T &&rhs) const {
    return *this == string_view(std::forward<T>(rhs));
  }

  ////////////////
  // operator < //
  ////////////////

  /**
   * Returns true if the string represented by `rhs` is lexicographically
   * less than the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  template <typename T>
  bool operator <(T &&rhs) const {
    return compare(std::forward<T>(rhs)) < 0;
  }

  ////////////////
  // operator > //
  ////////////////

  /**
   * Returns true if the string represented by `rhs` is lexicographically
   * greater than the string represented by this rope, or false otherwise.
   *
   * @author: Marcelo Juchem
   */
  template <typename T>
  bool operator >(T &&rhs) const {
    return compare(std::forward<T>(rhs)) > 0;
  }

  struct hasher {
    using argument = rope;
    using result_type = std::size_t;

    result_type operator ()(rope const &r) const {
      bytes_hasher<result_type> inner_hasher;

      for (piece_index i = 0, pieces = r.pieces(); i < pieces; ++i) {
        auto piece = r.piece(i);

        inner_hasher(piece.begin(), piece.end());
      }

      return *inner_hasher;
    }
  };

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
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator ==(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return rhs.operator==(lhs);
}

////////////////
// operator < //
////////////////

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator <(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return rhs > lhs;
}

////////////////
// operator > //
////////////////

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator >(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return rhs < lhs;
}

/////////////////
// operator != //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator !=(rope<SmallBufferSize> const &lhs, T const &rhs) {
  return !(lhs == rhs);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator !=(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return !(rhs == lhs);
}

/////////////////
// operator <= //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator <=(rope<SmallBufferSize> const &lhs, T const &rhs) {
  return !(lhs > rhs);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator <=(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return !(rhs < lhs);
}

/////////////////
// operator >= //
/////////////////

template <typename T, std::size_t SmallBufferSize>
bool operator >=(rope<SmallBufferSize> const &lhs, T const &rhs) {
  return !(lhs < rhs);
}

template <
  typename T,
  std::size_t SmallBufferSize,
  typename = safe_overload<rope<SmallBufferSize>, T>
>
bool operator >=(T const &lhs, rope<SmallBufferSize> const &rhs) {
  return !(rhs > lhs);
}

/////////////////////////////////////
// operator <<(std::basic_ostream) //
/////////////////////////////////////

template <typename C, typename T, std::size_t SmallBufferSize>
std::ostream &operator <<(
  std::basic_ostream<C, T> &out,
  rope<SmallBufferSize> const &r
) {
  using piece_index = typename rope<SmallBufferSize>::piece_index;

  for (piece_index i = 0, pieces = r.pieces(); i < pieces; ++i) {
    auto piece = r.piece(i);

    out.write(piece.data(), static_cast<std::streamsize>(piece.size()));
  }

  return out;
}

} // namespace fatal {

FATAL_DIAGNOSTIC_POP

#endif // FATAL_INCLUDE_fatal_string_rope_h
