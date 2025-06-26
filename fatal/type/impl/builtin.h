/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/preprocessor.h>

// note: no dependencies!

namespace fatal {

namespace builtin {
namespace make_integer_seq {

namespace impl {

template <typename...> struct cat;

template <
  template <typename T, T...> class Sequence,
  typename T, T... V0, T... V1, T... V
>
struct cat<Sequence<T, V0...>, Sequence<T, V1...>, Sequence<T, V...>> {
  using type = Sequence<
    T,
    V0...,
    (T(sizeof...(V0)) + V1)...,
    (T(sizeof...(V0)) + T(sizeof...(V1)) + V)...
  >;
};

template <std::size_t N>
struct go_ {
  template <typename S0, typename S1>
  using apply = typename cat<
    typename go_<N / 2>::template apply<S0, S1>,
    typename go_<N / 2>::template apply<S0, S1>,
    typename go_<N % 2>::template apply<S0, S1>
  >::type;
};

template <> struct go_<1> {
  template <typename S0, typename S1>
  using apply = S1;
};
template <> struct go_<0> {
  template <typename S0, typename S1>
  using apply = S0;
};

} // namespace impl

template <template <typename T, T...> class S, typename T, std::size_t N>
using __make_integer_seq = typename impl::go_<N>::template apply<S<T>, S<T, T(0)>>;

} // namespace make_integer_seq
} // namespace builtin

#if !FATAL_HAS_BUILTIN(__make_integer_seq) && _MSC_FULL_VER < 190023918

/// __make_integer_seq
///
/// A lookalike for __make_integer_seq, when the builtin is unavailable.
///
/// mimic: __make_integer_seq, clang++
using builtin::make_integer_seq::__make_integer_seq;

#endif

namespace builtin {
namespace type_pack_element {

namespace impl {

/// rec
///
/// Simplest way of doing __type_pack_element, but not the most efficient.
/// Should work everywhere.
///
/// mimic: __type_pack_element, clang++

template <std::size_t I, typename T, typename... Ts>
struct rec_: rec_<I - 1, Ts...> {};
template <typename T, typename... Ts>
struct rec_<0, T, Ts...> { using type = T; };

template <std::size_t I, typename... T>
using rec = typename rec_<I, T...>::type;

/// set
///
/// Relatively efficient but somewhat simple way of doing __type_pack_element.
/// Should work everywhere. Some compilers may not be able to instantiate this.
///
/// mimic: __type_pack_element, clang++

template <typename T, T...> struct iseq;
template <std::size_t I, typename T> struct indexed {};

template <typename, typename...>
struct iset;
template <std::size_t... I, typename... T>
struct iset<iseq<std::size_t, I...>, T...>: indexed<I, T>... {};
template <typename... T>
using iset_t = iset<__make_integer_seq<iseq, std::size_t, sizeof...(T)>, T...>;
template <typename... T>
constexpr iset_t<T...>* iset_v = nullptr;

struct setq {
  template <std::size_t I, typename T>
  static T go(indexed<I, T>*);
};

template <std::size_t I, typename... Ts>
using set = decltype(setq::go<I>(iset_v<Ts...>));

/// choose an implementation for the current platform

/// any
///
/// Either rec or set, depending on the platform.
///
/// mimic: __type_pack_element, clang++

template <template <std::size_t, typename...> class Impl>
struct any1_ {
  template <std::size_t I, typename... T>
  using apply = Impl<I, T...>;
};

#if defined(_MSC_VER) && _MSC_VER < /* vs2019 = */ 1920
using any_ = any1_<rec>;
#else
using any_ = any1_<set>;
#endif

template <std::size_t I, typename... T>
using any = typename any_::template apply<I, T...>;

} // namespace impl {

template <std::size_t I, typename... T>
using __type_pack_element = impl::any<I, T...>;

} // namespace type_pack_element {
} // namespace builtin {

#if !FATAL_HAS_BUILTIN(__type_pack_element)

/// __type_pack_element
///
/// A lookalike for __type_pack_element, when the builtin is unavailable.
///
/// mimic: __type_pack_element, clang++
using builtin::type_pack_element::__type_pack_element;

#endif

} // namespace fatal {
