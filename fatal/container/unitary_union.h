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

///////////////////
// unitary_union //
///////////////////

namespace detail {

template <typename T>
// TODO: current compilers don't implement std::is_trivially_constructible<T>
//       this should be changed once support is in place
using is_trivially_constructible_impl = std::false_type;

template <typename T, bool, bool = is_trivially_constructible_impl<T>::value>
struct unitary_union_impl;

template <typename T>
struct unitary_union_impl<T, false, false> {
  union type {
    type() noexcept {}
    type(type const &) = delete;
    type(type &&) = delete;
    ~type() noexcept {}

    T value;
  };
};

template <typename T>
struct unitary_union_impl<T, false, true> {
  union type {
    type() noexcept = default;
    type(type const &) = delete;
    type(type &&) = delete;
    ~type() noexcept {}

    T value;
  };
};

template <typename T>
struct unitary_union_impl<T, true, false> {
  union type {
    type() noexcept {}
    type(type const &) = delete;
    type(type &&) = delete;
    ~type() noexcept { value.~T(); }

    T value;
  };
};

template <typename T>
struct unitary_union_impl<T, true, true> {
  union type {
    type() noexcept = default;
    type(type const &) = delete;
    type(type &&) = delete;
    ~type() noexcept { value.~T(); }

    T value;
  };
};

} // namespace detail {

// TODO: DOCUMENT AND TEST
template <typename T, bool AutoDestroy>
using unitary_union = typename detail::unitary_union_impl<T, AutoDestroy>::type;

} // namespace fatal {
