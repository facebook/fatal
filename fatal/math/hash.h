/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/math/numerics.h>

#include <iterator>
#include <limits>
#include <type_traits>

#include <cassert>

namespace fatal {
namespace detail  {

template <typename T, std::size_t = sizeof(T)>
struct bytes_hasher_impl {
  using result_type = T;

private:
  static_assert(
    std::is_unsigned<result_type>::value,
    "result_type must be unsigned"
  );

  static_assert(
    std::numeric_limits<result_type>::max()
      >= std::numeric_limits<unsigned char>::max(),
    "result_type is not big enough"
  );

  using prime = std::integral_constant<result_type, 223>;

public:
  static result_type hash(result_type state, char const data) {
    return state ^ (state * prime::value + static_cast<result_type>(data));
  }

  static result_type hash(
    result_type state,
    char const *begin,
    char const *end
  ) {
    using step = std::integral_constant<std::size_t, 8>;

    static_assert(
      (step::value & (step::value - 1)) == 0,
      "step must be a power of two"
    );

    assert(begin <= end);
    auto const size = unsigned_cast(std::distance(begin, end));
    auto const delta = signed_cast(size - (size & (step::value - 1)));
    auto const tail = std::next(begin, delta);
    assert(begin <= tail);
    assert(tail <= end);

    auto next = [](auto it, auto i) {
      return static_cast<result_type>(*std::next(it, i));
    };

    for (; begin != tail; std::advance(begin, step::value)) {
      assert(begin < tail);

      state ^= state * prime::value + next(begin, 0);
      state ^= state * prime::value + next(begin, 1);
      state ^= state * prime::value + next(begin, 2);
      state ^= state * prime::value + next(begin, 3);
      state ^= state * prime::value + next(begin, 4);
      state ^= state * prime::value + next(begin, 5);
      state ^= state * prime::value + next(begin, 6);
      state ^= state * prime::value + next(begin, 7);
    }

    assert(begin == tail);

    for (; begin != end; std::advance(begin, 1)) {
      assert(begin < end);
      state ^= state * prime::value + next(begin, 0);
    }

    assert(begin == end);

    return state;
  }
};

/* TODO
template <typename T>
struct bytes_hasher_impl<T, 4> {
  static T hash(T state, char const *data, std::size_t size) {
    // TODO: IMPLEMENT USING fnv32
  }
};

template <typename T>
struct bytes_hasher_impl<T, 8> {
  static T hash(T state, char const *data, std::size_t size) {
    // TODO: IMPLEMENT USING fnv64
  }
};
*/

// TODO: SPECIALIZE FOR POWERS OF TWO ABOVE 64bits: USE HASH COMBIME

} // namespace detail {

template <typename T = std::size_t>
class bytes_hasher {
  using impl = detail::bytes_hasher_impl<T>;

public:
  using result_type = typename impl::result_type;

private:
  using default_seed = largest_mersenne_prime_under<
    data_bits<result_type>::value / 2 + 1
  >;

public:
  explicit bytes_hasher(result_type seed = default_seed::value): hash_(seed) {}

  bytes_hasher &operator ()(char const *const begin, char const *end) {
    assert(begin <= end);
    hash_ = impl::hash(hash_, begin, end);
    return *this;
  }

  bytes_hasher &operator ()(char const *const data, std::size_t const size) {
    hash_ = impl::hash(hash_, data, std::next(data, signed_cast(size)));
    return *this;
  }

  bytes_hasher &operator ()(char const data) {
    hash_ = impl::hash(hash_, data);
    return *this;
  }

  result_type operator *() const { return hash_; }
  explicit operator result_type() const { return hash_; }

private:
  result_type hash_;
};

} // namespace fatail {
