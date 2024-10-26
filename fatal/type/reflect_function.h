/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/deprecated/type_list.h>

#include <type_traits>

namespace fatal {

/**
 * Compile-time reflection for free and static member functions.
 *
 * Extracts return type and arguments type information
 * parameters types), declaring class and const/volatile qualifiers.
 *
 * Example:
 *
 *  double foo(bool b, float f) { return 0; }
 *
 *  using info = reflect_function<decltype(foo)>;
 *
 *  // yields `double`
 *  using return_type = info::result;
 *
 *  // yields `type_list<bool, float>`
 *  using parameters = info::args;
 *
 *  // yields `double(bool, float)`
 *  using signature = info::signature;
 *
 *  // yields `double(*)(bool, float)`
 *  using pointer = info::pointer;
 *
 *  struct bar {
 *    static void baz(bool *) {}
 *  };
 *
 *  // yields `void(bool *)`
 *  using x = reflect_function<decltype(bar::baz)>::signature;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename> struct reflect_function;

template <typename Result, typename... Args>
  struct reflect_function<Result(Args...)> {
  /**
   * The type returned by the function.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using result = Result;

  /**
   * The list of arguments accepted by the function.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using args = type_list<Args...>;

  /**
   * A type representing the signature of the function.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using signature = Result(Args...);

  /**
   * A type representing the function pointer.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using pointer = Result(*)(Args...);

  /**
   * A flat list of the result type followed by each argument's type.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using types = typename args::template push_front<result>;
};

} // namespace fatal {
