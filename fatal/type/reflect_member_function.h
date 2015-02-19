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

////////////////
// qualifiers //
////////////////

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
 * Examples below assume this example class:
 *
 *  struct foo {
 *    void bar();
 *    void bar_c() const;
 *    void bar_v() volatile;
 *    void bar_cv() const volatile;
 *  };
 */

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

/**
 * A boolean std::integral_constant-like type traits that tells whether
 * a member function has neither a const nor a volatile qualifier.
 *
 * Example:
 *
 *  // yields `true`
 *  is_noncv_member_function<decltype(&foo::bar)>::value
 *
 *  // yields `false`
 *  is_noncv_member_function<decltype(&foo::bar_c)>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename>
struct is_noncv_member_function: public std::false_type {};
template <typename C, typename R, typename... Args>
struct is_noncv_member_function<R(C::*)(Args...)>: public std::true_type {};

/**
 * A boolean std::integral_constant-like type traits that tells whether
 * a member function has a const but not a volatile qualifier.
 *
 * Example:
 *
 *  // yields `true`
 *  is_const_member_function<decltype(&foo::bar_c)>::value
 *
 *  // yields `false`
 *  is_const_member_function<decltype(&foo::bar)>::value
 *
 *  // yields `false`
 *  is_const_member_function<decltype(&foo::bar_cv)>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename>
struct is_const_member_function: public std::false_type {};
template <typename C, typename R, typename... Args>
struct is_const_member_function<R(C::*)(Args...) const>:
  public std::true_type
{};

/**
 * A boolean std::integral_constant-like type traits that tells whether
 * a member function has a volatile but not a const qualifier.
 *
 * Example:
 *
 *  // yields `true`
 *  is_volatile_member_function<decltype(&foo::bar_v)>::value
 *
 *  // yields `false`
 *  is_volatile_member_function<decltype(&foo::bar)>::value
 *
 *  // yields `false`
 *  is_volatile_member_function<decltype(&foo::bar_cv)>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename>
struct is_volatile_member_function: public std::false_type {};
template <typename C, typename R, typename... Args>
struct is_volatile_member_function<R(C::*)(Args...) volatile>:
  public std::true_type
{};

/**
 * A boolean std::integral_constant-like type traits that tells whether
 * a member function has both a const and a volatile qualifier.
 *
 * Example:
 *
 *  // yields `true`
 *  is_cv_member_function<decltype(&foo::bar_cv)>::value
 *
 *  // yields `false`
 *  is_cv_member_function<decltype(&foo::bar)>::value
 *
 *  // yields `false`
 *  is_cv_member_function<decltype(&foo::bar_v)>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename>
struct is_cv_member_function: public std::false_type {};
template <typename C, typename R, typename... Args>
struct is_cv_member_function<R(C::*)(Args...) const volatile>:
  public std::true_type
{};

/**
 * A `cv_qualifier` std::integral_constant-like type traits that tells
 * what are the const/volatile qualifiers of a member function.
 *
 * Example:
 *
 *  // yields `cv_qualifier::none`
 *  member_function_qualifier<decltype(&foo::bar)>::value
 *
 *  // yields `cv_qualifier::c`
 *  member_function_qualifier<decltype(&foo::bar_c)>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct member_function_qualifier:
  public std::integral_constant<
    cv_qualifier,
    is_const_member_function<T>::value
      ? cv_qualifier::c
      : is_volatile_member_function<T>::value
        ? cv_qualifier::v
        : is_cv_member_function<T>::value
          ? cv_qualifier::cv
          : cv_qualifier::none
  >
{
  static_assert(
    static_cast<unsigned>(is_const_member_function<T>::value)
    + static_cast<unsigned>(is_volatile_member_function<T>::value)
    + static_cast<unsigned>(is_cv_member_function<T>::value)
    + static_cast<unsigned>(is_noncv_member_function<T>::value)
    == 1,
    "only one member function qualifier traits expected to match"
  );
};

///////////////////////////////
// reflected_member_function //
///////////////////////////////

/**
 * A class that holds information obtained through
 * reflection about a member function.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename TClass, typename TResult,
  cv_qualifier Qualifier,
  typename... Args
>
struct reflected_member_function {
  /**
   * The class that declares the member function.
   */
  using owner = TClass;

  /**
   * The type returned by the member function.
   */
  using result = TResult;

  /**
   * The const/volatile qualifiers of the member function.
   */
  using cv = std::integral_constant<cv_qualifier, Qualifier>;

  /**
   * The list of arguments accepted by the member function.
   */
  using args = type_list<Args...>;

  /**
   * A flat list of the result type followed by each argument's type.
   */
  using types = typename args::template push_front<result>;
};

namespace detail {

#define REFLECTED_MEMBER_FUNCTION_IMPL(CV) \
  template <typename C, typename R, typename... Args> \
  auto reflect_member_function_impl(R(C::*fn)(Args...) CV) \
    -> reflected_member_function< \
      C, R, member_function_qualifier<decltype(fn)>::value, Args... \
    > { return {}; }

REFLECTED_MEMBER_FUNCTION_IMPL()
REFLECTED_MEMBER_FUNCTION_IMPL(const)
REFLECTED_MEMBER_FUNCTION_IMPL(volatile)
REFLECTED_MEMBER_FUNCTION_IMPL(const volatile)

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
using reflect_member_function = decltype(
  detail::reflect_member_function_impl(static_cast<T>(nullptr))
);

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_reflect_member_function_h
