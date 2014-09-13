/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/traits.h>

#include <type_traits>
#include <limits>
#include <stdexcept>

#include <cstddef>
#include <cstdint>
#include <climits>

namespace fatal {

/**
 * Returns the amount of bits that can be represented using the
 * memory taken by the given type.
 *
 * Example:
 *
 *  // yields `1`
 *  data_bits<bool>::value
 *
 *  // yields `CHAR_BIT`
 *  data_bits<char>::value
 *
 *  // yields `32`
 *  data_bits<std::int32_t>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using data_bits = std::integral_constant<
  std::size_t,
  std::is_same<bool, typename std::decay<T>::type>::value
    ? 1
    : sizeof(typename std::decay<T>::type) * CHAR_BIT
>;

/**
 * The maximum shift count that can be applied to a non-negative value of type
 * `T` without causing overflow, given that the value already occupies `Size`
 * bits.
 *
 * Shifting the least significant bit left by this amount will result in the
 * most significant non-sign bit of an integer.
 *
 * Example:
 *
 *  // yields `6`
 *  shift_left_count_upperbound<int8_t>::value
 *
 *  // yields `7`
 *  shift_left_count_upperbound<uint8_t>::value
 *
 *  // yields `5`
 *  shift_left_count_upperbound<int8_t, 2>::value
 *
 *  // yields `4`
 *  shift_left_count_upperbound<uint8_t, 4>::value
 *
 *  // yields `14`
 *  shift_left_count_upperbound<int16_t>::value
 *
 *  // yields `15`
 *  shift_left_count_upperbound<uint16_t>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <typename T, std::size_t Size>
struct shift_left_count_upperbound_impl {
  static_assert(
    Size + std::is_signed<T>::value <= data_bits<T>::value,
    "value already uses up all the bits available"
  );

  typedef std::integral_constant<
    std::size_t, (data_bits<T>::value - Size - std::is_signed<T>::value)
  > type;
};

} // namespace detail {

template <typename T, std::size_t Size = 1>
using shift_left_count_upperbound
  = typename detail::shift_left_count_upperbound_impl<
    T, Size
  >::type;

/**
 * The upper bound on values of type `T` that can be shifted left by `Shift`
 * positions without overflow.
 *
 * Example:
 *
 *  template <typename T, std::size_t Shift>
 *  T foo(T value) {
 *    if (value < shift_left_upperbound<T>::value) {
 *      std::cout << "no overflow" << std::endl;
 *    } else {
 *      std::cout << "will overflow" << std::endl;
 *    }
 *    return value << Shift;
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, std::size_t Shift = 1>
using shift_left_upperbound = std::integral_constant<
  T, (static_cast<T>(1) << shift_left_count_upperbound<T, Shift>::value)
>;

/**
 * most_significant_bit_mp() returns the 1-based position of the
 * most significant bit (0 for no bits set) for n
 *
 * This is the same as log_2(n) + 1, except for n = 0
 *
 * Usage:
 *
 * auto msb = most_significant_bit_mp<10>();
 *  // msb is now 4
 *
 * template <std::size_t msb>
 * struct Foo { ... };
 *
 * Foo<most_significant_bit_mp<1024>()> foo;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <std::size_t n>
struct msb_mp_impl:
  public std::integral_constant<std::size_t, 1 + msb_mp_impl<(n >> 1)>::value>
{};

template <>
struct msb_mp_impl<0>:
  public std::integral_constant<std::size_t, 0>
{};

} // namespace detail {

template <std::size_t n>
constexpr std::size_t most_significant_bit_mp() {
  return detail::msb_mp_impl<n>::value;
}

/**
 * smallest_signed_integral & friends allow you to get the smallest possible
 * integer type that can accommodate some data with at least the given bits size
 *
 * Say, for instance, you need to store 8 bits of data.
 * smallest_signed_integral<8> will provide a member typedef named type,
 * evaluating to int8_t.
 *
 * smallest_signed_integral: chooses among int8_t, int16_t...
 * smallest_fast_signed_integral: chooses among int_fast8_t, int_fast16_t...
 * smallest_least_signed_integral: chooses among int_least8_t, int_least16_t...
 * smallest_unsigned_integral: chooses among bool, uint8_t, uint16_t...
 * smallest_fast_unsigned_integral: chooses among bool, uint_fast8_t,
 *                                  uint_fast16_t...
 * smallest_least_unsigned_integral: chooses among bool, uint_least8_t,
 *                                   uint_least16_t...
 *
 * Example:
 *
 * typename smallest_unsigned_integral<8>::type data = 255;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail  {

template <std::size_t bits_size, typename... Args>
struct smallest_integral_impl;

template <std::size_t bits_size, typename T, typename... Args>
struct smallest_integral_impl<bits_size, T, Args...> {
  typedef typename std::conditional<
    bits_size <= data_bits<T>::value,
    T,
    typename smallest_integral_impl<bits_size, Args...>::type
  >::type type;
};

template <std::size_t bits_size, typename T>
struct smallest_integral_impl<bits_size, T> {
  static_assert(
    bits_size <= data_bits<T>::value,
    "there's no known type to hold that many bits"
  );
  typedef T type;
};

} // namespace detail

template <std::size_t bits_size>
using smallest_signed_integral = typename detail::smallest_integral_impl<
  bits_size, int8_t, int16_t,
  int32_t, int64_t, long long
>::type;

template <std::size_t bits_size>
using smallest_fast_signed_integral = typename detail::smallest_integral_impl<
  bits_size, int_fast8_t, int_fast16_t,
  int_fast32_t, int_fast64_t, long long
>::type;

template <std::size_t bits_size>
using smallest_least_signed_integral = typename detail::smallest_integral_impl<
  bits_size, int_least8_t, int_least16_t,
  int_least32_t, int_least64_t, long long
>::type;

template <std::size_t bits_size>
using smallest_unsigned_integral = typename detail::smallest_integral_impl<
  bits_size, bool, uint8_t, uint16_t,
  uint32_t, uint64_t, unsigned long long
>::type;

template <std::size_t bits_size>
using smallest_fast_unsigned_integral = typename detail::smallest_integral_impl<
  bits_size,
  bool, uint_fast8_t, uint_fast16_t,
  uint_fast32_t, uint_fast64_t, unsigned long long
>::type;

template <std::size_t bits_size>
using smallest_least_unsigned_integral
= typename detail::smallest_integral_impl<
  bits_size,
  bool, uint_least8_t, uint_least16_t,
  uint_least32_t, uint_least64_t, unsigned long long
>::type;

/**
 * smallest_uint_for_value gives you the smallest possible unsigned integer type
 * able to store the given value.
 *
 * Example:
 *
 * typename smallest_uint_for<255>::type data = 255;
 *
 * template <typename ...Args>
 * struct Foo {
 *   typedef typename smallest_uint_for<sizeof...(Args)>::type type_id;
 *   ...
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <unsigned long long value>
using smallest_uint_for_value = smallest_unsigned_integral<
  most_significant_bit_mp<value>()
>;

/**
 * compile-time multiplication with overflow detection
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <unsigned long long lhs, unsigned long long rhs>
struct multiply_mp:
  public std::integral_constant<unsigned long long, lhs * rhs>
{
  static_assert(
    ((std::numeric_limits<decltype(rhs)>::max() / lhs) >= rhs),
    "multiplication overflow detected"
  );
};

template <unsigned long long lhs>
struct multiply_mp<lhs, 0>:
  public std::integral_constant<unsigned long long, 0>
{};

template <unsigned long long rhs>
struct multiply_mp<0, rhs>:
  public std::integral_constant<unsigned long long, 0>
{};

template <>
struct multiply_mp<0, 0>:
  public std::integral_constant<unsigned long long, 0>
{};

template <unsigned long long n>
struct square_mp:
  public multiply_mp<n, n>
{};

/**
 * pow_mp: O(lg exponent) compile-time pow()
 *
 * Exposes a static member constant value with the result of base ^ exponent.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <unsigned long long base, unsigned long long exponent>
struct pow_mp:
  public std::integral_constant<
    unsigned long long,
    multiply_mp<
      exponent & 1 ? base : 1,
      pow_mp<
        square_mp<base>::value,
        exponent ? (exponent >> 1) : 0
      >::value
    >::value
  >
{};

template <unsigned long long exponent>
struct pow_mp<0, exponent>:
  public std::integral_constant<unsigned long long, 0>
{};

template <unsigned long long base>
struct pow_mp<base, 0>:
  public std::integral_constant<unsigned long long, 1>
{};

// http://en.wikipedia.org/wiki/Exponentiation#Zero_to_the_power_of_zero
template <>
struct pow_mp<0, 0>:
  public std::integral_constant<unsigned long long, 1>
{};

/**
 * unchecked_pow_mp: O(lg exponent) compile-time pow()
 * that allows unsigned multiplication overflows
 *
 * Exposes a static member constant value with the result of base ^ exponent.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <unsigned long long base, unsigned long long exponent>
struct unchecked_pow_mp:
  public std::integral_constant<
    unsigned long long,
    (exponent & 1 ? base : 1)
      * unchecked_pow_mp<base * base, exponent ? (exponent >> 1) : 0>::value
  >
{};

template <unsigned long long exponent>
struct unchecked_pow_mp<0, exponent>:
  public std::integral_constant<unsigned long long, 0>
{};

template <unsigned long long base>
struct unchecked_pow_mp<base, 0>:
  public std::integral_constant<unsigned long long, 1>
{};

template <>
struct unchecked_pow_mp<0, 0>:
  public std::integral_constant<unsigned long long, 1>
{};

/**
 * is_power_of_two: O(1) compile-time check on whether n is a power of two.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
inline constexpr bool is_power_of_two(T n) {
  return n != 0 && !(n & (n - 1));
}

/**
 * is_mersenne_number: O(1) compile-time check on whether n
 * is a mersenne number.
 *
 * http://mathworld.wolfram.com/MersenneNumber.html
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
inline constexpr bool is_mersenne_number(T n) {
  return n == std::numeric_limits<T>::max()
    ? (n & 1) && (
      n == 1 || is_mersenne_number(n >> 1)
    )
    : n != 0 && !(n & (n + 1));
}

/**
 * A Mersenne Number is a natural number of the form 2^p - 1. They are
 * particularly interesting because:
 *
 * - they are closely related to powers of two
 * - the modulo operation is cheaper when the divisor is a Mersenne number as
 *   opposed to a regular natural number which is not a power of two
 * - its exponent equals the amount of bits needed to represent the number
 * - some of these numbers are primes
 *
 * This class is a compile-time representation of the Mersenne Numbers that can
 * be expressed with standard integer types. They expose these properties:
 *
 * - value - the Mersenne Number itself
 * - exponent::value - the exponent p such that the value of this Mersenne
 *   Number is 2^p - 1
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Exponent>
struct mersenne_number:
  public std::integral_constant<
    smallest_fast_unsigned_integral<Exponent + 1>,
    (static_cast<smallest_fast_unsigned_integral<Exponent + 1>>(1) << Exponent)
      - 1
  >
{
  typedef std::integral_constant<
    smallest_fast_unsigned_integral<
      most_significant_bit_mp<Exponent>()
    >,
    Exponent
  > exponent;
};

namespace detail {

template <std::size_t Nth, std::size_t Exponent>
struct mersenne_prime_impl:
  public mersenne_number<Exponent>
{
  typedef std::integral_constant<
    smallest_fast_unsigned_integral<
      most_significant_bit_mp<Nth>()
    >,
    Nth
  > nth;
};

} // namespace detail

/**
 * This class is a compile-time representation of the Mersenne primes that can
 * be expressed with standard integer types. They extend the `mersenne_number`
 * class with this property:
 *
 * - nth::value - the one-based index  of the Mersenne prime (first known
 *   Mersenne prime has index 1, second one has index 2 and so on)
 *
 * http://en.wikipedia.org/wiki/Mersenne_prime
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t nth> struct mersenne_prime;

template <> // 3
struct mersenne_prime<1>: public detail::mersenne_prime_impl<1, 2> {};

template <> // 7
struct mersenne_prime<2>: public detail::mersenne_prime_impl<2, 3> {};

template <> // 31
struct mersenne_prime<3>: public detail::mersenne_prime_impl<3, 5> {};

template <> // 127
struct mersenne_prime<4>: public detail::mersenne_prime_impl<4, 7> {};

template <> // 8191
struct mersenne_prime<5>: public detail::mersenne_prime_impl<5, 13> {};

template <> // 131071
struct mersenne_prime<6>: public detail::mersenne_prime_impl<6, 17> {};

template <> // 524287
struct mersenne_prime<7>: public detail::mersenne_prime_impl<7, 19> {};

template <> // 2147483647
struct mersenne_prime<8>: public detail::mersenne_prime_impl<8, 31> {};

template <> // 2305843009213693951
struct mersenne_prime<9>: public detail::mersenne_prime_impl<9, 61> {};

constexpr std::size_t first_known_mersenne_prime_index = 1;
constexpr std::size_t last_known_mersenne_prime_index = 9;

typedef mersenne_prime<
  first_known_mersenne_prime_index
> first_known_mersenne_prime;
typedef mersenne_prime<
  last_known_mersenne_prime_index
> last_known_mersenne_prime;

/**
 * largest_mersenne_prime_under: at compile-time, gets the largets mersenne
 * prime which can be stored in bits_size bits, up to the Nth mersenne prime.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <
  std::size_t bits_size,
  std::size_t nth = last_known_mersenne_prime_index
>
struct largest_mersenne_prime_under_impl {
  typedef typename std::conditional<
    bits_size >= mersenne_prime<nth>::exponent::value,
    mersenne_prime<nth>,
    typename largest_mersenne_prime_under_impl<bits_size, nth - 1>::type
  >::type type;
};

template <std::size_t bits_size>
struct largest_mersenne_prime_under_impl<
  bits_size, first_known_mersenne_prime_index
> {
  typedef typename std::enable_if<
    bits_size >= mersenne_prime<
      first_known_mersenne_prime_index
    >::exponent::value,
    mersenne_prime<first_known_mersenne_prime_index>
  >::type type;
};

} // namespace detail {

template <
  std::size_t bits_size,
  std::size_t nth = last_known_mersenne_prime_index
>
using largest_mersenne_prime_under
= typename detail::largest_mersenne_prime_under_impl<bits_size, nth>::type;

/**
 * largest_mersenne_prime_for: at compile-time, gets the largets mersenne
 * prime which can be stored in the given type (with Diff bits to spare),
 * up to the Nth mersenne prime.
 *
 * Say T has a size of 8 bits and diff is 3, then the mersenne
 * prime returned will fit in at most 8 - 3 = 5 bits
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename T, std::size_t diff = 0,
  std::size_t nth = last_known_mersenne_prime_index
>
using largest_mersenne_prime_for_type = typename std::enable_if<
  std::is_integral<T>::value
    && (data_bits<T>::value - std::is_signed<T>::value > diff),
  largest_mersenne_prime_under<
    data_bits<T>::value - std::is_signed<T>::value - diff, nth
  >
>::type;

/**
 * An adapter to convert a discrete range into a continuous one,
 * given upper and lower bounds.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TDiscrete, typename TContinuous>
struct discrete_to_continuous {
  typedef TDiscrete discrete_type;
  typedef TContinuous continuous_type;

  discrete_to_continuous(
    discrete_type discreteMin, discrete_type discreteMax,
    continuous_type continuousMin, continuous_type continuousMax
  ):
    discreteMin_(discreteMin),
    continuousMin_(continuousMin),
    normalizer_(discreteMax - discreteMin_),
    adjustment_(continuousMax - continuousMin_)
  {
    if(!normalizer_) {
      throw std::invalid_argument(
        "the difference between discreteMin and discreteMax cannot be zero"
      );
    }
  }

  continuous_type operator ()(fast_pass<discrete_type> discrete) const {
    return (discrete - discreteMin_) / normalizer_
      * adjustment_ + continuousMin_;
  }

  fast_pass<discrete_type> discrete_min() const { return discreteMin_; }
  discrete_type discrete_max() const { return normalizer_ + discreteMin_; }

  fast_pass<continuous_type> min() const { return continuousMin_; }
  continuous_type max() const { return adjustment_ + continuousMin_; }

private:
  discrete_type const discreteMin_;
  continuous_type const continuousMin_;
  continuous_type const normalizer_;
  continuous_type const adjustment_;
};

} // namespace fatal {
