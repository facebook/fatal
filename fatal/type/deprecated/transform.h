/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/compilability.h>
#include <fatal/type/conditional.h>
#include <fatal/type/deprecated/apply.h>
#include <fatal/type/deprecated/type_tag.h>
#include <fatal/type/identity.h>
#include <fatal/type/is_complete.h>
#include <fatal/type/maybe.h>

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {

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
 * TODO: DOCUMENT AND TEST. ALSO TEST bool SPECIAL CASE
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TTo>
struct caster {
  template <typename T>
  using apply = std::integral_constant<TTo, static_cast<TTo>(T::value)>;
};

/**
 * A convenience specialization of `caster` for `bool`.
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_true_transform = typename caster<bool>::template apply<T>;

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

/////////////
// compose //
/////////////

/**
 * A convenience adapter that allows nesting multiple transforms into one.
 *
 * The transforms are applied in the order specified.
 *
 * If no transforms are given, it acts like `identity`.
 *
 * Example:
 *
 *  template <typename> struct T1 {};
 *  template <typename> struct T2 {};
 *  template <typename> struct T3 {};
 *
 *  using tr = compose<T1, T2, T3>;
 *
 *  // yields `T3<T2<T1<int>>>`
 *  using result = tr::apply<int>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class...> struct compose;

template <
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
struct compose<TTransform, TTransforms...> {
  template <typename... Args>
  using apply = typename compose<TTransforms...>::template apply<
    typename fatal::apply<TTransform, Args...>
  >;
};

template <>
struct compose<> {
  template <typename T, typename...>
  using apply = T;
};

////////////////
// arithmetic //
////////////////

namespace detail {
namespace transform_impl {
namespace arithmetic {
template <typename...> struct add;
template <typename...> struct subtract;
template <typename...> struct multiply;
template <typename...> struct divide;
template <typename...> struct modulo;
} // namespace arithmetic {
} // namespace transform_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct arithmetic {
  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using add = typename detail::transform_impl::arithmetic::add<Args...>::type;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using subtract = typename detail::transform_impl::arithmetic::subtract<
    Args...
  >::type;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using multiply = typename detail::transform_impl::arithmetic::multiply<
    Args...
  >::type;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using divide = typename detail::transform_impl::arithmetic::divide<
    Args...
  >::type;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using modulo = typename detail::transform_impl::arithmetic::modulo<
    Args...
  >::type;
};

/////////////
// logical //
/////////////

namespace detail {
namespace transform_impl {
namespace logical {
template <typename...> struct all;
template <typename...> struct any;
} // namespace logical {
} // namespace transform_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct logical {
  /**
   * Helper class similar to std::integral_constant whose value is the logical
   * AND of the value of each `Arg`.
   *
   * Example:
   *
   *  template <typename A, typename B, typename C>
   *  using all_equal = logical::all<
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
   *      logical::all<std::is_signed<Args>...)::value,
   *      "Args should be signed arithmetic types"
   *    );
   *  }
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using all = typename detail::transform_impl::logical::all<Args...>::type;

  /**
   * Helper class similar to std::integral_constant whose value is the logical
   * OR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <typename A, typename B, typename C>
   *  using has_duplicate = logical::any<
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
  template <typename... Args>
  using any = typename detail::transform_impl::logical::any<Args...>::type;

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
   *  using is_non_negative = logical::negate<is_negative<X>>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using negate = std::integral_constant<
    typename std::decay<decltype(!T::value)>::type,
    !T::value
  >;

  // TODO: DOCUMENT AND TEST
  template <typename... Args>
  using not_all = negate<all<Args...>>;

  // TODO: DOCUMENT AND TEST
  template <typename... Args>
  using none = negate<any<Args...>>;
};

/////////////
// bitwise //
/////////////

namespace detail {
namespace transform_impl {
namespace bitwise {
template <typename...> struct all;
template <typename...> struct any;
template <typename...> struct diff;
} // namespace bitwise {
} // namespace transform_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise {
  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * AND of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using all = bitwise::all<
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
  template <typename... Args>
  using all = typename detail::transform_impl::bitwise::all<Args...>::type;

  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * OR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using any = bitwise::any<
   *    std::integral_constant<int, Args>...
   *  >;
   *
   *  // yields `7`
   *  any<1, 2, 4>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using any = typename detail::transform_impl::bitwise::any<Args...>::type;

  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * XOR of the value of each `Arg`.
   *
   * Example:
   *
   *  template <int... Args>
   *  using diff = bitwise::diff<
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
  template <typename... Args>
  using diff = typename detail::transform_impl::bitwise::diff<Args...>::type;

private:
  template<typename T>
  using comptype = typename std::conditional<
    std::is_integral<typename std::decay<decltype(T::value)>::type>::value,
    typename std::decay<decltype(T::value)>::type,
    typename std::decay<decltype(~T::value)>::type
  >::type;

public:
  /**
   * Helper class similar to std::integral_constant whose value is the bitwise
   * complement of T's value.
   *
   * Example:
   *
   *  // yields `0xf0`
   *  bitwise::complement<
   *    std::integral_constant<std::uint8_t, 0xf>
   *  >::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using complement = std::integral_constant<
    comptype<T>,
    static_cast<comptype<T>>(~T::value)
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
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value == TRHS::value)>::type,
    (TLHS::value == TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the NOT EQUAL TO
   * comparison, figuratively `TLHS` != `TRHS`.
   *
   * Example:
   *
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value != TRHS::value)>::type,
    (TLHS::value != TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the LESS THAN
   * comparison, figuratively `TLHS` < `TRHS`.
   *
   * Example:
   *
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value < TRHS::value)>::type,
    (TLHS::value < TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the LESS THAN or
   * EQUAL TO comparison, figuratively `TLHS` <= `TRHS`.
   *
   * Example:
   *
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value <= TRHS::value)>::type,
    (TLHS::value <= TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the GREATER THAN
   * comparison, figuratively `TLHS` > `TRHS`.
   *
   * Example:
   *
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value > TRHS::value)>::type,
    (TLHS::value > TRHS::value)
  >;

  /**
   * Helper class similar to std::integral_constant for comparing two types
   * `TLHS` and `TRHS`. Its boolean value is the result of the GREATER THAN or
   * EQUAL TO comparison, figuratively `TLHS` >= `TRHS`.
   *
   * Example:
   *
   *  using A = std::integral_constant<int, 10>;
   *  using B = std::integral_constant<int, 20>;
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
    typename std::decay<decltype(TLHS::value >= TRHS::value)>::type,
    (TLHS::value >= TRHS::value)
  >;
};

/**
 * A convenient macro that creates a transform which
 * evaluates to the given member type.
 *
 * Provides a template alias named `Name` which extracts
 * the member type with the same name.
 *
 * Example:
 *
 *  FATAL_GET_MEMBER_TYPE_AS(get_type, type);
 *
 *  // evaluates to `int const`
 *  using result = get_type<std::add_const<int>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
# define FATAL_GET_MEMBER_TYPE_AS(Name, ...) \
  template <typename T> using Name = typename T::__VA_ARGS__

/**
 * A convenient macro that creates a transform which
 * evaluates to the given member type.
 *
 * Provides a template alias named `Name` which extracts
 * the member type with the same name.
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
  FATAL_GET_MEMBER_TYPE_AS(Name, Name)

/**
 * Provides transforms that evaluate to a member type of a given type.
 *
 * Example:
 *
 *  // yields `int`
 *  using result1 = get_member_type::type<std::add_const<int>>;
 *
 *  using map = std::map<double, std::string>;
 *
 *  // yields `std::pair<double, std::string>`
 *  using result2 = get_member_type::value_type<map>;
 *
 *  // yields `double`
 *  using result3 = get_member_type::key_type<map>;
 *
 *  // yields `std::string`
 *  using result4 = get_member_type::mapped_type<map>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: OFFER THIS LIST AS AN INTERNAL HIGHER ORDER MACRO
struct get_member_type {
# define FATAL_IMPL_GET_MEMBER_TYPE(Name) \
  FATAL_GET_MEMBER_TYPE(Name)

  FATAL_IMPL_GET_MEMBER_TYPE(char_type);
  FATAL_IMPL_GET_MEMBER_TYPE(return_type);

# define FATAL_IMPL_GET_MEMBER_TYPE_TYPE(Name) \
  FATAL_IMPL_GET_MEMBER_TYPE(Name); \
  FATAL_IMPL_GET_MEMBER_TYPE(Name##_type)

  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(allocator);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(args);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(array);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(category);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(client);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(clients);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(config);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_pointer);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_ptr);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_ref);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_reference);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(const_reverse_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(constant);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(constants);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(data);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(decode);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(decoder);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(difference);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(element);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(encode);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(encoder);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(enums);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(extension);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(first);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(flag);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(get);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(getter);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(getters);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(hash);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(id);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(ids);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(index);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(info);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(information);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(instance);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(int_type);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(item);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(iterator);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(key);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_geter);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_id);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_name);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_property);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_setter);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(legacy_value);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(list);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(map);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(mapped);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(mapping);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(mappings);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(member);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(members);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(metadata);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(name);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(names);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(pair);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(pointer);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(predicate);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(ptr);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(reader);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(ref);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(reference);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(request);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(response);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(result);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(reverse);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(reverse_iterator);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(second);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(service);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(services);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(set);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(setter);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(setters);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(size);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(str);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(string);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(structs);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(tag);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(traits);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(tuple);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(type);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(type_class);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(types);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(unions);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(value);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(values);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(version);
  FATAL_IMPL_GET_MEMBER_TYPE_TYPE(writer);

# undef FATAL_IMPL_GET_MEMBER_TYPE_TYPE
# undef FATAL_IMPL_GET_MEMBER_TYPE
};

///////////////////////////
// conditional_transform //
///////////////////////////

namespace detail {
namespace transform_impl {

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

} // namespace transform_impl {
} // namespace detail {

/**
 * Applies either `TWhenTrueTransform` or `TWhenFalseTransform` to the type `T`
 * depending whether the predicate `TPredicate` returns, respectively, true or
 * false, when applied to to the type `T`.
 *
 * The default transform for `TWhenFalseTransform` is `identity`.
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
  template <typename...> class TWhenFalseTransform = identity
>
struct conditional_transform {
  template <typename T>
  using apply = typename detail::transform_impl::conditional_transform_impl<
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
class transform_traits {
  struct impl {
    template <typename T, typename = TTransform<T>>
    static std::true_type sfinae(T *);

    template <typename...>
    static std::false_type sfinae(...);
  };

public:
  // TODO: DOCUMENT
  template <typename T>
  using supports = decltype(impl::sfinae(static_cast<T *>(nullptr)));
};

///////////////////
// try_transform //
///////////////////

// TODO: DOCUMENT AND TEST
template <
  template <typename...> class TTransform,
  template <typename...> class TFallback = identity
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
namespace transform_impl {

template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
struct transform_aggregator {
  template <typename... Args>
  class apply {
    template <template <typename...> class TTransform>
    using impl = fatal::apply<TTransform, Args...>;

  public:
    using type = fatal::apply<TAggregator, impl<TTransforms>...>;
  };
};

} // namespace transform_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT AND TEST
 * TODO: review member_transform_stack now that we have this
 * TODO: review recursive_transform now that we have this
 *
 *  TAggregator<
 *    TTransforms_0<Args...>,
 *    TTransforms_1<Args...>,
 *    ...
 *    TTransforms_n<Args...>
 *  >
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
struct transform_aggregator {
  template <typename... Args>
  using apply = typename detail::transform_impl::transform_aggregator<
    TAggregator, TTransforms...
  >::template apply<Args...>::type;
};

///////////////////////////
// transform_distributor //
///////////////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 *  TAggregator<
 *    compose<TTransforms...>::apply<Arg_0>,
 *    compose<TTransforms...>::apply<Arg_1>,
 *    ...
 *    compose<TTransforms...>::apply<Arg_n>
 *  >
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
class transform_distributor {
  template <typename T>
  using impl = typename compose<TTransforms...>::template apply<T>;

public:
  template <typename... Args>
  using apply = fatal::apply<TAggregator, impl<Args>...>;
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
 * An adapter that allows using a template class' member type `type`
 * as a transform.
 *
 * This is useful for using C++'s type modifiers (decay, make_signed, ...)
 * as transforms on a pre-C++14 compiler.
 *
 * `TTransform` is any class that receives a given type `T` as a template
 * parameter and provides a member type `type`, which is to be used as
 * the transform result.
 *
 * Example:
 *
 *  using dc = type_member_transform<std::decay>;
 *
 *  // yields `int`
 *  using result1 = dc::apply<int const &>;
 *
 *  template <typename T, template <typename...> class MyTransform>
 *  using foo = MyTransform<T>;
 *
 *  // yields `double`
 *  using result2 = foo<double &&, dc::template apply>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *
   *  // yields `std::tuple<int, double>`
   *  using result1 = c1::apply<>;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  using result2 = c1::apply<long, std::string, bool, float>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *  using c2 = c1::curry<long, std::string>;
   *
   *  // yields `std::tuple<int, double, long, std::string>`
   *  using result1 = c2::apply<>;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  using result2 = c2::apply<bool, float>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *  using c2 = c1::rebind<long, std::string>;
   *
   *  // yields `std::tuple<long, std::string>`
   *  using result1 = c2::apply<>;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  using result2 = c2::apply<bool, float>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *
   *  using c2 = transform_alias<c1::template uncurry, long, std::string>;
   *
   *  // yields `std::tuple<long, std::string>`
   *  using result1 = c2::apply<>;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  using result2 = c2::apply<bool, float>;
   *
   *  // yields `std::tuple<long, std::string>`
   *  using result3 = c1::uncurry<long, std::string>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *
   *  // yields `V1<int, double>`
   *  using result1 = c1::rebind_args<V1>::apply<>;
   *
   *  // yields `V1<int, double, bool, float>`
   *  using result2 = c1::rebind_args<V1, bool, float>::apply<>;
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
   *  using c1 = transform_alias<std::tuple, int, double>;
   *
   *  // yields `V1<int, double>`
   *  using result1 = c1::apply_args<V1>;
   *
   *  // yields `V1<int, double, bool, float>`
   *  using result2 = c1::apply_args<V1, bool, float>;
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

/////////////////////
// identity_switch //
/////////////////////

/**
 * A convenience version of `transform_switch` that uses `identity`
 * as the fallback transform.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... Args>
using identity_switch = transform_switch<identity, Args...>;

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
    template <typename...> class TPreTransform = ::fatal::identity, \
    template <typename...> class TPostTransform = ::fatal::identity \
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
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(append);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(apply);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(invert);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(push_back);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(push_front);
  FATAL_IMPL_BIND_MEMBER_TRANSFORM(type);

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
    template <typename...> class TPreTransform = ::fatal::identity, \
    template <typename...> class TPostTransform = ::fatal::identity \
  > \
  struct Name { \
    template < \
      template <typename...> class TTransform = ::fatal::identity, \
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
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(invert);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(merge);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(reject);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(search);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(separate);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(sort);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(transform);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(type);
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
        template <typename...> class TTransform = identity,
        typename... UArgs
      >
      using use = typename compose<
        TPostTransforms...
      >::template apply<
        TMember<
          typename compose<TPreTransforms...>::template apply<T>,
          TTransform,
          UArgs...
        >
      >;
    };

    template <
      typename T,
      template <typename...> class TTransform = identity,
      typename... UArgs
    >
    using use = typename post<>::template use<T, TTransform, UArgs...>;
  };

  template <template <typename...> class... TPostTransforms>
  struct post {
    template <
      typename T,
      template <typename...> class TTransform = identity,
      typename... UArgs
    >
    using use = typename compose<TPostTransforms...>::template apply<
      TMember<T, TTransform, UArgs...>
    >;
  };

  template <
    typename T,
    template <typename...> class TTransform = identity,
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
    compose<TPre, TPost>::template apply,
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
    template <typename...> class TPreTransform = identity,
    template <typename...> class TPostTransform = identity,
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
    compose<TTransforms...>::template apply
  >;

  template <typename T>
  using apply = typename with<>::template apply<T>;
};

/////////////////////////
// recursive_type_sort //
/////////////////////////

// TODO: write in terms of `recursive_transform`

template <typename T, std::size_t Depth>
struct recursive_type_sort_impl { using type = T; };

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
 * supported by `type_get`. It must provide a member type `type` with
 * the `Index`-th type of the `TDataStructure` data structure.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename>
struct type_get_traits;

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
 *  using third = type_get<2>::from<list>;
 *
 *  // yields `int`
 *  using first = type_get<0>::from<list>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <
  typename T,
  std::size_t Index,
  bool = type_get_traits<T>::template supported<Index>::value
>
struct type_get_impl;

template <typename T, std::size_t Index>
struct type_get_impl<T, Index, true> {
  using type = typename type_get_traits<T>::template type<Index>;
};

} // namespace detail {

template <std::size_t Index>
struct type_get {
  template <typename T>
  using from = typename detail::type_get_impl<T, Index>::type;
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

/////////////////////
// type_get_traits //
/////////////////////

/**
 * Specialization of `type_get_traits` so that `type_get` supports `std::pair`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TFirst, typename TSecond>
struct type_get_traits<std::pair<TFirst, TSecond>> {
  template <std::size_t Index>
  using supported = std::integral_constant<bool, (Index < 2)>;

  template <std::size_t Index>
  using type = typename std::tuple_element<
    Index,
    std::pair<TFirst, TSecond>
  >::type;
};

/**
 * Specialization of `type_get_traits` so that `type_get` supports `std::tuple`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct type_get_traits<std::tuple<Args...>> {
  template <std::size_t Index>
  using supported = std::integral_constant<bool, (Index < sizeof...(Args))>;

  template <std::size_t Index>
  using type = typename std::tuple_element<
    Index,
    std::tuple<Args...>
  >::type;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace transform_impl {
namespace arithmetic {

/////////
// add //
/////////

template <typename T, typename... Args>
struct add<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value + add<Args...>::type::value)>::type,
    (T::value + add<Args...>::type::value)
  >;
};

template <typename T>
struct add<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

//////////////
// subtract //
//////////////

template <typename TLHS, typename TRHS, typename... Args>
struct subtract<TLHS, TRHS, Args...> {
  using type = typename subtract<
    typename subtract<TLHS, TRHS>::type,
    Args...
  >::type;
};

template <typename TLHS, typename TRHS>
struct subtract<TLHS, TRHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(TLHS::value - TRHS::value)>::type,
    (TLHS::value - TRHS::value)
  >;
};

//////////////
// multiply //
//////////////

template <typename T, typename... Args>
struct multiply<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<
      decltype(T::value * multiply<Args...>::type::value)
    >::type,
    (T::value * multiply<Args...>::type::value)
  >;
};

template <typename T>
struct multiply<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

////////////
// divide //
////////////

template <typename TLHS, typename TRHS, typename... Args>
struct divide<TLHS, TRHS, Args...> {
  using type = typename divide<
    typename divide<TLHS, TRHS>::type,
    Args...
  >::type;
};

template <typename TLHS, typename TRHS>
struct divide<TLHS, TRHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(TLHS::value / TRHS::value)>::type,
    (TLHS::value / TRHS::value)
  >;
};

////////////
// modulo //
////////////

template <typename TLHS, typename TRHS, typename... Args>
struct modulo<TLHS, TRHS, Args...> {
  using type = typename modulo<
    typename modulo<TLHS, TRHS>::type,
    Args...
  >::type;
};

template <typename TLHS, typename TRHS>
struct modulo<TLHS, TRHS> {
  using type = std::integral_constant<
    typename std::decay<decltype(TLHS::value % TRHS::value)>::type,
    (TLHS::value % TRHS::value)
  >;
};

} // namespace arithmetic {

namespace logical {

/////////
// all //
/////////

template <typename T, typename... Args>
struct all<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value && all<Args...>::type::value)>::type,
    (T::value && all<Args...>::type::value)
  >;
};

template <>
struct all<> {
  using type = std::true_type;
};

/////////
// any //
/////////

template <typename T, typename... Args>
struct any<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value || any<Args...>::type::value)>::type,
    (T::value || any<Args...>::type::value)
  >;
};

template <>
struct any<> {
  using type = std::false_type;
};

} // logical {

namespace bitwise {

/////////
// all //
/////////

template <typename T, typename... Args>
struct all<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value & all<Args...>::type::value)>::type,
    (T::value & all<Args...>::type::value)
  >;
};

template <typename T>
struct all<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

/////////
// any //
/////////

template <typename T, typename... Args>
struct any<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value | any<Args...>::type::value)>::type,
    (T::value | any<Args...>::type::value)
  >;
};

template <typename T>
struct any<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

//////////
// diff //
//////////

template <typename T, typename... Args>
struct diff<T, Args...> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value ^ diff<Args...>::type::value)>::type,
    (T::value ^ diff<Args...>::type::value)
  >;
};

template <typename T>
struct diff<T> {
  using type = std::integral_constant<
    typename std::decay<decltype(T::value)>::type,
    T::value
  >;
};

} // namespace bitwise {
} // namespace transform_impl {
} // namespace detail {
} // namespace fatal {
