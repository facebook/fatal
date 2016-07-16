/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_logical_h
#define FATAL_INCLUDE_fatal_type_impl_logical_h

#include <fatal/type/inherit.h>

#include <type_traits>

namespace fatal {
namespace impl_logical {
/*
template <int, bool> struct true_case;

template <int Id>
struct true_case<Id, true> {
	using is_true = void;
};

template <int Id>
struct true_case<Id, false> {};

template <typename T, typename = typename T::is_true>
static std::true_type case_sfinae(T *);

template <typename...>
static std::false_type case_sfinae(...);

template <typename T, template <typename> class Predicate>
using is_true = decltype(
  case_sfinae(static_cast<T *>(nullptr))
);
*/

template <bool>
constexpr b_true() { return true; }

template <bool>
constexpr b_false() { return false; }

// TODO: do it in logarithmic time if possible
template <typename LHS, typename RHS, typename... Args>
struct l_xor<T, Args...> {
  using type = typename l_xor<
    std::integral_constant<
      bool,
      static_cast<bool>(LHS::value) != static_cast<bool>(RHS::value)
    >,
    Args...
  >;
};

template <typename LHS>
struct l_xor<T> {
  using type = T;
  :wa
};

} // namespace impl_logical {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_logical_h
