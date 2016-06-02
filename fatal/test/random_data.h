/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_random_data_h
#define FATAL_INCLUDE_fatal_test_random_data_h

#include <array>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include <cassert>
#include <climits>
#include <cstring>

namespace fatal {

struct random_data {
  using rng_type = std::mt19937;
  using result_type = typename rng_type::result_type;

  random_data(): rng_([] { std::random_device r; return r(); }()) {}

  result_type operator ()() { return rng_(); }

  bool coin_flip() {
    return rng_() & (result_type(1) << (sizeof(result_type) * CHAR_BIT / 2));
  }

  template <typename Iterator>
  void string(
    Iterator begin,
    Iterator end,
    char const *alphabet,
    std::size_t alphabet_size
  ) {
    assert(alphabet);
    assert(alphabet_size > 0);

    for (; begin != end; ++begin) {
      auto const index = rng_() % alphabet_size;
      assert(index >= 0);
      *begin = alphabet[index];
    }
  }

  std::string string(
    std::size_t size,
    char const *alphabet,
    std::size_t alphabet_size
  ) {
    std::string result;
    result.resize(size);
    string(result.begin(), result.end(), alphabet, alphabet_size);
    assert(result.size() == size);
    return result;
  }

  std::string string(std::size_t size) {
    auto const alphabet = "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
    assert(std::strlen(alphabet) == 62);
    return string(size, alphabet, 62);
  }

  template <typename T>
  std::size_t chop(std::size_t size, T &&fn) {
    std::size_t chunks = 0;

    for (std::size_t offset = 0; offset < size; ++chunks) {
      std::size_t const chunk = rng_() % (size - offset) + 1;
      assert(chunk <= size - offset);
      fn(offset, chunk);
      offset += chunk;
      assert(offset <= size);
    }

    return chunks;
  }

  template <typename T, typename Iterator>
  std::size_t chop(Iterator begin, Iterator const end, T &&fn) {
    assert(begin <= end);

    return chop(
      std::distance(begin, end),
      [&](std::size_t offset, std::size_t size) {
        auto const b = std::next(begin, offset);
        assert(begin < end);
        auto const e = std::next(b, size);
        assert(e <= end);

        fn(b, e);
      }
    );
  }
  
  std::vector<std::string> chop(std::string const &s) {
    std::vector<std::string> result;

    auto const chunks = chop(
      s.begin(), s.end(),
      [&](std::string::const_iterator begin, std::string::const_iterator end) {
        assert(begin < end);
        result.emplace_back(begin, end);
      }
    );

    assert(result.size() == chunks);
    return result;
  }

  static constexpr result_type min() { return rng_type::min(); }
  static constexpr result_type max() { return rng_type::max(); }

private:
  rng_type rng_;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_random_data_h
