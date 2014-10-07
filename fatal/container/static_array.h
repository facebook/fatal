/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/list.h>

#include <array>
#include <type_traits>

namespace fatal {
namespace detail {
namespace static_array_impl {

template <typename...> struct head;

template <typename T, typename... Args>
struct head<T, Args...> {
  using type = T;
};

template <typename...> struct element;

template <typename T>
struct element<T> {
  template <typename...>
  using type = T;
};

template <>
struct element<> {
  template <typename... Args>
  using type = typename head<Args...>::type;
};

template <typename, typename, typename...> class instance;
template <typename... Args, typename TFactory, typename... T>
class instance<type_list<Args...>, TFactory, T...> {
  using list = type_list<Args...>;

public:
  using value_type = typename element<T...>::template type<
    typename std::decay<decltype(TFactory::template create<Args>())>::type...
  >;

  using type = std::array<value_type, list::size>;

  static type const get;
};

template <typename... Args, typename TFactory, typename... T>
typename instance<
  type_list<Args...>, TFactory, T...
>::type const instance<
  type_list<Args...>, TFactory, T...
>::get{{
  TFactory::template create<Args>()...
}};

} // namespace static_array_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TFactory, typename... T>
struct static_array_from {
  template <typename TList>
  using list = detail::static_array_impl::instance<
    typename TList::template indexed_transform<indexed_type_tag>,
    TFactory,
    T...
  >;

  template <typename... Args>
  using args = list<type_list<Args...>>;
};

} // namespace fatal {
