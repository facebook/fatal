/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/identity.h>
#include <fatal/type/list.h>
#include <fatal/type/pair.h>
#include <fatal/type/sequence.h>

#include <type_traits>

#include <fatal/type/impl/convert.h>

namespace fatal {

template <
  typename From,
  template <typename V, V...> class Sequence = sequence,
  typename... T
>
using as_sequence = typename impl_cv::s<Sequence<int>, From, T...>::type;

template <typename T, template <typename...> class List = list>
using as_list = typename impl_cv::l<List, T>::type;

template <typename To, typename T, typename... Args>
static constexpr To to_instance(Args &&...args) {
  return impl_cv::t<To, T>::to(static_cast<Args &&>(args)...);
}

namespace bound {

template <template <typename V, V...> class Sequence, typename... T>
struct as_sequence {
  template <typename From>
  using apply = typename impl_cv::s<Sequence<int>, From, T...>::type;
};

template <template <typename...> class List>
struct as_list {
  template <typename T>
  using apply = fatal::as_list<T, List>;
};

} // namespace bound {
} // namespace fatal {
