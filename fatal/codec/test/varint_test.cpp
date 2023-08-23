/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/codec/varint.h>

#include <fatal/test/driver.h>

#include <fatal/math/numerics.h>

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

#include <cassert>
#include <cstdint>

using count = std::integral_constant<std::uintmax_t, 1000000>;

namespace fatal {

struct use {
  template <typename T> using enc = typename T::encode;
  template <typename T> using encr = typename T::encoder;
  template <typename T> using dec = typename T::decode;
  template <typename T> using decr = typename T::decoder;
  template <typename T> using tdec = typename T::tracking_decode;
};

template <typename T>
class impl {
  using type = T;
  using codec = varint<type>;
  using buffer = typename codec::template automatic_buffer<>;
  using size_type = typename buffer::size_type;
  using encoded = std::pair<buffer, size_type>;

  template <typename TIterator>
  static encoded result(buffer &b, TIterator end) {
    auto const size = std::distance(b.begin(), end);
    FATAL_EXPECT_GT(size, 0);
    return encoded(b, size);
  }

public:
  struct encode {
    encoded operator ()(fast_pass<type> value) const {
      buffer out;
      return result(out, codec::encode(value, out.begin()));
    }
  };

  struct encoder {
    encoded operator ()(fast_pass<type> value) const {
      buffer out;
      typename codec::encoder e(value);
      auto const begin = out.begin();
      auto const end = out.end();
      assert(std::distance(begin, end) == 1 * out.size());
      auto const i = e(begin, end);
      FATAL_EXPECT_TRUE(e.done());
      return result(out, i);
    }
  };

  struct decode {
    type operator ()(fast_pass<encoded> data) const {
      auto const begin = data.first.begin();
      auto const end = std::next(begin, signed_cast(data.second));
      auto const value = codec::decode(begin, end);
      FATAL_EXPECT_TRUE(value.second);
      return value.first;
    }
  };

  struct tracking_decode {
    type operator ()(fast_pass<encoded> data) const {
      auto begin = data.first.begin();
      auto const end = std::next(begin, signed_cast(data.second));
      auto const value = codec::tracking_decode(begin, end);
      FATAL_EXPECT_TRUE(value.second);
      FATAL_EXPECT_EQ(end, begin);
      return value.first;
    }
  };

  struct decoder {
    type operator ()(fast_pass<encoded> data) const {
      auto const begin = data.first.begin();
      auto const end = std::next(begin, signed_cast(data.second));
      typename codec::decoder d;
      FATAL_EXPECT_EQ(end, d(begin, end));
      FATAL_EXPECT_TRUE(d.done());
      return d.value();
    }
  };
};

template <
  bool,
  template <typename> class TEncoder,
  template <typename> class TDecoder,
  typename T
>
struct signed_tester {
  void operator ()(std::uintmax_t upper_limit) const {
    using limit = std::numeric_limits<T>;

    TEncoder<impl<T>> encoder;
    TDecoder<impl<T>> decoder;

    auto const lower_limit = static_cast<T>(-static_cast<T>(upper_limit));

    FATAL_VLOG(1) << "[-count, 0): [" << static_cast<std::intmax_t>(lower_limit)
      << ", 0)";

    for (T i = lower_limit; i < T(0); ++i) {
      FATAL_ASSERT_EQ(i, decoder(encoder(i)));
    }

    if (limit::min() < lower_limit) {
      T i = limit::min() + static_cast<T>(std::min(
        static_cast<std::uintmax_t>(-(limit::min() - lower_limit)),
        count::value
      ));
      FATAL_VLOG(1) << "[min, -count): ["
        << static_cast<std::intmax_t>(limit::min()) << ", "
        << static_cast<std::intmax_t>(i) << ')';

      while (i > limit::min()) {
        --i;
        FATAL_ASSERT_EQ(i, decoder(encoder(i)));
      }
    }
  }
};

template <
  template <typename> class TEncoder,
  template <typename> class TDecoder,
  typename T
>
struct signed_tester<false, TEncoder, TDecoder, T> {
  void operator ()(std::uintmax_t) const {}
};

template <
  template <typename> class TEncoder,
  template <typename> class TDecoder,
  typename T
>
struct tester_impl {
  void operator ()() const {
    using limit = std::numeric_limits<T>;

    TEncoder<impl<T>> encoder;
    TDecoder<impl<T>> decoder;

    FATAL_ASSERT_EQ(limit::max(), decoder(encoder(limit::max())));
    FATAL_ASSERT_EQ(limit::min(), decoder(encoder(limit::min())));

    auto const max = static_cast<std::uintmax_t>(limit::max());
    auto const upper_limit = std::min(max, count::value);

    T i = static_cast<T>(upper_limit);
    FATAL_VLOG(1) << "[0, count]: [0, " << static_cast<std::uintmax_t>(i)
      << ']';

    do {
      FATAL_ASSERT_EQ(i, decoder(encoder(i)));
    } while (i--);

    if (upper_limit < max) {
      i = static_cast<T>(max - std::min(max - upper_limit, count::value));
      FATAL_VLOG(1) << "(count, max]: [" << static_cast<std::uintmax_t>(i)
        << ", " << max << ']';

      while (i < limit::max()) {
        ++i;
        FATAL_ASSERT_EQ(i, decoder(encoder(i)));
      }
    }

    signed_tester<std::is_signed<T>::value, TEncoder, TDecoder, T>()(
      upper_limit
    );
  }
};

template <
  template <typename> class TEncoder,
  template <typename> class TDecoder
>
struct tester_impl<TEncoder, TDecoder, bool> {
  void operator ()() const {
    TEncoder<impl<bool>> encoder;
    TDecoder<impl<bool>> decoder;

    FATAL_ASSERT_FALSE(decoder(encoder(false)));
    FATAL_ASSERT_TRUE(decoder(encoder(true)));
  }
};

template <
  template <typename> class TEncoder,
  template <typename> class TDecoder,
  typename T
>
void chk() {
  tester_impl<TEncoder, TDecoder, T>()();
}

FATAL_TEST(codec, sanity_check) {
  {
    use::enc<impl<std::int64_t>> encoder;
    use::dec<impl<std::int64_t>> decoder;
    FATAL_ASSERT_EQ(-10000000, decoder(encoder(-10000000)));
  }
  {
    use::enc<impl<std::int8_t>> encoder;
    use::dec<impl<std::int8_t>> decoder;
    FATAL_ASSERT_EQ(127, decoder(encoder(127)));
  }
  {
    use::enc<impl<std::int16_t>> encoder;
    use::dec<impl<std::int16_t>> decoder;
    FATAL_ASSERT_EQ(32767, decoder(encoder(32767)));
  }
}

FATAL_TEST(encode, decode_i8) { chk<use::enc, use::dec, std::int8_t>(); }
FATAL_TEST(encode, decode_i16) { chk<use::enc, use::dec, std::int16_t>(); }
FATAL_TEST(encode, decode_i32) { chk<use::enc, use::dec, std::int32_t>(); }
FATAL_TEST(encode, decode_i64) { chk<use::enc, use::dec, std::int64_t>(); }
FATAL_TEST(encode, decode_bool) { chk<use::enc, use::dec, bool>(); }
FATAL_TEST(encode, decode_u8) { chk<use::enc, use::dec, std::uint8_t>(); }
FATAL_TEST(encode, decode_u16) { chk<use::enc, use::dec, std::uint16_t>(); }
FATAL_TEST(encode, decode_u32) { chk<use::enc, use::dec, std::uint32_t>(); }
FATAL_TEST(encode, decode_u64) { chk<use::enc, use::dec, std::uint64_t>(); }

FATAL_TEST(encode, tdecode_i8) { chk<use::enc, use::tdec, std::int8_t>(); }
FATAL_TEST(encode, tdecode_i16) { chk<use::enc, use::tdec, std::int16_t>(); }
FATAL_TEST(encode, tdecode_i32) { chk<use::enc, use::tdec, std::int32_t>(); }
FATAL_TEST(encode, tdecode_i64) { chk<use::enc, use::tdec, std::int64_t>(); }
FATAL_TEST(encode, tdecode_bool) { chk<use::enc, use::tdec, bool>(); }
FATAL_TEST(encode, tdecode_u8) { chk<use::enc, use::tdec, std::uint8_t>(); }
FATAL_TEST(encode, tdecode_u16) { chk<use::enc, use::tdec, std::uint16_t>(); }
FATAL_TEST(encode, tdecode_u32) { chk<use::enc, use::tdec, std::uint32_t>(); }
FATAL_TEST(encode, tdecode_u64) { chk<use::enc, use::tdec, std::uint64_t>(); }

FATAL_TEST(encode, decoder_i8) { chk<use::enc, use::decr, std::int8_t>(); }
FATAL_TEST(encode, decoder_i16) { chk<use::enc, use::decr, std::int16_t>(); }
FATAL_TEST(encode, decoder_i32) { chk<use::enc, use::decr, std::int32_t>(); }
FATAL_TEST(encode, decoder_i64) { chk<use::enc, use::decr, std::int64_t>(); }
FATAL_TEST(encode, decoder_bool) { chk<use::enc, use::decr, bool>(); }
FATAL_TEST(encode, decoder_u8) { chk<use::enc, use::decr, std::uint8_t>(); }
FATAL_TEST(encode, decoder_u16) { chk<use::enc, use::decr, std::uint16_t>(); }
FATAL_TEST(encode, decoder_u32) { chk<use::enc, use::decr, std::uint32_t>(); }
FATAL_TEST(encode, decoder_u64) { chk<use::enc, use::decr, std::uint64_t>(); }

FATAL_TEST(encoder, decode_i8) { chk<use::encr, use::dec, std::int8_t>(); }
FATAL_TEST(encoder, decode_i16) { chk<use::encr, use::dec, std::int16_t>(); }
FATAL_TEST(encoder, decode_i32) { chk<use::encr, use::dec, std::int32_t>(); }
FATAL_TEST(encoder, decode_i64) { chk<use::encr, use::dec, std::int64_t>(); }
FATAL_TEST(encoder, decode_bool) { chk<use::encr, use::dec, bool>(); }
FATAL_TEST(encoder, decode_u8) { chk<use::encr, use::dec, std::uint8_t>(); }
FATAL_TEST(encoder, decode_u16) { chk<use::encr, use::dec, std::uint16_t>(); }
FATAL_TEST(encoder, decode_u32) { chk<use::encr, use::dec, std::uint32_t>(); }
FATAL_TEST(encoder, decode_u64) { chk<use::encr, use::dec, std::uint64_t>(); }

FATAL_TEST(encoder, tdecode_i8) { chk<use::encr, use::tdec, std::int8_t>(); }
FATAL_TEST(encoder, tdecode_i16) { chk<use::encr, use::tdec, std::int16_t>(); }
FATAL_TEST(encoder, tdecode_i32) { chk<use::encr, use::tdec, std::int32_t>(); }
FATAL_TEST(encoder, tdecode_i64) { chk<use::encr, use::tdec, std::int64_t>(); }
FATAL_TEST(encoder, tdecode_bool) { chk<use::encr, use::tdec, bool>(); }
FATAL_TEST(encoder, tdecode_u8) { chk<use::encr, use::tdec, std::uint8_t>(); }
FATAL_TEST(encoder, tdecode_u16) { chk<use::encr, use::tdec, std::uint16_t>(); }
FATAL_TEST(encoder, tdecode_u32) { chk<use::encr, use::tdec, std::uint32_t>(); }
FATAL_TEST(encoder, tdecode_u64) { chk<use::encr, use::tdec, std::uint64_t>(); }

FATAL_TEST(encoder, decoder_i8) { chk<use::encr, use::decr, std::int8_t>(); }
FATAL_TEST(encoder, decoder_i16) { chk<use::encr, use::decr, std::int16_t>(); }
FATAL_TEST(encoder, decoder_i32) { chk<use::encr, use::decr, std::int32_t>(); }
FATAL_TEST(encoder, decoder_i64) { chk<use::encr, use::decr, std::int64_t>(); }
FATAL_TEST(encoder, decoder_bool) { chk<use::encr, use::decr, bool>(); }
FATAL_TEST(encoder, decoder_u8) { chk<use::encr, use::decr, std::uint8_t>(); }
FATAL_TEST(encoder, decoder_u16) { chk<use::encr, use::decr, std::uint16_t>(); }
FATAL_TEST(encoder, decoder_u32) { chk<use::encr, use::decr, std::uint32_t>(); }
FATAL_TEST(encoder, decoder_u64) { chk<use::encr, use::decr, std::uint64_t>(); }

} // namespace fatal {
