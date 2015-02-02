/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/codec/varint.h>

#include <fatal/test/driver.h>

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

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
    EXPECT_GT(size, 0);
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
      CHECK_EQ(std::distance(begin, end), out.size());
      auto const i = e(begin, end);
      EXPECT_TRUE(e.done());
      return result(out, i);
    }
  };

  struct decode {
    type operator ()(fast_pass<encoded> data) const {
      auto const begin = data.first.begin();
      auto const end = std::next(begin, data.second);
      auto const value = codec::decode(begin, end);
      EXPECT_TRUE(value.second);
      return value.first;
    }
  };

  struct tracking_decode {
    type operator ()(fast_pass<encoded> data) const {
      auto begin = data.first.begin();
      auto const end = std::next(begin, data.second);
      auto const value = codec::tracking_decode(begin, end);
      EXPECT_TRUE(value.second);
      EXPECT_EQ(end, begin);
      return value.first;
    }
  };

  struct decoder {
    type operator ()(fast_pass<encoded> data) const {
      auto const begin = data.first.begin();
      auto const end = std::next(begin, data.second);
      typename codec::decoder d;
      EXPECT_EQ(end, d(begin, end));
      EXPECT_TRUE(d.done());
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

    auto const lower_limit = -static_cast<T>(upper_limit);

    VLOG(1) << "[-count, 0): [" << static_cast<std::intmax_t>(lower_limit)
      << ", 0)";

    for (T i = lower_limit; i < 0; ++i) {
      ASSERT_EQ(i, decoder(encoder(i)));
    }

    if (limit::min() < lower_limit) {
      T i = limit::min() + std::min(
        static_cast<std::uintmax_t>(-(limit::min() - lower_limit)),
        count::value
      );
      VLOG(1) << "[min, -count): ["
        << static_cast<std::intmax_t>(limit::min()) << ", "
        << static_cast<std::intmax_t>(i) << ')';

      while (i-- > limit::min()) {
        ASSERT_EQ(i, decoder(encoder(i)));
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
struct tester {
  void operator ()() const {
    using limit = std::numeric_limits<T>;

    TEncoder<impl<T>> encoder;
    TDecoder<impl<T>> decoder;

    ASSERT_EQ(limit::max(), decoder(encoder(limit::max())));
    ASSERT_EQ(limit::min(), decoder(encoder(limit::min())));

    auto const max = static_cast<std::uintmax_t>(limit::max());
    auto const upper_limit = std::min(max, count::value);

    T i = static_cast<T>(upper_limit);
    VLOG(1) << "[0, count]: [0, " << static_cast<std::uintmax_t>(i) << ']';
    do {
      ASSERT_EQ(i, decoder(encoder(i)));
    } while (i--);

    if (upper_limit < max) {
      i = static_cast<T>(max - std::min(max - upper_limit, count::value));
      VLOG(1) << "(count, max]: [" << static_cast<std::uintmax_t>(i) << ", "
        << max << ']';

      while (i++ < limit::max()) {
        ASSERT_EQ(i, decoder(encoder(i)));
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
struct tester<TEncoder, TDecoder, bool> {
  void operator ()() const {
    TEncoder<impl<bool>> encoder;
    TDecoder<impl<bool>> decoder;

    ASSERT_FALSE(decoder(encoder(false)));
    ASSERT_TRUE(decoder(encoder(true)));
  }
};

template <
  template <typename> class TEncoder,
  template <typename> class TDecoder,
  typename T
>
void test() {
  tester<TEncoder, TDecoder, T>()();
}

TEST(stuff, stuff) {
  {
    use::enc<impl<std::int64_t>> encoder;
    use::dec<impl<std::int64_t>> decoder;
    ASSERT_EQ(-10000000, decoder(encoder(-10000000)));
  }
  {
    use::enc<impl<std::int8_t>> encoder;
    use::dec<impl<std::int8_t>> decoder;
    ASSERT_EQ(127, decoder(encoder(127)));
  }
  {
    use::enc<impl<std::int16_t>> encoder;
    use::dec<impl<std::int16_t>> decoder;
    ASSERT_EQ(32767, decoder(encoder(32767)));
  }
}

TEST(encode, decode_i8) { test<use::enc, use::dec, std::int8_t>(); }
TEST(encode, decode_i16) { test<use::enc, use::dec, std::int16_t>(); }
TEST(encode, decode_i32) { test<use::enc, use::dec, std::int32_t>(); }
TEST(encode, decode_i64) { test<use::enc, use::dec, std::int64_t>(); }
TEST(encode, decode_bool) { test<use::enc, use::dec, bool>(); }
TEST(encode, decode_u8) { test<use::enc, use::dec, std::uint8_t>(); }
TEST(encode, decode_u16) { test<use::enc, use::dec, std::uint16_t>(); }
TEST(encode, decode_u32) { test<use::enc, use::dec, std::uint32_t>(); }
TEST(encode, decode_u64) { test<use::enc, use::dec, std::uint64_t>(); }

TEST(encode, tdecode_i8) { test<use::enc, use::tdec, std::int8_t>(); }
TEST(encode, tdecode_i16) { test<use::enc, use::tdec, std::int16_t>(); }
TEST(encode, tdecode_i32) { test<use::enc, use::tdec, std::int32_t>(); }
TEST(encode, tdecode_i64) { test<use::enc, use::tdec, std::int64_t>(); }
TEST(encode, tdecode_bool) { test<use::enc, use::tdec, bool>(); }
TEST(encode, tdecode_u8) { test<use::enc, use::tdec, std::uint8_t>(); }
TEST(encode, tdecode_u16) { test<use::enc, use::tdec, std::uint16_t>(); }
TEST(encode, tdecode_u32) { test<use::enc, use::tdec, std::uint32_t>(); }
TEST(encode, tdecode_u64) { test<use::enc, use::tdec, std::uint64_t>(); }

TEST(encode, decoder_i8) { test<use::enc, use::decr, std::int8_t>(); }
TEST(encode, decoder_i16) { test<use::enc, use::decr, std::int16_t>(); }
TEST(encode, decoder_i32) { test<use::enc, use::decr, std::int32_t>(); }
TEST(encode, decoder_i64) { test<use::enc, use::decr, std::int64_t>(); }
TEST(encode, decoder_bool) { test<use::enc, use::decr, bool>(); }
TEST(encode, decoder_u8) { test<use::enc, use::decr, std::uint8_t>(); }
TEST(encode, decoder_u16) { test<use::enc, use::decr, std::uint16_t>(); }
TEST(encode, decoder_u32) { test<use::enc, use::decr, std::uint32_t>(); }
TEST(encode, decoder_u64) { test<use::enc, use::decr, std::uint64_t>(); }

TEST(encoder, decode_i8) { test<use::encr, use::dec, std::int8_t>(); }
TEST(encoder, decode_i16) { test<use::encr, use::dec, std::int16_t>(); }
TEST(encoder, decode_i32) { test<use::encr, use::dec, std::int32_t>(); }
TEST(encoder, decode_i64) { test<use::encr, use::dec, std::int64_t>(); }
TEST(encoder, decode_bool) { test<use::encr, use::dec, bool>(); }
TEST(encoder, decode_u8) { test<use::encr, use::dec, std::uint8_t>(); }
TEST(encoder, decode_u16) { test<use::encr, use::dec, std::uint16_t>(); }
TEST(encoder, decode_u32) { test<use::encr, use::dec, std::uint32_t>(); }
TEST(encoder, decode_u64) { test<use::encr, use::dec, std::uint64_t>(); }

TEST(encoder, tdecode_i8) { test<use::encr, use::tdec, std::int8_t>(); }
TEST(encoder, tdecode_i16) { test<use::encr, use::tdec, std::int16_t>(); }
TEST(encoder, tdecode_i32) { test<use::encr, use::tdec, std::int32_t>(); }
TEST(encoder, tdecode_i64) { test<use::encr, use::tdec, std::int64_t>(); }
TEST(encoder, tdecode_bool) { test<use::encr, use::tdec, bool>(); }
TEST(encoder, tdecode_u8) { test<use::encr, use::tdec, std::uint8_t>(); }
TEST(encoder, tdecode_u16) { test<use::encr, use::tdec, std::uint16_t>(); }
TEST(encoder, tdecode_u32) { test<use::encr, use::tdec, std::uint32_t>(); }
TEST(encoder, tdecode_u64) { test<use::encr, use::tdec, std::uint64_t>(); }

TEST(encoder, decoder_i8) { test<use::encr, use::decr, std::int8_t>(); }
TEST(encoder, decoder_i16) { test<use::encr, use::decr, std::int16_t>(); }
TEST(encoder, decoder_i32) { test<use::encr, use::decr, std::int32_t>(); }
TEST(encoder, decoder_i64) { test<use::encr, use::decr, std::int64_t>(); }
TEST(encoder, decoder_bool) { test<use::encr, use::decr, bool>(); }
TEST(encoder, decoder_u8) { test<use::encr, use::decr, std::uint8_t>(); }
TEST(encoder, decoder_u16) { test<use::encr, use::decr, std::uint16_t>(); }
TEST(encoder, decoder_u32) { test<use::encr, use::decr, std::uint32_t>(); }
TEST(encoder, decoder_u64) { test<use::encr, use::decr, std::uint64_t>(); }

} // namespace fatal {
