/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_transform_h
#define FATAL_INCLUDE_fatal_type_transform_h

#include <fatal/type/tag.h>

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {

///////////
// apply //
///////////

namespace detail {

template <template <typename...> class T, typename... Args>
struct apply_impl { using type = T<Args...>; };

} // namespace detail {

/**
 * Applies arguments to a transform.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTransform, typename... Args>
using apply = typename detail::apply_impl<TTransform, Args...>::type;

////////////////////////
// identity_transform //
////////////////////////

/**
 * Helper alias similar to std::decay_t, that resolves to the type
 * given as the template parameter.
 *
 * Example:
 *
 *  // yields `int`
 *  typedef identity_transform<int> result1;
 *
 *  // yields `std::string`
 *  typedef identity_transform<std::string> result2;
 *
 *  // yields `double`
 *  typedef identity_transform<identity_transform<double>>> result3;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {
// needed due to gcc bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62276
template <typename T> struct identity_transform_impl { typedef T type; };
} // namespace detail {

template <typename T>
using identity_transform = typename detail::identity_transform_impl<T>::type;

/////////////////////
// fixed_transform //
/////////////////////

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
 *  void foo(fixed_transform<int>::apply<Args>... args) {
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
struct fixed_transform {
  template <typename...> using apply = T;
};

////////////////////////
// constant_transform //
////////////////////////

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
 *      counters(constant_transform<int, 0>::template apply<TTags>::value...)
 *    {}
 *
 *  private:
 *    // creates a `std::tuple<int, int...>` with
 *    // as many `int`s as there are `TTag`s
 *    std::tuple<constant_transform<int, 0>::template apply<TTags>...> counters;
 *  };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Value>
struct constant_transform {
  template <typename...> using apply = std::integral_constant<T, Value>;
};

/**
 * A convenience specialization of `constant_transform`
 * that always yields std::true_type.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename...> using true_predicate = std::true_type;

/**
 * A convenience specialization of `constant_transform`
 * that always yields std::false_type.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename...> using false_predicate = std::false_type;

/**
 * A transform which casts the given type's `value` member to type `TTo`.
 * Returns a `std::integral_constant` of type `TTo`.
 *
 * There's a specialization for when `TTo` is `bool` that either returns
 * `std::true_type` or `std::false_type`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TTo>
struct cast_transform {
  template <typename T>
  using apply = std::integral_constant<TTo, static_cast<TTo>(T::value)>;
};

template <>
struct cast_transform<bool> {
  template <typename T>
  using apply = typename std::conditional<
    static_cast<bool>(T::value),
    std::true_type,
    std::false_type
  >::type;
};

/**
 * A convenience specialization of `cast_transform` for `bool`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_true_transform = typename cast_transform<bool>::template apply<T>;

/**
 * A convenience transform which returns the logical
 * negation of `is_true_predicate`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_false_transform = std::integral_constant<
  bool,
  !is_true_transform<T>::value
>;

/**
 * Tells whether the given type's `value` member equals `0`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_zero_transform = std::integral_constant<bool, T::value == 0>;

/**
 * Tells whether the given type's `value` member doesn't equal `0`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using not_zero_transform = std::integral_constant<bool, T::value != 0>;

//////////////////////
// sizeof_transform //
//////////////////////

// TODO: DOCUMENT AND TEST
template <typename T>
using sizeof_transform = std::integral_constant<std::size_t, sizeof(T)>;

////////////////////////
// transform_sequence //
////////////////////////

/**
 * A convenience adapter that allows nesting multiple transforms into one.
 *
 * The transforms are applied in the order specified.
 *
 * If no transforms are given, it acts like `identity_transform`.
 *
 * Example:
 *
 *  template <typename> struct T1 {};
 *  template <typename> struct T2 {};
 *  template <typename> struct T3 {};
 *
 *  typedef transform_sequence<T1, T2, T3> tr;
 *
 *  // yields `T3<T2<T1<int>>>`
 *  typedef tr::apply<int> result;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class...> struct transform_sequence;

template <
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
struct transform_sequence<TTransform, TTransforms...> {
  template <typename... Args>
  using apply = typename transform_sequence<TTransforms...>::template apply<
    typename fatal::apply<TTransform, Args...>
  >;
};

template <>
struct transform_sequence<> {
  template <typename T, typename...>
  using apply = T;
};

//////////////////////////
// arithmetic_transform //
//////////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace arithmetic_transform {
  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct add:
    public std::integral_constant<
      decltype(T::value + add<Args...>::value),
      (T::value + add<Args...>::value)
    >
  {};

  template <typename T>
  struct add<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename...> struct subtract;

  template <typename TLHS, typename TRHS, typename... Args>
  struct subtract<TLHS, TRHS, Args...>:
    public subtract<subtract<TLHS, TRHS>, Args...>
  {};

  template <typename TLHS, typename TRHS>
  struct subtract<TLHS, TRHS>:
    public std::integral_constant<
      decltype(TLHS::value - TRHS::value),
      (TLHS::value - TRHS::value)
    >
  {};

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct multiply:
    public std::integral_constant<
      decltype(T::value * multiply<Args...>::value),
      (T::value * multiply<Args...>::value)
    >
  {};

  template <typename T>
  struct multiply<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename...> struct divide;

  template <typename TLHS, typename TRHS, typename... Args>
  struct divide<TLHS, TRHS, Args...>:
    public divide<divide<TLHS, TRHS>, Args...>
  {};

  template <typename TLHS, typename TRHS>
  struct divide<TLHS, TRHS>:
    public std::integral_constant<
      decltype(TLHS::value / TRHS::value),
      (TLHS::value / TRHS::value)
    >
  {};

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename...> struct modulo;

  template <typename TLHS, typename TRHS, typename... Args>
  struct modulo<TLHS, TRHS, Args...>:
    public modulo<modulo<TLHS, TRHS>, Args...>
  {};

  template <typename TLHS, typename TRHS>
  struct modulo<TLHS, TRHS>:
    public std::integral_constant<
      decltype(TLHS::value % TRHS::value),
      (TLHS::value % TRHS::value)
    >
  {};
};

///////////////////////
// logical_transform //
///////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace logical_transform {
  /**
   * Helper class similar to std::integral_constant whose value is the logical
   * AND of the value of each `Arg`.
   *
   * Example:
   *
   *  template <typename A, typename B, typename C>
   *  using all_equal = logical_transform::all<
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
   *      logical_transform::all<std::is_signed<Args>...)::value,
   *      "Args should be signed arithmetic types"
   *    );
   *  }
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct all:
    public std::integral_constant<
      decltype(T::value && all<Args...>::value),
      (T::value && all<Args...>::value)
    >
  {};

  template <typename T>
  struct all<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * Helper class similar to std::integral_constant whose value is the logical
   * OR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <typename A, typename B, typename C>
   *  using has_duplicate = logical_transform::any<
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
  struct any:
    public std::integral_constant<
      decltype(T::value || any<Args...>::value),
      (T::value || any<Args...>::value)
    >
  {};

  template <typename T>
  struct any<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * Helper class similar to std::integral whose value is the logical
   * negation of T's value.
   *
   * Example:
   *
   *  template <int X>
   *  using is_negative = std::integral_constant<bool, (X < 0)>;
   *
   *  template <int X>
   *  using is_non_negative = logical_transform::negate<is_negative<X>>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using negate = std::integral_constant<decltype(!T::value), !T::value>;
};

///////////////////////
// bitwise_transform //
///////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace bitwise_transform {
  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * AND of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using all = bitwise_transform::all<
   *    std::integral_constant<int, Args>...
   *  >;
   *
   *  // yields `0`
   *  all<1, 2, 4>::value
   *
   *  // yields `3`
   *  all<7, 11>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct all:
    public std::integral_constant<
      decltype(T::value & all<Args...>::value),
      (T::value & all<Args...>::value)
    >
  {};

  template <typename T>
  struct all<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * OR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using any = bitwise_transform::any<
   *    std::integral_constant<int, Args>...
   *  >;
   *
   *  // yields `7`
   *  any<1, 2, 4>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct any:
    public std::integral_constant<
      decltype(T::value | any<Args...>::value),
      (T::value | any<Args...>::value)
    >
  {};

  template <typename T>
  struct any<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * XOR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using diff = bitwise_transform::diff<
   *    std::integral_constant<int, Args>...
   *  >;
   *
   *  // yields `3`
   *  diff<1, 2>::value
   *
   *  // yields `12`
   *  diff<7, 11>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  struct diff:
    public std::integral_constant<
      decltype(T::value ^ diff<Args...>::value),
      (T::value ^ diff<Args...>::value)
    >
  {};

  template <typename T>
  struct diff<T>:
    public std::integral_constant<decltype(T::value), T::value>
  {};

  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * complement of T's value.
   *
   * Example:
   *
   *  // yields `0xf0`
   *  bitwise_transform::complement<
   *    std::integral_constant<std::uint8_t, 0xf>
   *  >::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using complement = std::integral_constant<
    typename std::conditional<
      std::is_integral<decltype(T::value)>::value,
      decltype(T::value),
      decltype(~T::value)
    >::type,
    static_cast<
      typename std::conditional<
        std::is_integral<decltype(T::value)>::value,
        decltype(T::value),
        decltype(~T::value)
      >::type
    >(~T::value)
  >;
};

//////////////////////////
// comparison_transform //
//////////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct comparison_transform {
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
   *  comparison_transform::equal<A, B>::value
   *
   *  // yields `false`
   *  comparison_transform::equal<B, A>::value
   *
   *  // yields `true`
   *  comparison_transform::equal<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using equal = std::integral_constant<
    decltype(TLHS::value == TRHS::value),
    (TLHS::value == TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the NOT EQUAL TO
   * comparison, figuratively `TLHS` != `TRHS`.
   *
   * Example:
   *
   *  typedef std::integral_constant<int, 10> A;
   *  typedef std::integral_constant<int, 20> B;
   *
   *  // yields `true`
   *  comparison_transform::not_equal<A, B>::value
   *
   *  // yields `true`
   *  comparison_transform::not_equal<B, A>::value
   *
   *  // yields `false`
   *  comparison_transform::not_equal<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using not_equal = std::integral_constant<
    decltype(TLHS::value != TRHS::value),
    (TLHS::value != TRHS::value)
  >;

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
   *  comparison_transform::less_than<A, B>::value
   *
   *  // yields `false`
   *  comparison_transform::less_than<B, A>::value
   *
   *  // yields `false`
   *  comparison_transform::less_than<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using less_than = std::integral_constant<
    decltype(TLHS::value < TRHS::value),
    (TLHS::value < TRHS::value)
  >;

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
   *  comparison_transform::less_than_equal<A, B>::value
   *
   *  // yields `false`
   *  comparison_transform::less_than_equal<B, A>::value
   *
   *  // yields `true`
   *  comparison_transform::less_than_equal<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using less_than_equal = std::integral_constant<
    decltype(TLHS::value <= TRHS::value),
    (TLHS::value <= TRHS::value)
  >;

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
   *  comparison_transform::greater_than<A, B>::value
   *
   *  // yields `true`
   *  comparison_transform::greater_than<B, A>::value
   *
   *  // yields `false`
   *  comparison_transform::greater_than<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using greater_than = std::integral_constant<
    decltype(TLHS::value > TRHS::value),
    (TLHS::value > TRHS::value)
  >;

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
   *  comparison_transform::greater_than_equal<A, B>::value
   *
   *  // yields `true`
   *  comparison_transform::greater_than_equal<B, A>::value
   *
   *  // yields `true`
   *  comparison_transform::greater_than_equal<A, A>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TLHS, typename TRHS>
  using greater_than_equal = std::integral_constant<
    decltype(TLHS::value >= TRHS::value),
    (TLHS::value >= TRHS::value)
  >;
};

/**
 * A convenient macro that creates a transform which
 * evaluates to the given member typedef.
 *
 * Provides a template alias named `Name` which extracts
 * the member typedef with the same name.
 *
 * Example:
 *
 *  FATAL_GET_MEMBER_TYPE(type);
 *
 *  // evaluates to `int const`
 *  using result = type<std::add_const<int>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
# define FATAL_GET_MEMBER_TYPE(Name) \
  template <typename T> using Name = typename T::Name

/**
 * Provides transforms that evaluate to a member typedef of a given type.
 *
 * Example:
 *
 *  // yields `int`
 *  typedef get_member_type::type<std::add_const<int>> result1;
 *
 *  typedef std::map<double, std::string> map;
 *
 *  // yields `std::pair<double, std::string>`
 *  typedef get_member_type::value_type<map> result2;
 *
 *  // yields `double`
 *  typedef get_member_type::key_type<map> result3;
 *
 *  // yields `std::string`
 *  typedef get_member_type::mapped_type<map> result4;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct get_member_type {
  FATAL_GET_MEMBER_TYPE(char_type);
  FATAL_GET_MEMBER_TYPE(type);
  FATAL_GET_MEMBER_TYPE(types);

# define FATAL_IMPL_GET_MEMBER_TYPE(Name) \
  FATAL_GET_MEMBER_TYPE(Name); \
  FATAL_GET_MEMBER_TYPE(Name##_type)

  FATAL_IMPL_GET_MEMBER_TYPE(allocator);
  FATAL_IMPL_GET_MEMBER_TYPE(args);
  FATAL_IMPL_GET_MEMBER_TYPE(array);
  FATAL_IMPL_GET_MEMBER_TYPE(config);
  FATAL_IMPL_GET_MEMBER_TYPE(const_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE(const_pointer);
  FATAL_IMPL_GET_MEMBER_TYPE(const_reference);
  FATAL_IMPL_GET_MEMBER_TYPE(const_reverse_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE(difference);
  FATAL_IMPL_GET_MEMBER_TYPE(element);
  FATAL_IMPL_GET_MEMBER_TYPE(first);
  FATAL_IMPL_GET_MEMBER_TYPE(flag);
  FATAL_IMPL_GET_MEMBER_TYPE(hash);
  FATAL_IMPL_GET_MEMBER_TYPE(id);
  FATAL_IMPL_GET_MEMBER_TYPE(ids);
  FATAL_IMPL_GET_MEMBER_TYPE(index);
  FATAL_IMPL_GET_MEMBER_TYPE(instance);
  FATAL_IMPL_GET_MEMBER_TYPE(item);
  FATAL_IMPL_GET_MEMBER_TYPE(iterator);
  FATAL_IMPL_GET_MEMBER_TYPE(key);
  FATAL_IMPL_GET_MEMBER_TYPE(list);
  FATAL_IMPL_GET_MEMBER_TYPE(map);
  FATAL_IMPL_GET_MEMBER_TYPE(mapped);
  FATAL_IMPL_GET_MEMBER_TYPE(mapping);
  FATAL_IMPL_GET_MEMBER_TYPE(mappings);
  FATAL_IMPL_GET_MEMBER_TYPE(name);
  FATAL_IMPL_GET_MEMBER_TYPE(names);
  FATAL_IMPL_GET_MEMBER_TYPE(pair);
  FATAL_IMPL_GET_MEMBER_TYPE(pointer);
  FATAL_IMPL_GET_MEMBER_TYPE(reference);
  FATAL_IMPL_GET_MEMBER_TYPE(result);
  FATAL_IMPL_GET_MEMBER_TYPE(reverse);
  FATAL_IMPL_GET_MEMBER_TYPE(reverse_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE(second);
  FATAL_IMPL_GET_MEMBER_TYPE(set);
  FATAL_IMPL_GET_MEMBER_TYPE(size);
  FATAL_IMPL_GET_MEMBER_TYPE(string);
  FATAL_IMPL_GET_MEMBER_TYPE(tag);
  FATAL_IMPL_GET_MEMBER_TYPE(traits);
  FATAL_IMPL_GET_MEMBER_TYPE(tuple);
  FATAL_IMPL_GET_MEMBER_TYPE(value);
  FATAL_IMPL_GET_MEMBER_TYPE(values);
  FATAL_IMPL_GET_MEMBER_TYPE(version);

# undef FATAL_IMPL_GET_MEMBER_TYPE
};

///////////////////////////
// conditional_transform //
///////////////////////////

namespace detail {

template <
  bool,
  template <typename...> class TWhenTrueTransform,
  template <typename...> class,
  typename T
>
struct conditional_transform_impl {
  using type = TWhenTrueTransform<T>;
};

template <
  template <typename...> class TWhenTrueTransform,
  template <typename...> class TWhenFalseTransform,
  typename T
>
struct conditional_transform_impl<
  false, TWhenTrueTransform, TWhenFalseTransform, T
> {
  using type = TWhenFalseTransform<T>;
};

} // namespace detail {

/**
 * Applies either `TWhenTrueTransform` or `TWhenFalseTransform` to the type `T`
 * depending whether the predicate `TPredicate` returns, respectively, true or
 * false, when applied to to the type `T`.
 *
 * The default transform for `TWhenFalseTransform` is `identity_transform`.
 *
 * Example:
 *
 *  using transformA = conditional_transform<
 *    std::is_integral,
 *    std::make_unsigned_t
 *  >;
 *
 *  // yields `unsigned long`
 *  using resultA1 = transformA::apply<long>;
 *
 *  // yields `std::string`
 *  using resultA2 = transformA::apply<std::string>;
 *
 *  using transformB = conditional_transform<
 *    std::is_integral,
 *    std::tuple,
 *    std::add_pointer_t
 *  >;
 *
 *  // yields `std::tuple<long>`
 *  using resultB1 = transformB::apply<long>;
 *
 *  // yields `std::string *`
 *  using resultB2 = transformB::apply<std::string>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TPredicate,
  template <typename...> class TWhenTrueTransform,
  template <typename...> class TWhenFalseTransform = identity_transform
>
struct conditional_transform {
  template <typename T>
  using apply = typename detail::conditional_transform_impl<
    fatal::apply<TPredicate, T>::value,
    TWhenTrueTransform,
    TWhenFalseTransform,
    T
  >::type;
};

//////////////////////
// transform_traits //
//////////////////////

// TODO: DOCUMENT
template <template <typename...> class TTransform>
struct transform_traits {
  template <std::size_t> struct dummy;

  template <typename T, template <typename...> class UTransform>
  static std::true_type sfinae(dummy<sizeof(UTransform<T>)> *);

  template <typename, template <typename...> class>
  static std::false_type sfinae(...);

  // TODO: DOCUMENT
  template <typename T>
  using supports = decltype(sfinae<T, TTransform>(nullptr));
};

///////////////////
// try_transform //
///////////////////

// TODO: DOCUMENT AND TEST
template <
  template <typename...> class TTransform,
  template <typename...> class TFallback = identity_transform
>
struct try_transform {
  template <typename T>
  using apply = typename conditional_transform<
    transform_traits<TTransform>::template supports,
    TTransform,
    TFallback
  >::template apply<T>;
};

//////////////////////////
// transform_aggregator //
//////////////////////////

namespace detail {

template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
struct transform_aggregator_impl {
  template <typename... Args>
  class apply {
    template <template <typename...> class TTransform>
    using impl = fatal::apply<TTransform, Args...>;

  public:
    using type = fatal::apply<TAggregator, impl<TTransforms>...>;
  };
};

} // namespace detail {

/**
 * TODO: DOCUMENT AND TEST
 * TODO: review member_transform_stack now that we have this
 * TODO: review recursive_transform now that we have this
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
struct transform_aggregator {
  template <typename... Args>
  using apply = typename detail::transform_aggregator_impl<
    TAggregator, TTransforms...
  >::template apply<Args...>::type;
};

////////////////////////
// variadic_transform //
////////////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TCombiner,
  template <typename...> class... TTransforms
>
//* TODO: CHOOSE AN IMPLEMENTATION
class variadic_transform {
  template <typename... Args>
  struct impl {
    using type = TCombiner<TTransforms<Args>...>;
  };

public:
  template <typename... Args>
  using apply = typename impl<Args...>::type;
};
/*/
class variadic_transform {
  template <typename... Args>
  using apply = TCombiner<TTransforms<Args>...>;
};
//*/

///////////////////////////
// type_member_transform //
///////////////////////////

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
 *  typedef type_member_transform<std::decay> dc;
 *
 *  // yields `int`
 *  typedef dc::apply<int const &> result1;
 *
 *  template <typename T, template <typename...> class MyTransform>
 *  using foo = MyTransform<T>;
 *
 *  // yields `double`
 *  typedef foo<double &&, dc::template apply> result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTransform>
struct type_member_transform {
  template <typename... Args>
  using apply = typename TTransform<Args...>::type;
};

/////////////////////
// transform_alias //
/////////////////////

/**
 * Provides an alias for type transforms, so they can be handled as regular
 * types.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTemplate, typename... Args>
struct transform_alias {
  /**
   * Applies the given arguments `UArgs...`, in addition
   * to the partially applied ones, to the original template.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
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
  using apply = fatal::apply<TTemplate, Args..., UArgs...>;

  /**
   * Partially applies arguments `UArgs...` in addition
   * to those partially applied so far.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *  typedef c1::curry<long, std::string> c2;
   *
   *  // yields `std::tuple<int, double, long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using curry = transform_alias<TTemplate, Args..., UArgs...>;

  /**
   * Replaces all arguments partially applied so far with `UArgs...`.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *  typedef c1::rebind<long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using rebind = transform_alias<TTemplate, UArgs...>;

  /**
   * Discards any arguments partially applied so far.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  typedef transform_alias<c1::template uncurry, long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c1::uncurry<long, std::string> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using uncurry = fatal::apply<TTemplate, UArgs...>;

  /**
   * Re-applies arguments `UArgs...`, in addition to all
   * partially applied arguments, to a separate template.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  // yields `V1<int, double>`
   *  typedef c1::rebind_args<V1>::apply<> result1;
   *
   *  // yields `V1<int, double, bool, float>`
   *  typedef c1::rebind_args<V1, bool, float>::apply<> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class UTemplate, typename... UArgs>
  using rebind_args = transform_alias<UTemplate, Args..., UArgs...>;

  /**
   * Applies the partially applied arguments, in addition
   * to the arguments `UArgs...`, to another template.
   *
   * This acts as a shortcut to `rebind_args<UTemplate, UArgs...>::apply<>`
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
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
  using apply_args = fatal::apply<
    UTemplate, Args..., UArgs...
  >;
};

//////////////////////
// transform_switch //
//////////////////////

template <template <typename...> class, template <typename...> class...>
struct transform_switch;

namespace detail {
namespace transform_switch_impl {

template <template <typename...> class, template <typename...> class...>
struct select;

template <
  bool,
  template <typename...> class,
  template <typename...> class TTransform,
  template <typename...> class...
>
struct branch {
  template <typename... UArgs>
  using apply = fatal::apply<TTransform, UArgs...>;
};

template <
  template <typename...> class TFallback,
  template <typename...> class TTransform,
  template <typename...> class... Args
>
struct branch<false, TFallback, TTransform, Args...> {
  template <typename... UArgs>
  using apply = typename select<TFallback, Args...>::template apply<UArgs...>;
};

template <
  template <typename...> class TFallback,
  template <typename...> class TPredicate,
  template <typename...> class TTransform,
  template <typename...> class... Args
>
struct select<TFallback, TPredicate, TTransform, Args...> {
  template <typename... UArgs>
  using apply = typename branch<
    fatal::apply<TPredicate, UArgs...>::value, TFallback, TTransform, Args...
  >::template apply<UArgs...>;
};

template <template <typename...> class TFallback>
struct select<TFallback> {
  template <typename... UArgs>
  using apply = fatal::apply<TFallback, UArgs...>;
};

template <typename, typename...> struct extend;

template <typename TSwitch>
struct extend<TSwitch> {
  using type = TSwitch;
};

template <typename TSwitch, typename TEntry, typename... Args>
struct extend<TSwitch, TEntry, Args...> {
  using type = typename extend<
    typename TSwitch::template push_back<
      TEntry::template predicate,
      TEntry::template transform
    >,
    Args...
  >::type;
};

template <typename...> struct concat;
template <typename T> struct concat<T> { using type = T; };

template <
  template <typename...> class TLHSFallback,
  template <typename...> class... TLHSEntries,
  template <typename...> class TRHSFallback,
  template <typename...> class... TRHSEntries,
  typename... Args
>
struct concat<
  transform_switch<TLHSFallback, TLHSEntries...>,
  transform_switch<TRHSFallback, TRHSEntries...>,
  Args...
> {
  // TODO: uniquify?
  using type = concat<
    transform_switch<TLHSFallback, TLHSEntries..., TRHSEntries...>,
    Args...
  >;
};

} // namespace transform_switch_impl {
} // namespace detail {

// TODO: DOCUMENT AND TEST
template <
  template <typename...> class TPredicate,
  template <typename...> class TTransform
>
struct transform_switch_entry {
  template <typename... Args>
  using predicate = fatal::apply<TPredicate, Args...>;

  template <typename... Args>
  using transform = fatal::apply<TTransform, Args...>;
};

// TODO: DOCUMENT AND TEST
template <
  template <typename...> class TFallback,
  template <typename...> class... Args
>
struct transform_switch {
  template <typename... UArgs>
  using apply = typename detail::transform_switch_impl::select<
    TFallback, Args...
  >::template apply<UArgs...>;

  template <template <typename...> class... UArgs>
  using push_back = transform_switch<TFallback, Args..., UArgs...>;

  // accepts `transform_switch_entry`
  template <typename... UArgs>
  using extend = typename detail::transform_switch_impl::extend<
    transform_switch, UArgs...
  >::type;

  template <typename... TTransformSwitches>
  using concat = typename detail::transform_switch_impl::concat<
    transform_switch, TTransformSwitches...
  >;
};

///////////////////////////////
// identity_transform_switch //
///////////////////////////////

/**
 * A convenience version of `transform_switch` that uses `identity_transform`
 * as the fallback transform.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... Args>
using identity_transform_switch = transform_switch<identity_transform, Args...>;

//////////////////////
// member_transform //
//////////////////////

// TODO: DOCUMENT AND TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_MEMBER_TRANSFORM(Name, Member) \
  template < \
    template <typename...> class TPreTransform = ::fatal::identity_transform, \
    template <typename...> class TPostTransform = ::fatal::identity_transform \
  > \
  struct Name { \
    template <typename... Args> \
    struct bind { \
      template <typename T, typename... UArgs> \
      using use = TPostTransform< \
        typename TPreTransform<T>::template Member<Args..., UArgs...> \
      >; \
    }; \
    \
    template <typename T, typename... Args> \
    using use = typename bind<>::template use<T, Args...>; \
  }

// TODO: DOCUMENT AND TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct member_transform {
# define FATAL_IMPL_BIND_MEMBER_TRANSFORM(Name) \
    FATAL_MEMBER_TRANSFORM(Name, Name)

  // TODO: POPULATE WITH WELL KNOWN MEMBER TRANSFORMS
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(apply);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(push_back);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(push_front);

# undef FATAL_IMPL_BIND_MEMBER_TRANSFORM
};

////////////////////////
// member_transformer //
////////////////////////

// TODO: DOCUMENT AND PROPERLY TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_MEMBER_TRANSFORMER(Name, Member) \
  template < \
    template <typename...> class TPreTransform = ::fatal::identity_transform, \
    template <typename...> class TPostTransform = ::fatal::identity_transform \
  > \
  struct Name { \
    template < \
      template <typename...> class TTransform = ::fatal::identity_transform, \
      typename... Args \
    > \
    struct bind { \
      template <typename T, typename... UArgs> \
      using use = TPostTransform< \
        typename TPreTransform<T>::template Member< \
          TTransform, Args..., UArgs... \
        > \
      >; \
    }; \
    \
    template < \
      typename T, \
      template <typename...> class TTransform, \
      typename... Args \
    > \
    using use = typename bind<TTransform, Args...>::template use<T>; \
  }

// TODO: DOCUMENT AND PROPERLY TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct member_transformer {
# define FATAL_IMPL_BIND_MEMBER_TRANSFORMER(Name) \
    FATAL_MEMBER_TRANSFORMER(Name, Name)

  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(apply);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(combine);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(conditional);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(filter);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(merge);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(reject);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(search);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(separate);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(sort);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(transform);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(unique);

# undef FATAL_IMPL_BIND_MEMBER_TRANSFORMER
};

//////////////////////////////
// member_transformer_stack //
//////////////////////////////

// TODO: DOCUMENT AND TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename, template <typename...> class, typename...> class TMember
>
struct member_transformer_stack {
  template <template <typename...> class... TPreTransforms>
  struct pre {
    template <template <typename...> class... TPostTransforms>
    struct post {
      template <
        typename T,
        template <typename...> class TTransform = identity_transform,
        typename... UArgs
      >
      using use = typename transform_sequence<
        TPostTransforms...
      >::template apply<
        TMember<
          typename transform_sequence<TPreTransforms...>::template apply<T>,
          TTransform,
          UArgs...
        >
      >;
    };

    template <
      typename T,
      template <typename...> class TTransform = identity_transform,
      typename... UArgs
    >
    using use = typename post<>::template use<T, TTransform, UArgs...>;
  };

  template <template <typename...> class... TPostTransforms>
  struct post {
    template <
      typename T,
      template <typename...> class TTransform = identity_transform,
      typename... UArgs
    >
    using use = typename transform_sequence<TPostTransforms...>::template apply<
      TMember<T, TTransform, UArgs...>
    >;
  };

  template <
    typename T,
    template <typename...> class TTransform = identity_transform,
    typename... UArgs
  >
  using use = typename pre<>::template post<>::template use<
    T, TTransform, UArgs...
  >;
};

/////////////////////////
// recursive_transform //
/////////////////////////

namespace detail {
namespace recursive_transform_impl {

template <
  bool, std::size_t,
  template <typename...> class, template <typename...> class,
  template <typename...> class,
  template <typename, template <typename...> class, typename...> class,
  template <typename...> class
>
struct recurse;

template <
  bool, std::size_t, typename,
  template <typename...> class, template <typename...> class,
  template <typename...> class,
  template <typename, template <typename...> class, typename...> class,
  template <typename...> class
>
struct branch;

template <
  std::size_t Depth,
  typename T,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct branch<
  true, Depth, T, TPre, TPost, TPredicate, TTransformer, TTransform
> {
  using type = TPost<
    TTransformer<
      TPre<T>,
      recurse<true, Depth, TPre, TPost, TPredicate, TTransformer, TTransform>
        ::template apply
    >
  >;
};

template <
  std::size_t Depth,
  typename T,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct branch<
  false, Depth, T, TPre, TPost, TPredicate, TTransformer, TTransform
> {
  using type = TTransform<T>;
};

template <
  std::size_t Depth,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct recurse<true, Depth, TPre, TPost, TPredicate, TTransformer, TTransform> {
  static_assert(Depth > 0, "invalid recursion depth");

  template <typename T>
  using apply = typename branch<
    fatal::apply<TPredicate, T>::value, Depth - 1, T,
    TPre, TPost, TPredicate, TTransformer, TTransform
  >::type;
};

template <
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct recurse<true, 0, TPre, TPost, TPredicate, TTransformer, TTransform> {
  template <typename T>
  using apply = typename conditional_transform<
    TPredicate,
    transform_sequence<TPre, TPost>::template apply,
    TTransform
  >::template apply<T>;
};

} // namespace recursive_transform_impl {
} // namespace detail {

// TODO: DOCUMENT & PROPERLY TEST
// TODO: REFINE THE INTERFACE

/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer = member_transformer::transform<>::use,
  template <typename...> class... TTransforms
>
struct recursive_transform {
  template <
    template <typename...> class TPreTransform = identity_transform,
    template <typename...> class TPostTransform = identity_transform,
    std::size_t Depth = std::numeric_limits<std::size_t>::max()
  >
  // TODO: split into pre, post and depth
  using with = detail::recursive_transform_impl::recurse<
    true,
    Depth,
    TPreTransform,
    TPostTransform,
    TPredicate,
    TTransformer,
    transform_sequence<TTransforms...>::template apply
  >;

  template <typename T>
  using apply = typename with<>::template apply<T>;
};

/////////////////////////
// recursive_type_sort //
/////////////////////////

// TODO: write in terms of `recursive_transform`

template <typename T, std::size_t Depth>
struct recursive_type_sort_impl { typedef T type; };

template <std::size_t Depth = std::numeric_limits<std::size_t>::max()>
struct recursive_type_sort {
  template <typename T>
  using apply = typename recursive_type_sort_impl<T, Depth>::type;
};

template <typename T>
using full_recursive_type_sort = typename recursive_type_sort<>
  ::template apply<T>;

/////////////////////
// type_get_traits //
/////////////////////

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

//////////////
// type_get //
//////////////

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
 *  typedef third = type_get<2>::from<list>
 *
 *  // yields `int`
 *  typedef third = type_get<0>::from<list>
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index>
struct type_get {
  template <typename TDataStructure>
  using from = typename type_get_traits<TDataStructure, Index>::type;
};

/**
 * A convenience shortcut for type_get<0>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_first = type_get<0>::template from<T>;

/**
 * A convenience shortcut for type_get<1>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_second = type_get<1>::template from<T>;

/**
 * A convenience shortcut for type_get<2>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_third = type_get<2>::template from<T>;

/**
 * A convenience shortcut for type_get<3>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_fourth = type_get<3>::template from<T>;

/**
 * A convenience shortcut for type_get<4>.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using type_get_fifth = type_get<4>::template from<T>;

/**
 * A convenience template that binds to a type comparer.
 * It compares the result of `type_get_first` when applied to the operands.
 *
 * Example:
 *
 *  template <int LHS, int RHS>
 *  class Foo {
 *    using lhs = std::pair<std::integral_constant<int, LHS>, void>;
 *    using rhs = std::pair<std::integral_constant<int, RHS>, double>;
 *
 *  public:
 *    template <template <typename...> class TComparer>
 *    using comparison = std::integral_constant<
 *      bool, TComparer<lhs, rhs>::value
 *    >;
 *  };
 *
 *  // yields `std::integral_constant<bool, true>`
 *  using result1 = Foo<5, 8>::comparison<type_get_first_comparer<>>;
 *
 *  // yields `std::integral_constant<bool, false>`
 *  using result2 = Foo<5, 8>::comparison<
 *    type_get_first_comparer<comparison_transform::greater_than>
 *  >;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TComparer = comparison_transform::less_than
>
struct type_get_first_comparer {
  template <typename TLHS, typename TRHS>
  using compare = is_true_transform<
    TComparer<type_get_first<TLHS>, type_get_first<TRHS>>
  >;
};

/**
 * A convenience template that binds to a type comparer.
 * It compares the result of `type_get_second` when applied to the operands.
 *
 * Example:
 *
 *  template <int LHS, int RHS>
 *  class Foo {
 *    using lhs = std::pair<void, std::integral_constant<int, LHS>>;
 *    using rhs = std::pair<double, std::integral_constant<int, RHS>>;
 *
 *  public:
 *    template <template <typename...> class TComparer>
 *    using comparison = std::integral_constant<
 *      bool, TComparer<lhs, rhs>::value
 *    >;
 *  };
 *
 *  // yields `std::integral_constant<bool, true>`
 *  using result1 = Foo<5, 8>::comparison<type_get_second_comparer<>>;
 *
 *  // yields `std::integral_constant<bool, false>`
 *  using result2 = Foo<5, 8>::comparison<
 *    type_get_second_comparer<comparison_transform::greater_than>
 *  >;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TComparer = comparison_transform::less_than
>
struct type_get_second_comparer {
  template <typename TLHS, typename TRHS>
  using compare = is_true_transform<
    TComparer<type_get_second<TLHS>, type_get_second<TRHS>>
  >;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

///////////
// apply //
///////////

namespace detail {

template <template <typename> class T, typename U>
struct apply_impl<T, U> { using type = T<U>; };

template <template <typename, typename> class T, typename U0, typename U1>
struct apply_impl<T, U0, U1> { using type = T<U0, U1>; };

template <
  template <typename, typename, typename> class T,
  typename U0, typename U1, typename U2
>
struct apply_impl<T, U0, U1, U2> { using type = T<U0, U1, U2>; };

template <
  template <typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3
>
struct apply_impl<T, U0, U1, U2, U3> { using type = T<U0, U1, U2, U3>; };

template <
  template <typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4
>
struct apply_impl<T, U0, U1, U2, U3, U4> {
  using type = T<U0, U1, U2, U3, U4>;
};

template <
  template <typename, typename, typename, typename, typename, typename> class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5> {
  using type = T<U0, U1, U2, U3, U4, U5>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0> {
  using type = T<U0, U1, U2, U3, U4, U5, V0>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0, V1> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0, V1, V2> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4>;
};

template <
  template <
    typename, typename, typename, typename, typename, typename,
    typename, typename, typename, typename, typename, typename
  > class T,
  typename U0, typename U1, typename U2, typename U3, typename U4, typename U5,
  typename V0, typename V1, typename V2, typename V3, typename V4, typename V5
>
struct apply_impl<T, U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5> {
  using type = T<U0, U1, U2, U3, U4, U5, V0, V1, V2, V3, V4, V5>;
};

} // namespace detail {

/////////////////////
// type_get_traits //
/////////////////////

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

#endif // FATAL_INCLUDE_fatal_type_transform_h
