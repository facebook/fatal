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

#include <fatal/type/list.h>

namespace fatal {
namespace i_cpl {

template <template <typename...> class T, std::size_t Arity>
struct checker {
  static_assert(Arity > 0);

  template <typename U, typename... Args, typename = T<U, Args...>>
  static std::true_type sfinae(list<U, Args...> *);

  template <typename... Args, typename = T<Args...>>
  static std::true_type sfinae(list<Args...> *);

  template <typename...>
  static std::false_type sfinae(...);
};

template <template <typename...> class T>
struct checker<T, 0> {
  template <typename = T<>>
  static std::true_type sfinae(list<> *);

  template <typename...>
  static std::false_type sfinae(...);
};

template <template <typename...> class T>
struct checker<T, 1> {
  template <typename U, typename = T<U>>
  static std::true_type sfinae(list<U> *);

  template <typename...>
  static std::false_type sfinae(...);
};

template <template <typename...> class T>
struct checker<T, 2> {
  template <typename U0, typename U1, typename = T<U0, U1>>
  static std::true_type sfinae(list<U0, U1> *);

  template <typename...>
  static std::false_type sfinae(...);
};

template <template <typename...> class T>
struct checker<T, 3> {
  template <typename U0, typename U1, typename U2, typename = T<U0, U1, U2>>
  static std::true_type sfinae(list<U0, U1, U2> *);

  template <typename...>
  static std::false_type sfinae(...);
};

} // namespace i_cpl {
} // namespace fatal {
