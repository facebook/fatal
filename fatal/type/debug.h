/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {

template <typename...> struct debugged;

/**
 * Macro that can be used to help debug meta-programs by printing out types.
 *
 * It will force a compilation error so that the compiler's output stack trace
 * will contain the actuall type passed to this macro.
 *
 * Many types can be passed in at the same type, given that this macro accepts
 * a variadic list of types.
 *
 * Syntactically, this macro evaluates to a type.
 *
 * Example 1:
 *
 *  // original code
 *  template <typename SomeInputType>
 *  struct foo {
 *    using type = SomeInputType;
 *
 *    // ...
 *  };
 *
 *  // temporarily changed code
 *  template <typename SomeInputType>
 *  struct foo {
 *    using type = FATAL_DEBUG_TYPE(SomeInputType);
 *
 *    // ...
 *  };
 *
 *  void bar() {
 *    // will print `fatal::debugged<the_unknown_type>` to the compiler output
 *    foo<the_unknown_type>::do_something();
 *  }
 *
 * Example 2:
 *
 *  template <typename... Args>
 *  void foo() {
 *    using debug = FATAL_DEBUG_TYPE(Args...);
 *  }
 *
 *  void bar() {
 *    // will print `fatal::debugged<A, B, C>` to the compiler output
 *    foo<A, B, C>();
 *  }
 *
 * @author: Marcelo Juchem <juchem@gmail.com>
 */
#define FATAL_DEBUG_TYPE(...) decltype(::fatal::debugged<__VA_ARGS__>())

/**
 * Helper to use `FATAL_DEBUG_TYPE` with an expression, rather than a type.
 *
 * Output will be the type of the expression.
 *
 * Example:
 *
 *  template <typename T>
 *  void foo(T value) {
 *    using debug = FATAL_DEBUG_TYPE_OF(value);
 *  }
 *
 *  void bar() {
 *    // will print `int` to the compiler output
 *    foo(10);
 *  }
 *
 * @author: Marcelo Juchem <juchem@gmail.com>
 */
#define FATAL_DEBUG_TYPE_OF(...) FATAL_DEBUG_TYPE(decltype(__VA_ARGS__))

template <bool Condition, typename... T>
struct debug_type_if {
  static_assert(
    Condition, "there's a bug in fatal, no debug should be generated"
  );

  using type = FATAL_DEBUG_TYPE(T...);
};

template <typename... T>
struct debug_type_if<false, T...> { using type = void; };

template <bool Condition, typename... Args>
using debug_type_if_t = typename debug_type_if<Condition, Args...>::type;

/**
 * Helper to conditionally use `FATAL_DEBUG_TYPE` depending on a boolean
 * condition.
 *
 * Output will only be issued when the condition is `true`.
 *
 * Example:
 *
 *  template <typename... Args>
 *  void foo() {
 *    using debug = FATAL_DEBUG_TYPE_IF(sizeof...(Args) == 2, Args...);
 *  }
 *
 *  void bar() {
 *    // won't print anything since the condition is not met
 *    foo<A, B, C>();
 *
 *    // won't print anything since the condition is not met
 *    foo<K>();
 *
 *    // will print `fatal::debugged<X, Z>` to the compiler output
 *    foo<X, Z>();
 *  }
 *
 * @author: Marcelo Juchem <juchem@gmail.com>
 */
#define FATAL_DEBUG_TYPE_IF(Condition, ...) \
  ::fatal::debug_type_if_t<Condition, __VA_ARGS__>

} // namespace fatal
