/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/preprocessor.h>
#include <fatal/type/constify.h>
#include <fatal/type/constify_from.h>
#include <fatal/type/compilability.h>
#include <fatal/type/data_member_getter.h>
#include <fatal/type/fast_pass.h> // TODO: REMOVE AND FIX DEPENDENCIES
#include <fatal/type/is_complete.h>
#include <fatal/type/logical.h>
#include <fatal/type/qualifier.h>
#include <fatal/type/remove_rvalue_reference.h>
#include <fatal/type/same_reference_as.h>
#include <fatal/type/safe_overload.h>
#include <fatal/type/scalar.h>
#include <fatal/type/sequence.h>

#include <type_traits>

namespace fatal {

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
  using apply = logical_or<impl<TTemplates, T>...>;

  template <typename T>
  using type = logical_or<impl<TTemplates, T>...>;
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
    logical_and<Predicates...>::value
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
    logical_or<Predicates...>::value
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
    !logical_or<Predicates...>::value
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
    !logical_and<Predicates...>::value
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
    negate<std::is_const<typename std::remove_reference<T>::type>>
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
          static_cast<Args &&>(std::declval<Args>())...
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

} // namespace traits_impl {
} // namespace detail {
} // namespace fatal
