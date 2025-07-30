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
#include <fatal/type/sequence.h>

namespace fatal {

//////////////////////
// reflect_template //
//////////////////////

/**
 * The category of a reflected class template, as follows:
 *
 * - `typed_value_list` follows this signature:
 *    template <typename Type, Type... Values> class T;
 *
 * - `type_list` follows this signature:
 *    template <typename... Types> class T;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
enum class reflect_template_category {
  non_template, typed_value_list, type_list
};

/**
 * Compile-time reflection for templates.
 *
 * Extracts template signature and template parameters out of
 * a template instantiation.
 *
 * Unspecialized version matches non-template classes.
 *
 * Example:
 *
 *  template <typename... T> struct foo {};
 *  template <int... Seq> struct int_seq {};
 *
 *  typedef foo<int> foo_int;
 *  typedef int_seq<1, 2, 3> int_seq_123;
 *
 *  typedef reflect_template<foo_int> foo_r;
 *  typedef reflect_template<int_seq_123> is_r;
 *
 *  // yields `type_list<int>`
 *  typedef foo_r::list result1;
 *
 *  // yields `sequence<int, 1, 2, 3>`
 *  typedef is_r::values result2;
 *
 *  // yields `foo<short, bool>`
 *  typedef foo_r::rebind<short, bool> result3;
 *
 *  // yields `int_seq<99, 100>`
 *  typedef is_r::rebind<99, 100> result4;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct reflect_template {
  /**
   * The original reflected type (whatever was passed to `reflect_template`).
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = T;

  /**
   * The category of the reflected class template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using category = std::integral_constant<reflect_template_category, reflect_template_category::non_template>;

  /**
   * Tells if the original reflected type was instantiated from the same
   * type_list category template as the one given.
   *
   * This implementation always returns false since the original reflected
   * type is not a template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class>
  using is_same_tl = std::false_type;

  /**
   * Tells if the original reflected type was instantiated from the same
   * typed_value_list category template as the one given.
   *
   * This implementation always returns false since the original reflected
   * type is not a template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename X, X...> class>
  using is_same_tvl = std::false_type;
};


/**
 * Reflect template specialization for class templates with the signature:
 *
 *  template <typename... Types> class T;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class T, typename... Types>
struct reflect_template<T<Types...>> {
  /**
   * The original reflected type (whatever was passed to `reflect_template`).
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = T<Types...>;

  /**
   * The category of the reflected class template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using category = std::integral_constant<
    reflect_template_category,
    reflect_template_category::type_list
  >;

  /**
   * Rebinds the reflected class template with new types.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... U>
  using rebind = T<U...>;

  /**
   * A `type_list` with all template parameters.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using types = type_list<Types...>;

  /**
   * Tells if the original reflected type was instantiated from the same
   * template as the one given.
   *
   * Example:
   *
   *  typedef std::tuple<int, bool, char> type;
   *
   *  // yields `true`
   *  reflect_template<type>::is_same_tl<std::tuple>::value
   *
   *  // yields `false`
   *  reflect_template<type>::is_same_tl<std::basic_string>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class U>
  using is_same_tl = std::is_same<type, U<Types...>>;

  /**
   * Tells if the original reflected type was instantiated from the same
   * typed_value_list category template as the one given.
   *
   * This implementation always returns false since the original reflected
   * type is not a typed_value_list category template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename X, X...> class>
  using is_same_tvl = std::false_type;
};

/**
 * Reflect template specialization for class templates with the signature:
 *
 *  template <typename Type, Type... Values> class T;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename V, template <typename, V...> class T, V... Values>
struct reflect_template<T<V, Values...>> {
  /**
   * The original reflected type (whatever was passed to `reflect_template`).
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = T<V, Values...>;

  /**
   * The category of the reflected class template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using category = std::integral_constant<
    reflect_template_category,
    reflect_template_category::typed_value_list
  >;

  /**
   * The type of the values.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using value_type = V;

  /**
   * Rebinds the reflected class template with new type and values.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename U = V>
  struct rebind {
    template <U... UValues>
    using apply = T<U, UValues...>;
  };

  /**
   * A `sequence` of all values.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using values = sequence<V, Values...>;

  /**
   * A `type_list` of `std::integral_constant`s for all values.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using types = type_list<std::integral_constant<V, Values>...>;

  /**
   * Tells if the original reflected type was instantiated from the same
   * type_list category template as the one given.
   *
   * This implementation always returns false since the original reflected
   * type is not a type_list category template.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class>
  using is_same_tl = std::false_type;

  /**
   * Tells if the original reflected type was instantiated from the same
   * typed_value_list category template as the one given.
   *
   * Example:
   *
   *  typedef std::integer_sequence<int, bool, char> type;
   *  template <typename T, T...> struct seq;
   *
   *  // yields `true`
   *  reflect_template<type>::is_same_tvl<std::integer_sequence>::value
   *
   *  // yields `false`
   *  reflect_template<type>::is_same_tvl<seq>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename X, X...> class U>
  using is_same_tvl = std::is_same<type, U<V, Values...>>;
};

// TODO: DOCUMENT AND TEST
template <typename T>
using reflect_values = typename reflect_template<T>::values;

// TODO: DOCUMENT AND TEST
template <typename T>
using reflect_types = typename reflect_template<T>::types;

} // namespace fatal {
