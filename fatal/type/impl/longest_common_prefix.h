/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {
namespace impl_lcp {

// longest common prefix size //

template <
  bool,
  template <typename, std::size_t> class,
  std::size_t Offset,
  std::size_t,
  typename...
>
struct s {
  using type = std::integral_constant<std::size_t, Offset>;
};

template <
  template <typename, std::size_t> class Key,
  std::size_t Offset,
  std::size_t End
>
struct s<true, Key, Offset, End> {
  using type = std::integral_constant<std::size_t, Offset>;
};

template <
  template <typename, std::size_t> class Key,
  std::size_t End,
  typename... Args
>
struct s<false, Key, End, End, Args...> {
  using type = std::integral_constant<std::size_t, End>;
};

template <
  template <typename, std::size_t> class Key,
  std::size_t End,
  typename T, typename... Args
>
struct s<true, Key, End, End, T, Args...> {
  using type = std::integral_constant<std::size_t, End>;
};

template <
  template <typename, std::size_t> class Key,
  std::size_t Offset,
  std::size_t End,
  typename T, typename... Args
>
struct s<true, Key, Offset, End, T, Args...>:
  s<
    std::is_same<
      list<Key<T, Offset>, Key<T, Offset>, Key<Args, Offset>...>,
      list<Key<T, Offset>, Key<Args, Offset>..., Key<T, Offset>>
    >::value,
    Key,
    Offset + std::is_same<
      list<Key<T, Offset>, Key<T, Offset>, Key<Args, Offset>...>,
      list<Key<T, Offset>, Key<Args, Offset>..., Key<T, Offset>>
    >::value,
    End,
    T, Args...
  >
{};

} // namespace impl_lcp {
} // namespace fatal {
