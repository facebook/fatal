/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_traits_h
#define FATAL_INCLUDE_fatal_type_traits_h

#include <fatal/type/transform.h>

#include <type_traits>
#include <utility>

namespace fatal {

/////////////////
// TYPE TRAITS //
/////////////////

/**
 * A traits class to check for incomplete types.
 *
 * Example:
 *
 *  struct complete_type {}; // complete type
 *  struct forward_declared_type;
 *
 *  // yields `true`
 *  is_complete<int>::type::value
 *
 *  // yields `true`
 *  is_complete<complete_type>::type::value
 *
 *  // yields `false`
 *  is_complete<forward_declared_type>::type::value
 *
 *  // now it has been declared and is not incomplete anymore
 *  struct forward_declared_type {};
 *
 *  // yields `true`
 *  is_complete<forward_declared_type>::type::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

struct is_complete_impl {
  template <std::size_t> struct dummy {};
  template <typename U> static std::true_type sfinae(dummy<sizeof(U)> *);
  template <typename> static std::false_type sfinae(...);
};

} // namespace detail {

template <typename T>
struct is_complete: decltype(detail::is_complete_impl::sfinae<T>(nullptr)) {};

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
  template <template <typename...> class UTemplate>
  struct test {
    template <typename... V> static std::true_type sfinae(UTemplate<V...> *);
    static std::false_type sfinae(...);
  };

  template <template <typename...> class UTemplate, typename U>
  using impl = decltype(test<UTemplate>::sfinae(static_cast<U *>(nullptr)));

public:
  template <typename U>
  using type = logical_transform::any<impl<TTemplates, U>...>;
};

/**
 * fast_pass_by_value: tells if pass-by-value is the fastest way of passing a
 * given type as a read-only argument or return value.
 *
 * Pointers, integers and whatnot are passed by value while user defined
 * types are passed by reference to const.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using fast_pass_by_value = std::integral_constant<
  bool, std::is_scalar<typename std::decay<T>::type>::value
>;

/**
 * fast_pass: resolves to the fastest way of passing a given type as a read-only
 * argument or return value.
 *
 * See fast_pass_by_value for details.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using fast_pass = typename std::conditional<
  fast_pass_by_value<
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
>::type;

/**
 * Type traits to prevent the universal constructor from overloading
 * the copy/move constructor.
 *
 * For more information, see http://ericniebler.com/2013/08/07/ \
 *     universal-references-and-the-copy-constructo/
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
 *       safe_ctor_overload<Foo, T>::value, void
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
 *       safe_ctor_overload<Foo, Args...>::value, void
 *     >::type
 *   >
 *   Foo(Args &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename, typename...>
struct safe_ctor_overload: public std::true_type {};

template <typename Class, typename T>
struct safe_ctor_overload<Class, T>:
  public std::integral_constant<
    bool,
    !std::is_base_of<
      Class,
      typename std::remove_cv<
        typename std::remove_reference<T>::type
      >::type
    >::value
  >
{};

/**
 * Template alias for safe_ctor_overload above.
 *
 * Usage:
 *
 * struct Foo {
 *   template <typename T, typename X = safe_ctor_overload_t<Foo, T>>
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * struct Foo {
 *   template <
 *     typename... UArgs,
 *     typename X = safe_ctor_overload_t<Foo, UArgs...>
 *   >
 *   Foo(UArgs &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Class, typename... Args>
using safe_ctor_overload_t = typename std::enable_if<
  safe_ctor_overload<Class, Args...>::value
>::type;

/**
 * Traits class to tell if some type is a functor class for a given signature.
 * is_functor<T, A, B, C>::value will evaluate to true iff T is a class and T
 * has an overload of operator () accepting three parameters whose types are
 * respectively equal or implicitly constructible from A, B and C.
 *
 * Example:
 *
 *  struct Foo {
 *    void operator ()() {}
 *    void operator ()(int i, std::string s) {}
 *  };
 *
 *  auto const lambda_is = [](int, std::string) {};
 *  typedef decltype(lambda_is) lambda;
 *
 *  cout << std::boolalpha << is_functor<Foo>::value
 *    << ' ' << std::boolalpha << is_functor<Foo, int>::value
 *    << ' ' << std::boolalpha << is_functor<Foo, int, double>::value
 *    << ' ' << std::boolalpha << is_functor<Foo, int, std::string>::value
 *    << std::endl
 *    << ' ' << std::boolalpha << is_functor<lambda>::value
 *    << ' ' << std::boolalpha << is_functor<lambda, int>::value
 *    << ' ' << std::boolalpha << is_functor<lambda, int, double>::value
 *    << ' ' << std::boolalpha << is_functor<lambda, int, std::string>::value;
 *
 * Outputs:
 *  true false false true
 *  false false false true
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
class is_functor {
  template <typename> struct sfinae {};
  template <typename U>
  static constexpr bool test(
    sfinae<decltype(std::declval<U>().operator()(
      std::forward<Args>(std::declval<typename std::decay<Args>::type>())...
    ))> *
  ) { return true; }
  template <typename> static constexpr bool test(...) { return false; }

public:
  static constexpr bool value = test<T>(nullptr);
};

template <typename T, typename... Args>
constexpr bool is_functor<T, Args...>::value;

template <typename T, typename... Args>
class is_callable {
  template <typename> struct sfinae {};
  template <typename U>
  static constexpr bool test(
    sfinae<decltype(std::declval<U>()(
      std::forward<Args>(std::declval<typename std::decay<Args>::type>())...
    ))> *
  ) { return true; }
  template <typename> static constexpr bool test(...) { return false; }

public:
  static constexpr bool value = test<T>(nullptr);
};

template <typename T, typename... Args>
constexpr bool is_callable<T, Args...>::value;

/**
 * This macro creates a traits class that can check whether some type `T`
 * has a public instance member function with a given name, which can be
 * called by passing in the given parameters.
 *
 * The macro receives two parameters, `Class` and `Member`.  The traits
 * class created by this macro will be named whatever `Class` is, and it
 * will check for members named whatever `Member` is.
 *
 * This is the interface of the created traits class:
 *
 *  template <typename T, typename... Args>
 *  struct Class {
 *    // a bool std::integral_constant-like that tells whether `T` has a public
 *    // instance member function named `Member` accepting `Args...` parameters
 *    using has_member = *IMPLEMENTATION-DEFINED*;
 *
 *    template <typename U, typename... UArgs>
 *    using rebind = Class<U, UArgs...>;
 *  };
 *
 * Example:
 *
 *  // checks for a member named `fn`
 *  FATAL_CREATE_HAS_MEMBER_TRAITS(traits, fn);
 *  // now we have a class names `traits`
 *
 *  struct A {};
 *  struct F {
 *    int fn() { return 0; }
 *    float fn(char) { return 0; }
 *    short fn(double, A &) { return 0; }
 *    double doit(std:string) { return 0; }
 *  };
 *
 *  // yields `true`
 *  traits<F>::value
 *
 *  // yields `true`
 *  traits<F, char>::value
 *
 *  // yields `false`
 *  traits<F, double>::value
 *
 *  // yields `false`
 *  traits<F, double, A>::value
 *
 *  // yields `true`
 *  traits<F, double, A &>::value
 *
 *  // yields `false`
 *  traits<F, std::string>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_CREATE_HAS_MEMBER_FUNCTION_TRAITS(Class, Member) \
  template <typename TTheClass_, typename... TTheArgs_> \
  class Class { \
    template <typename> struct dummy; \
    template <typename UTheClass_> \
    static std::true_type sfinae(dummy<decltype( \
      std::declval<UTheClass_>().Member( \
        std::forward<TTheArgs_>(std::declval<TTheArgs_>())... \
      ) \
    )> *); \
    template <typename UTheClass_> \
    static std::true_type sfinae(dummy<decltype( \
      std::declval<typename std::add_const<UTheClass_>::type>().Member( \
        std::forward<TTheArgs_>(std::declval<TTheArgs_>())... \
      ) \
    )> *); \
    template <typename> static std::false_type sfinae(...); \
  public: \
    using has_member = decltype(sfinae<TTheClass_>(nullptr)); \
    template <typename UTheClass_, typename... UTheArgs_> \
    using rebind = Class<UTheClass_, UTheArgs_...>; \
  }

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_traits_h
