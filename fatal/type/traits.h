/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_traits_h
#define FATAL_INCLUDE_fatal_type_traits_h

#include <fatal/preprocessor.h>
#include <fatal/type/fast_pass.h> // TODO: REMOVE AND FIX DEPENDENCIES
#include <fatal/type/scalar.h>
#include <fatal/type/sequence.h>
#include <fatal/type/transform.h>

#include <type_traits>
#include <utility>

namespace fatal {

/////////////////////////////
// remove_rvalue_reference //
/////////////////////////////

/**
 * Removes any r-value references from a given type.
 * L-value references remain untouched.
 *
 * Example:
 *
 *  // yields `int`
 *  using result1 = remove_rvalue_reference<int &&>::type;
 *
 *  // yields `int const`
 *  using result2 = remove_rvalue_reference<int const &&>::type;
 *
 *  // yields `int *`
 *  using result3 = remove_rvalue_reference<int *>::type;
 *
 *  // yields `int *&`
 *  using result4 = remove_rvalue_reference<int *&>::type;
 *
 *  // yields `int const &`
 *  using result5 = remove_rvalue_reference<int const &>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct remove_rvalue_reference { using type = T; };

template <typename T>
struct remove_rvalue_reference<T &&> { using type = T; };

///////////////////////
// same_reference_as //
///////////////////////

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, turn `T` into an l-value reference
 * - if `U` is an r-value reference, turn `T` into an r-value reference
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = same_reference_as<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = same_reference_as<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = same_reference_as<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = same_reference_as<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = same_reference_as<foo, int &&>::type;
 *
 *  // yields `foo &&`
 *  using result6 = same_reference_as<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct same_reference_as {
  using type = T;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &> {
  using type = typename std::add_lvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

////////////////////////
// add_reference_from //
////////////////////////

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, yield `std::add_lvalue_reference<T>::type`
 * - if `U` is an r-value reference, yield `std::add_rvalue_reference<T>::type`
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_reference_from<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = add_reference_from<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = add_reference_from<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = add_reference_from<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = add_reference_from<foo, int &&>::type;
 *
 *  // yields `foo &`
 *  using result6 = add_reference_from<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_reference_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &> {
  using type = typename std::add_lvalue_reference<T>::type;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<T>::type;
};

////////////////////
// add_const_from //
////////////////////

/**
 * Applies `std::add_const` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_const_from<foo, int>::type;
 *
 *  // yields `foo const`
 *  using result2 = add_const_from<foo, int const>::type;
 *
 *  // yields `foo const`
 *  using result3 = add_const_from<foo const, int const>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_const_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_const_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

//////////////
// constify //
//////////////

/**
 * Adds the const qualifier to a type.
 *
 * If the type is a reference, to some other type `U`,
 * make it the same kind of reference to `U const`.
 *
 * Example:
 *
 *  // yields `int const`
 *  using result1 = constify<int>::type;
 *
 *  // yields `int const &`
 *  using result2 = constify<int &>::type;
 *
 *  // yields `int const &&`
 *  using result3 = constify<int &&>::type;
 *
 *  // yields `int const &`
 *  using result4 = constify<int const &>::type;
 *
 *  // yields `int *const`
 *  using result4 = constify<int *>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct constify {
  using type = typename add_reference_from<
    typename std::decay<T>::type const,
    T
  >::type;
};

///////////////////
// constify_from //
///////////////////

/**
 * Applies `constify` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo const`
 *  using result1 = constify_from<foo, int const>::type;
 *
 *  // yields `foo const &`
 *  using result2 = constify_from<foo &, int const>::type;
 *
 *  // yields `foo &&`
 *  using result3 = constify_from<foo &&, int>::type;
 *
 *  // yields `foo const &`
 *  using result4 = constify_from<foo const &, int const>::type;
 *
 *  // yields `foo const &`
 *  using result5 = constify_from<foo const &, int>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct constify_from {
  using type = T;
};

template <typename T, typename TFrom>
struct constify_from<T, TFrom const> {
  using type = typename constify<T>::type;
};

/////////////////
// is_template //
/////////////////

namespace detail {
namespace traits_impl {
template <template <typename...> class, typename> struct is_template;
} // namespace traits_impl {
} // namespace detail {

/**
 * Checks whether a given type is an instantiation of at least one of a list of
 * class templates.
 *
 * The template member `type` which evaluates to either `std::true_type` or
 * `std::false_type`.
 *
 * Example:
 *
 *  // yields `true` for std::string
 *  is_template<std::basic_string>::type<std::string>::value
 *
 *  // yields `true` for std::wstring
 *  is_template<std::basic_string>::type<std::wstring>::value
 *
 *  // yields `true` for std::basic_string<int>
 *  is_template<std::basic_string>::type<std::basic_string<int>>::value
 *
 *  // yields `false` for std::vector<int>
 *  is_template<std::basic_string>::type<std::vector<int>>::value
 *
 *  // yields `true` for std::vector<int>
 *  is_template<std::basic_string, std::vector>::type<std::string>::value
 *
 *  // yields `true` for std::vector<int>
 *  is_template<std::basic_string, std::vector>::type<std::vector<int>>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... TTemplates>
class is_template {
  template <template <typename...> class TTemplate, typename T>
  using impl = is_complete<detail::traits_impl::is_template<TTemplate, T>>;

public:
  template <typename T>
  using type = logical::any<impl<TTemplates, T>...>;
};

/////////////////
// integral_of //
/////////////////

namespace detail {
namespace traits_impl {
template <typename T, bool = std::is_enum<T>::value> struct integral_of;
} // namespace traits_impl {
} // namespace detail {

/**
 * Gets the integral representation of a type that provides it.
 *
 * This is a convenient way to transparently treat enum classes as integrals.
 *
 * Example:
 *
 *  // yields `short`
 *  using result1 = integral_of<short>;
 *
 *  enum class foo: int { a, b, c };
 *
 *  // yields `int`
 *  using result2 = integral_of<foo>;
 *
 *  // yields `bool`
 *  using result3 = integral_of<std::true_type>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using integral_of = typename detail::traits_impl::integral_of<T>::type;

/////////////////
// as_integral //
/////////////////

/**
 * Converts a value to its integral representation, for types that provide it.
 *
 * This is a convenient way to transparently treat enum classes as integrals.
 *
 * Example:
 *
 *  // yields an `int` with value `10`
 *  auto result1 = as_integral(10);
 *
 *  enum class foo: int { a = 5, b = 7, c = 9 };
 *
 *  // yields an `int` with value `5`
 *  auto result2 = as_integral(foo::a);
 *
 *  // yields a `bool` with value `true`
 *  auto result3 = as_integral(std::true_type());
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
integral_of<typename std::decay<T>::type> as_integral(T value) noexcept {
  return detail::traits_impl::integral_of<
    typename std::decay<T>::type
  >::convert(value);
}

/////////////////
// is_callable //
/////////////////

namespace detail {
namespace traits_impl {
template <typename...> class is_callable;
} // namespace traits_impl {
} // namespace detail {

/**
 * Tells whether an instance of a given type supports the
 * call operator, when passing arguments of given types.
 *
 * Example:
 *
 *  void noop();
 *
 *  int fn(std::string const &s, bool b);
 *
 *  struct non_callable {};
 *
 *  struct callable {
 *    void operator ()(int i, double d);
 *  };
 *
 *  // yields `true`
 *  is_callable<noop>::value
 *
 *  // yields `false`
 *  is_callable<decltype(fn), int, double>::value
 *
 *  // yields `true`
 *  is_callable<decltype(fn), char const *, bool>::value
 *
 *  // yields `false`
 *  is_callable<non_callable>::value
 *
 *  // yields `false`
 *  is_callable<non_callable, int>::value
 *
 *  // yields `true`
 *  is_callable<callable, int, double>::value
 *
 *  // yields `false`
 *  is_callable<callable, int, double, std::string>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
using is_callable = typename detail::traits_impl::is_callable<Args...>
  ::template type<T>;

/////////////////
// enable_when //
/////////////////

/**
 * Helpers for enabling methods when specific predicates match.
 *
 * Example:
 *
 *  template <typename T, typename = enable_when::is_false<std::is_const<T>>>
 *  void foo(T &value) {
 *    std::cout << "got a non-const: " << value;
 *  }
 *
 *  int i = 10;
 *  int const c = 5;
 *
 *  // prints "got a non-const: 10"
 *  foo(i);
 *
 *  // fails to compile
 *  foo(c);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct enable_when {
  /**
   * Helps conditionally enable methods when the given `Predicate` provides a
   * `value` member that evaluates to `true`.
   *
   * Example:
   *
   *  template <typename T, typename = enable_when::is_true<std::is_const<T>>>
   *  void foo(T &value) {
   *    std::cout << "got a const: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // prints "got a const: 5"
   *  foo(c);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Predicate>
  using is_true = typename std::enable_if<Predicate::value>::type;

  /**
   * Helps conditionally enable methods when all of the given `Predicates`
   * provide a `value` member that evaluates to `true`.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::all_true<
   *      std::is_const<T>,
   *      std::is_integral<T>,
   *      std::is_signed<T>
   *    >
   *  >
   *  void foo(T &value) {
   *    std::cout << "got a const, integral and signed: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *  double const d = 7.2;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // prints "got a const, integral and signed: 5"
   *  foo(c);
   *
   *  // fails to compile
   *  foo(d);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Predicates>
  using all_true = typename std::enable_if<
    logical::all<Predicates...>::value
  >::type;

  /**
   * Helps conditionally enable methods when any of the given `Predicates`
   * provide a `value` member that evaluates to `true`.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::any_true<
   *      std::is_const<T>,
   *      std::is_integral<T>,
   *      std::is_unsigned<T>
   *    >
   *  >
   *  void foo(T &value) {
   *    std::cout << "got a const, integral or unsigned: " << value;
   *  }
   *
   *  int const i = 5;
   *  unsigned const u = 10;
   *  double const c = 7.2;
   *  double d = 5.6;
   *
   *  // prints "got a const, integral or unsigned: 5"
   *  foo(i);
   *
   *  // prints "got a const, integral or unsigned: 10"
   *  foo(u);
   *
   *  // prints "got a const, integral or unsigned: 7.2"
   *  foo(c);
   *
   *  // fails to compile
   *  foo(d);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Predicates>
  using any_true = typename std::enable_if<
    logical::any<Predicates...>::value
  >::type;

  /**
   * Helps conditionally enable methods when the given `Predicate` provides a
   * `value` member that evaluates to `false`.
   *
   * Example:
   *
   *  template <typename T, typename = enable_when::is_false<std::is_const<T>>>
   *  void foo(T &value) {
   *    std::cout << "got a non-const: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *
   *  // prints "got a non-const: 10"
   *  foo(i);
   *
   *  // fails to compile
   *  foo(c);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Predicate>
  using is_false = typename std::enable_if<!Predicate::value>::type;

  /**
   * Helps conditionally enable methods when all of the given `Predicates`
   * provide a `value` member that evaluates to `false`.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::all_false<
   *      std::is_const<T>,
   *      std::is_integral<T>,
   *      std::is_unsigned<T>
   *    >
   *  >
   *  void foo(T &value) {
   *    std::cout << "got a non-const, non-integral and signed: " << value;
   *  }
   *
   *  int const i = 5;
   *  unsigned const u = 10;
   *  double const c = 7.2;
   *  double d = 5.6;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // fails to compile
   *  foo(u);
   *
   *  // fails to compile
   *  foo(c);
   *
   *  // prints "got a non-const, non-integral or signed: 5.6"
   *  foo(d);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Predicates>
  using all_false = typename std::enable_if<
    !logical::any<Predicates...>::value
  >::type;

  /**
   * Helps conditionally enable methods when any of the given `Predicates`
   * provide a `value` member that evaluates to `false`.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::any_false<
   *      std::is_const<T>,
   *      std::is_integral<T>,
   *      std::is_signed<T>
   *    >
   *  >
   *  void foo(T &value) {
   *    std::cout << "got a non-const, non-integral or non-signed: " << value;
   *  }
   *
   *  int const i = 10;
   *  unsigned const u = 5;
   *  double const d = 7.2;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // yields "got a non-const, non-integral or non-signed: 5"
   *  foo(u);
   *
   *  // yields "got a non-const, non-integral or non-signed: 7.2"
   *  foo(d);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Predicates>
  using any_false = typename std::enable_if<
    !logical::all<Predicates...>::value
  >::type;

  /**
   * Helps conditionally enable methods when the given type `T` has a `const`
   * qualifier or is a reference to a `const` qualified type.
   *
   * Example:
   *
   *  template <typename T, typename = enable_when::is_const<T>>
   *  void foo(T value) {
   *    std::cout << "got a const: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // prints "got a const: 10"
   *  foo(static_cast<int const>(i));
   *
   *  // prints "got a const: 5"
   *  foo(c);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using is_const = is_true<
    std::is_const<typename std::remove_reference<T>::type>
  >;

  /**
   * Helps conditionally enable methods when the given type `T` doesn't have a
   * `const` qualifier or is a reference to a non-`const` qualified type.
   *
   * Example:
   *
   *  template <typename T, typename = enable_when::non_const<T>>
   *  void foo(T value) {
   *    std::cout << "got a non-const: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *
   *  // prints "got a non-const: 10"
   *  foo(i);
   *
   *  // fails to compile
   *  foo(static_cast<int const>(i));
   *
   *  // fails to compile
   *  foo(c);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using non_const = is_false<
    std::is_const<typename std::remove_reference<T>::type>
  >;

  /**
   * Helps conditionally enable methods when the given type `T` was forwarded
   * as an rvalue reference. This is useful when a forwarding reference must
   * only accept r-value references.
   *
   * Example:
   *
   *  template <typename T, typename = enable_when::forwarded_rvalue<T>>
   *  void foo(T &&value) {
   *    std::cout << "got an r-value reference: " << value;
   *  }
   *
   *  int i = 10;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // prints "got an r-value reference: 10"
   *  foo(std::move(i));
   *
   *  // prints "got an r-value reference: 5"
   *  foo(5);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using forwarded_rvalue = is_true<std::is_rvalue_reference<T &&>>;

  /**
   * Helps conditionally enable methods when the given type `T` was forwarded
   * as an rvalue reference to a non-const qualified type. This is useful when a
   * forwarding reference must only accept r-value references that can be moved.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::movable<T>
   *  >
   *  void foo(T &&value) {
   *    std::cout << "got a non-const r-value reference: " << value;
   *  }
   *
   *  int i = 10;
   *  int const c = 5;
   *
   *  // fails to compile
   *  foo(i);
   *
   *  // prints "got a non-const r-value reference: 10"
   *  foo(std::move(i));
   *
   *  // fails to compile
   *  foo(c);
   *
   *  // fails to compile
   *  foo(std::move(c));
   *
   *  // prints "got a non-const r-value reference: 72"
   *  foo(72);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using movable = all_true<
    std::is_rvalue_reference<T &&>,
    logical::negate<
      std::is_const<
        typename std::remove_reference<T>::type
      >
    >
  >;

  /**
   * Helps conditionally enable methods when an instance of the
   * given type `T` can be called with arguments of type `Args`.
   *
   * See `is_callable` for more information.
   *
   * Example:
   *
   *  template <
   *    typename T,
   *    typename = enable_when::callable<T, int>
   *  >
   *  void foo(T &&value) {
   *    std::cout << "got a callable object that accepts an int";
   *  }
   *
   *  struct bar { void operator ()(int x) const {} };
   *  struct baz { void operator ()(int x, int y) const {} };
   *
   *  // prints "got a callable object that accepts an int"
   *  foo(std::ceil);
   *
   *  // prints "got a callable object that accepts an int"
   *  foo(bar());
   *
   *  // fails to compile
   *  foo(baz());
   *
   *  // fails to compile
   *  foo(10);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  using callable = is_true<is_callable<T, Args...>>;
};

/////////////////////
// safe_overload_t //
/////////////////////

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
namespace detail {
namespace traits_impl {
template <typename, typename...>
struct is_safe_overload { using type = std::true_type; };
} // namespace traits_impl {
} // namespace detail {

template <typename Class, typename... Args>
using is_safe_overload = typename detail::traits_impl::is_safe_overload<
  Class, Args...
>::type;

/**
 * Template alias for is_safe_overload above.
 *
 * Usage:
 *
 * struct Foo {
 *   template <typename T, typename X = safe_overload_t<Foo, T>>
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * struct Foo {
 *   template <
 *     typename... UArgs,
 *     typename X = safe_overload_t<Foo, UArgs...>
 *   >
 *   Foo(UArgs &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: RENAME TO safe_overload
template <typename Class, typename... Args>
using safe_overload_t = enable_when::is_true<is_safe_overload<Class, Args...>>;

///////////////////////////
// FATAL_HAS_MEMBER_TYPE //
///////////////////////////

/**
 * This macro creates a class named `Class` that can check whether some
 * type has a nested member type with name `Member`.
 *
 * The class created by this macro looks like this:
 *
 *  struct Class {
 *    template <typename T>
 *    using check = <either std::true_type or std::false_type>;
 *  };
 *
 * Example:
 *
 *  FATAL_HAS_MEMBER_TYPE(has_xyz, xyz);
 *
 *  struct foo { using xyz = int; };
 *  struct bar { typedef int xyz; };
 *  struct baz {};
 *
 *  // yields `std::true_type`
 *  using result1 = has_xyz::check<foo>;
 *
 *  // yields `std::true_type`
 *  using result2 = has_xyz::check<bar>;
 *
 *  // yields `std::false_type`
 *  using result3 = has_xyz::check<baz>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_HAS_MEMBER_TYPE(Class, ...) \
  struct Class { \
    template <typename T> \
    static std::true_type sfinae(typename T::__VA_ARGS__ *); \
    \
    template <typename> \
    static std::false_type sfinae(...); \
    \
    template <typename T> \
    using check = decltype(sfinae<T>(nullptr)); \
  }

/////////////////////
// has_member_type //
/////////////////////

struct has_member_type {
# define FATAL_IMPL_HAS_MEMBER_TYPE_DECL(Class, Name) \
  private: \
    FATAL_HAS_MEMBER_TYPE(Class, Name); \
    \
  public: \
    template <typename T> \
    using Name = typename Class::template check<T>

#   define FATAL_IMPL_HAS_MEMBER_TYPE(Name) \
    FATAL_IMPL_HAS_MEMBER_TYPE_DECL( \
      FATAL_UID(FATAL_CAT(has_member_type_impl, Name)), \
      Name \
    )

    FATAL_IMPL_HAS_MEMBER_TYPE(char_type);
    FATAL_IMPL_HAS_MEMBER_TYPE(type);
    FATAL_IMPL_HAS_MEMBER_TYPE(types);

#   define FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(Name) \
    FATAL_IMPL_HAS_MEMBER_TYPE(Name); \
    FATAL_IMPL_HAS_MEMBER_TYPE(Name##_type)

    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(allocator);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(args);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(array);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(category);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(config);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_iterator);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_pointer);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_ptr);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_ref);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_reference);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_reverse_iterator);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(data);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(decode);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(decoder);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(difference);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(element);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(encode);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(encoder);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(extension);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(first);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(flag);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(hash);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(id);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ids);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(index);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(info);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(information);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(instance);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(item);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(iterator);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(key);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(list);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(map);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mapped);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mapping);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mappings);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(member);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(members);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(name);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(names);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(pair);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(pointer);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(predicate);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ptr);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reader);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ref);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reference);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(request);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(response);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(result);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reverse);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reverse_iterator);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(second);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(set);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(size);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(str);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(string);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(tag);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(traits);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(tuple);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(value);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(values);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(version);
    FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(writer);

#   undef FATAL_IMPL_HAS_MEMBER_TYPE_TYPE
#   undef FATAL_IMPL_HAS_MEMBER_TYPE
# undef FATAL_IMPL_HAS_MEMBER_TYPE_DECL
};

//////////////////////////////
// FATAL_DATA_MEMBER_GETTER //
//////////////////////////////

/**
 * TODO: DOCUMENT
 *
 *  struct getter_name {
 *    FATAL_STR(name, "member_name");
 *
 *    template <typename Owner>
 *    static reference ref(Owner &owner);
 *
 *    struct ref_getter {
 *      template <typename Owner>
 *      reference operator ()(Owner &owner);
 *    };
 *
 *    template <typename Owner>
 *    static pointer ptr(Owner &owner);
 *
 *    struct ptr_getter {
 *      template <typename Owner>
 *      pointer operator ()(Owner &owner);
 *    };
 *  };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_DATA_MEMBER_GETTER(Name, ...) \
  FATAL_IMPL_DATA_MEMBER_GETTER( \
    Name, \
    FATAL_UID(FATAL_CAT(Name, _data_member_getter_impl)), \
    __VA_ARGS__ \
  ) \

namespace detail {

template <typename Impl>
class data_member_getter {
  using impl = Impl;

public:
  template <typename Owner>
  using type = typename impl::template type<Owner>;

  template <typename Owner>
  using reference = typename impl::template reference<Owner>;

  using name = typename impl::name;

  template <typename Owner>
  static reference<Owner> ref(Owner &&owner) {
    return impl::ref(std::forward<Owner>(owner));
  }

  struct ref_getter {
    template <typename Owner>
    reference<Owner> operator ()(Owner &&owner) const {
      return ref(std::forward<Owner>(owner));
    }
  };

  template <typename Owner>
  using pointer = typename std::remove_reference<reference<Owner>>::type *;

  template <typename Owner>
  static pointer<Owner> ptr(Owner &owner) { return std::addressof(ref(owner)); }

  struct ptr_getter {
    template <typename Owner>
    pointer<Owner> operator ()(Owner &owner) const { return ptr(owner); }
  };
};

#define FATAL_IMPL_DATA_MEMBER_GETTER(Class, Impl, ...) \
  struct Impl { \
    template <typename Owner> \
    using type = decltype(::std::declval<Owner>().__VA_ARGS__); \
    \
    template <typename Owner> \
    using reference = typename ::fatal::add_reference_from< \
      typename ::fatal::constify_from< \
        type<Owner>, \
        typename ::std::remove_reference<Owner>::type \
      >::type, \
      Owner && \
    >::type; \
    \
    FATAL_STR(name, FATAL_TO_STR(__VA_ARGS__)); \
    \
    template <typename Owner> \
    static reference<Owner> ref(Owner &&owner) { \
      return static_cast<reference<Owner>>( \
        ::std::forward<Owner>(owner).__VA_ARGS__ \
      ); \
    } \
  }; \
  \
  using Class = ::fatal::detail::data_member_getter<Impl>

} // namespace detail {

////////////////////////
// data_member_getter //
////////////////////////

/**
 * Instantiations of FATAL_DATA_MEMBER_GETTER
 * for some popular names.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct data_member_getter {
# define FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(...) \
  FATAL_DATA_MEMBER_GETTER(__VA_ARGS__, __VA_ARGS__); \
  FATAL_DATA_MEMBER_GETTER(FATAL_CAT(__VA_ARGS__, _), FATAL_CAT(__VA_ARGS__, _))

  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(allocator);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(args);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(array);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(category);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(config);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(data);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(decoder);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(difference);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(element);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(encoder);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(extension);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(first);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(flag);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(hash);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(id);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(ids);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(index);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(info);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(information);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(instance);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(item);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(iterator);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(key);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(list);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(map);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(mapped);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(mapping);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(mappings);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(member);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(members);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(name);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(names);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(pair);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(pointer);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(predicate);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(ptr);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(reader);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(ref);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(reference);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(request);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(response);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(result);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(second);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(set);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(size);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(str);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(string);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(tag);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(tuple);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(type);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(types);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(value);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(values);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(version);
  FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER(writer);

# undef FATAL_IMPL_WELL_KNOWN_DATA_MEMBER_GETTER
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace traits_impl {

/////////////////
// is_template //
/////////////////

template <template <typename...> class T, typename... Args>
struct is_template<T, T<Args...>> {};

/////////////////
// integral_of //
/////////////////

template <typename T, bool IsEnum>
struct integral_of {
  static_assert(IsEnum, "wrong specialization");

  using type = typename std::underlying_type<T>::type;

  static type convert(T value) { return static_cast<type>(value); }
};

template <typename T, T Value>
struct integral_of<std::integral_constant<T, Value>, false> {
  using type = T;

  static type convert(T) { return Value; }
};

template <typename T>
struct integral_of<T, false> {
  static_assert(
    std::is_integral<T>::value,
    "type does not represent an integral"
  );

  using type = T;

  static type convert(T value) { return value; }
};

/////////////////
// is_callable //
/////////////////

template <typename... Args>
class is_callable {
  struct impl {
    template <
      typename T,
      typename = decltype(
        std::declval<T>()(
          std::forward<Args>(std::declval<Args>())...
        )
      )
    >
    static std::true_type sfinae(T *);

    template <typename = void>
    static std::false_type sfinae(...);
  };

public:
  template <typename T>
  using type = decltype(
    impl::sfinae(
      static_cast<typename std::remove_reference<T>::type *>(nullptr)
    )
  );
};

//////////////////////
// is_safe_overload //
//////////////////////

template <typename Class, typename T>
struct is_safe_overload<Class, T> {
  using type = bool_constant<
    !std::is_base_of<
      Class,
      typename std::remove_cv<
        typename std::remove_reference<T>::type
      >::type
    >::value
  >;
};

} // namespace traits_impl {
} // namespace detail {
} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_traits_h
