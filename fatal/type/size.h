/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_size_h
#define FATAL_INCLUDE_fatal_type_size_h

#ifndef NDEBUG
#include <fatal/type/debug.h>
#endif // NDEBUG

#include <type_traits>

namespace fatal {

template <typename> struct size;

template <template <typename...> class T, typename... Args>
struct size<T<Args...>>:
  std::integral_constant<std::size_t, sizeof...(Args)>
{};

template <typename T, template <typename, T...> class Sequence, T... Values>
struct size<Sequence<T, Values...>>:
  std::integral_constant<std::size_t, sizeof...(Values)>
{};

template <typename T>
using empty = std::integral_constant<bool, size<T>::value == 0>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_size_h
