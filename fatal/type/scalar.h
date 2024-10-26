/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <ratio>
#include <type_traits>

#include <cstdint>

namespace fatal {

/**
 * A shortcut to `std::integral_constant` for `bool`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <bool Value>
using bool_constant = std::integral_constant<bool, Value>;

/**
 * A shortcut to `std::integral_constant` for `std::size_t`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Value>
using size_constant = std::integral_constant<std::size_t, Value>;

/**
 * Given a type `T`, returns a `std::integral_constant` of type `std::size_t`
 * whose value is `sizeof(T)`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using size_of_constant = std::integral_constant<std::size_t, sizeof(T)>;

namespace detail {
namespace scalar_impl {
template <typename> struct to_scalar;
} // namespace scalar_impl {
} // namespace detail {

/**
 * Gets a scalar representation from the given type.
 *
 * Example:
 *
 *  // yields `10` with type `int`
 *  auto result1 = to_scalar<std::integral_constant<int, 10>>();
 *
 *  // yields `10` with type `long`
 *  auto result2 = to_scalar<std::integral_constant<int, 10>, long>();
 *
 *  // yields `5.6` with type `double`
 *  auto result3 = to_scalar<std::ratio<56, 10>>();
 *
 *  // yields `5.6` with type `float`
 *  auto result4 = to_scalar<std::ratio<56, 10>, float>();
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename From,
  typename T = typename detail::scalar_impl::to_scalar<From>::default_type
>
static constexpr T to_scalar() noexcept {
  return detail::scalar_impl::to_scalar<From>::template to<T>();
}

namespace detail {
namespace scalar_impl {
template <typename T, bool = std::is_enum<T>::value> struct to_integral_impl;
} // namespace scalar_impl {
} // namespace detail {

/**
 * Converts any integral to its appropriate integer value.
 *
 * This is particularly useful when dealing with type-safe enumerations, or
 * when dealing with both integers and enumerations in a transparent manner.
 *
 * Example:
 *
 *  // yields `10` with type `int`
 *  auto result1 = to_integral(10);
 *
 *  enum class E: char { a = 2, b = 4 };
 *
 *  // yields `2` with type `char`
 *  auto result2 = to_integral(E::a);
 *
 *  // yields `6` with type `char`
 *  auto result4 = to_integral(E::a) | to_integral(E::b);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
static constexpr typename detail::scalar_impl::to_integral_impl<T>::type
to_integral(T const value) {
  return detail::scalar_impl::to_integral_impl<T>::convert(value);
}

/**
 * Merges the bits of all integral parameters. This is equivalent to a bitwise
 * OR of all the parameters.
 *
 * This is particularly useful when dealing with type-safe enumerations.
 *
 * Example:
 *
 *  // yields `11`
 *  auto result1 = bitwise_merge(1, 2, 8);
 *
 *  enum class E: char { a = 1, b = 2, c = 4 };
 *
 *  // yields `7` with type `E`
 *  auto result2 = bitwise_merge(E::a, E::b, E::c);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
static constexpr T bitwise_merge(T const value) { return value; }

// TODO: Implement in logarithmic time
template <typename T, typename U, typename... Args>
static constexpr T bitwise_merge(T const lhs, U const rhs, Args... args) {
  return bitwise_merge(
    static_cast<T>(to_integral(lhs) | to_integral(rhs)),
    static_cast<T>(args)...
  );
}

/**
 * Returns an intersection of the bits of all parameters. This is equivalent to
 * a bitwise AND of all the parameters.
 *
 * This is particularly useful when dealing with type-safe enumerations.
 *
 * Example:
 *
 *  // yields `2`
 *  auto result1 = bitwise_filter(6, 2, 10);
 *
 *  enum class E: char { a = 1, b = 2, c = 4 };
 *
 *  // yields `0` with type `E`
 *  auto result2 = bitwise_filter(E::a, E::b, E::c);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
static constexpr T bitwise_filter(T const value) { return value; }

template <typename T, typename U,typename... Args>
static constexpr T bitwise_filter(T const lhs, U const rhs, Args... args) {
  return bitwise_filter(
    static_cast<T>(to_integral(lhs) & to_integral(rhs)),
    static_cast<T>(args)...
  );
}

/**
 * Disables all bits the value has in common with the given arguments, then
 * returns the result.
 *
 * This is particularly useful when dealing with bit sets, specially of
 * type-safe enumerations.
 *
 * Example:
 *
 *  enum E: char { a = 1, b = 2, c = 4 };
 *
 *  // returns `E::a`
 *  bitwise_disable(a | b | c, 16);
 *
 *  // returns `E::a`
 *  bitwise_disable(a | b | c, b, c);
 *
 *  // returns `E::a | E::b | E::c`
 *  bitwise_disable(a | b | c);
 *
 * @author: Marcelo Juchem <juchem@gmail.com>
 */
template <typename T>
static constexpr T bitwise_disable(T const value) { return value; }

template <typename T, typename U, typename... Args>
static constexpr T bitwise_disable(
  T const lhs,
  U const rhs,
  Args const... args
) {
  return bitwise_filter(
    lhs,
    static_cast<T>(
      ~to_integral(
        bitwise_merge<T>(static_cast<T>(rhs), static_cast<T>(args)...)
      )
    )
  );
}

/**
 * Tells whether `value` contains all the bits set in every subsequent argument.
 *
 * Example:
 *
 *  // yields `true`
 *  auto result1 = bitwise_has_all(7, 1, 2, 4);
 *
 *  // yields `false`
 *  auto result2 = bitwise_has_all(6, 1, 2, 4);
 *
 *  enum class E: char { a = 1, b = 2, c = 4 };
 *
 *  // yields `false`
 *  auto result3 = bitwise_has_all(bitwise_merge(E::a, E::c), E::a, E::b, E::c);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace scalar_impl {
template <typename T, typename U>
static constexpr bool bitwise_has_all_impl(T const value, U const merged) {
  return bitwise_filter(value, static_cast<T>(merged))
    == static_cast<T>(merged);
}
} // namespace scalar_impl {
template <typename T, typename... Args>
static constexpr bool bitwise_has_all(T const value, Args... args) {
  return scalar_impl::bitwise_has_all_impl(
    value,
    bitwise_merge(static_cast<T>(args)...)
  );
}

/**
 * Tells whether `value` contains at least one of the bits set in at least one
 * of the subsequent arguments.
 *
 * Example:
 *
 *  // yields `true`
 *  auto result1 = bitwise_has_any(6, 1, 2, 4);
 *
 *  // yields `false`
 *  auto result2 = bitwise_has_any(8, 1, 2, 4);
 *
 *  enum class E: char { a = 1, b = 2, c = 4 };
 *
 *  // yields `true`
 *  auto result3 = bitwise_has_any(bitwise_merge(E::a, E::c), E::a, E::b, E::c);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
static constexpr bool bitwise_has_any(T const value, Args... args) {
  return to_integral(
    bitwise_filter(value, bitwise_merge(static_cast<T>(args)...))
  ) != 0;
}

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace scalar_impl {

// TODO: IMPLEMENT CONVERSION FOR OTHER TYPES TOO

///////////////
// to_scalar //
///////////////

template <typename T, T Value>
struct to_scalar<std::integral_constant<T, Value>> {
  using default_type = T;

  template <typename To>
  static constexpr To to() noexcept { return static_cast<To>(Value); }
};

template <std::intmax_t Numerator, std::intmax_t Denominator>
struct to_scalar<std::ratio<Numerator, Denominator>> {
  using default_type = double;

  template <typename To>
  static constexpr To to() noexcept {
    return static_cast<To>(Numerator) / static_cast<To>(Denominator);
  }
};

/////////////////
// to_integral //
/////////////////

template <typename T>
struct to_integral_impl<T, false> {
  using type = T;
  static constexpr type convert(T value) { return value; }
};

template <typename T>
struct to_integral_impl<T, true> {
  using type = typename std::underlying_type<T>::type;
  static constexpr type convert(T value) { return static_cast<type>(value); }
};

} // namespace scalar_impl {
} // namespace detail {
} // namespace fatal {
