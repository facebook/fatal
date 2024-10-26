/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/tag.h>

#include <type_traits>

namespace fatal {
namespace impl_t {

template <typename T, typename = typename T::type>
std::true_type s(T *);
std::false_type s(...);

template <typename T, bool = decltype(s(static_cast<T *>(nullptr)))::value>
struct to;

template <typename T>
struct to<T, true> {
  using type = typename T::type;
};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct to<Variadics<T, Values...>, false> {
  using type = T;
};

template <typename T, typename = typename T::value_type>
std::true_type S(T *);
std::false_type S(...);

template <typename T, bool = decltype(S(static_cast<T *>(nullptr)))::value>
struct vto;

template <typename T>
struct vto<T, true> {
  using type = typename T::value_type;
};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct vto<Variadics<T, Values...>, false> {
  using type = T;
};

} // namespace impl_t {
} // namespace fatal {
