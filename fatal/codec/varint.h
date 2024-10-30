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

#include <array>
#include <iterator>
#include <type_traits>

#include <cassert>

namespace fatal {
namespace detail {
namespace varint_impl {

// the maximum amount of data units with (DataSize + 1) bits of data taken
// by the varint encoding, for a type that takes up ValueSize bytes
template <std::size_t ValueSize, std::size_t DataSize>
using size = std::integral_constant<
  std::size_t,
  DataSize < ValueSize ? (ValueSize - 1) / DataSize + 1 : 1
>;

auto constexpr byte_size = data_bits<char>::value;
auto constexpr byte_payload = byte_size - 1;

static_assert(size<1, byte_payload>::value == 1, "size mismatch");
static_assert(size<2, byte_payload>::value == 1, "size mismatch");
static_assert(size<3, byte_payload>::value == 1, "size mismatch");
static_assert(size<4, byte_payload>::value == 1, "size mismatch");
static_assert(size<5, byte_payload>::value == 1, "size mismatch");
static_assert(size<6, byte_payload>::value == 1, "size mismatch");
static_assert(size<7, byte_payload>::value == 1, "size mismatch");
static_assert(size<8, byte_payload>::value == 2, "size mismatch");
static_assert(size<9, byte_payload>::value == 2, "size mismatch");
static_assert(size<10, byte_payload>::value == 2, "size mismatch");
static_assert(size<11, byte_payload>::value == 2, "size mismatch");
static_assert(size<12, byte_payload>::value == 2, "size mismatch");
static_assert(size<13, byte_payload>::value == 2, "size mismatch");
static_assert(size<14, byte_payload>::value == 2, "size mismatch");
static_assert(size<15, byte_payload>::value == 3, "size mismatch");
static_assert(size<16, byte_payload>::value == 3, "size mismatch");
static_assert(size<17, byte_payload>::value == 3, "size mismatch");
static_assert(size<18, byte_payload>::value == 3, "size mismatch");
static_assert(size<19, byte_payload>::value == 3, "size mismatch");
static_assert(size<20, byte_payload>::value == 3, "size mismatch");
static_assert(size<21, byte_payload>::value == 3, "size mismatch");
static_assert(size<22, byte_payload>::value == 4, "size mismatch");
static_assert(size<23, byte_payload>::value == 4, "size mismatch");
static_assert(size<24, byte_payload>::value == 4, "size mismatch");
static_assert(size<25, byte_payload>::value == 4, "size mismatch");
static_assert(size<26, byte_payload>::value == 4, "size mismatch");
static_assert(size<27, byte_payload>::value == 4, "size mismatch");
static_assert(size<28, byte_payload>::value == 4, "size mismatch");
static_assert(size<29, byte_payload>::value == 5, "size mismatch");
static_assert(size<30, byte_payload>::value == 5, "size mismatch");
static_assert(size<31, byte_payload>::value == 5, "size mismatch");
static_assert(size<32, byte_payload>::value == 5, "size mismatch");
static_assert(size<33, byte_payload>::value == 5, "size mismatch");
static_assert(size<34, byte_payload>::value == 5, "size mismatch");
static_assert(size<35, byte_payload>::value == 5, "size mismatch");
static_assert(size<36, byte_payload>::value == 6, "size mismatch");
static_assert(size<37, byte_payload>::value == 6, "size mismatch");
static_assert(size<38, byte_payload>::value == 6, "size mismatch");
static_assert(size<39, byte_payload>::value == 6, "size mismatch");
static_assert(size<40, byte_payload>::value == 6, "size mismatch");
static_assert(size<41, byte_payload>::value == 6, "size mismatch");
static_assert(size<42, byte_payload>::value == 6, "size mismatch");
static_assert(size<43, byte_payload>::value == 7, "size mismatch");
static_assert(size<44, byte_payload>::value == 7, "size mismatch");
static_assert(size<45, byte_payload>::value == 7, "size mismatch");
static_assert(size<46, byte_payload>::value == 7, "size mismatch");
static_assert(size<47, byte_payload>::value == 7, "size mismatch");
static_assert(size<48, byte_payload>::value == 7, "size mismatch");
static_assert(size<49, byte_payload>::value == 7, "size mismatch");
static_assert(size<50, byte_payload>::value == 8, "size mismatch");
static_assert(size<51, byte_payload>::value == 8, "size mismatch");
static_assert(size<52, byte_payload>::value == 8, "size mismatch");
static_assert(size<53, byte_payload>::value == 8, "size mismatch");
static_assert(size<54, byte_payload>::value == 8, "size mismatch");
static_assert(size<55, byte_payload>::value == 8, "size mismatch");
static_assert(size<56, byte_payload>::value == 8, "size mismatch");
static_assert(size<57, byte_payload>::value == 9, "size mismatch");
static_assert(size<58, byte_payload>::value == 9, "size mismatch");
static_assert(size<59, byte_payload>::value == 9, "size mismatch");
static_assert(size<60, byte_payload>::value == 9, "size mismatch");
static_assert(size<61, byte_payload>::value == 9, "size mismatch");
static_assert(size<62, byte_payload>::value == 9, "size mismatch");
static_assert(size<63, byte_payload>::value == 9, "size mismatch");
static_assert(size<64, byte_payload>::value == 10, "size mismatch");
static_assert(size<65, byte_payload>::value == 10, "size mismatch");
static_assert(size<66, byte_payload>::value == 10, "size mismatch");
static_assert(size<67, byte_payload>::value == 10, "size mismatch");
static_assert(size<68, byte_payload>::value == 10, "size mismatch");
static_assert(size<69, byte_payload>::value == 10, "size mismatch");
static_assert(size<70, byte_payload>::value == 10, "size mismatch");

static_assert(size<byte_size * 1, byte_payload>::value == 2, "size mismatch");
static_assert(size<byte_size * 2, byte_payload>::value == 3, "size mismatch");
static_assert(size<byte_size * 3, byte_payload>::value == 4, "size mismatch");
static_assert(size<byte_size * 4, byte_payload>::value == 5, "size mismatch");
static_assert(size<byte_size * 5, byte_payload>::value == 6, "size mismatch");
static_assert(size<byte_size * 6, byte_payload>::value == 7, "size mismatch");
static_assert(size<byte_size * 7, byte_payload>::value == 8, "size mismatch");
static_assert(size<byte_size * 8, byte_payload>::value == 10, "size mismatch");
static_assert(size<byte_size * 9, byte_payload>::value == 11, "size mismatch");
static_assert(size<byte_size * 10, byte_payload>::value == 12, "size mismatch");
static_assert(size<byte_size * 11, byte_payload>::value == 13, "size mismatch");
static_assert(size<byte_size * 12, byte_payload>::value == 14, "size mismatch");
static_assert(size<byte_size * 13, byte_payload>::value == 15, "size mismatch");
static_assert(size<byte_size * 14, byte_payload>::value == 16, "size mismatch");
static_assert(size<byte_size * 15, byte_payload>::value == 18, "size mismatch");
static_assert(size<byte_size * 16, byte_payload>::value == 19, "size mismatch");

template <typename TData>
struct data_traits {
  using data_unit = TData;
  using unsigned_unit = typename std::make_unsigned<data_unit>::type;

  static_assert(
    std::is_integral<data_unit>::value,
    "expected integral data unit"
  );

  static_assert(
    data_bits<unsigned_unit>::value == data_bits<data_unit>::value,
    "unsupported data unit"
  );

  static_assert(
    data_bits<unsigned_unit>::value > 1,
    "at least 2 bits of data needed"
  );

  using data_size = data_bits<unsigned_unit>;

  using payload_size = std::integral_constant<
    std::size_t, data_size::value - 1
  >;

  using continuation_bit = std::integral_constant<
    unsigned_unit, static_cast<unsigned_unit>(1) << payload_size::value
  >;

  static_assert(
    most_significant_bit<continuation_bit::value>::value == data_size::value,
    "invalid continuation bit"
  );

  static_assert(
    pop_count<continuation_bit::value>::value == 1,
    "invalid continuation bit"
  );

  using filter_mask = std::integral_constant<
    unsigned_unit,
    static_cast<unsigned_unit>(~continuation_bit::value)
  >;

  static_assert(
    most_significant_bit<filter_mask::value>::value == payload_size::value,
    "invalid filter mask"
  );

  static_assert(
    pop_count<filter_mask::value>::value == payload_size::value,
    "invalid filter mask"
  );

  // the maximum amount of TData units taken by the varint encoding of a type T
  template <typename T>
  class size_for {
    static_assert(std::is_integral<T>::value, "expected an integral");
    static_assert(
      std::is_same<typename std::decay<T>::type, T>::value,
      "plain type expected"
    );

  public:
    using type = size<data_bits<T>::value, payload_size::value>;
  };

  static unsigned_unit from(fast_pass<data_unit> value) {
    return *reinterpret_cast<unsigned_unit const *>(std::addressof(value));
  }

  static data_unit to(fast_pass<unsigned_unit> value) {
    return *reinterpret_cast<data_unit const *>(std::addressof(value));
  }
};

template <bool, typename T>
struct value_traits {
  using external = T;
  using internal = external;

  static_assert(std::is_unsigned<external>::value, "implementation mismatch");

  static internal pre(external value) noexcept { return value; }
  static external post(internal value) noexcept { return value; }
};

template <typename T>
struct value_traits<true, T> {
  using external = T;
  using internal = typename std::make_unsigned<external>::type;

  static_assert(std::is_signed<external>::value, "implementation mismatch");
  static_assert(sizeof(external) == sizeof(internal), "invalid integral");

  static internal pre(fast_pass<external> value) noexcept {
    auto ivalue = *reinterpret_cast<internal const *>(std::addressof(value));
    return internal(ivalue << 1) | internal(value < 0 ? 1 : 0);
  }

  static external post(internal value) noexcept {
    auto const shift = (data_bits<internal>::value - 1);
    value = internal(value >> 1) | internal((value & 1) << shift);
    return *reinterpret_cast<external const *>(std::addressof(value));
  }
};

} // namespace varint_impl {
} // namespace detail {

// TODO: DOCUMENT
template <typename T>
struct varint {
  using type = T;

private:
  using shift_counter = smallest_fast_unsigned_integral<
    most_significant_bit<data_bits<type>::value>::value
  >;

  using value_traits = detail::varint_impl::value_traits<
    std::is_signed<type>::value, type
  >;
  using internal = typename value_traits::internal;

public:
  // largest amount of `TData` written when encoding a value of type `type`
  template <typename TData>
  using max_size = typename detail::varint_impl::data_traits<TData>
    ::template size_for<type>::type;

  // an automatically allocated buffer of `TData` able
  // to hold any encoding of a value of type `type`
  template <typename TData = char>
  using automatic_buffer = std::array<TData, max_size<TData>::value>;

  struct encoder {
    explicit encoder(fast_pass<type> value) noexcept:
      value_(value_traits::pre(value))
    {}

    // TODO: RETURN SIZE?
    template <typename TOutputIterator>
    TOutputIterator operator ()(
      TOutputIterator begin,
      TOutputIterator const end
    ) noexcept {
      using traits = detail::varint_impl::data_traits<
        typename std::iterator_traits<TOutputIterator>::value_type
      >;
      using unsigned_unit = typename traits::unsigned_unit;

      for (; begin != end; std::advance(begin, 1)) {
        unsigned_unit data = value_ & traits::filter_mask::value;

        value_ >>= traits::payload_size::value;

        if (value_) {
          *begin = traits::to(data | traits::continuation_bit::value);
        } else {
          *begin = traits::to(data);
          continued_ = false;
          return std::next(begin);
        }
      }

      return begin;
    }

    void reset(fast_pass<type> value) noexcept {
      value_ = value;
      continued_ = true;
    }

    // returns true if the encoding is done, false if it needs more data
    bool done() const noexcept { return !continued_; }

    // returns false if the encoding is done, true if it needs more data
    bool operator !() const noexcept { return continued_; }

    // returns true if the encoding is done, false if it needs more data
    explicit operator bool() const noexcept { return !continued_; }

  private:
    internal value_;
    bool continued_ = true;
  };

  // returns the iterator `i` to the first unused element of the output
  // buffer such that [out, i) represents the data that have been encoded
  // buffer must be able to fit at least `max_size<decltype(*out)>` elements
  // TODO: RETURN SIZE?
  template <typename TOutputIterator>
  static TOutputIterator encode(
    type value,
    TOutputIterator out
  ) noexcept {
    using traits = detail::varint_impl::data_traits<
      typename std::iterator_traits<TOutputIterator>::value_type
    >;
    using unsigned_unit = typename traits::unsigned_unit;

    for (internal x = value_traits::pre(value); ; std::advance(out, 1)) {
      unsigned_unit data = x & traits::filter_mask::value;

      x >>= traits::payload_size::value;

      if (x) {
        *out = traits::to(data | traits::continuation_bit::value);
      } else {
        *out = traits::to(data);
        return std::next(out);
      }
    }
  }

  struct decoder {
    // returns the iterator `i` to the first unused element such
    // that [begin, i) represents the data that have been decoded
    template <typename TInputIterator>
    TInputIterator operator ()(
      TInputIterator begin,
      TInputIterator const end
    ) noexcept {
      using traits = detail::varint_impl::data_traits<
        typename std::iterator_traits<TInputIterator>::value_type
      >;

      for (; continuation_ && begin != end; std::advance(begin, 1)) {
        assert(continuation_);

        value_ |= static_cast<internal>(
          traits::from(*begin) & traits::filter_mask::value
        ) << shift_;
        shift_ += traits::payload_size::value;

        continuation_ = traits::from(*begin) & traits::continuation_bit::value;
      }

      return begin;
    }

    // resets the internal structure of this decoder as if
    // no data had been fed to it
    void reset() noexcept {
      value_ = 0;
      shift_ = 0;
      continuation_ = true;
    }

    // the value decoded so far
    type value() const noexcept { return value_traits::post(value_); }

    // returns true if the decoding is done, false if it needs more data
    bool done() const noexcept { return !continuation_; }

    // returns false if the decoding is done, true if it needs more data
    bool operator !() const noexcept { return continuation_; }

    // returns true if the decoding is done, false if it needs more data
    explicit operator bool() const noexcept { return !continuation_; }

  private:
    internal value_ = 0;
    shift_counter shift_ = 0;
    bool continuation_ = true;
  };

  template <typename TInputIterator>
  static std::pair<type, bool> decode(
    TInputIterator begin,
    TInputIterator end
  ) noexcept {
    decoder decode;
    decode(begin, end);
    return std::make_pair(decode.value(), decode.done());
  }

  // TODO: bike-shed
  template <typename TInputIterator>
  static std::pair<type, bool> tracking_decode(
    TInputIterator &begin,
    TInputIterator end
  ) noexcept {
    decoder decode;
    begin = decode(begin, end);
    return std::make_pair(decode.value(), decode.done());
  }
};

} // namespace fatal {
