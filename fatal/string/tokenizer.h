/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_tokenizer_h
#define FATAL_INCLUDE_fatal_string_tokenizer_h

#include <fatal/container/uninitialized.h>
#include <fatal/string/string_ref.h>
#include <fatal/type/traits.h>

#include <type_traits>
#include <utility>

namespace fatal {

template <typename Token, char Delimiter>
struct tokenizer {
  using token = Token;
  using delimiter = std::integral_constant<char, Delimiter>;

  template <typename U, typename = safe_overload_t<tokenizer, U>>
  explicit tokenizer(U &&data): data_(std::forward<U>(data)) {}

  struct const_iterator {
    const_iterator(const_iterator const &rhs):
      data_(rhs.data_)
    {
      token_.construct(*rhs.token_);
    }

    const_iterator(const_iterator &&rhs):
      data_(std::move(rhs.data_))
    {
      token_.construct(std::move(*rhs.token_));
    }

    explicit const_iterator(string_ref data):
      data_(data)
    {
      token_.construct(data_.split_step(delimiter::value));
    }

    const_iterator &operator ++() {
      token_.destroy();
      token_.construct(data_.split_step(delimiter::value));
      return *this;
    }

    const_iterator &operator ++(int) {
      auto copy(*this);
      ++*this;
      return copy;
    }

    token const *operator ->() const { return token_.ptr(); }
    token const &operator *() const { return *token_; }

    bool operator ==(const_iterator const &rhs) const {
      return data_.begin() == rhs.data_.begin()
        && data_.end() == rhs.data_.end()
        && *token_ == *rhs.token_;
    }

    bool operator !=(const_iterator const &rhs) const {
      return !(*this == rhs);
    }

  private:
    string_ref data_;
    uninitialized<token, true> token_;
  };

  const_iterator cbegin() const { return const_iterator(data_); }

  const_iterator begin() const { return cbegin(); }

  const_iterator cend() const {
    return const_iterator(string_ref(data_.end(), data_.end()));
  }

  const_iterator end() const { return cend(); }

  bool empty() const { return !data_; }

  bool operator ==(tokenizer const &rhs) const {
    return data_.begin() == rhs.data_.begin()
      && data_.end() == rhs.data_.end();
  }

  bool operator !=(tokenizer const &rhs) const { return !(*this == rhs); }

private:
  string_ref data_;
};


using space_tokenizer = tokenizer<string_ref, ' '>;
using line_tokenizer = tokenizer<string_ref, '\n'>;
using comma_tokenizer = tokenizer<string_ref, ','>;
using colon_tokenizer = tokenizer<string_ref, ':'>;

using csv_tokenizer = tokenizer<comma_tokenizer, '\n'>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_string_tokenizer_h
