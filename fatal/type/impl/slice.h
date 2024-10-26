/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/builtin.h>
#include <fatal/type/cat.h>
#include <fatal/type/conditional.h>
#include <fatal/type/inherit.h>
#include <fatal/type/sequence.h>
#include <fatal/type/tag.h>

#include <type_traits>

namespace fatal {
namespace i_at {

template <typename List>
struct pick_var_;
template <
  template <typename...> class Variadic,
  typename... Args
>
struct pick_var_<Variadic<Args...>> {
  template <std::size_t i>
  using t = __type_pack_element<i, Args...>;
  template <std::size_t... i>
  using apply = Variadic<t<i>...>;
};
template <
  template <typename V, V...> class Variadic,
  typename T, T... Args
>
struct pick_var_<Variadic<T, Args...>> {
  template <std::size_t i>
  using t = __type_pack_element<i, std::integral_constant<T, Args>...>;
  template <std::size_t... i>
  using apply = Variadic<T, t<i>::value...>;
};

template <typename Seq>
struct pick_seq_;
template <std::size_t... Indexes>
struct pick_seq_<index_sequence<Indexes...>> {
  template <typename List>
  using apply = typename pick_var_<List>::template apply<Indexes...>;
};

template <typename> struct a;

template <template <typename> class Single, typename T>
struct a<Single<T>> {
  template <std::size_t Index>
  using apply = __type_pack_element<Index, T>;
};

template <
  template <typename...> class List,
  typename... Args
>
struct a<List<Args...>> {
  template <std::size_t Index>
  using apply = __type_pack_element<Index, Args...>;
};

template <
  template <typename V, V...> class Sequence,
  typename T, T... Values
>
struct a<Sequence<T, Values...>> {
  template <std::size_t Index>
  using apply =
    __type_pack_element<Index, std::integral_constant<T, Values>...>;
};

template <typename Default>
struct a0 {
  template <std::size_t>
  using apply = Default;
};

template <typename T, std::size_t Index>
std::integral_constant<std::size_t, Index> I(indexed<T, Index>);

template <std::size_t, typename T, std::size_t Index>
std::integral_constant<std::size_t, Index> TI(indexed<T, Index>);

template <std::size_t Size, typename>
std::integral_constant<std::size_t, Size> TI(...);

template <typename T, T, std::size_t> struct vi {};

template <typename T, T Value, std::size_t Index>
std::integral_constant<std::size_t, Index> IV(vi<T, Value, Index>);

template <std::size_t, typename T, T Value, std::size_t Index>
std::integral_constant<std::size_t, Index> TIV(vi<T, Value, Index>);

template <std::size_t Size, typename T, T>
std::integral_constant<std::size_t, Size> TIV(...);

template <typename...> struct i;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct i<List<Args...>, index_sequence<Indexes...>> {
  template <typename T>
  using apply = decltype(I<T>(inherit<indexed<Args, Indexes>...>()));
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct i<Sequence<T, Values...>, index_sequence<Indexes...>> {
  template <T Value>
  using apply = decltype(IV<T, Value>(inherit<vi<T, Values, Indexes>...>()));
};

template <typename...> struct ti;

template <
  template <typename...> class List,
  typename... Args,
  std::size_t... Indexes
>
struct ti<List<Args...>, index_sequence<Indexes...>> {
  template <typename T>
  using apply = decltype(
    TI<sizeof...(Args), T>(inherit<indexed<Args, Indexes>...>())
  );
};

template <
  template <typename V, V...> class Sequence,
  typename T,
  T... Values,
  std::size_t... Indexes
>
struct ti<Sequence<T, Values...>, index_sequence<Indexes...>> {
  template <T Value>
  using apply = decltype(
    TIV<sizeof...(Values), T, Value>(inherit<vi<T, Values, Indexes>...>())
  );
};

} // namespace i_at {
} // namespace fatal {
