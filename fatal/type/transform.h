/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/tag.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace ftl {
namespace transform {

//////////////
// identity //
//////////////

/**
 * Helper alias similar to std::decay_t, that resolves to the type
 * given as the template parameter.
 *
 * Example:
 *
 *  // yields `int`
 *  typedef identity<int> result1;
 *
 *  // yields `std::string`
 *  typedef identity<std::string> result2;
 *
 *  // yields `double`
 *  typedef identity<identity<double>>> result3;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {
// needed due to gcc bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62276
template <typename T> struct identity_impl { typedef T type; };
} // namespace detail {
template <typename T> using identity = typename detail::identity_impl<T>::type;

///////////
// fixed //
///////////

/**
 * Helper alias that always resolves to the same type regardless of parameters.
 *
 * It is useful, for instance, in situations where you have a variadic method
 * and you want to receive parameters of a given type in the same amount as the
 * variadic template types.
 *
 * Example:
 *
 *  template <typename... Args>
 *  void foo(fixed<int>::type<Args>... args) {
 *    // ...
 *  }
 *
 *  // calls foo passing 3 integers
 *  foo<bool, int, std::string>(0, 1, 2);
 *
 *  // compilation error: foo expects 3 integers as parameters
 *  foo<bool, int, std::string>(0, 1);
 *
 *  // compilation error: foo expects 3 integers as parameters
 *  foo<bool, int, std::string>(true, 0, std::string("hello"));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct fixed {
  template <typename...> using type = T;
};

//////////////
// constant //
//////////////

/**
 * Helper alias that always evaluates to `std::integral_constant<T, Value>`.
 *
 * This is very useful when initializing a variadic
 * container with the same type:
 *
 * Example:
 *
 *  template <typename... TTags>
 *  struct tag_counters {
 *    tag_counters():
 *      // initializes the `counters` tuple with as many 0s as there are TTags
 *      counters(constant<int, 0>::template type<TTags>::value...)
 *    {}
 *
 *  private:
 *    // creates a `std::tuple<int, int...>` with
 *    // as many `int`s as there are `TTag`s
 *    std::tuple<fixed<int>::template type<TTags>...> counters;
 *  };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Value>
struct constant {
  template <typename...> using type = std::integral_constant<T, Value>;
};

//////////////
// type_mod //
//////////////

/**
 * An adapter that allows using a template class' member typedef `type`
 * as a transform.
 *
 * This is useful for using C++'s type modifiers (decay, make_signed, ...)
 * as transforms on a pre-C++14 compiler.
 *
 * `TTransform` is any class that receives a given type `T` as a template
 * parameter and provides a member typedef `type`, which is to be used as
 * the transform result.
 *
 * Example:
 *
 *  typedef type_mod<std::decay> dc;
 *
 *  // yields `int`
 *  typedef dc::type<int const &> result1;
 *
 *  template <typename T, template <typename...> class MyTransform>
 *  using foo = MyTransform<T>;
 *
 *  // yields `double`
 *  typedef foo<double &&, dc::template type> result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTransform>
struct type_mod {
  template <typename... Args>
  using type = typename TTransform<Args...>::type;
};

////////////
// nested //
////////////

namespace detail {

template <
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
struct nested_impl {
  template <typename... Args>
  using type = typename nested_impl<TTransforms...>::template type<
    typename nested_impl<TTransform>::template type<Args...>
  >;
};

template <template <typename...> class TTransform>
struct nested_impl<TTransform> {
  template <typename... Args>
  using type = TTransform<Args...>;
};

} // namespace detail {

/**
 * A convenience adapter that allows nesting multiple transforms into one.
 *
 * The transforms are applied in the order specified.
 *
 * Example:
 *
 *  template <typename> struct T1 {};
 *  template <typename> struct T2 {};
 *  template <typename> struct T3 {};
 *
 *  typedef nested<T1, T2, T3> tr;
 *
 *  // yields `T3<T2<T1<int>>>`
 *  typedef tr::type<int> result;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... TTransforms>
using nested = detail::nested_impl<TTransforms...>;

///////////
// alias //
///////////

/**
 * Provides an alias for type transforms, so they can be handled as regular
 * types.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTemplate, typename... Args>
struct alias {
  /**
   * Applies the given arguments `UArgs...`, in addition
   * to the partially applied ones, to the original template.
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *
   *  // yields `std::tuple<int, double>`
   *  typedef c1::apply<> result1;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  typedef c1::apply<long, std::string, bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using apply = TTemplate<Args..., UArgs...>;

  /**
   * The same as apply.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using type = apply<UArgs...>;

  /**
   * Partially applies arguments `UArgs...` in addition
   * to those partially applied so far.
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *  typedef c1::curry<long, std::string> c2;
   *
   *  // yields `std::tuple<int, double, long, std::string>`
   *  typedef c2::type<> result1;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  typedef c2::type<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using curry = alias<TTemplate, Args..., UArgs...>;

  /**
   * Replaces all arguments partially applied so far with `UArgs...`.
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *  typedef c1::rebind<long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::type<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::type<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using rebind = alias<TTemplate, UArgs...>;

  /**
   * Discards any arguments partially applied so far.
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *
   *  typedef alias<c1::template uncurry, long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::type<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::type<bool, float> result2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c1::uncurry<long, std::string> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using uncurry = TTemplate<UArgs...>;

  /**
   * Re-applies arguments `UArgs...`, in addition to all
   * partially applied arguments, to a separate template.
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *
   *  // yields `V1<int, double>`
   *  typedef c1::rebind_args<V1>::type<> result1;
   *
   *  // yields `V1<int, double, bool, float>`
   *  typedef c1::rebind_args<V1, bool, float>::type<> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class UTemplate, typename... UArgs>
  using rebind_args = alias<UTemplate, Args..., UArgs...>;

  /**
   * Applies the partially applied arguments, in addition
   * to the arguments `UArgs...`, to another template.
   *
   * This acts as a shortcut to `rebind_args<UTemplate, UArgs...>::type<>`
   *
   * Example:
   *
   *  typedef alias<std::tuple, int, double> c1;
   *
   *  // yields `V1<int, double>`
   *  typedef c1::apply_args<V1> result1;
   *
   *  // yields `V1<int, double, bool, float>`
   *  typedef c1::apply_args<V1, bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class UTemplate, typename... UArgs>
  using apply_args = UTemplate<Args..., UArgs...>;
};

///////////////////////////////
// recursive_type_merge_sort //
///////////////////////////////

template <typename T, std::size_t Depth>
struct recursive_type_merge_sort_impl { typedef T type; };

template <std::size_t Depth = std::numeric_limits<std::size_t>::max()>
struct recursive_type_merge_sort {
  template <typename T>
  using type = typename recursive_type_merge_sort_impl<T, Depth>::type;
};

template <typename T>
using full_recursive_type_merge_sort = typename recursive_type_merge_sort<>
  ::template type<T>;

} // namespace transform {
} // namespace ftl
