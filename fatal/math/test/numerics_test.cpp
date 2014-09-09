/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/math/numerics.h>

#include <fatal/test/driver.h>

#include <folly/String.h>
#include <folly/Random.h>

namespace ftl {

///////////////
// data_bits //
///////////////

TEST(numerics, data_bits) {
  EXPECT_EQ(1, data_bits<bool>::value);
  EXPECT_EQ(CHAR_BIT, data_bits<char>::value);
  EXPECT_EQ(CHAR_BIT, data_bits<int8_t>::value);
  EXPECT_EQ(2 * CHAR_BIT, data_bits<int16_t>::value);
  EXPECT_EQ(4 * CHAR_BIT, data_bits<int32_t>::value);
}

/////////////////////////////////
// shift_left_count_upperbound //
/////////////////////////////////

TEST(numerics, shift_left_count_upperbound) {
  EXPECT_EQ(6, (shift_left_count_upperbound<int8_t>::value));
  EXPECT_EQ(7, (shift_left_count_upperbound<uint8_t>::value));
  EXPECT_EQ(5, (shift_left_count_upperbound<int8_t, 2>::value));
  EXPECT_EQ(4, (shift_left_count_upperbound<uint8_t, 4>::value));
  EXPECT_EQ(14, (shift_left_count_upperbound<int16_t>::value));
  EXPECT_EQ(15, (shift_left_count_upperbound<uint16_t>::value));
}

///////////////////////////
// shift_left_upperbound //
///////////////////////////

#define CHECK_SHIFT_LEFT_UPPERBOUND(Type, Shift, Expected) \
  EXPECT_EQ(Expected, (shift_left_upperbound<Type, Shift>::value))

TEST(numerics, shift_left_upperbound) {
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

/////////////////////////////
// most_significant_bit_mp //
/////////////////////////////

TEST(numerics, most_significant_bit_mp) {
  EXPECT_EQ(0, most_significant_bit_mp<0>());
  EXPECT_EQ(1, most_significant_bit_mp<1>());
  EXPECT_EQ(2, most_significant_bit_mp<2>());
  EXPECT_EQ(2, most_significant_bit_mp<3>());
  EXPECT_EQ(3, most_significant_bit_mp<7>());
  EXPECT_EQ(4, most_significant_bit_mp<8>());
  EXPECT_EQ(4, most_significant_bit_mp<15>());
  EXPECT_EQ(5, most_significant_bit_mp<16>());
  EXPECT_EQ(5, most_significant_bit_mp<31>());
  EXPECT_EQ(6, most_significant_bit_mp<32>());
  EXPECT_EQ(6, most_significant_bit_mp<63>());
  EXPECT_EQ(7, most_significant_bit_mp<64>());
  EXPECT_EQ(7, most_significant_bit_mp<127>());
  EXPECT_EQ(10, most_significant_bit_mp<997>());
  EXPECT_EQ(16, most_significant_bit_mp<65535>());
  EXPECT_EQ(17, most_significant_bit_mp<65536>());

  EXPECT_EQ(0, most_significant_bit_mp<0ull>());
  EXPECT_EQ(1, most_significant_bit_mp<1ull>());
  EXPECT_EQ(2, most_significant_bit_mp<2ull>());
  EXPECT_EQ(2, most_significant_bit_mp<3ull>());
  EXPECT_EQ(3, most_significant_bit_mp<7ull>());
  EXPECT_EQ(4, most_significant_bit_mp<8ull>());
  EXPECT_EQ(4, most_significant_bit_mp<15ull>());
  EXPECT_EQ(5, most_significant_bit_mp<16ull>());
  EXPECT_EQ(5, most_significant_bit_mp<31ull>());
  EXPECT_EQ(6, most_significant_bit_mp<32ull>());
  EXPECT_EQ(6, most_significant_bit_mp<63ull>());
  EXPECT_EQ(7, most_significant_bit_mp<64ull>());
  EXPECT_EQ(7, most_significant_bit_mp<127ull>());
  EXPECT_EQ(10, most_significant_bit_mp<997ull>());
  EXPECT_EQ(16, most_significant_bit_mp<65535ull>());
  EXPECT_EQ(17, most_significant_bit_mp<65536ull>());
}

/////////////////////////////////
// smallest integral & friends //
/////////////////////////////////

#define SEL_TEST(expected, selector, n) \
  EXPECT_TRUE((std::is_same<expected, selector<n>>::value))

TEST(numerics, smallest_signed_integral) {
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

TEST(numerics, smallest_fast_signed_integral) {
  SEL_TEST(int_fast8_t, smallest_fast_signed_integral, 0);
  SEL_TEST(int_fast8_t, smallest_fast_signed_integral, 1);
  SEL_TEST(int_fast8_t, smallest_fast_signed_integral, 2);
  SEL_TEST(int_fast8_t, smallest_fast_signed_integral, 7);
  SEL_TEST(int_fast8_t, smallest_fast_signed_integral, 8);
  SEL_TEST(int_fast16_t, smallest_fast_signed_integral, 9);
  SEL_TEST(int_fast16_t, smallest_fast_signed_integral, 15);
  SEL_TEST(int_fast16_t, smallest_fast_signed_integral, 16);
  SEL_TEST(int_fast32_t, smallest_fast_signed_integral, 17);
  SEL_TEST(int_fast32_t, smallest_fast_signed_integral, 31);
  SEL_TEST(int_fast32_t, smallest_fast_signed_integral, 32);
  SEL_TEST(int_fast64_t, smallest_fast_signed_integral, 33);
  SEL_TEST(int_fast64_t, smallest_fast_signed_integral, 63);
  SEL_TEST(int_fast64_t, smallest_fast_signed_integral, 64);
}

TEST(numerics, smallest_least_signed_integral) {
  SEL_TEST(int_least8_t, smallest_least_signed_integral, 0);
  SEL_TEST(int_least8_t, smallest_least_signed_integral, 1);
  SEL_TEST(int_least8_t, smallest_least_signed_integral, 2);
  SEL_TEST(int_least8_t, smallest_least_signed_integral, 7);
  SEL_TEST(int_least8_t, smallest_least_signed_integral, 8);
  SEL_TEST(int_least16_t, smallest_least_signed_integral, 9);
  SEL_TEST(int_least16_t, smallest_least_signed_integral, 15);
  SEL_TEST(int_least16_t, smallest_least_signed_integral, 16);
  SEL_TEST(int_least32_t, smallest_least_signed_integral, 17);
  SEL_TEST(int_least32_t, smallest_least_signed_integral, 31);
  SEL_TEST(int_least32_t, smallest_least_signed_integral, 32);
  SEL_TEST(int_least64_t, smallest_least_signed_integral, 33);
  SEL_TEST(int_least64_t, smallest_least_signed_integral, 63);
  SEL_TEST(int_least64_t, smallest_least_signed_integral, 64);
}

TEST(numerics, smallest_unsigned_integral) {
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

TEST(numerics, smallest_fast_unsigned_integral) {
  SEL_TEST(bool, smallest_fast_unsigned_integral, 0);
  SEL_TEST(bool, smallest_fast_unsigned_integral, 1);
  SEL_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 2);
  SEL_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 7);
  SEL_TEST(uint_fast8_t, smallest_fast_unsigned_integral, 8);
  SEL_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 9);
  SEL_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 15);
  SEL_TEST(uint_fast16_t, smallest_fast_unsigned_integral, 16);
  SEL_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 17);
  SEL_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 31);
  SEL_TEST(uint_fast32_t, smallest_fast_unsigned_integral, 32);
  SEL_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 33);
  SEL_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 63);
  SEL_TEST(uint_fast64_t, smallest_fast_unsigned_integral, 64);
}

TEST(numerics, smallest_least_unsigned_integral) {
  SEL_TEST(bool, smallest_least_unsigned_integral, 0);
  SEL_TEST(bool, smallest_least_unsigned_integral, 1);
  SEL_TEST(uint_least8_t, smallest_least_unsigned_integral, 2);
  SEL_TEST(uint_least8_t, smallest_least_unsigned_integral, 7);
  SEL_TEST(uint_least8_t, smallest_least_unsigned_integral, 8);
  SEL_TEST(uint_least16_t, smallest_least_unsigned_integral, 9);
  SEL_TEST(uint_least16_t, smallest_least_unsigned_integral, 15);
  SEL_TEST(uint_least16_t, smallest_least_unsigned_integral, 16);
  SEL_TEST(uint_least32_t, smallest_least_unsigned_integral, 17);
  SEL_TEST(uint_least32_t, smallest_least_unsigned_integral, 31);
  SEL_TEST(uint_least32_t, smallest_least_unsigned_integral, 32);
  SEL_TEST(uint_least64_t, smallest_least_unsigned_integral, 33);
  SEL_TEST(uint_least64_t, smallest_least_unsigned_integral, 63);
  SEL_TEST(uint_least64_t, smallest_least_unsigned_integral, 64);
}

#undef SEL_TEST

#define SMALLEST_FOR_VALUE_TEST(n, expected) \
  LOG(INFO) << "expected = " << #expected << ", actual = " \
    << folly::demangle( \
      typeid(smallest_uint_for_value<n>).name() \
    ).toStdString(); \
  EXPECT_TRUE((std::is_same<expected, smallest_uint_for_value<n>>::value))

TEST(numerics, smallest_uint_for_value) {
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

TEST(numerics, multiply_mp) {
  EXPECT_EQ(0, (multiply_mp<0, 0>::value));
  EXPECT_EQ(0, (multiply_mp<0, 1>::value));
  EXPECT_EQ(0, (multiply_mp<1, 0>::value));
  EXPECT_EQ(1, (multiply_mp<1, 1>::value));
  EXPECT_EQ(0, (multiply_mp<999, 0>::value));
  EXPECT_EQ(999, (multiply_mp<1, 999>::value));
  EXPECT_EQ(999, (multiply_mp<999, 1>::value));
  EXPECT_EQ(998001, (multiply_mp<999, 999>::value));
  EXPECT_EQ(18, (multiply_mp<9, 2>::value));
  EXPECT_EQ(8, (multiply_mp<4, 2>::value));
  EXPECT_EQ(8, (multiply_mp<2, 4>::value));
  EXPECT_EQ(12, (multiply_mp<3, 4>::value));
  EXPECT_EQ(100000000, (multiply_mp<10000, 10000>::value));
  EXPECT_EQ(4096, (multiply_mp<64, 64>::value));
}

///////////////
// square_mp //
///////////////

TEST(numerics, square_mp) {
  EXPECT_EQ(0, (square_mp<0>::value));
  EXPECT_EQ(1, (square_mp<1>::value));
  EXPECT_EQ(4, (square_mp<2>::value));
  EXPECT_EQ(9, (square_mp<3>::value));
  EXPECT_EQ(256, (square_mp<16>::value));
  EXPECT_EQ(4096, (square_mp<64>::value));
  EXPECT_EQ(998001, (square_mp<999>::value));
  EXPECT_EQ(100000000, (square_mp<10000>::value));
}

////////////
// pow_mp //
////////////

TEST(numerics, pow_mp) {
  EXPECT_EQ(1, (pow_mp<0, 0>::value));
  EXPECT_EQ(1, (pow_mp<1, 0>::value));
  EXPECT_EQ(0, (pow_mp<0, 1>::value));
  EXPECT_EQ(0, (pow_mp<0, 999>::value));
  EXPECT_EQ(1, (pow_mp<999, 0>::value));
  EXPECT_EQ(999, (pow_mp<999, 1>::value));
  EXPECT_EQ(998001, (pow_mp<999, 2>::value));
  EXPECT_EQ(81, (pow_mp<9, 2>::value));
  EXPECT_EQ(16, (pow_mp<4, 2>::value));
  EXPECT_EQ(16, (pow_mp<2, 4>::value));
  EXPECT_EQ(81, (pow_mp<3, 4>::value));
  EXPECT_EQ(10000000000, (pow_mp<10, 10>::value));
  EXPECT_EQ(1073741824, (pow_mp<2, 30>::value));
}

TEST(numerics, unchecked_pow_mp) {
  EXPECT_EQ(1, (unchecked_pow_mp<0, 0>::value));
  EXPECT_EQ(1, (unchecked_pow_mp<1, 0>::value));
  EXPECT_EQ(0, (unchecked_pow_mp<0, 1>::value));
  EXPECT_EQ(0, (unchecked_pow_mp<0, 999>::value));
  EXPECT_EQ(1, (unchecked_pow_mp<999, 0>::value));
  EXPECT_EQ(999, (unchecked_pow_mp<999, 1>::value));
  EXPECT_EQ(998001, (unchecked_pow_mp<999, 2>::value));
  EXPECT_EQ(81, (unchecked_pow_mp<9, 2>::value));
  EXPECT_EQ(16, (unchecked_pow_mp<4, 2>::value));
  EXPECT_EQ(16, (unchecked_pow_mp<2, 4>::value));
  EXPECT_EQ(81, (unchecked_pow_mp<3, 4>::value));
  EXPECT_EQ(10000000000, (unchecked_pow_mp<10, 10>::value));
  EXPECT_EQ(1073741824, (unchecked_pow_mp<2, 30>::value));
}

TEST(numerics, unchecked_pow_mp_overflows) {
  // Make sure overflow works. Wolfram Alpha ftw.
  EXPECT_EQ(6203307696791771937, (unchecked_pow_mp<3, 1000>::value));
  EXPECT_EQ(0, (unchecked_pow_mp<10, 1000>::value));
}

/////////////////////
// is_power_of_two //
/////////////////////

TEST(numerics, is_power_of_two) {
  EXPECT_TRUE(is_power_of_two(1));
  EXPECT_TRUE(is_power_of_two(2));
  EXPECT_TRUE(is_power_of_two(4));
  EXPECT_TRUE(is_power_of_two(8));
  EXPECT_TRUE(is_power_of_two(16));
  EXPECT_TRUE(is_power_of_two(32));
  EXPECT_TRUE(is_power_of_two(64));
  EXPECT_TRUE(is_power_of_two(128));
  EXPECT_TRUE(is_power_of_two(256));
  EXPECT_TRUE(is_power_of_two(512));
  EXPECT_TRUE(is_power_of_two(1024));
  EXPECT_TRUE(is_power_of_two(2048));
  EXPECT_TRUE(is_power_of_two(4096));
  EXPECT_TRUE(is_power_of_two(8192));
  EXPECT_TRUE(is_power_of_two(16384));
  EXPECT_TRUE(is_power_of_two(32768));
  EXPECT_TRUE(is_power_of_two(65536));
  EXPECT_TRUE(is_power_of_two(131072));
  EXPECT_TRUE(is_power_of_two(262144));
  EXPECT_TRUE(is_power_of_two(524288));
  EXPECT_TRUE(is_power_of_two(1048576));

  EXPECT_FALSE(is_power_of_two(0));
  EXPECT_FALSE(is_power_of_two(3));
  EXPECT_FALSE(is_power_of_two(5));
  EXPECT_FALSE(is_power_of_two(7));
  EXPECT_FALSE(is_power_of_two(9));
  EXPECT_FALSE(is_power_of_two(15));
  EXPECT_FALSE(is_power_of_two(17));
  EXPECT_FALSE(is_power_of_two(31));
  EXPECT_FALSE(is_power_of_two(33));
  EXPECT_FALSE(is_power_of_two(63));
  EXPECT_FALSE(is_power_of_two(65));
  EXPECT_FALSE(is_power_of_two(127));
  EXPECT_FALSE(is_power_of_two(129));
  EXPECT_FALSE(is_power_of_two(255));
  EXPECT_FALSE(is_power_of_two(257));
  EXPECT_FALSE(is_power_of_two(511));
  EXPECT_FALSE(is_power_of_two(513));
  EXPECT_FALSE(is_power_of_two(1023));
  EXPECT_FALSE(is_power_of_two(1025));
  EXPECT_FALSE(is_power_of_two(2047));
  EXPECT_FALSE(is_power_of_two(2049));
  EXPECT_FALSE(is_power_of_two(4095));
  EXPECT_FALSE(is_power_of_two(4097));
  EXPECT_FALSE(is_power_of_two(8191));
  EXPECT_FALSE(is_power_of_two(8193));
  EXPECT_FALSE(is_power_of_two(16383));
  EXPECT_FALSE(is_power_of_two(16385));
  EXPECT_FALSE(is_power_of_two(32767));
  EXPECT_FALSE(is_power_of_two(32769));
  EXPECT_FALSE(is_power_of_two(65535));
  EXPECT_FALSE(is_power_of_two(65537));
  EXPECT_FALSE(is_power_of_two(131071));
  EXPECT_FALSE(is_power_of_two(131073));
  EXPECT_FALSE(is_power_of_two(262143));
  EXPECT_FALSE(is_power_of_two(262145));
  EXPECT_FALSE(is_power_of_two(524287));
  EXPECT_FALSE(is_power_of_two(524289));
  EXPECT_FALSE(is_power_of_two(1048575));
  EXPECT_FALSE(is_power_of_two(1048577));
}

////////////////////////
// is_mersenne_number //
////////////////////////

TEST(numerics, is_mersenne_number) {
  EXPECT_TRUE(is_mersenne_number(1));
  EXPECT_TRUE(is_mersenne_number(3));
  EXPECT_TRUE(is_mersenne_number(7));
  EXPECT_TRUE(is_mersenne_number(15));
  EXPECT_TRUE(is_mersenne_number(31));
  EXPECT_TRUE(is_mersenne_number(63));
  EXPECT_TRUE(is_mersenne_number(127));
  EXPECT_TRUE(is_mersenne_number(255));
  EXPECT_TRUE(is_mersenne_number(511));
  EXPECT_TRUE(is_mersenne_number(1023));
  EXPECT_TRUE(is_mersenne_number(2047));
  EXPECT_TRUE(is_mersenne_number(4095));
  EXPECT_TRUE(is_mersenne_number(8191));
  EXPECT_TRUE(is_mersenne_number(16383));
  EXPECT_TRUE(is_mersenne_number(32767));
  EXPECT_TRUE(is_mersenne_number(65535));
  EXPECT_TRUE(is_mersenne_number(131071));
  EXPECT_TRUE(is_mersenne_number(262143));
  EXPECT_TRUE(is_mersenne_number(524287));
  EXPECT_TRUE(is_mersenne_number(1048575));

  EXPECT_FALSE(is_mersenne_number(0));
  EXPECT_FALSE(is_mersenne_number(2));
  EXPECT_FALSE(is_mersenne_number(4));
  EXPECT_FALSE(is_mersenne_number(8));
  EXPECT_FALSE(is_mersenne_number(16));
  EXPECT_FALSE(is_mersenne_number(32));
  EXPECT_FALSE(is_mersenne_number(64));
  EXPECT_FALSE(is_mersenne_number(128));
  EXPECT_FALSE(is_mersenne_number(256));
  EXPECT_FALSE(is_mersenne_number(512));
  EXPECT_FALSE(is_mersenne_number(1024));
  EXPECT_FALSE(is_mersenne_number(2048));
  EXPECT_FALSE(is_mersenne_number(4096));
  EXPECT_FALSE(is_mersenne_number(8192));
  EXPECT_FALSE(is_mersenne_number(16384));
  EXPECT_FALSE(is_mersenne_number(32768));
  EXPECT_FALSE(is_mersenne_number(65536));
  EXPECT_FALSE(is_mersenne_number(131072));
  EXPECT_FALSE(is_mersenne_number(262144));
  EXPECT_FALSE(is_mersenne_number(524288));
  EXPECT_FALSE(is_mersenne_number(1048576));

  EXPECT_FALSE(is_mersenne_number(5));
  EXPECT_FALSE(is_mersenne_number(9));
  EXPECT_FALSE(is_mersenne_number(17));
  EXPECT_FALSE(is_mersenne_number(33));
  EXPECT_FALSE(is_mersenne_number(65));
  EXPECT_FALSE(is_mersenne_number(129));
  EXPECT_FALSE(is_mersenne_number(257));
  EXPECT_FALSE(is_mersenne_number(513));
  EXPECT_FALSE(is_mersenne_number(1025));
  EXPECT_FALSE(is_mersenne_number(2049));
  EXPECT_FALSE(is_mersenne_number(4097));
  EXPECT_FALSE(is_mersenne_number(8193));
  EXPECT_FALSE(is_mersenne_number(16385));
  EXPECT_FALSE(is_mersenne_number(32769));
  EXPECT_FALSE(is_mersenne_number(65537));
  EXPECT_FALSE(is_mersenne_number(131073));
  EXPECT_FALSE(is_mersenne_number(262145));
  EXPECT_FALSE(is_mersenne_number(524289));
  EXPECT_FALSE(is_mersenne_number(1048577));

  // We want specifically int32 because we want to check for
  // overflow since 2^32-1 is int32_t::max
  ASSERT_EQ(32, (data_bits<int32_t>::value));
  EXPECT_TRUE(
    (is_mersenne_number<int32_t>(std::numeric_limits<int32_t>::max()))
  );

  // same for other types
  ASSERT_EQ(8, (data_bits<int8_t>::value));
  EXPECT_TRUE(
    (is_mersenne_number<int8_t>(std::numeric_limits<int8_t>::max()))
  );
  ASSERT_EQ(16, (data_bits<int16_t>::value));
  EXPECT_TRUE(
    (is_mersenne_number<int16_t>(std::numeric_limits<int16_t>::max()))
  );
  ASSERT_EQ(64, (data_bits<int64_t>::value));
  EXPECT_TRUE(
    (is_mersenne_number<int64_t>(std::numeric_limits<int64_t>::max()))
  );
}

template <std::size_t nth, std::size_t exponent, std::size_t value>
void check_mersenne_prime() {
  typedef mersenne_prime<nth> mp;
  EXPECT_EQ(nth, mp::nth::value);
  EXPECT_EQ(exponent, mp::exponent::value);
  EXPECT_EQ(value, mp::value);
}

TEST(numerics, mersenne_prime) {
  check_mersenne_prime<1, 2, 3>();
  check_mersenne_prime<2, 3, 7>();
  check_mersenne_prime<3, 5, 31>();
  check_mersenne_prime<4, 7, 127>();
  check_mersenne_prime<5, 13, 8191>();
  check_mersenne_prime<6, 17, 131071>();
  check_mersenne_prime<7, 19, 524287>();
  check_mersenne_prime<8, 31, 2147483647>();
  check_mersenne_prime<9, 61, 2305843009213693951ull>();
}

template <std::size_t bits_size, std::size_t nth_mersenne>
void check_largest_mersenne_prime_under() {
  typedef mersenne_prime<nth_mersenne> expected;
  typedef largest_mersenne_prime_under<bits_size> actual;

  LOG(INFO) << "bits_size = " << bits_size
    << ", nth_mersenne = " << nth_mersenne;

  EXPECT_EQ(
    static_cast<unsigned long long>(expected::value),
    static_cast<unsigned long long>(actual::value)
  );
}

TEST(numerics, largest_mersenne_prime_under) {
  check_largest_mersenne_prime_under<2, 1>();
  check_largest_mersenne_prime_under<3, 2>();
  check_largest_mersenne_prime_under<4, 2>();
  check_largest_mersenne_prime_under<5, 3>();
  check_largest_mersenne_prime_under<6, 3>();
  check_largest_mersenne_prime_under<7, 4>();
  check_largest_mersenne_prime_under<8, 4>();
  check_largest_mersenne_prime_under<9, 4>();
  check_largest_mersenne_prime_under<10, 4>();
  check_largest_mersenne_prime_under<11, 4>();
  check_largest_mersenne_prime_under<12, 4>();
  check_largest_mersenne_prime_under<13, 5>();
  check_largest_mersenne_prime_under<14, 5>();
  check_largest_mersenne_prime_under<15, 5>();
  check_largest_mersenne_prime_under<16, 5>();
  check_largest_mersenne_prime_under<17, 6>();
  check_largest_mersenne_prime_under<18, 6>();
  check_largest_mersenne_prime_under<19, 7>();
  check_largest_mersenne_prime_under<20, 7>();
  check_largest_mersenne_prime_under<21, 7>();
  check_largest_mersenne_prime_under<22, 7>();
  check_largest_mersenne_prime_under<23, 7>();
  check_largest_mersenne_prime_under<24, 7>();
  check_largest_mersenne_prime_under<25, 7>();
  check_largest_mersenne_prime_under<26, 7>();
  check_largest_mersenne_prime_under<27, 7>();
  check_largest_mersenne_prime_under<28, 7>();
  check_largest_mersenne_prime_under<29, 7>();
  check_largest_mersenne_prime_under<30, 7>();
  check_largest_mersenne_prime_under<31, 8>();
  check_largest_mersenne_prime_under<32, 8>();
  check_largest_mersenne_prime_under<33, 8>();
  check_largest_mersenne_prime_under<34, 8>();
  check_largest_mersenne_prime_under<35, 8>();
  check_largest_mersenne_prime_under<36, 8>();
  check_largest_mersenne_prime_under<37, 8>();
  check_largest_mersenne_prime_under<38, 8>();
  check_largest_mersenne_prime_under<39, 8>();
  check_largest_mersenne_prime_under<40, 8>();
  check_largest_mersenne_prime_under<41, 8>();
  check_largest_mersenne_prime_under<42, 8>();
  check_largest_mersenne_prime_under<43, 8>();
  check_largest_mersenne_prime_under<44, 8>();
  check_largest_mersenne_prime_under<45, 8>();
  check_largest_mersenne_prime_under<46, 8>();
  check_largest_mersenne_prime_under<47, 8>();
  check_largest_mersenne_prime_under<48, 8>();
  check_largest_mersenne_prime_under<49, 8>();
  check_largest_mersenne_prime_under<50, 8>();
  check_largest_mersenne_prime_under<51, 8>();
  check_largest_mersenne_prime_under<52, 8>();
  check_largest_mersenne_prime_under<53, 8>();
  check_largest_mersenne_prime_under<54, 8>();
  check_largest_mersenne_prime_under<55, 8>();
  check_largest_mersenne_prime_under<56, 8>();
  check_largest_mersenne_prime_under<57, 8>();
  check_largest_mersenne_prime_under<58, 8>();
  check_largest_mersenne_prime_under<59, 8>();
  check_largest_mersenne_prime_under<60, 8>();
  check_largest_mersenne_prime_under<61, 9>();
  check_largest_mersenne_prime_under<62, 9>();
  check_largest_mersenne_prime_under<63, 9>();
  check_largest_mersenne_prime_under<64, 9>();
}

template <typename T, std::size_t diff, std::size_t nth_mersenne>
void check_largest_mersenne_prime_for_type() {
  typedef mersenne_prime<nth_mersenne> expected;
  typedef largest_mersenne_prime_for_type<T, diff> actual;

  LOG(INFO) << "T = " << folly::demangle(typeid(T).name())
    << ", unsigned_bits_size = "
    << (data_bits<T>::value - std::is_signed<T>::value)
    << ", diff = " << diff << ", nth_mersenne = " << nth_mersenne;

  EXPECT_EQ(
    static_cast<unsigned long long>(expected::value),
    static_cast<unsigned long long>(actual::value)
  );
}

TEST(numerics, largest_mersenne_prime_for_type) {
  check_largest_mersenne_prime_for_type<int8_t, 0, 4>();
  check_largest_mersenne_prime_for_type<int8_t, 1, 3>();
  check_largest_mersenne_prime_for_type<int8_t, 2, 3>();
  check_largest_mersenne_prime_for_type<int8_t, 3, 2>();
  check_largest_mersenne_prime_for_type<int8_t, 4, 2>();
  check_largest_mersenne_prime_for_type<int8_t, 5, 1>();

  check_largest_mersenne_prime_for_type<uint8_t, 0, 4>();
  check_largest_mersenne_prime_for_type<uint8_t, 1, 4>();
  check_largest_mersenne_prime_for_type<uint8_t, 2, 3>();
  check_largest_mersenne_prime_for_type<uint8_t, 3, 3>();
  check_largest_mersenne_prime_for_type<uint8_t, 4, 2>();
  check_largest_mersenne_prime_for_type<uint8_t, 5, 2>();
  check_largest_mersenne_prime_for_type<uint8_t, 6, 1>();

  check_largest_mersenne_prime_for_type<int16_t, 0, 5>();
  check_largest_mersenne_prime_for_type<int16_t, 1, 5>();
  check_largest_mersenne_prime_for_type<int16_t, 2, 5>();
  check_largest_mersenne_prime_for_type<int16_t, 3, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 4, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 5, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 6, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 7, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 8, 4>();
  check_largest_mersenne_prime_for_type<int16_t, 9, 3>();
  check_largest_mersenne_prime_for_type<int16_t, 10, 3>();
  check_largest_mersenne_prime_for_type<int16_t, 11, 2>();
  check_largest_mersenne_prime_for_type<int16_t, 12, 2>();
  check_largest_mersenne_prime_for_type<int16_t, 13, 1>();

  check_largest_mersenne_prime_for_type<uint16_t, 0, 5>();
  check_largest_mersenne_prime_for_type<uint16_t, 1, 5>();
  check_largest_mersenne_prime_for_type<uint16_t, 2, 5>();
  check_largest_mersenne_prime_for_type<uint16_t, 3, 5>();
  check_largest_mersenne_prime_for_type<uint16_t, 4, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 5, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 6, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 7, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 8, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 9, 4>();
  check_largest_mersenne_prime_for_type<uint16_t, 10, 3>();
  check_largest_mersenne_prime_for_type<uint16_t, 11, 3>();
  check_largest_mersenne_prime_for_type<uint16_t, 12, 2>();
  check_largest_mersenne_prime_for_type<uint16_t, 13, 2>();
  check_largest_mersenne_prime_for_type<uint16_t, 14, 1>();

  check_largest_mersenne_prime_for_type<int32_t, 0, 8>();
  check_largest_mersenne_prime_for_type<uint32_t, 0, 8>();

  check_largest_mersenne_prime_for_type<int64_t, 0, 9>();
  check_largest_mersenne_prime_for_type<uint64_t, 0, 9>();
}

////////////////////////////
// DISCRETE_TO_CONTINUOUS //
////////////////////////////

#define C_TEST_IMPL(conv, dmin, dmax) \
  do { \
    auto const drange = dmax - dmin; \
    auto const crange = conv.max() - conv.min(); \
    for(auto i = 10000; i--; ) { \
      auto n = dmin + (folly::Random::rand32() % drange); \
      auto expected = static_cast<double>(n - dmin) \
        / drange * crange + conv.min(); \
      EXPECT_EQ(expected, conv(n)); \
    } \
  } while(false);

TEST(discrete_to_continuous, sanity_check) {
  discrete_to_continuous<unsigned, double> conv(11, 9999, -5.5, 5.5);
  C_TEST_IMPL(conv, 11, 9999);
}

#undef C_TEST_IMPL

} // namespace ftl {
