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
#include <fatal/type/qualifier.h>

#include <type_traits>

namespace fatal {

/**
 * A class that holds information obtained through reflection about a member
 * function. Refer to `reflect_member_function` on how to properly obtain a
 * template instantiation of this class.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename Pointer,
  cv_qualifier CV,
  ref_qualifier Ref,
  typename Owner,
  typename Result,
  typename... Args
>
struct reflected_member_function {
  /**
   * The class that declares the member function.
   *
   * Example:
   *
   *  struct foo { int bar(bool a, double b); };
   *
   *  // yields `foo`
   *  using owner = reflect_member_function<decltype(&foo::bar)>::owner;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using owner = Owner;

  /**
   * The type returned by the member function.
   *
   * Example:
   *
   *  struct foo { int bar(bool a, double b); };
   *
   *  // yields `int`
   *  using result = reflect_member_function<decltype(&foo::bar)>::result;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using result = Result;

  /**
   * The list of arguments accepted by the member function.
   *
   * Example:
   *
   *  struct foo { int bar(bool a, double b); };
   *
   *  // yields `type_list<bool, double>`
   *  using args = reflect_member_function<decltype(&foo::bar)>::args;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using args = type_list<Args...>;

  /**
   * The type of the function pointer for the member function.
   *
   * Example:
   *
   *  struct foo {
   *    int bar(bool a, double b) &;
   *    int baz(bool a, double b) const;
   *    int gaz(bool a, double b) const volatile &&;
   *  };
   *
   *  // yields `int(foo::*)(bool, double) &`
   *  using p1 = reflect_member_function<decltype(&foo::bar)>::pointer;
   *
   *  // yields `int(foo::*)(bool, double) const`
   *  using p2 = reflect_member_function<decltype(&foo::baz)>::pointer;
   *
   *  // yields `int(foo::*)(bool, double) const volatile &&`
   *  using p3 = reflect_member_function<decltype(&foo::gaz)>::pointer;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using pointer = Pointer;

  /**
   * The reference qualifier, if any, of the member function.
   *
   * Example:
   *
   *  struct foo {
   *    int bar(bool a, double b) &;
   *    int baz(bool a, double b) const;
   *    int gaz(bool a, double b) const volatile &&;
   *  };
   *
   *  // yields `ref_qualifier::lvalue`
   *  using ref1 = reflect_member_function<decltype(&foo::bar)>::ref;
   *
   *  // yields `ref_qualifier::none`
   *  using ref2 = reflect_member_function<decltype(&foo::baz)>::ref;
   *
   *  // yields `ref_qualifier::rvalue`
   *  using ref3 = reflect_member_function<decltype(&foo::gaz)>::ref;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using ref = std::integral_constant<ref_qualifier, Ref>;

  /**
   * The const/volatile qualifiers of the member function.
   *
   * Example:
   *
   *  struct foo {
   *    int bar(bool a, double b) &;
   *    int baz(bool a, double b) const;
   *    int gaz(bool a, double b) const volatile &&;
   *  };
   *
   *  // yields `cv_qualifier::none`
   *  using cv1 = reflect_member_function<decltype(&foo::bar)>::cv;
   *
   *  // yields `cv_qualifier::c`
   *  using cv2 = reflect_member_function<decltype(&foo::baz)>::cv;
   *
   *  // yields `cv_qualifier::cv`
   *  using cv3 = reflect_member_function<decltype(&foo::gaz)>::cv;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using cv = std::integral_constant<cv_qualifier, CV>;

  /**
   * A flat list of the result type followed by each argument's type.
   *
   * Example:
   *
   *  struct foo { int bar(bool a, double b); };
   *
   *  // yields `type_list<int, bool, double>`
   *  using types = reflect_member_function<decltype(&foo::bar)>::types;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using types = typename args::template push_front<result>;
};

namespace detail {
template <typename> struct reflect_member_function_impl;
} // namespace detail {

/**
 * Compile-time reflection for member functions.
 *
 * Extracts information like member function signature (return and
 * parameters types), declaring class and const/volatile qualifiers.
 *
 * Example:
 *
 *  template <typename T>
 *  struct foo {
 *    int bar(bool a, T const &b, double *c) const;
 *  };
 *
 *  using info = reflect_member_function<decltype(&foo<long>::bar)>;
 *
 *  // yields `int`
 *  using return_type = info::result;
 *
 *  // yields `foo<long>`
 *  using declaring_class = info::owner;
 *
 *  // yields `type_list<bool, long const &, double *>`
 *  using parameters = info::args;
 *
 *  // yields `cv_qualifier::c`
 * auto cv = info::cv::value;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using reflect_member_function = typename detail::reflect_member_function_impl<
  T
>::type;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {

#define REFLECTED_MEMBER_FUNCTION_IMPL(CV, Ref, Qualifiers) \
  template <typename Result, typename Owner, typename... Args> \
  struct reflect_member_function_impl<Result(Owner::*)(Args...) Qualifiers> { \
    using pointer = Result(Owner::*)(Args...) Qualifiers; \
    using type = reflected_member_function< \
      pointer, cv_qualifier::CV, ref_qualifier::Ref, \
      Owner, Result, Args... \
    >; \
  }

REFLECTED_MEMBER_FUNCTION_IMPL(none, none, );
REFLECTED_MEMBER_FUNCTION_IMPL(c, none, const);
REFLECTED_MEMBER_FUNCTION_IMPL(v, none, volatile);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, none, const volatile);
REFLECTED_MEMBER_FUNCTION_IMPL(none, lvalue, &);
REFLECTED_MEMBER_FUNCTION_IMPL(c, lvalue, const &);
REFLECTED_MEMBER_FUNCTION_IMPL(v, lvalue, volatile &);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, lvalue, const volatile &);
REFLECTED_MEMBER_FUNCTION_IMPL(none, rvalue, &&);
REFLECTED_MEMBER_FUNCTION_IMPL(c, rvalue, const &&);
REFLECTED_MEMBER_FUNCTION_IMPL(v, rvalue, volatile &&);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, rvalue, const volatile &&);

#undef REFLECTED_MEMBER_FUNCTION_IMPL

} // namespace detail {
} // namespace fatal {
