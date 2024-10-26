/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

// note: no dependencies!

#include <cstddef>

#include <fatal/type/impl/builtin.h>

namespace fatal {

/// make_integer_seq
///
/// Makes either the clang++ builtin __make_integer_seq or a fallback for it
/// available within namespace fatal, and publicly re-exports it with the name
/// fatal::make_integer_seq.
///
/// mimic: __make_integer_seq, clang++
template <template <typename T, T...> class S, typename T, std::size_t N>
using make_integer_seq = __make_integer_seq<S, T, N>;

/// type_pack_element
///
/// Makes either the clang++ builtin __type_pack_eleemnt or a fallback for it
/// available within namespace fatal, and publicly re-exports it with the name
/// fatal::type_pack_element.
///
/// mimic: __type_pack_element, clang++
template <std::size_t I, typename... T>
using type_pack_element = __type_pack_element<I, T...>;

} // namespace fatal {
