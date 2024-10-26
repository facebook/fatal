/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/deprecated/apply.h>

namespace fatal {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename...> struct maybe;

template <typename First, typename... Args>
struct maybe<First, Args...> {
  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename>
  using first = First;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Default>
  using second = Default;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Default>
  using third = Default;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class U>
  using all = fatal::apply<U, First, Args...>;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

template <>
struct maybe<> {
  template <typename Default>
  using first = Default;

  template <typename Default>
  using second = Default;

  template <typename Default>
  using third = Default;

  template <template <typename...> class U>
  using all = U<>;
};

template <typename First, typename Second, typename... Args>
struct maybe<First, Second, Args...> {
  template <typename>
  using first = First;

  template <typename>
  using second = Second;

  template <typename Default>
  using third = Default;

  template <template <typename...> class U>
  using all = fatal::apply<U, First, Second, Args...>;
};

template <typename First, typename Second, typename Third, typename... Args>
struct maybe<First, Second, Third, Args...> {
  template <typename>
  using first = First;

  template <typename>
  using second = Second;

  template <typename>
  using third = Third;

  template <template <typename...> class U>
  using all = fatal::apply<U, First, Second, Third, Args...>;
};

} // namespace fatal
