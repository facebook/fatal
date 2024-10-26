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

namespace fatal {
namespace i_P {

template <typename...> struct b;

template <template <typename...> class T, typename... Args, typename... Suffix>
struct b<T<Args...>, Suffix...> {
  using type = T<Args..., Suffix...>;
};

template <bool, typename...> struct B;

template <template <typename...> class T, typename... Args, typename... Suffix>
struct B<true, T<Args...>, Suffix...> {
  using type = T<Args..., Suffix...>;
};

template <typename T, typename... Suffix>
struct B<false, T, Suffix...> {
  using type = T;
};

template <typename...> struct f;

template <template <typename...> class T, typename... Args, typename... Prefix>
struct f<T<Args...>, Prefix...> {
  using type = T<Prefix..., Args...>;
};

template <bool, typename...> struct F;

template <template <typename...> class T, typename... Args, typename... Prefix>
struct F<true, T<Args...>, Prefix...> {
  using type = T<Prefix..., Args...>;
};

template <typename T, typename... Prefix>
struct F<false, T, Prefix...> {
  using type = T;
};

template <typename> struct p;

template <template <typename...> class List, typename... Args>
struct p<List<Args...>> {
  template <typename... Suffix>
  using back = List<Args..., Suffix...>;

  template <typename... Prefix>
  using front = List<Prefix..., Args...>;

  template <bool C, typename... Suffix>
  using back_if = typename B<C, List<Args...>, Suffix...>::type;

  template <bool C, typename... Prefix>
  using front_if = typename F<C, List<Args...>, Prefix...>::type;
};

template <bool, typename T, typename, T...> struct E;

template <
  typename T, template <typename V, V...> class Sequence,
  T... Values, T... Suffix
>
struct E<true, T, Sequence<T, Values...>, Suffix...> {
  using type = Sequence<T, Values..., Suffix...>;
};

template <typename T, typename Sequence, T... Suffix>
struct E<false, T, Sequence, Suffix...> {
  using type = T;
};

template <bool, typename T, typename, T...> struct S;

template <
  typename T, template <typename V, V...> class Sequence,
  T... Values, T... Prefix
>
struct S<true, T, Sequence<T, Values...>, Prefix...> {
  using type = Sequence<T, Prefix..., Values...>;
};

template <typename T, typename Sequence, T... Prefix>
struct S<false, T, Sequence, Prefix...> {
  using type = Sequence;
};

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct p<Sequence<T, Values...>> {
  template <T... Suffix>
  using back = Sequence<T, Values..., Suffix...>;

  template  <T... Prefix>
  using front = Sequence<T, Prefix..., Values...>;

  using type = Sequence<T, Values...>;

  template <bool C, T... Suffix>
  using back_if = typename E<C, T, type, Suffix...>::type;

  template<bool C, T... Prefix>
  using front_if = typename S<C, T, type, Prefix...>::type;
};

} // namespace i_P {
} // namespace fatal {
