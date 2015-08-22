/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_reflect_member_function_h
#define FATAL_INCLUDE_fatal_type_reflect_member_function_h

#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {

/**
 * Member function const/volatile qualifiers category.
 *
 * `none`:  neither const nor volatile
 * `c`:     const but not volatile
 * `v`:     volatile but not const
 * `cv`:    both const and volatile (guaranteed
 *          to be the bitwise or of `c` and `v`)
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
enum class cv_qualifier: char {
  none = 0,
  c = 1,
  v = 2,
  cv = c | v
};

/**
 * Bitwise and (&) operator for `cv_qualifier` to make it easy to
 * check for the presence of either the const or volatile qualifier:
 *
 * Example:
 *
 *  void check_constness(cv_qualifier q) {
 *    if (q & cv_qualifier::c) {
 *      cout << "it is const, dunno about volatile";
 *    }
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
constexpr bool operator &(cv_qualifier lhs, cv_qualifier rhs) {
  using type = std::underlying_type<cv_qualifier>::type;
  return static_cast<bool>(static_cast<type>(lhs) & static_cast<type>(rhs));
}

///////////////////////////////
// reflected_member_function //
///////////////////////////////

/**
 * A class that holds information obtained through reflection about a member
 * function. Refer to `reflect_member_function` on how to properly obtain a
 * template instantiation of this class.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename Pointer,
  cv_qualifier Qualifier,
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
  using cv = std::integral_constant<cv_qualifier, Qualifier>;

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

#define REFLECTED_MEMBER_FUNCTION_IMPL(CV, Qualifiers) \
  template <typename Result, typename Owner, typename... Args> \
  struct reflect_member_function_impl<Result(Owner::*)(Args...) Qualifiers> { \
    using type = reflected_member_function< \
      Result(Owner::*)(Args...) Qualifiers, \
      cv_qualifier::CV, Owner, Result, Args... \
    >; \
  }

REFLECTED_MEMBER_FUNCTION_IMPL(none, );
REFLECTED_MEMBER_FUNCTION_IMPL(c, const);
REFLECTED_MEMBER_FUNCTION_IMPL(v, volatile);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, const volatile);
REFLECTED_MEMBER_FUNCTION_IMPL(none, &);
REFLECTED_MEMBER_FUNCTION_IMPL(c, const &);
REFLECTED_MEMBER_FUNCTION_IMPL(v, volatile &);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, const volatile &);
REFLECTED_MEMBER_FUNCTION_IMPL(none, &&);
REFLECTED_MEMBER_FUNCTION_IMPL(c, const &&);
REFLECTED_MEMBER_FUNCTION_IMPL(v, volatile &&);
REFLECTED_MEMBER_FUNCTION_IMPL(cv, const volatile &&);

#undef REFLECTED_MEMBER_FUNCTION_IMPL

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

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_reflect_member_function_h
