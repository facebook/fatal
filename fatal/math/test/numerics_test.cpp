/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/math/numerics.h>

#include <fatal/test/driver.h>

#include <algorithm>
#include <iterator>
#include <random>

namespace fatal {

///////////////
// data_bits //
///////////////

FATAL_TEST(numerics, data_bits) {
  FATAL_EXPECT_EQ(1, data_bits<bool>::value);
  FATAL_EXPECT_EQ(CHAR_BIT, data_bits<char>::value);
  FATAL_EXPECT_EQ(CHAR_BIT, data_bits<int8_t>::value);
  FATAL_EXPECT_EQ(2 * CHAR_BIT, data_bits<int16_t>::value);
  FATAL_EXPECT_EQ(4 * CHAR_BIT, data_bits<int32_t>::value);
}

////////////////////////////
// reverse_integral_bytes //
////////////////////////////

FATAL_TEST(numerics, reverse_integral_bytes signed 8 bytes) {
  FATAL_EXPECT_EQ(
    std::int8_t(0x00),
    reverse_integral_bytes(std::int8_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::int8_t(0x01),
    reverse_integral_bytes(std::int8_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::int8_t(0x0a),
    reverse_integral_bytes(std::int8_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::int8_t(0xc2),
    reverse_integral_bytes(std::int8_t(0xc2))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes unsigned 8 bytes) {
  FATAL_EXPECT_EQ(
    std::uint8_t(0x00),
    reverse_integral_bytes(std::uint8_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::uint8_t(0x01),
    reverse_integral_bytes(std::uint8_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::uint8_t(0x0a),
    reverse_integral_bytes(std::uint8_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::uint8_t(0xc2),
    reverse_integral_bytes(std::uint8_t(0xc2))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes signed 16 bytes) {
  FATAL_EXPECT_EQ(
    std::int16_t(0x0000),
    reverse_integral_bytes(std::int16_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0x0100),
    reverse_integral_bytes(std::int16_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0x0a00),
    reverse_integral_bytes(std::int16_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0xc200),
    reverse_integral_bytes(std::int16_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0xf30c),
    reverse_integral_bytes(std::int16_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes unsigned 16 bytes) {
  FATAL_EXPECT_EQ(
    std::uint16_t(0x0000),
    reverse_integral_bytes(std::uint16_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0x0100),
    reverse_integral_bytes(std::uint16_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0x0a00),
    reverse_integral_bytes(std::uint16_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0xc200),
    reverse_integral_bytes(std::uint16_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0xf30c),
    reverse_integral_bytes(std::uint16_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes signed 32 bytes) {
  FATAL_EXPECT_EQ(
    std::int32_t(0x00000000),
    reverse_integral_bytes(std::int32_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0x01000000),
    reverse_integral_bytes(std::int32_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0x0a000000),
    reverse_integral_bytes(std::int32_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0xc2000000),
    reverse_integral_bytes(std::int32_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0xf30c0000),
    reverse_integral_bytes(std::int32_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes unsigned 32 bytes) {
  FATAL_EXPECT_EQ(
    std::uint32_t(0x00000000),
    reverse_integral_bytes(std::uint32_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0x01000000),
    reverse_integral_bytes(std::uint32_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0x0a000000),
    reverse_integral_bytes(std::uint32_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0xc2000000),
    reverse_integral_bytes(std::uint32_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0xf30c0000),
    reverse_integral_bytes(std::uint32_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes signed 64 bytes) {
  FATAL_EXPECT_EQ(
    std::int64_t(0x0000000000000000),
    reverse_integral_bytes(std::int64_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0x0100000000000000),
    reverse_integral_bytes(std::int64_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0x0a00000000000000),
    reverse_integral_bytes(std::int64_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0xc200000000000000),
    reverse_integral_bytes(std::int64_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0xf30c000000000000),
    reverse_integral_bytes(std::int64_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bytes unsigned 64 bytes) {
  FATAL_EXPECT_EQ(
    std::uint64_t(0x0000000000000000),
    reverse_integral_bytes(std::uint64_t(0x00))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0x0100000000000000),
    reverse_integral_bytes(std::uint64_t(0x01))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0x0a00000000000000),
    reverse_integral_bytes(std::uint64_t(0x0a))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0xc200000000000000),
    reverse_integral_bytes(std::uint64_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0xf30c000000000000),
    reverse_integral_bytes(std::uint64_t(0xcf3))
  );
}

template <typename T>
T naive_reverse_bytes(T value) {
  std::reverse(
    reinterpret_cast<char *>(std::addressof(value)),
    reinterpret_cast<char *>(std::next(std::addressof(value)))
  );
  return value;
}

FATAL_TEST(numerics, reverse_integral_bytes randomized) {
  random_data rng;

  for (auto i = 100000; i--; ) {
    auto const value = rng();

    FATAL_EXPECT_EQ(
      naive_reverse_bytes(signed_cast(value)),
      reverse_integral_bytes(signed_cast(value))
    );

    FATAL_EXPECT_EQ(
      naive_reverse_bytes(unsigned_cast(value)),
      reverse_integral_bytes(unsigned_cast(value))
    );
  }
}

///////////////////////////
// reverse_integral_bits //
///////////////////////////

FATAL_TEST(numerics, reverse_integral_bits signed 8 bits) {
  FATAL_EXPECT_EQ(std::int8_t(0x00), reverse_integral_bits(std::int8_t(0)));
  FATAL_EXPECT_EQ(std::int8_t(0x80), reverse_integral_bits(std::int8_t(1)));
  FATAL_EXPECT_EQ(std::int8_t(0x50), reverse_integral_bits(std::int8_t(10)));
  FATAL_EXPECT_EQ(std::int8_t(0x43), reverse_integral_bits(std::int8_t(0xc2)));
}

FATAL_TEST(numerics, reverse_integral_bits unsigned 8 bits) {
  FATAL_EXPECT_EQ(std::uint8_t(0x00), reverse_integral_bits(std::uint8_t(0)));
  FATAL_EXPECT_EQ(std::uint8_t(0x80), reverse_integral_bits(std::uint8_t(1)));
  FATAL_EXPECT_EQ(std::uint8_t(0x50), reverse_integral_bits(std::uint8_t(10)));
  FATAL_EXPECT_EQ(
    std::uint8_t(0x43),
    reverse_integral_bits(std::uint8_t(0xc2))
  );
}

FATAL_TEST(numerics, reverse_integral_bits signed 16 bits) {
  FATAL_EXPECT_EQ(std::int16_t(0x0000), reverse_integral_bits(std::int16_t(0)));
  FATAL_EXPECT_EQ(std::int16_t(0x8000), reverse_integral_bits(std::int16_t(1)));
  FATAL_EXPECT_EQ(
    std::int16_t(0x5000),
    reverse_integral_bits(std::int16_t(10))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0x4300),
    reverse_integral_bits(std::int16_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int16_t(0xcf30),
    reverse_integral_bits(std::int16_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bits unsigned 16 bits) {
  FATAL_EXPECT_EQ(
    std::uint16_t(0x0000),
    reverse_integral_bits(std::uint16_t(0))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0x8000),
    reverse_integral_bits(std::uint16_t(1))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0x5000),
    reverse_integral_bits(std::uint16_t(10))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0x4300),
    reverse_integral_bits(std::uint16_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint16_t(0xcf30),
    reverse_integral_bits(std::uint16_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bits signed 32 bits) {
  FATAL_EXPECT_EQ(
    std::int32_t(0x00000000),
    reverse_integral_bits(std::int32_t(0))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0x80000000),
    reverse_integral_bits(std::int32_t(1))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0x50000000),
    reverse_integral_bits(std::int32_t(10))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0x43000000),
    reverse_integral_bits(std::int32_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int32_t(0xcf300000),
    reverse_integral_bits(std::int32_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bits unsigned 32 bits) {
  FATAL_EXPECT_EQ(
    std::uint32_t(0x00000000),
    reverse_integral_bits(std::uint32_t(0))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0x80000000),
    reverse_integral_bits(std::uint32_t(1))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0x50000000),
    reverse_integral_bits(std::uint32_t(10))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0x43000000),
    reverse_integral_bits(std::uint32_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint32_t(0xcf300000),
    reverse_integral_bits(std::uint32_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bits signed 64 bits) {
  FATAL_EXPECT_EQ(
    std::int64_t(0x0000000000000000),
    reverse_integral_bits(std::int64_t(0))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0x8000000000000000),
    reverse_integral_bits(std::int64_t(1))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0x5000000000000000),
    reverse_integral_bits(std::int64_t(10))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0x4300000000000000),
    reverse_integral_bits(std::int64_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::int64_t(0xcf30000000000000),
    reverse_integral_bits(std::int64_t(0xcf3))
  );
}

FATAL_TEST(numerics, reverse_integral_bits unsigned 64 bits) {
  FATAL_EXPECT_EQ(
    std::uint64_t(0x0000000000000000),
    reverse_integral_bits(std::uint64_t(0))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0x8000000000000000),
    reverse_integral_bits(std::uint64_t(1))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0x5000000000000000),
    reverse_integral_bits(std::uint64_t(10))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0x4300000000000000),
    reverse_integral_bits(std::uint64_t(0xc2))
  );
  FATAL_EXPECT_EQ(
    std::uint64_t(0xcf30000000000000),
    reverse_integral_bits(std::uint64_t(0xcf3))
  );
}

template <typename T>
T naive_reverse_bits(T value) {
  T result = 0;

  for (auto i = data_bits<T>::value; i--; value >>= 1) {
    result = (result << 1) | (value & 1);
  }

  return result;
}

FATAL_TEST(numerics, reverse_integral_bits randomized) {
  random_data rng;

  for (auto i = 100000; i--; ) {
    auto const value = rng();

    FATAL_EXPECT_EQ(
      naive_reverse_bits(signed_cast(value)),
      reverse_integral_bits(signed_cast(value))
    );

    FATAL_EXPECT_EQ(
      naive_reverse_bits(unsigned_cast(value)),
      reverse_integral_bits(unsigned_cast(value))
    );
  }
}

/////////////////////////////////
// shift_left_count_upperbound //
/////////////////////////////////

FATAL_TEST(numerics, shift_left_count_upperbound) {
  FATAL_EXPECT_EQ(6, (shift_left_count_upperbound<int8_t>::value));
  FATAL_EXPECT_EQ(7, (shift_left_count_upperbound<uint8_t>::value));
  FATAL_EXPECT_EQ(5, (shift_left_count_upperbound<int8_t, 2>::value));
  FATAL_EXPECT_EQ(4, (shift_left_count_upperbound<uint8_t, 4>::value));
  FATAL_EXPECT_EQ(14, (shift_left_count_upperbound<int16_t>::value));
  FATAL_EXPECT_EQ(15, (shift_left_count_upperbound<uint16_t>::value));
}

///////////////////////////
// shift_left_upperbound //
///////////////////////////

#define CHECK_SHIFT_LEFT_UPPERBOUND(Type, Shift, Expected) \
  FATAL_EXPECT_EQ(Expected, (shift_left_upperbound<Type, Shift>::value))

FATAL_TEST(numerics, shift_left_upperbound) {
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 1, 64);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 2, 32);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 3, 16);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 4, 8);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 5, 4);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 6, 2);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int8_t, 7, 1);

  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 1, 128);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 2, 64);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 3, 32);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 4, 16);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 5, 8);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 6, 4);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 7, 2);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint8_t, 8, 1);

  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 1, 16384);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 2, 8192);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 3, 4096);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 4, 2048);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 5, 1024);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 6, 512);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 7, 256);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 8, 128);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 9, 64);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 10, 32);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 11, 16);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 12, 8);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 13, 4);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 14, 2);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::int16_t, 15, 1);

  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 1, 32768);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 2, 16384);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 3, 8192);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 4, 4096);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 5, 2048);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 6, 1024);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 7, 512);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 8, 256);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 9, 128);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 10, 64);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 11, 32);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 12, 16);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 13, 8);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 14, 4);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 15, 2);
  CHECK_SHIFT_LEFT_UPPERBOUND(std::uint16_t, 16, 1);
}

//////////////////////////
// most_significant_bit //
//////////////////////////

FATAL_TEST(numerics, most_significant_bit) {
  FATAL_EXPECT_EQ(0, most_significant_bit<0>::value);
  FATAL_EXPECT_EQ(1, most_significant_bit<1>::value);
  FATAL_EXPECT_EQ(2, most_significant_bit<2>::value);
  FATAL_EXPECT_EQ(2, most_significant_bit<3>::value);
  FATAL_EXPECT_EQ(3, most_significant_bit<7>::value);
  FATAL_EXPECT_EQ(4, most_significant_bit<8>::value);
  FATAL_EXPECT_EQ(4, most_significant_bit<15>::value);
  FATAL_EXPECT_EQ(5, most_significant_bit<16>::value);
  FATAL_EXPECT_EQ(5, most_significant_bit<31>::value);
  FATAL_EXPECT_EQ(6, most_significant_bit<32>::value);
  FATAL_EXPECT_EQ(6, most_significant_bit<63>::value);
  FATAL_EXPECT_EQ(7, most_significant_bit<64>::value);
  FATAL_EXPECT_EQ(7, most_significant_bit<127>::value);
  FATAL_EXPECT_EQ(10, most_significant_bit<997>::value);
  FATAL_EXPECT_EQ(16, most_significant_bit<65535>::value);
  FATAL_EXPECT_EQ(17, most_significant_bit<65536>::value);

  FATAL_EXPECT_EQ(0, most_significant_bit<0ull>::value);
  FATAL_EXPECT_EQ(1, most_significant_bit<1ull>::value);
  FATAL_EXPECT_EQ(2, most_significant_bit<2ull>::value);
  FATAL_EXPECT_EQ(2, most_significant_bit<3ull>::value);
  FATAL_EXPECT_EQ(3, most_significant_bit<7ull>::value);
  FATAL_EXPECT_EQ(4, most_significant_bit<8ull>::value);
  FATAL_EXPECT_EQ(4, most_significant_bit<15ull>::value);
  FATAL_EXPECT_EQ(5, most_significant_bit<16ull>::value);
  FATAL_EXPECT_EQ(5, most_significant_bit<31ull>::value);
  FATAL_EXPECT_EQ(6, most_significant_bit<32ull>::value);
  FATAL_EXPECT_EQ(6, most_significant_bit<63ull>::value);
  FATAL_EXPECT_EQ(7, most_significant_bit<64ull>::value);
  FATAL_EXPECT_EQ(7, most_significant_bit<127ull>::value);
  FATAL_EXPECT_EQ(10, most_significant_bit<997ull>::value);
  FATAL_EXPECT_EQ(16, most_significant_bit<65535ull>::value);
  FATAL_EXPECT_EQ(17, most_significant_bit<65536ull>::value);
}

/////////////////////////////////
// smallest integral & friends //
/////////////////////////////////

#define SEL_TEST(Expected, Selector, N) \
  FATAL_EXPECT_EQ(data_bits<Expected>::value, data_bits<Selector<N>>::value)

#define LEAST_TEST(Expected, Selector, N) \
  FATAL_EXPECT_LE(sizeof(Expected), sizeof(Selector<N>))

FATAL_TEST(numerics, smallest_signed_integral) {
  SEL_TEST(int8_t, smallest_signed_integral, 0);
  SEL_TEST(int8_t, smallest_signed_integral, 1);
  SEL_TEST(int8_t, smallest_signed_integral, 2);
  SEL_TEST(int8_t, smallest_signed_integral, 7);
  SEL_TEST(int8_t, smallest_signed_integral, 8);
  SEL_TEST(int16_t, smallest_signed_integral, 9);
  SEL_TEST(int16_t, smallest_signed_integral, 15);
  SEL_TEST(int16_t, smallest_signed_integral, 16);
  SEL_TEST(int32_t, smallest_signed_integral, 17);
  SEL_TEST(int32_t, smallest_signed_integral, 31);
  SEL_TEST(int32_t, smallest_signed_integral, 32);
  SEL_TEST(int64_t, smallest_signed_integral, 33);
  SEL_TEST(int64_t, smallest_signed_integral, 63);
  SEL_TEST(int64_t, smallest_signed_integral, 64);
}

FATAL_TEST(numerics, smallest_fast_signed_integral) {
  LEAST_TEST(int_fast8_t, smallest_fast_signed_integral, 0);
  LEAST_TEST(int_fast8_t, smallest_fast_signed_integral, 1);
  LEAST_TEST(int_fast8_t, smallest_fast_signed_integral, 2);
  LEAST_TEST(int_fast8_t, smallest_fast_signed_integral, 7);
  LEAST_TEST(int_fast8_t, smallest_fast_signed_integral, 8);
  LEAST_TEST(int_fast16_t, smallest_fast_signed_integral, 9);
  LEAST_TEST(int_fast16_t, smallest_fast_signed_integral, 15);
  LEAST_TEST(int_fast16_t, smallest_fast_signed_integral, 16);
  LEAST_TEST(int_fast32_t, smallest_fast_signed_integral, 17);
  LEAST_TEST(int_fast32_t, smallest_fast_signed_integral, 31);
  LEAST_TEST(int_fast32_t, smallest_fast_signed_integral, 32);
  LEAST_TEST(int_fast64_t, smallest_fast_signed_integral, 33);
  LEAST_TEST(int_fast64_t, smallest_fast_signed_integral, 63);
  LEAST_TEST(int_fast64_t, smallest_fast_signed_integral, 64);
}

FATAL_TEST(numerics, smallest_least_signed_integral) {
  LEAST_TEST(int_least8_t, smallest_least_signed_integral, 0);
  LEAST_TEST(int_least8_t, smallest_least_signed_integral, 1);
  LEAST_TEST(int_least8_t, smallest_least_signed_integral, 2);
  LEAST_TEST(int_least8_t, smallest_least_signed_integral, 7);
  LEAST_TEST(int_least8_t, smallest_least_signed_integral, 8);
  LEAST_TEST(int_least16_t, smallest_least_signed_integral, 9);
  LEAST_TEST(int_least16_t, smallest_least_signed_integral, 15);
  LEAST_TEST(int_least16_t, smallest_least_signed_integral, 16);
  LEAST_TEST(int_least32_t, smallest_least_signed_integral, 17);
  LEAST_TEST(int_least32_t, smallest_least_signed_integral, 31);
  LEAST_TEST(int_least32_t, smallest_least_signed_integral, 32);
  LEAST_TEST(int_least64_t, smallest_least_signed_integral, 33);
  LEAST_TEST(int_least64_t, smallest_least_signed_integral, 63);
  LEAST_TEST(int_least64_t, smallest_least_signed_integral, 64);
}

FATAL_TEST(numerics, smallest_unsigned_integral) {
  SEL_TEST(bool, smallest_unsigned_integral, 0);
  SEL_TEST(bool, smallest_unsigned_integral, 1);
  SEL_TEST(uint8_t, smallest_unsigned_integral, 2);
  SEL_TEST(uint8_t, smallest_unsigned_integral, 7);
  SEL_TEST(uint8_t, smallest_unsigned_integral, 8);
  SEL_TEST(uint16_t, smallest_unsigned_integral, 9);
  SEL_TEST(uint16_t, smallest_unsigned_integral, 15);
  SEL_TEST(uint16_t, smallest_unsigned_integral, 16);
  SEL_TEST(uint32_t, smallest_unsigned_integral, 17);
  SEL_TEST(uint32_t, smallest_unsigned_integral, 31);
  SEL_TEST(uint32_t, smallest_unsigned_integral, 32);
  SEL_TEST(uint64_t, smallest_unsigned_integral, 33);
  SEL_TEST(uint64_t, smallest_unsigned_integral, 63);
  SEL_TEST(uint64_t, smallest_unsigned_integral, 64);
}

FATAL_TEST(numerics, smallest_fast_unsigned_integral) {
  LEAST_TEST(bool, smallest_fast_unsigned_integral, 0);
  LEAST_TEST(bool, smallest_fast_unsigned_integral, 1);
  LEAST_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 2);
  LEAST_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 7);
  LEAST_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 8);
  LEAST_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 9);
  LEAST_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 15);
  LEAST_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 16);
  LEAST_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 17);
  LEAST_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 31);
  LEAST_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 32);
  LEAST_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 33);
  LEAST_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 63);
  LEAST_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 64);
}

FATAL_TEST(numerics, smallest_least_unsigned_integral) {
  LEAST_TEST(bool, smallest_least_unsigned_integral, 0);
  LEAST_TEST(bool, smallest_least_unsigned_integral, 1);
  LEAST_TEST(uint_least8_t, smallest_least_unsigned_integral, 2);
  LEAST_TEST(uint_least8_t, smallest_least_unsigned_integral, 7);
  LEAST_TEST(uint_least8_t, smallest_least_unsigned_integral, 8);
  LEAST_TEST(uint_least16_t, smallest_least_unsigned_integral, 9);
  LEAST_TEST(uint_least16_t, smallest_least_unsigned_integral, 15);
  LEAST_TEST(uint_least16_t, smallest_least_unsigned_integral, 16);
  LEAST_TEST(uint_least32_t, smallest_least_unsigned_integral, 17);
  LEAST_TEST(uint_least32_t, smallest_least_unsigned_integral, 31);
  LEAST_TEST(uint_least32_t, smallest_least_unsigned_integral, 32);
  LEAST_TEST(uint_least64_t, smallest_least_unsigned_integral, 33);
  LEAST_TEST(uint_least64_t, smallest_least_unsigned_integral, 63);
  LEAST_TEST(uint_least64_t, smallest_least_unsigned_integral, 64);
}

#undef SEL_TEST

#define SMALLEST_FOR_VALUE_TEST(N, Expected) \
  FATAL_VLOG(1) << "expected = " << #Expected << ", actual = " \
    << ::fatal::type_str<smallest_uint_for_value<N>>(); \
  FATAL_EXPECT_EQ( \
    data_bits<Expected>::value, data_bits<smallest_uint_for_value<N>>::value \
  )

FATAL_TEST(numerics, smallest_uint_for_value) {
  SMALLEST_FOR_VALUE_TEST(0u, bool);
  SMALLEST_FOR_VALUE_TEST(1, bool);
  SMALLEST_FOR_VALUE_TEST(2, uint8_t);
  SMALLEST_FOR_VALUE_TEST(uint8_t(~0ull), uint8_t);
  SMALLEST_FOR_VALUE_TEST((1ull << 8), uint16_t);
  SMALLEST_FOR_VALUE_TEST(uint16_t(~0ull), uint16_t);
  SMALLEST_FOR_VALUE_TEST((1ull << 16), uint32_t);
  SMALLEST_FOR_VALUE_TEST(uint32_t(~0ull), uint32_t);
  SMALLEST_FOR_VALUE_TEST((1ull << 32), uint64_t);
  SMALLEST_FOR_VALUE_TEST(uint64_t(~0ull), uint64_t);
}

#undef SMALLEST_FOR_VALUE_TEST

/////////////////
// multiply_mp //
/////////////////

FATAL_TEST(numerics, multiply_mp) {
  FATAL_EXPECT_EQ(0, (multiply_mp<0, 0>::value));
  FATAL_EXPECT_EQ(0, (multiply_mp<0, 1>::value));
  FATAL_EXPECT_EQ(0, (multiply_mp<1, 0>::value));
  FATAL_EXPECT_EQ(1, (multiply_mp<1, 1>::value));
  FATAL_EXPECT_EQ(0, (multiply_mp<999, 0>::value));
  FATAL_EXPECT_EQ(999, (multiply_mp<1, 999>::value));
  FATAL_EXPECT_EQ(999, (multiply_mp<999, 1>::value));
  FATAL_EXPECT_EQ(998001, (multiply_mp<999, 999>::value));
  FATAL_EXPECT_EQ(18, (multiply_mp<9, 2>::value));
  FATAL_EXPECT_EQ(8, (multiply_mp<4, 2>::value));
  FATAL_EXPECT_EQ(8, (multiply_mp<2, 4>::value));
  FATAL_EXPECT_EQ(12, (multiply_mp<3, 4>::value));
  FATAL_EXPECT_EQ(100000000, (multiply_mp<10000, 10000>::value));
  FATAL_EXPECT_EQ(4096, (multiply_mp<64, 64>::value));
}

///////////////
// square_mp //
///////////////

FATAL_TEST(numerics, square_mp) {
  FATAL_EXPECT_EQ(0, (square_mp<0>::value));
  FATAL_EXPECT_EQ(1, (square_mp<1>::value));
  FATAL_EXPECT_EQ(4, (square_mp<2>::value));
  FATAL_EXPECT_EQ(9, (square_mp<3>::value));
  FATAL_EXPECT_EQ(256, (square_mp<16>::value));
  FATAL_EXPECT_EQ(4096, (square_mp<64>::value));
  FATAL_EXPECT_EQ(998001, (square_mp<999>::value));
  FATAL_EXPECT_EQ(100000000, (square_mp<10000>::value));
}

////////////
// pow_mp //
////////////

FATAL_TEST(numerics, pow_mp) {
  FATAL_EXPECT_EQ(1, (pow_mp<0, 0>::value));
  FATAL_EXPECT_EQ(1, (pow_mp<1, 0>::value));
  FATAL_EXPECT_EQ(0, (pow_mp<0, 1>::value));
  FATAL_EXPECT_EQ(0, (pow_mp<0, 999>::value));
  FATAL_EXPECT_EQ(1, (pow_mp<999, 0>::value));
  FATAL_EXPECT_EQ(999, (pow_mp<999, 1>::value));
  FATAL_EXPECT_EQ(998001, (pow_mp<999, 2>::value));
  FATAL_EXPECT_EQ(81, (pow_mp<9, 2>::value));
  FATAL_EXPECT_EQ(16, (pow_mp<4, 2>::value));
  FATAL_EXPECT_EQ(16, (pow_mp<2, 4>::value));
  FATAL_EXPECT_EQ(81, (pow_mp<3, 4>::value));
  FATAL_EXPECT_EQ(10000000000, (pow_mp<10, 10>::value));
  FATAL_EXPECT_EQ(1073741824, (pow_mp<2, 30>::value));
}

FATAL_TEST(numerics, unchecked_pow_mp) {
  FATAL_EXPECT_EQ(1, (unchecked_pow_mp<0, 0>::value));
  FATAL_EXPECT_EQ(1, (unchecked_pow_mp<1, 0>::value));
  FATAL_EXPECT_EQ(0, (unchecked_pow_mp<0, 1>::value));
  FATAL_EXPECT_EQ(0, (unchecked_pow_mp<0, 999>::value));
  FATAL_EXPECT_EQ(1, (unchecked_pow_mp<999, 0>::value));
  FATAL_EXPECT_EQ(999, (unchecked_pow_mp<999, 1>::value));
  FATAL_EXPECT_EQ(998001, (unchecked_pow_mp<999, 2>::value));
  FATAL_EXPECT_EQ(81, (unchecked_pow_mp<9, 2>::value));
  FATAL_EXPECT_EQ(16, (unchecked_pow_mp<4, 2>::value));
  FATAL_EXPECT_EQ(16, (unchecked_pow_mp<2, 4>::value));
  FATAL_EXPECT_EQ(81, (unchecked_pow_mp<3, 4>::value));
  FATAL_EXPECT_EQ(10000000000, (unchecked_pow_mp<10, 10>::value));
  FATAL_EXPECT_EQ(1073741824, (unchecked_pow_mp<2, 30>::value));
}

FATAL_TEST(numerics, unchecked_pow_mp_overflows) {
  // Make sure overflow works. Wolfram Alpha ftw.
  FATAL_EXPECT_EQ(6203307696791771937, (unchecked_pow_mp<3, 1000>::value));
  FATAL_EXPECT_EQ(0, (unchecked_pow_mp<10, 1000>::value));
}

/////////////////////
// is_power_of_two //
/////////////////////

FATAL_TEST(numerics, is_power_of_two) {
  FATAL_EXPECT_TRUE(is_power_of_two(1));
  FATAL_EXPECT_TRUE(is_power_of_two(2));
  FATAL_EXPECT_TRUE(is_power_of_two(4));
  FATAL_EXPECT_TRUE(is_power_of_two(8));
  FATAL_EXPECT_TRUE(is_power_of_two(16));
  FATAL_EXPECT_TRUE(is_power_of_two(32));
  FATAL_EXPECT_TRUE(is_power_of_two(64));
  FATAL_EXPECT_TRUE(is_power_of_two(128));
  FATAL_EXPECT_TRUE(is_power_of_two(256));
  FATAL_EXPECT_TRUE(is_power_of_two(512));
  FATAL_EXPECT_TRUE(is_power_of_two(1024));
  FATAL_EXPECT_TRUE(is_power_of_two(2048));
  FATAL_EXPECT_TRUE(is_power_of_two(4096));
  FATAL_EXPECT_TRUE(is_power_of_two(8192));
  FATAL_EXPECT_TRUE(is_power_of_two(16384));
  FATAL_EXPECT_TRUE(is_power_of_two(32768));
  FATAL_EXPECT_TRUE(is_power_of_two(65536));
  FATAL_EXPECT_TRUE(is_power_of_two(131072));
  FATAL_EXPECT_TRUE(is_power_of_two(262144));
  FATAL_EXPECT_TRUE(is_power_of_two(524288));
  FATAL_EXPECT_TRUE(is_power_of_two(1048576));

  FATAL_EXPECT_FALSE(is_power_of_two(0));
  FATAL_EXPECT_FALSE(is_power_of_two(3));
  FATAL_EXPECT_FALSE(is_power_of_two(5));
  FATAL_EXPECT_FALSE(is_power_of_two(7));
  FATAL_EXPECT_FALSE(is_power_of_two(9));
  FATAL_EXPECT_FALSE(is_power_of_two(15));
  FATAL_EXPECT_FALSE(is_power_of_two(17));
  FATAL_EXPECT_FALSE(is_power_of_two(31));
  FATAL_EXPECT_FALSE(is_power_of_two(33));
  FATAL_EXPECT_FALSE(is_power_of_two(63));
  FATAL_EXPECT_FALSE(is_power_of_two(65));
  FATAL_EXPECT_FALSE(is_power_of_two(127));
  FATAL_EXPECT_FALSE(is_power_of_two(129));
  FATAL_EXPECT_FALSE(is_power_of_two(255));
  FATAL_EXPECT_FALSE(is_power_of_two(257));
  FATAL_EXPECT_FALSE(is_power_of_two(511));
  FATAL_EXPECT_FALSE(is_power_of_two(513));
  FATAL_EXPECT_FALSE(is_power_of_two(1023));
  FATAL_EXPECT_FALSE(is_power_of_two(1025));
  FATAL_EXPECT_FALSE(is_power_of_two(2047));
  FATAL_EXPECT_FALSE(is_power_of_two(2049));
  FATAL_EXPECT_FALSE(is_power_of_two(4095));
  FATAL_EXPECT_FALSE(is_power_of_two(4097));
  FATAL_EXPECT_FALSE(is_power_of_two(8191));
  FATAL_EXPECT_FALSE(is_power_of_two(8193));
  FATAL_EXPECT_FALSE(is_power_of_two(16383));
  FATAL_EXPECT_FALSE(is_power_of_two(16385));
  FATAL_EXPECT_FALSE(is_power_of_two(32767));
  FATAL_EXPECT_FALSE(is_power_of_two(32769));
  FATAL_EXPECT_FALSE(is_power_of_two(65535));
  FATAL_EXPECT_FALSE(is_power_of_two(65537));
  FATAL_EXPECT_FALSE(is_power_of_two(131071));
  FATAL_EXPECT_FALSE(is_power_of_two(131073));
  FATAL_EXPECT_FALSE(is_power_of_two(262143));
  FATAL_EXPECT_FALSE(is_power_of_two(262145));
  FATAL_EXPECT_FALSE(is_power_of_two(524287));
  FATAL_EXPECT_FALSE(is_power_of_two(524289));
  FATAL_EXPECT_FALSE(is_power_of_two(1048575));
  FATAL_EXPECT_FALSE(is_power_of_two(1048577));
}

////////////////////////
// is_mersenne_number //
////////////////////////

FATAL_TEST(numerics, is_mersenne_number) {
  FATAL_EXPECT_TRUE(is_mersenne_number(1));
  FATAL_EXPECT_TRUE(is_mersenne_number(3));
  FATAL_EXPECT_TRUE(is_mersenne_number(7));
  FATAL_EXPECT_TRUE(is_mersenne_number(15));
  FATAL_EXPECT_TRUE(is_mersenne_number(31));
  FATAL_EXPECT_TRUE(is_mersenne_number(63));
  FATAL_EXPECT_TRUE(is_mersenne_number(127));
  FATAL_EXPECT_TRUE(is_mersenne_number(255));
  FATAL_EXPECT_TRUE(is_mersenne_number(511));
  FATAL_EXPECT_TRUE(is_mersenne_number(1023));
  FATAL_EXPECT_TRUE(is_mersenne_number(2047));
  FATAL_EXPECT_TRUE(is_mersenne_number(4095));
  FATAL_EXPECT_TRUE(is_mersenne_number(8191));
  FATAL_EXPECT_TRUE(is_mersenne_number(16383));
  FATAL_EXPECT_TRUE(is_mersenne_number(32767));
  FATAL_EXPECT_TRUE(is_mersenne_number(65535));
  FATAL_EXPECT_TRUE(is_mersenne_number(131071));
  FATAL_EXPECT_TRUE(is_mersenne_number(262143));
  FATAL_EXPECT_TRUE(is_mersenne_number(524287));
  FATAL_EXPECT_TRUE(is_mersenne_number(1048575));

  FATAL_EXPECT_FALSE(is_mersenne_number(0));
  FATAL_EXPECT_FALSE(is_mersenne_number(2));
  FATAL_EXPECT_FALSE(is_mersenne_number(4));
  FATAL_EXPECT_FALSE(is_mersenne_number(8));
  FATAL_EXPECT_FALSE(is_mersenne_number(16));
  FATAL_EXPECT_FALSE(is_mersenne_number(32));
  FATAL_EXPECT_FALSE(is_mersenne_number(64));
  FATAL_EXPECT_FALSE(is_mersenne_number(128));
  FATAL_EXPECT_FALSE(is_mersenne_number(256));
  FATAL_EXPECT_FALSE(is_mersenne_number(512));
  FATAL_EXPECT_FALSE(is_mersenne_number(1024));
  FATAL_EXPECT_FALSE(is_mersenne_number(2048));
  FATAL_EXPECT_FALSE(is_mersenne_number(4096));
  FATAL_EXPECT_FALSE(is_mersenne_number(8192));
  FATAL_EXPECT_FALSE(is_mersenne_number(16384));
  FATAL_EXPECT_FALSE(is_mersenne_number(32768));
  FATAL_EXPECT_FALSE(is_mersenne_number(65536));
  FATAL_EXPECT_FALSE(is_mersenne_number(131072));
  FATAL_EXPECT_FALSE(is_mersenne_number(262144));
  FATAL_EXPECT_FALSE(is_mersenne_number(524288));
  FATAL_EXPECT_FALSE(is_mersenne_number(1048576));

  FATAL_EXPECT_FALSE(is_mersenne_number(5));
  FATAL_EXPECT_FALSE(is_mersenne_number(9));
  FATAL_EXPECT_FALSE(is_mersenne_number(17));
  FATAL_EXPECT_FALSE(is_mersenne_number(33));
  FATAL_EXPECT_FALSE(is_mersenne_number(65));
  FATAL_EXPECT_FALSE(is_mersenne_number(129));
  FATAL_EXPECT_FALSE(is_mersenne_number(257));
  FATAL_EXPECT_FALSE(is_mersenne_number(513));
  FATAL_EXPECT_FALSE(is_mersenne_number(1025));
  FATAL_EXPECT_FALSE(is_mersenne_number(2049));
  FATAL_EXPECT_FALSE(is_mersenne_number(4097));
  FATAL_EXPECT_FALSE(is_mersenne_number(8193));
  FATAL_EXPECT_FALSE(is_mersenne_number(16385));
  FATAL_EXPECT_FALSE(is_mersenne_number(32769));
  FATAL_EXPECT_FALSE(is_mersenne_number(65537));
  FATAL_EXPECT_FALSE(is_mersenne_number(131073));
  FATAL_EXPECT_FALSE(is_mersenne_number(262145));
  FATAL_EXPECT_FALSE(is_mersenne_number(524289));
  FATAL_EXPECT_FALSE(is_mersenne_number(1048577));

  // We want specifically int32 because we want to check for
  // overflow since 2^32-1 is int32_t::max
  FATAL_ASSERT_EQ(32, (data_bits<int32_t>::value));
  FATAL_EXPECT_TRUE(
    (is_mersenne_number<int32_t>(std::numeric_limits<int32_t>::max()))
  );

  // same for other types
  FATAL_ASSERT_EQ(8, (data_bits<int8_t>::value));
  FATAL_EXPECT_TRUE(
    (is_mersenne_number<int8_t>(std::numeric_limits<int8_t>::max()))
  );
  FATAL_ASSERT_EQ(16, (data_bits<int16_t>::value));
  FATAL_EXPECT_TRUE(
    (is_mersenne_number<int16_t>(std::numeric_limits<int16_t>::max()))
  );
  FATAL_ASSERT_EQ(64, (data_bits<int64_t>::value));
  FATAL_EXPECT_TRUE(
    (is_mersenne_number<int64_t>(std::numeric_limits<int64_t>::max()))
  );
}

template <std::size_t nth, std::size_t exponent, std::size_t value>
void check_mersenne_prime() {
  using mp = nth_mersenne_prime<nth>;
  FATAL_EXPECT_EQ(exponent, mersenne_number_exponent<mp>::value);
  FATAL_EXPECT_EQ(value, mp::value);
}

FATAL_TEST(numerics, mersenne_prime) {
  check_mersenne_prime<0, 2, 3>();
  check_mersenne_prime<1, 3, 7>();
  check_mersenne_prime<2, 5, 31>();
  check_mersenne_prime<3, 7, 127>();
  check_mersenne_prime<4, 13, 8191>();
  check_mersenne_prime<5, 17, 131071>();
  check_mersenne_prime<6, 19, 524287>();
  check_mersenne_prime<7, 31, 2147483647>();
  check_mersenne_prime<8, 61, 2305843009213693951ull>();
  FATAL_EXPECT_EQ(9, size<mersenne_primes>::value);
}

template <std::size_t bits_size, std::size_t nth_mersenne>
void check_largest_mersenne_prime_under() {
  using expected = nth_mersenne_prime<nth_mersenne>;
  using actual = largest_mersenne_prime_under<bits_size>;

  FATAL_EXPECT_EQ(expected::value, actual::value);
}

FATAL_TEST(numerics, largest_mersenne_prime_under) {
  check_largest_mersenne_prime_under<2, 0>();
  check_largest_mersenne_prime_under<3, 1>();
  check_largest_mersenne_prime_under<4, 1>();
  check_largest_mersenne_prime_under<5, 2>();
  check_largest_mersenne_prime_under<6, 2>();
  check_largest_mersenne_prime_under<7, 3>();
  check_largest_mersenne_prime_under<8, 3>();
  check_largest_mersenne_prime_under<9, 3>();
  check_largest_mersenne_prime_under<10, 3>();
  check_largest_mersenne_prime_under<11, 3>();
  check_largest_mersenne_prime_under<12, 3>();
  check_largest_mersenne_prime_under<13, 4>();
  check_largest_mersenne_prime_under<14, 4>();
  check_largest_mersenne_prime_under<15, 4>();
  check_largest_mersenne_prime_under<16, 4>();
  check_largest_mersenne_prime_under<17, 5>();
  check_largest_mersenne_prime_under<18, 5>();
  check_largest_mersenne_prime_under<19, 6>();
  check_largest_mersenne_prime_under<20, 6>();
  check_largest_mersenne_prime_under<21, 6>();
  check_largest_mersenne_prime_under<22, 6>();
  check_largest_mersenne_prime_under<23, 6>();
  check_largest_mersenne_prime_under<24, 6>();
  check_largest_mersenne_prime_under<25, 6>();
  check_largest_mersenne_prime_under<26, 6>();
  check_largest_mersenne_prime_under<27, 6>();
  check_largest_mersenne_prime_under<28, 6>();
  check_largest_mersenne_prime_under<29, 6>();
  check_largest_mersenne_prime_under<30, 6>();
  check_largest_mersenne_prime_under<31, 7>();
  check_largest_mersenne_prime_under<32, 7>();
  check_largest_mersenne_prime_under<33, 7>();
  check_largest_mersenne_prime_under<34, 7>();
  check_largest_mersenne_prime_under<35, 7>();
  check_largest_mersenne_prime_under<36, 7>();
  check_largest_mersenne_prime_under<37, 7>();
  check_largest_mersenne_prime_under<38, 7>();
  check_largest_mersenne_prime_under<39, 7>();
  check_largest_mersenne_prime_under<40, 7>();
  check_largest_mersenne_prime_under<41, 7>();
  check_largest_mersenne_prime_under<42, 7>();
  check_largest_mersenne_prime_under<43, 7>();
  check_largest_mersenne_prime_under<44, 7>();
  check_largest_mersenne_prime_under<45, 7>();
  check_largest_mersenne_prime_under<46, 7>();
  check_largest_mersenne_prime_under<47, 7>();
  check_largest_mersenne_prime_under<48, 7>();
  check_largest_mersenne_prime_under<49, 7>();
  check_largest_mersenne_prime_under<50, 7>();
  check_largest_mersenne_prime_under<51, 7>();
  check_largest_mersenne_prime_under<52, 7>();
  check_largest_mersenne_prime_under<53, 7>();
  check_largest_mersenne_prime_under<54, 7>();
  check_largest_mersenne_prime_under<55, 7>();
  check_largest_mersenne_prime_under<56, 7>();
  check_largest_mersenne_prime_under<57, 7>();
  check_largest_mersenne_prime_under<58, 7>();
  check_largest_mersenne_prime_under<59, 7>();
  check_largest_mersenne_prime_under<60, 7>();
  check_largest_mersenne_prime_under<61, 8>();
  check_largest_mersenne_prime_under<62, 8>();
  check_largest_mersenne_prime_under<63, 8>();
  check_largest_mersenne_prime_under<64, 8>();
}

template <typename T, std::size_t diff, std::size_t nth_mersenne>
void check_largest_mersenne_prime_for_type() {
  using expected = nth_mersenne_prime<nth_mersenne>;
  using actual = largest_mersenne_prime_for_type<T, diff>;

  FATAL_EXPECT_EQ(expected::value, actual::value);
}

FATAL_TEST(numerics, largest_mersenne_prime_for_type) {
  check_largest_mersenne_prime_for_type<std::int8_t, 0, 3>();
  check_largest_mersenne_prime_for_type<std::int8_t, 1, 2>();
  check_largest_mersenne_prime_for_type<std::int8_t, 2, 2>();
  check_largest_mersenne_prime_for_type<std::int8_t, 3, 1>();
  check_largest_mersenne_prime_for_type<std::int8_t, 4, 1>();
  check_largest_mersenne_prime_for_type<std::int8_t, 5, 0>();

  check_largest_mersenne_prime_for_type<std::uint8_t, 0, 3>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 1, 3>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 2, 2>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 3, 2>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 4, 1>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 5, 1>();
  check_largest_mersenne_prime_for_type<std::uint8_t, 6, 0>();

  check_largest_mersenne_prime_for_type<std::int16_t, 0, 4>();
  check_largest_mersenne_prime_for_type<std::int16_t, 1, 4>();
  check_largest_mersenne_prime_for_type<std::int16_t, 2, 4>();
  check_largest_mersenne_prime_for_type<std::int16_t, 3, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 4, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 5, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 6, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 7, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 8, 3>();
  check_largest_mersenne_prime_for_type<std::int16_t, 9, 2>();
  check_largest_mersenne_prime_for_type<std::int16_t, 10, 2>();
  check_largest_mersenne_prime_for_type<std::int16_t, 11, 1>();
  check_largest_mersenne_prime_for_type<std::int16_t, 12, 1>();
  check_largest_mersenne_prime_for_type<std::int16_t, 13, 0>();

  check_largest_mersenne_prime_for_type<std::uint16_t, 0, 4>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 1, 4>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 2, 4>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 3, 4>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 4, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 5, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 6, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 7, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 8, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 9, 3>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 10, 2>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 11, 2>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 12, 1>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 13, 1>();
  check_largest_mersenne_prime_for_type<std::uint16_t, 14, 0>();

  check_largest_mersenne_prime_for_type<std::int32_t, 0, 7>();
  check_largest_mersenne_prime_for_type<std::uint32_t, 0, 7>();

  check_largest_mersenne_prime_for_type<std::int64_t, 0, 8>();
  check_largest_mersenne_prime_for_type<std::uint64_t, 0, 8>();
}

////////////////////////////
// DISCRETE_TO_CONTINUOUS //
////////////////////////////

FATAL_TEST(discrete_to_continuous, sanity_check) {
  std::mt19937 rng(std::random_device{}());

  auto const dmin = 11;
  auto const dmax = 9999;
  discrete_to_continuous<unsigned, double> conv(dmin, dmax, -5.5, 5.5);

  auto const drange = dmax - dmin;
  auto const crange = conv.max() - conv.min();
  for(auto i = 100000; i--; ) {
    auto n = dmin + (static_cast<unsigned>(rng()) % drange);
    auto expected = static_cast<double>(n - dmin)
      / drange * crange + conv.min();
    FATAL_EXPECT_EQ(expected, conv(n));
  }
}

} // namespace fatal {
