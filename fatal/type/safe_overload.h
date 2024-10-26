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

#include <fatal/type/impl/safe_overload.h>

namespace fatal {

/**
 * Type traits to prevent the universal constructor from overloading
 * the copy/move constructor.
 *
 * For more information, see
 *   ericniebler.com/2013/08/07/universal-references-and-the-copy-constructo
 *
 * Usage:
 *
 * // before
 * struct Foo {
 *   template <typename T>
 *   Foo(T &&value) { ... }
 * };
 *
 * // after
 * struct Foo {
 *   template <
 *     typename T,
 *     typename X = typename std::enable_if<
 *       is_safe_overload<Foo, T>::value, void
 *     >::type
 *   >
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * // before
 * struct Foo {
 *   template <typename... Args>
 *   Foo(Args &&...args) { ... }
 * };
 *
 * // after
 * struct Foo {
 *   template <
 *     typename... Args,
 *     typename X = typename std::enable_if<
 *       is_safe_overload<Foo, Args...>::value, void
 *     >::type
 *   >
 *   Foo(Args &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Class, typename... Args>
using is_safe_overload = typename i_sfo::S<Class, Args...>::type;

/**
 * Template alias for is_safe_overload above.
 *
 * Usage:
 *
 * struct Foo {
 *   template <typename T, typename X = safe_overload<Foo, T>>
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * struct Foo {
 *   template <
 *     typename... UArgs,
 *     typename X = safe_overload<Foo, UArgs...>
 *   >
 *   Foo(UArgs &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Class, typename... Args>
using safe_overload = typename std::enable_if<
  is_safe_overload<Class, Args...>::value
>::type;

} // namespace fatal {
