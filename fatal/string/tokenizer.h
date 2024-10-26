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
#include <fatal/string/string_view.h>
#include <fatal/type/traits.h>

#include <type_traits>
#include <utility>

namespace fatal {

template <typename Token, char Delimiter>
struct tokenizer {
  using token = Token;
  using delimiter = std::integral_constant<char, Delimiter>;

  template <typename... Args, typename = safe_overload<tokenizer, Args...>>
  explicit tokenizer(Args &&...args): data_(std::forward<Args>(args)...) {}

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

    explicit const_iterator(string_view data):
      data_(data)
    {
      token_.construct(data_.seek_past(delimiter::value));
    }

    const_iterator &operator ++() {
      token_.destroy();
      token_.construct(data_.seek_past(delimiter::value));
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
    string_view data_;
    uninitialized<token, true> token_;
  };

  const_iterator cbegin() const { return const_iterator(data_); }

  const_iterator begin() const { return cbegin(); }

  const_iterator cend() const {
    return const_iterator(string_view(data_.end(), data_.end()));
  }

  const_iterator end() const { return cend(); }

  bool empty() const { return !data_; }

  bool operator ==(tokenizer const &rhs) const {
    return data_.begin() == rhs.data_.begin()
      && data_.end() == rhs.data_.end();
  }

  bool operator !=(tokenizer const &rhs) const { return !(*this == rhs); }

private:
  string_view data_;
};


using colon_tokenizer = tokenizer<string_view, ':'>;
using comma_tokenizer = tokenizer<string_view, ','>;
using line_tokenizer = tokenizer<string_view, '\n'>;
using semicolon_tokenizer = tokenizer<string_view, ';'>;
using space_tokenizer = tokenizer<string_view, ' '>;

using csv_tokenizer = tokenizer<comma_tokenizer, '\n'>;

} // namespace fatal {
