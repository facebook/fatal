/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_math_numerics_h
#define FATAL_INCLUDE_fatal_math_numerics_h

#include <fatal/type/list.h>
#include <fatal/type/traits.h>

#include <type_traits>
#include <limits>
#include <stdexcept>

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

// TODO: DOCUMENT AND TEST
template <typename T>
typename std::make_signed<T>::type signed_cast(T value) {
  return static_cast<typename std::make_signed<T>::type>(value);
}

// TODO: DOCUMENT AND TEST
template <typename T>
typename std::make_unsigned<T>::type unsigned_cast(T value) {
  return static_cast<typename std::make_unsigned<T>::type>(value);
}

namespace detail {

template <
  typename T,
  std::size_t EndPhase = 0,
  typename U = typename std::make_unsigned<T>::type,
  std::size_t Phase = ((sizeof(T) * CHAR_BIT) >> 1),
  U Mask = U(~U(0)) ^ U(U(~U(0)) << Phase)
>
struct integral_reverser {
  static_assert(sizeof(T) == sizeof(U), "internal error");
  static_assert(!(Phase & (Phase - 1)), "Phase must be a power of two");
  static_assert(std::is_integral<T>::value, "only integrals can be reversed");

  using tail = integral_reverser<
    T,
    EndPhase,
    U,
    (Phase >> 1),
    (Mask ^ (Mask << (Phase >> 1)))
  >;

  static constexpr T reverse(T value) noexcept {
    return tail::reverse(
      ((value >> Phase) & Mask) | ((value << Phase) & ~Mask)
    );
  }
};

template <typename T, std::size_t EndPhase, typename U, U Mask>
struct integral_reverser<T, EndPhase, U, EndPhase, Mask> {
  static constexpr T reverse(T value) noexcept { return value; }
};

} // namespace detail {

/**
 * Reverses the bytes of an integral in Big Theta(lg k), where k is the size of
 * the integral in bits.
 *
 * Example:
 *
 *  // yields `1`, assuming int is 32 bit
 *  auto result1 = reverse_integral_bits(0x01000000);
 *
 *  // yields `0x0a00`
 *  auto result2 = reverse_integral_bits<std::uint16_t>(0xa);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
T reverse_integral_bytes(T value) {
  return detail::integral_reverser<T, 4>::reverse(value);
}

/**
 * Reverses the bits of an integral in Big Theta(lg k), where k is the size of
 * the integral in bits.
 *
 * Example:
 *
 *  // yields `1`, assuming int is 32 bit
 *  auto result1 = reverse_integral_bits(0b10000000000000000000000000000000);
 *
 *  // yields `5000`
 *  auto result2 = reverse_integral_bits<std::uint16_t>(0xa);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
T reverse_integral_bits(T value) {
  return detail::integral_reverser<T, 0>::reverse(value);
}

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
 * most_significant_bit returns the 1-based position of the
 * most significant bit (0 for no bits set) for n
 *
 * This is the same as log_2(n) + 1, except for n = 0
 *
 * Usage:
 *
 * auto msb = most_significant_bit<10>::value;
 *  // msb is now 4
 *
 * template <std::uintmax_t msb>
 * struct Foo { ... };
 *
 * Foo<most_significant_bit<1024>::value> foo;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <std::uintmax_t Value>
struct msb_mp_impl {
  using type = std::integral_constant<
    std::size_t,
    1 + msb_mp_impl<(Value >> 1)>::type::value
  >;
};

template <>
struct msb_mp_impl<0> {
  using type = std::integral_constant<std::size_t, 0>;
};

} // namespace detail {

template <std::uintmax_t Value>
using most_significant_bit = typename detail::msb_mp_impl<Value>::type;

///////////////
// pop_count //
///////////////

// TODO: DOCUMENT AND TEST
namespace detail {

template <std::uintmax_t Value>
struct pop_count_impl {
  using type = std::integral_constant<
    std::size_t,
    pop_count_impl<Value & (Value - 1)>::type::value + 1
  >;
};

template <>
struct pop_count_impl<0> {
  using type = std::integral_constant<std::size_t, 0>;
};

} // namespace detail {

template <std::uintmax_t Value>
using pop_count = typename detail::pop_count_impl<Value>::type;

////////////////////
// known integers //
////////////////////

namespace detail {

template <typename TList>
using uniquify_list_by_bit_size_impl = typename TList::template unique<>
  ::template sort<
    variadic_transform<
      comparison_transform::less_than,
      data_bits,
      data_bits
    >::apply
  >;

template <typename... Args>
using uniquify_args_by_bit_size_impl = uniquify_list_by_bit_size_impl<
  type_list<Args...>
>;

} // namespace detail {

// TODO: DOCUMENT AND TEST

using known_signed_integers = detail::uniquify_args_by_bit_size_impl<
  short, int, long, long long,
  std::int8_t, std::int16_t, std::int32_t, std::int64_t
>;

static_assert(
  known_signed_integers
    ::transform<type_member_transform<std::is_signed>::apply>
    ::apply<logical::all>
    ::value,
  "invalid signed integer"
);

using known_unsigned_integers = detail::uniquify_args_by_bit_size_impl<
  bool, unsigned short, unsigned int, unsigned long, unsigned long long,
  std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
>;

static_assert(
  known_unsigned_integers
    ::transform<type_member_transform<std::is_unsigned>::apply>
    ::apply<logical::all>
    ::value,
  "invalid unsigned integer"
);

using known_fast_signed_integers = detail::uniquify_args_by_bit_size_impl<
  std::int_fast8_t, std::int_fast16_t, std::int_fast32_t,
  std::int_fast64_t, long long
>;

static_assert(
  known_fast_signed_integers
    ::transform<type_member_transform<std::is_signed>::apply>
    ::apply<logical::all>
    ::value,
  "invalid fast signed integer"
);

using known_fast_unsigned_integers = detail::uniquify_args_by_bit_size_impl<
  bool, std::uint_fast8_t, std::uint_fast16_t, std::uint_fast32_t,
  std::uint_fast64_t, unsigned long long
>;

static_assert(
  known_fast_unsigned_integers
    ::transform<type_member_transform<std::is_unsigned>::apply>
    ::apply<logical::all>
    ::value,
  "invalid fast unsigned integer"
);

using known_least_signed_integers = detail::uniquify_args_by_bit_size_impl<
  std::int_least8_t, std::int_least16_t, std::int_least32_t,
  std::int_least64_t, long long
>;

static_assert(
  known_least_signed_integers
    ::transform<type_member_transform<std::is_signed>::apply>
    ::apply<logical::all>
    ::value,
  "invalid least signed integer"
);

using known_least_unsigned_integers = detail::uniquify_args_by_bit_size_impl<
  bool, std::uint_least8_t, std::uint_least16_t, std::uint_least32_t,
  std::uint_least64_t, unsigned long long
>;

static_assert(
  known_least_unsigned_integers
    ::transform<type_member_transform<std::is_unsigned>::apply>
    ::apply<logical::all>
    ::value,
  "invalid least unsigned integer"
);

///////////////////////////
// known_floating_points //
///////////////////////////

// TODO: DOCUMENT AND TEST

using known_floating_points = detail::uniquify_args_by_bit_size_impl<
  float, double, long double
>;

static_assert(
  known_floating_points
    ::transform<type_member_transform<std::is_floating_point>::apply>
    ::apply<logical::all>
    ::value,
  "invalid floating point"
);

/////////////////////////////////
// smallest_type_for_bit_count //
/////////////////////////////////

// TODO: DOCUMENT AND TEST

namespace detail {

template <typename TList, std::size_t BitCount>
struct smallest_for_impl {
  using type = typename detail::uniquify_list_by_bit_size_impl<TList>
    ::template search<
      transform_aggregator<
        comparison_transform::less_than_equal,
        constant_transform<std::size_t, BitCount>::template apply,
        data_bits
      >::template apply
    >;

  static_assert(
    !std::is_same<type_not_found_tag, type>::value
      && BitCount <= data_bits<type>::value,
    "there's no known type to hold that many bits"
  );
};

} // namespace detail

template <typename TList, std::size_t BitCount>
using smallest_type_for_bit_count = typename detail::smallest_for_impl<
  TList, BitCount
>::type;

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
template <std::size_t BitCount>
using smallest_signed_integral = smallest_type_for_bit_count<
  known_signed_integers, BitCount
>;

template <std::size_t BitCount>
using smallest_unsigned_integral = smallest_type_for_bit_count<
  known_unsigned_integers, BitCount
>;

template <std::size_t BitCount>
using smallest_fast_signed_integral = smallest_type_for_bit_count<
  known_fast_signed_integers, BitCount
>;

template <std::size_t BitCount>
using smallest_fast_unsigned_integral = smallest_type_for_bit_count<
  known_fast_unsigned_integers, BitCount
>;

template <std::size_t BitCount>
using smallest_least_signed_integral = smallest_type_for_bit_count<
  known_least_signed_integers, BitCount
>;

template <std::size_t BitCount>
using smallest_least_unsigned_integral = smallest_type_for_bit_count<
  known_least_unsigned_integers, BitCount
>;

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
template <unsigned long long Value>
using smallest_uint_for_value = smallest_unsigned_integral<
  most_significant_bit<Value>::value
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
      most_significant_bit<Exponent>::value
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
      most_significant_bit<Nth>::value
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

#endif // FATAL_INCLUDE_fatal_math_numerics_h
