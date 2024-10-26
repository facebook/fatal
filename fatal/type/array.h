/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/array.h>

#include <fatal/type/identity.h>
#include <fatal/type/sequence.h>
#include <fatal/type/size.h>

#include <cstddef>

namespace fatal {

namespace c_array_ {

template <typename T, std::size_t S>
struct c_array {
  T data[S ? S : 1];
  constexpr T& operator[](std::size_t i) noexcept { return data[i]; }
  constexpr T const& operator[](std::size_t i) const noexcept { return data[i]; }
};

template <typename T, std::size_t S>
constexpr std::size_t size(c_array<T, S> const &) noexcept { return S; }
template <typename T, std::size_t S>
constexpr bool empty(c_array<T, S> const &) noexcept { return S == 0; }
template <typename T, std::size_t S>
constexpr T* data(c_array<T, S> &c) { return c.data; }
template <typename T, std::size_t S>
constexpr T const* data(c_array<T, S> const &c) { return c.data; }
template <typename T, std::size_t S>
constexpr T* begin(c_array<T, S> &c) { return c.data; }
template <typename T, std::size_t S>
constexpr T const* begin(c_array<T, S> const &c) { return c.data; }
template <typename T, std::size_t S>
constexpr T const* cbegin(c_array<T, S> const &c) { return c.data; }
template <typename T, std::size_t S>
constexpr T* end(c_array<T, S> &c) { return c.data + S; }
template <typename T, std::size_t S>
constexpr T const* end(c_array<T, S> const &c) { return c.data + S; }
template <typename T, std::size_t S>
constexpr T const* cend(c_array<T, S> const &c) { return c.data + S; }

} // namespace c_array_ {

using c_array_::c_array;

template <
  template <typename, std::size_t> class A, typename T, std::size_t S,
  A<T, S> const &V
>
using array_to_sequence =
    typename i_a::array_to_sequence_<A, T, S>::template apply<V>;

template <
  typename Array,
  typename OuterFilter,
  typename InnerFilter = get_identity,
  typename... T
>
using as_array_filtered = i_a::CF<
  make_index_sequence<size<typename OuterFilter::template apply<Array>>::value>,
  OuterFilter, InnerFilter, Array, T...
>;

template <typename Array, typename... T>
using as_array = i_a::C<Array, T...>;

template <typename Array, typename... T>
static constexpr typename std::decay<
  decltype(i_a::z<Array, T...>::data)
>::type z_data() {
  return i_a::z<Array, T...>::data;
}

template <typename Array, typename Factory, typename... T>
using as_array_from = i_a::A<i_a::c, Array, Factory, T...>;

template <typename Array, typename Factory, typename... T>
using as_runtime_array_from = i_a::A<i_a::n, Array, Factory, T...>;

template <typename Array, typename Filter, typename... T>
using z_array_filtered = i_a::ZAF<Array, Filter, T...>;

template <typename Array, typename... T>
using z_array = i_a::ZA<Array, T...>;

template <
  typename Array,
  typename StringView,
  typename OuterFilter,
  typename InnerFilter = get_identity
>
using string_view_array_filtered = i_a::SF<
  make_index_sequence<size<typename OuterFilter::template apply<Array>>::value>,
  Array, OuterFilter, InnerFilter, StringView
>;

template <typename Array, typename StringView>
using string_view_array = i_a::S<Array, StringView>;

} // namespace fatal {
