/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_scalar_h
#define FATAL_INCLUDE_fatal_type_scalar_h

#include <ratio>
#include <type_traits>

#include <cstdint>

namespace fatal {

// TODO: DOCUMENT AND TEST
template <bool Value>
using bool_constant = std::integral_constant<bool, Value>;

// TODO: DOCUMENT AND TEST
template <std::size_t Value>
using size_constant = std::integral_constant<std::size_t, Value>;

// TODO: DOCUMENT AND TEST
template <typename T>
using size_of_constant = std::integral_constant<std::size_t, sizeof(T)>;

namespace detail {
namespace scalar_impl {
template <typename> struct to_scalar;
} // namespace scalar_impl {
} // namespace detail {

// TODO: DOCUMENT AND TEST
template <
  typename From,
  typename T = typename detail::scalar_impl::to_scalar<From>::default_type
>
constexpr T to_scalar() noexcept {
  return detail::scalar_impl::to_scalar<From>::template to<T>();
}

#include <type_traits>

namespace detail {
namespace scalar_impl {

template <typename T, bool = std::is_enum<T>::value>
struct to_integral_impl {
  using type = T;
  static type convert(T value) { return value; }
};

} // namespace scalar_impl {
} // namespace detail {

// TODO: DOCUMENT AND TEST
template <typename T>
typename detail::scalar_impl::to_integral_impl<T>::type to_integral(
  T const value
) {
  return detail::scalar_impl::to_integral_impl<T>::convert(value);
}

// TODO: DOCUMENT AND TEST
template <typename T>
T bitwise_merge(T const value, T const head) {
  return static_cast<T>(to_integral(value) | to_integral(head));
}

// TODO: DOCUMENT AND TEST
template <typename T, typename... Args>
T bitwise_merge(T const value, T const head, Args... args) {
  return bitwise_merge(bitwise_merge(value, head), args...);
}

// TODO: DOCUMENT AND TEST
template <typename T, typename... Args>
T bitwise_filter(T const value, Args... args) {
  return static_cast<T>(
    to_integral(value) & to_integral(bitwise_merge(args...))
  );
}

// TODO: DOCUMENT AND TEST
template <typename T, typename... Args>
bool bitwise_has_all(T const value, Args... args) {
  auto const merged = bitwise_merge(args...);
  return static_cast<T>(to_integral(value) & to_integral(merged)) == merged;
}

// TODO: DOCUMENT AND TEST
template <typename T, typename... Args>
bool bitwise_has_any(T const value, Args... args) {
  return to_integral(bitwise_filter(value, args...)) != 0;
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
struct to_integral_impl<T, true> {
  using type = typename std::underlying_type<T>::type;
  static type convert(T value) { return static_cast<type>(value); }
};

} // namespace scalar_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_scalar_h
