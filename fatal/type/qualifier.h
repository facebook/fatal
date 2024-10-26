/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

namespace fatal {

/**
 * Member function const/volatile qualifiers category.
 *
 * `none`:  neither const nor volatile
 * `c`:     const but not volatile
 * `v`:     volatile but not const
 * `cv`:    both const and volatile (guaranteed
 *          to be the bitwise or of `c` and `v`)
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
enum class cv_qualifier:
  unsigned char
{
  none = 0,
  c = 1,
  v = 2,
  cv = c | v
};

/**
 * Bitwise and (&) operator for `cv_qualifier` to make it easy to
 * check for the presence of either the const or volatile qualifier:
 *
 * Example:
 *
 *  void check_constness(cv_qualifier q) {
 *    if (q & cv_qualifier::c) {
 *      cout << "it is const, dunno about volatile";
 *    }
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
static constexpr bool operator &(cv_qualifier lhs, cv_qualifier rhs) {
  using type = std::underlying_type<cv_qualifier>::type;
  return static_cast<bool>(static_cast<type>(lhs) & static_cast<type>(rhs));
}

enum class ref_qualifier:
  unsigned char
{
  none = 0,
  lvalue = 1,
  rvalue = 2
};

/**
 * Applies `std::add_const` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_const_from<foo, int>::type;
 *  using result1 = add_const_from_t<foo, int>;
 *
 *  // yields `foo const`
 *  using result2 = add_const_from<foo, int const>::type;
 *  using result2 = add_const_from_t<foo, int const>;
 *
 *  // yields `foo const`
 *  using result3 = add_const_from<foo const, int const>::type;
 *  using result3 = add_const_from_t<foo const, int const>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_const_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_const_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

template <typename T, typename TFrom>
using add_const_from_t = typename add_const_from<T, TFrom>::type;

/**
 * Applies `std::add_volatile` to a type iff some other type is volatile.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_volatile_from<foo, int>::type;
 *  using result1 = add_volatile_from_t<foo, int>;
 *
 *  // yields `foo const`
 *  using result2 = add_volatile_from<foo, int volatile>::type;
 *  using result2 = add_volatile_from_t<foo, int volatile>;
 *
 *  // yields `foo const`
 *  using result3 = add_volatile_from<foo volatile, int volatile>::type;
 *  using result3 = add_volatile_from_t<foo volatile, int volatile>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_volatile_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_volatile_from<T, TFrom volatile> {
  using type = typename std::add_volatile<T>::type;
};

template <typename T, typename TFrom>
using add_volatile_from_t = typename add_volatile_from<T, TFrom>::type;

/**
 * Applies `std::add_const` to a type iff some other type is const.
 * Applies `std::add_volatile` to a type iff some other type is volatile.
 * Applies `std::add_cv` toa  type iff some other type is const volatile.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_cv_from<foo, int>::type;
 *  using result1 = add_cv_from_t<foo, int>;
 *
 *  // yields `foo const`
 *  using result2 = add_cv_from<foo, int const>::type;
 *  using result2 = add_cv_from_t<foo, int const>;
 *
 *  // yields `foo const`
 *  using result3 = add_cv_from<foo const, int const>::type;
 *  using result3 = add_cv_from_t<foo const, int const>;
 *
 *  // yields `foo volatile`
 *  using result4 = add_cv_from<foo, int volatile>::type;
 *  using result4 = add_cv_from_t<foo, int volatile>;
 *
 *  // yields `foo volatile`
 *  using result5 = add_cv_from<foo volatile, int volatile>::type;
 *  using result5 = add_cv_from_t<foo volatile, int volatile>;
 *
 *  // yields `foo const volatile`
 *  using result6 = add_cv_from<foo, int const volatile>::type;
 *  using result6 = add_cv_from_t<foo, int const volatile>;
 *
 *  // yields `foo const volatile`
 *  using result7 = add_cv_from<foo const volatile, int const volatile>::type;
 *  using result7 = add_cv_from_t<foo const volatile, int const volatile>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_cv_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_cv_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

template <typename T, typename TFrom>
struct add_cv_from<T, TFrom volatile> {
  using type = typename std::add_volatile<T>::type;
};

template <typename T, typename TFrom>
struct add_cv_from<T, TFrom const volatile> {
  using type = typename std::add_cv<T>::type;
};

template <typename T, typename TFrom>
using add_cv_from_t = typename add_cv_from<T, TFrom>::type;

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, yield `std::add_lvalue_reference<T>::type`
 * - if `U` is an r-value reference, yield `std::add_rvalue_reference<T>::type`
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_reference_from<foo, int>::type;
 *  using result1 = add_reference_from_t<foo, int>;
 *
 *  // yields `foo &&`
 *  using result2 = add_reference_from<foo &&, int>::type;
 *  using result2 = add_reference_from_t<foo &&, int>;
 *
 *  // yields `foo &`
 *  using result3 = add_reference_from<foo, int &>::type;
 *  using result3 = add_reference_from_t<foo, int &>;
 *
 *  // yields `foo &`
 *  using result4 = add_reference_from<foo &&, int &>::type;
 *  using result4 = add_reference_from_t<foo &&, int &>;
 *
 *  // yields `foo &&`
 *  using result5 = add_reference_from<foo, int &&>::type;
 *  using result5 = add_reference_from_t<foo, int &&>;
 *
 *  // yields `foo &`
 *  using result6 = add_reference_from<foo &, int &&>::type;
 *  using result6 = add_reference_from_t<foo &, int &&>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_reference_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &> {
  using type = typename std::add_lvalue_reference<T>::type;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<T>::type;
};

template <typename T, typename TFrom>
using add_reference_from_t = typename add_reference_from<T, TFrom>::type;

/**
 * Combine the effects of `add_cv_from` and `add_reference_from`.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result_1 = add_cv_reference_from<foo, int>::type;
 *  using result_1 = add_cv_reference_from_t<foo, int>;
 *
 *  // yields `foo const &`
 *  using result_2 = add_cv_reference_from<foo, int const &>::type;
 *  using result_2 = add_cv_reference_from_t<foo, int const &>;
 *
 *  // yields `foo volatile &&`
 *  using result_3 = add_cv_reference_from<foo, int volatile &&>::type;
 *  using result_3 = add_cv_reference_from_t<foo, int volatile &&>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_cv_reference_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T &, TFrom const> {
  using type = typename std::add_lvalue_reference<
    typename std::add_const<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T &&, TFrom const> {
  using type = typename std::add_rvalue_reference<
    typename std::add_const<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T, TFrom volatile> {
  using type = typename std::add_volatile<T>::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T &, TFrom volatile> {
  using type = typename std::add_lvalue_reference<
    typename std::add_volatile<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T &&, TFrom volatile> {
  using type = typename std::add_rvalue_reference<
    typename std::add_volatile<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T, TFrom &> {
  using type = typename std::add_lvalue_reference<
    typename add_cv_reference_from<T, TFrom>::type
  >::type;
};

template <typename T, typename TFrom>
struct add_cv_reference_from<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<
    typename add_cv_reference_from<T, TFrom>::type
  >::type;
};

template <typename T, typename TFrom>
using add_cv_reference_from_t = typename add_cv_reference_from<T, TFrom>::type;

} // namespace fatal {
