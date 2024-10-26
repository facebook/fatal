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

#include <fatal/type/conditional.h>

namespace fatal {

//////////////////
// is_fast_pass //
//////////////////

/**
 * is_fast_pass: tells if pass-by-value is the fastest way of passing a
 * given type as a read-only argument or return value.
 *
 * Pointers, integers and whatnot are passed by value while user defined
 * types are passed by reference to const.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_fast_pass = std::integral_constant<
  bool, std::is_scalar<typename std::decay<T>::type>::value
>;

///////////////
// fast_pass //
///////////////

/**
 * fast_pass: resolves to the fastest way of passing a given type as a read-only
 * argument or return value.
 *
 * See is_fast_pass for details.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using fast_pass = conditional<
  is_fast_pass<
    typename std::decay<T>::type
  >::value,
  typename std::add_const<
    typename std::decay<T>::type
  >::type,
  typename std::add_lvalue_reference<
    typename std::add_const<
      typename std::decay<T>::type
    >::type
  >::type
>;

} // namespace fatal
