/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {
namespace detail {
namespace transform_impl {

template <template <typename...> class T, typename... Args>
struct apply { using type = T<Args...>; };

} // namespace transform_impl {
} // namespace detail {

/**
 * Applies arguments to a metafunction.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTransform, typename... Args>
using apply = typename detail::transform_impl::apply<TTransform, Args...>::type;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace transform_impl {

template <template <typename> class T, typename U>
struct apply<T, U> { using type = T<U>; };

template <template <typename, typename> class T, typename U0, typename U1>
struct apply<T, U0, U1> { using type = T<U0, U1>; };

template <
  template <typename, typename, typename> class T,
  typename U0, typename U1, typename U2
>
struct apply<T, U0, U1, U2> { using type = T<U0, U1, U2>; };

template <
  template <typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3
>
struct apply<T, U0, U1, U2, U3> { using type = T<U0, U1, U2, U3>; };

template <
  template <typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4
>
struct apply<T, U0, U1, U2, U3, U4> {
  using type = T<U0, U1, U2, U3, U4>;
};

template <
  template <typename, typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5
>
struct apply<T, U0, U1, U2, U3, U4, U5> {
  using type = T<U0, U1, U2, U3, U4, U5>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0> {
  using type = T<U0, U1, U2, U3, U4, U5, V0>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0, V1> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0, V1, V2> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4, typename V5
>
struct apply<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5>;
};

} // namespace transform_impl {
} // namespace detail {
} // namespace fatal {
