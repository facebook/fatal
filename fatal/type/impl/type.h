/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_type_h
#define FATAL_INCLUDE_fatal_type_impl_type_h

#include <type_traits>

namespace fatal {
namespace impl_t {

template <typename T>
struct to {
  using type = typename T::type;
};

template <typename T, T Value>
struct to<std::integral_constant<T, Value>> {
  using type = typename std::integral_constant<T, Value>::type;
};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct to<Variadics<T, Values...>> {
  using type = T;
};

template <
  template <typename, std::size_t> class Template,
  typename T,
  typename std::enable_if<
    !std::is_same<T, std::size_t>::value,
    std::size_t
  >::type Value
>
struct to<Template<T, Value>> {
  using type = T;
};

template <typename T>
struct vto {
  using type = typename T::value_type;
};

template <template <typename...> class Variadics, typename T, T... Values>
struct vto<Variadics<std::integral_constant<T, Values>...>> {
  using type = T;
};

template <template <typename V, V...> class Variadics, typename T, T... Values>
struct vto<Variadics<T, Values...>> {
  using type = T;
};

template <
  template <typename, std::size_t> class Template,
  typename T,
  typename std::enable_if<
    !std::is_same<T, std::size_t>::value,
    std::size_t
  >::type Value
>
struct vto<Template<T, Value>> {
  using type = T;
};

} // namespace impl_t {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_type_h
