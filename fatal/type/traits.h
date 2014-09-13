/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/transform.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {

/////////////////
// TYPE TRAITS //
/////////////////

/**
 * Checks whether a given type is an instantiation of a class template.
 * Provides the template member `instantiation` which evaluates to either
 * `std::true_type` or `std::false_type`.
 *
 * Example:
 *
 *  // we want to know if a given type is a specialization of std::basic_string
 *  typedef is_template<std::basic_string> is_std_string;
 *
 *  // now we perform the checks
 *
 *  // yields `true` for std::string
 *  is_std_string::instantiation<std::string>::value
 *
 *  // yields `true` for std::wstring
 *  is_std_string::instantiation<std::wstring>::value
 *
 *  // yields `true` for std::basic_string<int>
 *  is_std_string::instantiation<std::basic_string<int>>::value
 *
 *  // yields `false` for std::vector<int>
 *  is_std_string::instantiation<std::vector<int>>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTemplate>
struct is_template {
  template <typename... U> static std::true_type sfinae(TTemplate<U...> *);
  static std::false_type sfinae(...);

  template <typename U>
  using instantiation = decltype(sfinae(static_cast<U *>(nullptr)));
};

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
  template <unsigned long long> struct dummy {};
  template <typename U> static std::true_type sfinae(dummy<sizeof(U)> *);
  template <typename> static std::false_type sfinae(...);
};

} // namespace detail {

template <typename T>
struct is_complete: decltype(detail::is_complete_impl::sfinae<T>(nullptr)) {};

/**
 * Provides transforms that evaluate to a member typedef of a given type.
 *
 * Example:
 *
 *  // yields `int`
 *  typedef get_member_typedef::type<std::add_const<int>> result1;
 *
 *  typedef std::map<double, std::string> map;
 *
 *  // yields `std::pair<double, std::string>`
 *  typedef get_member_typedef::value_type<map> result2;
 *
 *  // yields `double`
 *  typedef get_member_typedef::key_type<map> result3;
 *
 *  // yields `std::string`
 *  typedef get_member_typedef::mapped_type<map> result4;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct get_member_typedef {
# define CREATE_GET_MEMBER_TYPEDEF(Name) \
  template <typename T> using Name = typename T::Name

  CREATE_GET_MEMBER_TYPEDEF(type);

  CREATE_GET_MEMBER_TYPEDEF(first_type);
  CREATE_GET_MEMBER_TYPEDEF(second_type);

  CREATE_GET_MEMBER_TYPEDEF(key_type);
  CREATE_GET_MEMBER_TYPEDEF(mapped_type);
  CREATE_GET_MEMBER_TYPEDEF(value_type);
  CREATE_GET_MEMBER_TYPEDEF(element_type);

  CREATE_GET_MEMBER_TYPEDEF(traits_type);
  CREATE_GET_MEMBER_TYPEDEF(allocator_type);

  CREATE_GET_MEMBER_TYPEDEF(size_type);
  CREATE_GET_MEMBER_TYPEDEF(difference_type);

  CREATE_GET_MEMBER_TYPEDEF(reference);
  CREATE_GET_MEMBER_TYPEDEF(const_reference);

  CREATE_GET_MEMBER_TYPEDEF(pointer);
  CREATE_GET_MEMBER_TYPEDEF(const_pointer);

  CREATE_GET_MEMBER_TYPEDEF(iterator);
  CREATE_GET_MEMBER_TYPEDEF(const_iterator);
  CREATE_GET_MEMBER_TYPEDEF(reverse_iterator);
  CREATE_GET_MEMBER_TYPEDEF(const_reverse_iterator);

# undef CREATE_GET_MEMBER_TYPEDEF
};

/**
 * Extracts the relevant type of some noteworthy template `T` like
 * `std::integral_constant`.
 *
 * Resolves to an incomplete type for an unsupported type `T`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 5> i5;
 *
 *  // yields `int`
 *  typedef type_of<i5> result1;
 *
 *  // yields `bool`
 *  typedef type_of<std::true_type> result2;
 */
namespace detail {

template <typename...> struct type_of_impl;

template <template <typename X, X> class U, typename T, T Value>
struct type_of_impl<U<T, Value>> {
  typedef T type;
};

} // namespace detail {

template <typename T>
using type_of = typename detail::type_of_impl<T>::type;

/**
 * Evaluates to `std::add_const<T>::type` iff `Condition`
 * is true, otherwise evaluates to `T`.
 *
 * Example:
 *
 *  // equivalent to `int const i = 0;`
 *  add_const_if<int, true> i = 0;
 *
 *  // equivalent to `double d = 0;`
 *  add_const_if<double, false> d = 0.0;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, bool Condition>
using add_const_if = typename std::conditional<
  Condition, typename std::add_const<T>::type, T
>::type;

/**
 * Helper class similar to std::integral_constant whose value is the logical
 * negation of T's value.
 *
 * Example:
 *
 *  template <int X>
 *  using is_negative = std::integral_constant<bool, (X < 0)>;
 *
 *  template <int X>
 *  using is_non_negative = negate_constant<is_negative<X>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct negate_constant: public std::integral_constant<bool, !T::value> {};

/**
 * Helper class similar to std::integral_constant whose value is the logical
 * AND of the value of each `Arg`.
 *
 * Example:
 *
 *  template <typename A, typename B, typename C>
 *  using all_equal = logical_and_constants<
 *    std::is_same<A, B>, std::is_same<B, C>
 *  >;
 *
 *  // yields `false`
 *  all_equal<int, bool, double>::value
 *
 *  // yields `false`
 *  all_equal<int, bool, int>::value
 *
 *  // yields `true`
 *  all_equal<int, int, int>::value
 *
 *  template <typename... Args>
 *  struct Foo {
 *    static_assert(
 *      logical_and_constants<std::is_signed<Args>...)::value,
 *      "Args should be signed arithmetic types"
 *    );
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
struct logical_and_constants:
  public std::integral_constant<
    bool,
    T::value && logical_and_constants<Args...>::value
  >
{};

template <typename T>
struct logical_and_constants<T>:
  public std::integral_constant<bool, T::value>
{};

/**
 * Helper class similar to std::integral_constant whose value is the logical
 * OR of the value of each `Arg`.
 *
 * Example:
 *
 *  template <typename A, typename B, typename C>
 *  using has_duplicate = logical_or_constants<
 *    std::is_same<A, B>, std::is_same<B, C>, std::is_same<A, C>
 *  >;
 *
 *  // yields `false`
 *  has_duplicate<int, bool, double>::value
 *
 *  // yields `true`
 *  has_duplicate<int, bool, int>::value
 *
 *  // yields `true`
 *  has_duplicate<int, int, int>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
struct logical_or_constants:
  public std::integral_constant<
    bool,
    T::value || logical_or_constants<Args...>::value
  >
{};

template <typename T>
struct logical_or_constants<T>:
  public std::integral_constant<bool, T::value>
{};

/**
 * Helper class similar to std::integral_constant whose value is the bitwise
 * complement of T's value.
 *
 * Example:
 *
 *  // yields `0xf0`
 *  complement_constant<std::integral_constant<uint8_t, 0xf>>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct complement_constant:
  public std::integral_constant<
    decltype(T::value),
    static_cast<decltype(T::value)>(~T::value)
  >
{};

/**
 * Helper class similar to std::integral_constant whose value is the bitwise
 * AND of the value of each `Arg`.
 *
 * Example:
 *
 *  template <int... Args>
 *  using bitwise_and = bitwise_and_constants<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `0`
 *  bitwise_and<1, 2, 4>::value
 *
 *  // yields `3`
 *  bitwise_and<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
struct bitwise_and_constants:
  public std::integral_constant<
    decltype(T::value & bitwise_and_constants<Args...>::value),
    T::value & bitwise_and_constants<Args...>::value
  >
{};

template <typename T>
struct bitwise_and_constants<T>:
  public std::integral_constant<decltype(T::value), T::value>
{};

/**
 * Helper class similar to std::integral_constant whose value is the bitwise
 * OR of the value of each `Arg`.
 *
 * Example:
 *
 *  template <int... Args>
 *  using bitwise_or = bitwise_or_constants<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `7`
 *  bitwise_or<1, 2, 4>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
struct bitwise_or_constants:
  public std::integral_constant<
    decltype(T::value | bitwise_or_constants<Args...>::value),
    T::value | bitwise_or_constants<Args...>::value
  >
{};

template <typename T>
struct bitwise_or_constants<T>:
  public std::integral_constant<decltype(T::value), T::value>
{};

/**
 * Helper class similar to std::integral_constant whose value is the bitwise
 * XOR of the value of each `Arg`.
 *
 * Example:
 *
 *  template <int... Args>
 *  using bitwise_xor = bitwise_xor_constants<
 *    std::integral_constant<int, Args>...
 *  >;
 *
 *  // yields `3`
 *  bitwise_xor<1, 2>::value
 *
 *  // yields `12`
 *  bitwise_xor<7, 11>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename... Args>
struct bitwise_xor_constants:
  public std::integral_constant<
    decltype(T::value ^ bitwise_xor_constants<Args...>::value),
    T::value ^ bitwise_xor_constants<Args...>::value
  >
{};

template <typename T>
struct bitwise_xor_constants<T>:
  public std::integral_constant<decltype(T::value), T::value>
{};

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `TLHS` and `TRHS`. Its boolean value is the result of the LESS THAN
 * comparison, figuratively `TLHS` < `TRHS`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 10> A;
 *  typedef std::integral_constant<int, 20> B;
 *
 *  // yields `true`
 *  constants_comparison_lt<A, B>::value
 *
 *  // yields `false`
 *  constants_comparison_lt<B, A>::value
 *
 *  // yields `false`
 *  constants_comparison_lt<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TLHS, typename TRHS>
struct constants_comparison_lt:
  public std::integral_constant<bool, (TLHS::value < TRHS::value)>
{};

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `TLHS` and `TRHS`. Its boolean value is the result of the GREATER THAN
 * comparison, figuratively `TLHS` > `TRHS`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 10> A;
 *  typedef std::integral_constant<int, 20> B;
 *
 *  // yields `false`
 *  constants_comparison_gt<A, B>::value
 *
 *  // yields `true`
 *  constants_comparison_gt<B, A>::value
 *
 *  // yields `false`
 *  constants_comparison_gt<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TLHS, typename TRHS>
struct constants_comparison_gt:
  public std::integral_constant<bool, (TLHS::value > TRHS::value)>
{};

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `TLHS` and `TRHS`. Its boolean value is the result of the EQUAL TO
 * comparison, figuratively `TLHS` == `TRHS`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 10> A;
 *  typedef std::integral_constant<int, 20> B;
 *
 *  // yields `false`
 *  constants_comparison_eq<A, B>::value
 *
 *  // yields `false`
 *  constants_comparison_eq<B, A>::value
 *
 *  // yields `true`
 *  constants_comparison_eq<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TLHS, typename TRHS>
struct constants_comparison_eq:
  public std::integral_constant<bool, TLHS::value == TRHS::value>
{};

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `TLHS` and `TRHS`. Its boolean value is the result of the LESS THAN or
 * EQUAL TO comparison, figuratively `TLHS` <= `TRHS`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 10> A;
 *  typedef std::integral_constant<int, 20> B;
 *
 *  // yields `true`
 *  constants_comparison_lte<A, B>::value
 *
 *  // yields `false`
 *  constants_comparison_lte<B, A>::value
 *
 *  // yields `true`
 *  constants_comparison_lte<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TLHS, typename TRHS>
struct constants_comparison_lte:
  public std::integral_constant<bool, (TLHS::value <= TRHS::value)>
{};

/**
 * Helper class similar to std::integral_constant for comparing two types
 * `TLHS` and `TRHS`. Its boolean value is the result of the GREATER THAN or
 * EQUAL TO comparison, figuratively `TLHS` >= `TRHS`.
 *
 * Example:
 *
 *  typedef std::integral_constant<int, 10> A;
 *  typedef std::integral_constant<int, 20> B;
 *
 *  // yields `false`
 *  constants_comparison_gte<A, B>::value
 *
 *  // yields `true`
 *  constants_comparison_gte<B, A>::value
 *
 *  // yields `true`
 *  constants_comparison_gte<A, A>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TLHS, typename TRHS>
struct constants_comparison_gte:
  public std::integral_constant<bool, (TLHS::value >= TRHS::value)>
{};

/**
 * A convenience template that binds to a type comparer
 * and to an optional transform that is applied to the
 * operands before the comparison takes place.
 *
 * Example:
 *
 *
 *  template <typename LHS, typename RHS>
 *  struct Foo {
 *    template <template <typename, typename> class TComparer>
 *    using comparison = std::integral_constant<
 *      bool, TComparer<LHS, RHS>::value
 *    >;
 *  };
 *
 *  typedef Foo<
 *    std::integral_constant<int, 5>,
 *    std::integral_constant<int, 8>
 *  > values_5_8;
 *
 *  // yields `std::integral_constant<bool, true>`
 *  typedef values_5_8::comparison<curried_type_comparer<>> result1;
 *
 *  // yields `std::integral_constant<bool, false>`
 *  typedef values_5_8::comparison<
 *    curried_type_comparer<constants_comparison_gt>
 *  > result2;
 *
 *  template <int X>
 *  struct Bar { typedef std::integral_constant<int, X> type; };
 *
 *  typedef Foo<Bar<80>, Bar<10>> values_80_10;
 *
 *  // yields `std::integral_constant<bool, true>`
 *  typedef values_80_10::comparison<
 *    curried_type_comparer<
 *      constants_comparison_gt,
 *      get_member_typedef::template type
 *    >
 *  > result3;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TComparer = constants_comparison_lt,
  template <typename...> class TTransform = transform::identity
>
struct curried_type_comparer {
  template <typename TLHS, typename TRHS>
  using type = TComparer<TTransform<TLHS>, TTransform<TRHS>>;
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
  safe_ctor_overload<Class, Args...>::value,
  void
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

/////////////////////////////////////////////////
// CORE FUNCTIONALITY FOR TYPE DATA STRUCTURES //
/////////////////////////////////////////////////

/**
 * Declaration of traits class used by `type_get`.
 *
 * This class should be specialized for new data structures so they are
 * supported by `type_get`. It must provide a member typedef `type` with
 * the `Index`-th type of the `TDataStructure` data structure.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TDataStructure, std::size_t Index> struct type_get_traits;

/**
 * `type_get` is for positional type data structures what std::get is for
 * positional value data structures.
 *
 * It allows you to find out what's the i-th type stored in a type data
 * structure.
 *
 * `type_get_first` and `type_get_second` are provided for convenience.
 *
 * Example:
 *
 *  type_list<int, void, bool> list;
 *
 *  // yields `bool`
 *  typedef third = type_get<2>::type<list>
 *
 *  // yields `int`
 *  typedef third = type_get<0>::type<list>
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index>
struct type_get {
  template <typename TDataStructure>
  using type = typename type_get_traits<TDataStructure, Index>::type;
};

/**
 * A convenience shortcut for type_get<0>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_first = type_get<0>::template type<T>;

/**
 * A convenience shortcut for type_get<1>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_second = type_get<1>::template type<T>;

/**
 * A convenience template that binds to a type comparer.
 * It compares the result of `type_get_first` when applied to the operands.
 *
 * Example:
 *
 *  template <int LHS, int RHS>
 *  class Foo {
 *    typedef type_pair<std::integral_constant<int, LHS>, void> lhs
 *    typedef type_pair<std::integral_constant<int, RHS>, double> rhs
 *
 *  public:
 *    template <template <typename, typename> class TComparer>
 *    using comparison = std::integral_constant<
 *      bool, TComparer<lhs, rhs>::value
 *    >;
 *  };
 *
 *  // yields `std::integral_constant<bool, true>`
 *  typedef Foo<5, 8>::comparison<type_get_first_comparer<>> result1;
 *
 *  // yields `std::integral_constant<bool, false>`
 *  typedef Foo<5, 8>::comparison<
 *    type_get_first_comparer<constants_comparison_gt>
 *  > result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TComparer = constants_comparison_lt
>
using type_get_first_comparer = curried_type_comparer<
  TComparer, type_get_first
>;

/**
 * A convenience template that binds to a type comparer.
 * It compares the result of `type_get_second` when applied to the operands.
 *
 * Example:
 *
 *  template <int LHS, int RHS>
 *  class Foo {
 *    typedef type_pair<void, std::integral_constant<int, LHS>> lhs;
 *    typedef type_pair<double, std::integral_constant<int, RHS>> rhs;
 *
 *  public:
 *    template <template <typename, typename> class TComparer>
 *    using comparison = std::integral_constant<
 *      bool, TComparer<lhs, rhs>::value
 *    >;
 *  };
 *
 *  // yields `std::integral_constant<bool, true>`
 *  typedef Foo<5, 8>::comparison<type_get_second_comparer<>> result1;
 *
 *  // yields `std::integral_constant<bool, false>`
 *  typedef Foo<5, 8>::comparison<
 *    type_get_second_comparer<constants_comparison_gt>
 *  > result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TComparer = constants_comparison_lt
>
using type_get_second_comparer = curried_type_comparer<
  TComparer, type_get_second
>;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

/**
 * Specialization of `type_get_traits` so that `type_get` supports `std::pair`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index, typename TFirst, typename TSecond>
struct type_get_traits<std::pair<TFirst, TSecond>, Index> {
  typedef typename std::tuple_element<
    Index,
    std::pair<TFirst, TSecond>
  >::type type;
};

/**
 * Specialization of `type_get_traits` so that `type_get` supports `std::tuple`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index, typename... Args>
struct type_get_traits<std::tuple<Args...>, Index> {
  typedef typename std::tuple_element<
    Index,
    std::tuple<Args...>
  >::type type;
};

} // namespace fatal
