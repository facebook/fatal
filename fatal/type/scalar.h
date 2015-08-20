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

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace scalar_impl {

// TODO: IMPLEMENT CONVERSION FOR OTHER TYPES TOO

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

} // namespace scalar_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_scalar_h
