/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_deprecated_list_h
#define FATAL_INCLUDE_fatal_type_deprecated_list_h

#include <fatal/portability.h>
#include <fatal/type/conditional.h>
#include <fatal/type/deprecated/flatten.h>
#include <fatal/type/deprecated/transform.h>
#include <fatal/type/deprecated/type_pair.h>
#include <fatal/type/deprecated/type_tag.h>

#include <limits>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace fatal {

/**
 * READ ME FIRST: You probably want to jump to
 * the line that says `type_list IMPLEMENTATION`
 */

///////////////////////////
// type_list DECLARATION //
///////////////////////////

template <typename...> struct type_list;

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

//////////////
// type_get //
//////////////

/**
 * Specialization of `type_get_traits` so that `type_get` supports `type_list`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct type_get_traits<type_list<Args...>> {
  template <std::size_t Index>
  using supported = std::integral_constant<bool, (Index < sizeof...(Args))>;

  template <std::size_t Index>
  using type = typename type_list<Args...>::template at<Index>;
};


////////////////////
// type_list_from //
////////////////////

/**
 * A convenience `type_list` builder based on transforms.
 * See members below for more info.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... Transforms>
struct type_list_from {
  /**
   * Creates a `type_list` from a given type `T` by applying a series of
   * transforms `Transforms`.
   *
   * The resulting list will be comprised of the result of the transforms when
   * applied to `T`, in the order they are given.
   *
   * Example:
   *
   *  template <typename TFirst, typename TSecond, typename TThird>
   *  struct Foo {
   *    using first = TFirst;
   *    using second = TThird;
   *    using third = TThird;
   *  };
   *
   *  using foo = Foo<int, void, double>;
   *
   *  template <typename T> using get_first = typename T::first;
   *  template <typename T> using get_second = typename T::second;
   *  template <typename T> using get_third = typename T::third;
   *
   *  // yields `type_list<int, void, double>`
   *  using result = type_list_from<get_first, get_second, get_third>
   *    ::type<foo>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using type = type_list<Transforms<T>...>;
};

/**
 * A convenience comparer implementation for `binary_search` that compares a
 * variable against an `std::integral_constant`-like type.
 *
 * Returns `-1`, `0` or `1` when `lhs` is, respectively, less than, equal to or
 * greater than `RHS`.
 *
 * Example:
 *
 *  // `Index` is not important for this example, using `0`.
 *  template <int N>
 *  using rhs = indexed_type_tag<std::integral_constant<int, N>, 0>;
 *
 *  // yields `-1`
 *  type_value_comparer::compare(5, rhs<7>{});
 *
 *  // yields `0`
 *  type_value_comparer::compare(5, rhs<5>{});
 *
 *  // yields `1`
 *  type_value_comparer::compare(5, rhs<3>{});
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: MOVE TO ANOTHER HEADER
struct type_value_comparer {
  template <typename LHS, typename RHS, std::size_t Index>
  static constexpr int compare(LHS &&lhs, indexed_type_tag<RHS, Index>) {
    return lhs < RHS::value
      ? -1
      : RHS::value < lhs
        ? 1
        : 0;
  }
};

/**
 * A convenience comparer implementation for `binary_search` that compares a
 * variable against the `indexed_type_tag`'s `Index`.
 *
 * Returns `-1`, `0` or `1` when `lhs` is, respectively, less than, equal to or
 * greater than `RHS`.
 *
 * Example:
 *
 *  // `T` is not important for this example, using `void`.
 *  template <std::size_t Index>
 *  using rhs = indexed_type_tag<void, Index>;
 *
 *  // yields `-1`
 *  type_value_comparer::compare(5, rhs<7>{});
 *
 *  // yields `0`
 *  type_value_comparer::compare(5, rhs<5>{});
 *
 *  // yields `1`
 *  type_value_comparer::compare(5, rhs<3>{});
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: MOVE TO ANOTHER HEADER
struct index_value_comparer {
  template <typename LHS, typename RHS, std::size_t Index>
  static constexpr int compare(LHS &&lhs, indexed_type_tag<RHS, Index>) {
    return lhs < Index
      ? -1
      : Index < lhs
        ? 1
        : 0;
  }
};

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace detail {
namespace type_list_impl {

template <std::size_t, typename...> struct at;
template <bool, std::size_t, typename, typename...> struct try_at;
template <std::size_t, typename, typename...> struct index_of;
template <std::size_t, typename, typename...> struct checked_index_of;
template <typename...> struct type_at;
template <typename, typename...> struct contains;
template <template <typename...> class, std::size_t, typename...>
struct foreach_if;
template <std::size_t, typename...> struct foreach;
template <std::size_t, typename...> struct visit;
template <std::size_t, typename...> struct indexed_transform;
template <bool, template <typename...> class, typename, typename, typename...>
struct cumulative_transform;
template <template <typename...> class, typename, typename...>
struct accumulate;
template <
  template <typename...> class, template <typename...> class, typename...
> struct choose;
template <typename, typename> struct replace_transform;
template <std::size_t, typename...> struct left;
template <std::size_t, std::size_t, typename...> struct slice;
template <template <typename...> class, typename...> struct separate;
template <typename, typename> struct zip;
template <std::size_t, std::size_t, typename...> struct skip;
template <std::size_t> struct curried_skip;
template <template <typename...> class, typename, typename...> struct search;
template <std::size_t, std::size_t, typename...> struct deep_flatten;
template <typename...> struct interleave;
template <typename...> struct concat;
template <template <typename...> class, typename, typename...>
struct insert_sorted;
template <std::size_t, typename...> struct multiply;
template <std::size_t, typename...> struct tail;
template <std::size_t, typename...> struct split;
template <template <typename...> class, typename...> struct is_sorted;
template <template <typename...> class, typename, typename...>
struct merge_entry_point;
template <template <typename...> class, typename> struct sort;
template <typename, typename...> struct unique;
template <typename, typename, typename...> struct binary_search_prepare;
template <typename, typename...> struct binary_search_exact;
template <typename...> struct binary_search_lower_bound;
template <typename...> struct binary_search_upper_bound;

} // namespace type_list_impl {
} // namespace detail {

//////////////////////////////
// type_list IMPLEMENTATION //
//////////////////////////////

/**
 * Type list for template metaprogramming.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: operations that would
 * mutate the list upon which they are performed actually create a new list.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct type_list {
  /**
   * The size of this list.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr std::size_t size = sizeof...(Args);

  /**
   * A boolean telling whether this list is empty or not.
   * This is the same as `type_list::size == 0`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr bool empty = sizeof...(Args) == 0;

  /**
   * Returns the type at the given index.
   * Yields a compilation error when not found.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index>
  using at = typename std::enable_if<
    !empty,
    typename detail::type_list_impl::at<Index, Args...>::type
  >::type;

  /**
   * Returns a the type at the given index.
   *
   * If there is no element at that index, returns `Default`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   *
   * TODO: TEST Default
   */
  template <std::size_t Index, typename Default>
  using try_at = typename detail::type_list_impl::try_at<
    (Index < size), Index, Default, Args...
  >::type;

  /**
   * Returns a std::integral_const with the 0-based index of the given type
   * in this list, or type_list::size if not found.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using index_of = typename detail::type_list_impl::index_of<
    0, T, Args...
  >::type;

  /**
   * Returns a std::integral_const with the 0-based index of the given type
   * in this list.
   *
   * A type not found is considered an error and will fail to compile.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using checked_index_of = typename detail::type_list_impl::checked_index_of<
    0, T, Args...
  >::type;

  /**
   * RTTI for the type at the given index.
   *
   * No bounds checking performed, returns the last
   * type of the type list when index >= size.
   *
   * Note: this is a runtime facility.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr std::type_info const &type_at(std::size_t index) {
    return detail::type_list_impl::type_at<Args...>::at(index);
  }

  /**
   * Returns a boolean std::integral_constant telling whether this
   * list contains the given type.
   *
   * Example:
   *
   *  using types = type_list<A, B, C>;
   *
   *  // yields `true`
   *  types::contains<A>::value
   *
   *  // yields `false`
   *  types::contains<D>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  using contains = typename detail::type_list_impl::contains<T, Args...>::type;

  // TODO: implement find (sublist)

  /**
   * Appends the types `Suffix...` to the end of this list.
   *
   * Example:
   *
   *  using types = type_list<A, B, C>;
   *
   *  // yields `type_list<A, B, C, D, E>`
   *  using result = types::push_back<D, E>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Suffix>
  using push_back = type_list<Args..., Suffix...>;

  /**
   * Prepends the types `Prefix...` to the start of this list.
   *
   * Example:
   *
   *  using types = type_list<C, D, E>;
   *
   *  // yields `type_list<A, B, C, D, E>`
   *  using result = types::push_front<A, B>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Prefix>
  using push_front = type_list<Prefix..., Args...>;

  /**
   * Interleaves the given separators between each pair of elements of this
   * list.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 4, 5, 6>;
   *
   *  // yields `constant_sequence<int, 4, 0, 5, 0, 6>`
   *  using result1 = seq::interleave<0>;
   *
   *  // yields `constant_sequence<int, 4, 0, 1, 5, 0, 1, 6>`
   *  using result2 = seq::interleave<0, 1>;
   *
   *  // yields `constant_sequence<int, 4, 5, 6>`
   *  using result3 = seq::interleave<>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Separator>
  using interleave = typename detail::type_list_impl::interleave<Args...>
    ::template apply<Separator...>::template after<>;

  /**
   * Appends the types from the given `type_list` to the end of this one.
   *
   * Example:
   *
   *  using lhs = type_list<A, B>;
   *  type_list<C, D> rhs;
   *
   *  // yields `type_list<A, B, C, D>`
   *  using result = lhs::concat<rhs>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  // TODO: accept a variadic number of lists to concat
  template <typename... TLists>
  using concat = typename detail::type_list_impl::concat<TLists...>::type
    ::template push_front<Args...>;

  /**
   * Inserts the type `T` in its sorted position from the beginning of
   * the list, according to the order relation defined by `LessComparer`.
   *
   * `LessComparer` defaults to `comparison_transform::less_than` when omitted.
   *
   * Example:
   *
   *  template <int Value>
   *  using val = std::integral_constant<int, Value>;
   *
   *  using list1 = type_list<val<0>, val<1>, val<3>>;
   *
   *  // yields `type_list<val<0>, val<1>, val<2>, val<3>>`
   *  using result1 = list1::insert_sorted<val<2>>;
   *
   *  template <typename LHS, typename RHS> struct cmp
   *  using gt = std::integral_constant<bool, (LHS::value > RHS::value)>;
   *
   *  using list2 = type_list<val<5>, val<3>, val<1>>;
   *
   *  // yields `type_list<val<5>, val<4>, val<3>, val<1>>`
   *  using result2 = list2::insert_sorted<val<4>, gt>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename T,
    template <typename...> class LessComparer = comparison_transform::less_than
  >
  using insert_sorted = typename detail::type_list_impl::insert_sorted<
    LessComparer, T, Args...
  >::type;

  // TODO: DOCUMENT
  template <std::size_t Multiplier>
  using multiply = typename detail::type_list_impl::multiply<
    Multiplier, Args...
  >::type;

  /**
   * Applies the elements of this list to the variadic template `T`.
   *
   * Example:
   *
   *  using types = type_list<A, B, C>;
   *
   *  // yields `std::tuple<A, B, C>`
   *  types::apply<std::tuple>
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class T>
  using apply = fatal::apply<T, Args...>;

  /**
   * Uses the std::integral_constant-like class `TGetter` to extract
   * values of type `T` from each of this list's types, then applies
   * them to the variadic template `TTo`. The type `T` is not applied,
   * only the values.
   *
   * The default `TGetter` simply extracts the static
   * constexpr member `value` from the types.
   *
   * Example:
   *
   *  template <int... Values> struct int_sequence;
   *  using list = type_list<
   *    std::integral_constant<int, 0>,
   *    std::integral_constant<int, 1>,
   *    std::integral_constant<int, 2>
   *  >;
   *  template <typename T>
   *  struct double_getter: public std::integral_constant<int, T::value * 2> {};
   *
   *  // yields `int_sequence<0, 1, 2>`
   *  using result1 = list::apply_values<int, int_sequence>;
   *
   *  // yields `int_sequence<0, 2, 4>`
   *  using result2 = list::apply_values<int, int_sequence, double_getter>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename T, template <T...> class TTo,
    template <typename...> class TGetter = identity
  >
  using apply_values = TTo<TGetter<Args>::value...>;

  /**
   * Uses the std::integral_constant-like class `TGetter` to extract
   * values of type `T` from each of this list's types, then applies
   * them to the variadic template `TTo`, with the type `T` being the
   * first template parameter.
   *
   * The default `TGetter` simply extracts the static
   * constexpr member `value` from the types.
   *
   * Example:
   *
   *  template <typename T, T... Values> struct sequence;
   *  using list = type_list<
   *    std::integral_constant<int, 0>,
   *    std::integral_constant<int, 1>,
   *    std::integral_constant<int, 2>
   *  >;
   *  template <typename T>
   *  struct double_getter: public std::integral_constant<int, T::value * 2> {};
   *
   *  // yields `sequence<0, 1, 2>`
   *  using result1 = list::apply_typed_values<int, sequence>;
   *
   *  // yields `sequence<0, 2, 4>`
   *  using result2 = list::apply_typed_values<int, sequence, double_getter>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   *
   * TODO: TEST
   */
  template <
    typename T, template <typename, T...> class TTo,
    template <typename...> class TGetter = identity
  >
  using apply_typed_values = TTo<T, TGetter<Args>::value...>;

  /**
   * A shorter and cheaper version of
   *
   *  this_list::concat<type_list<Suffix...>>::apply<T>
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class T, typename... Suffix>
  using apply_back = T<Args..., Suffix...>;

  /**
   * A shorted and cheaper version of
   *
   *  type_list<Prefix...>::concat<this_list>::apply<T>
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class T, typename... Prefix>
  using apply_front = T<Prefix..., Args...>;

  /**
   * Calls the given visitor for each type in the list that yields true when
   * fed to the given condition `Predicate`.
   *
   * No code for calling the visitor will be generated when the condition is
   * `false`.
   *
   * The first parameter given to the visitor is `indexed_type_tag` with the
   * list's type and its index, followed by `args`.
   *
   * This function returns the amount of types visited (i.e.: the amount of
   * calls to the visitor).
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  struct visitor {
   *    template <std::size_t Index>
   *    void operator ()(indexed_type_tag<int, Index>, char const *message) {
   *      std::cout << "overload for int at index " << Index << ": "
   *        << message << std::endl;
   *    }
   *
   *    template <std::size_t Index>
   *    void operator ()(indexed_type_tag<long, Index>, char const *message) {
   *      std::cout << "overload for long at index " << Index << ": "
   *        << message << std::endl;
   *    }
   *
   *    // calling `operator ()` for types other than `int` and `long` would
   *    // yield a compilation error since there are no such overloads
   *  };
   *
   *  using list = type_list<double, float, long, std::string, int>;
   *
   *  // yields `2` and prints `
   *  //  overload for long at index 2: visited!
   *  //  overload for int at index 4: visited!
   *  // `
   *  auto result = list::foreach_if<std::is_integral>(visitor(), "visited!");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class Predicate, typename V, typename... VArgs
  >
  static constexpr std::size_t foreach_if(V &&visitor, VArgs &&...args) {
    return detail::type_list_impl::foreach_if<Predicate, 0, Args...>::visit(
      0, std::forward<V>(visitor), std::forward<VArgs>(args)...
    );
  }

  /**
   * Calls the given visitor for each type in the list.
   *
   * The first parameter given to the visitor is `indexed_type_tag`
   * with the list's type and its index, followed by `args`.
   *
   * This function returns `true` if the list is not empty (visitor has been
   * called for all types in the list, in order) or `false` if the list is empty
   * (visitor hasn't beel called).
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  template <typename> struct t2s;
   *  template <> struct t2s<int> { char const *s() { return "int"; } };
   *  template <> struct t2s<long> { char const *s() { return "long"; } };
   *  template <> struct t2s<double> { char const *s() { return "double"; } };
   *  template <> struct t2s<float> { char const *s() { return "float"; } };
   *  template <> struct t2s<bool> { char const *s() { return "bool"; } };
   *
   *  struct visitor {
   *    template <typename T, std::size_t Index>
   *    void operator ()(indexed_type_tag<T, Index>, char const *message) {
   *      std::cout << "overload for " << t2s<T>::s() << " at index " << Index
   *        << ": " << message << std::endl;
   *    }
   *  };
   *
   *  using list = type_list<double, float, long, bool, int>;
   *
   *  // yields `true` and prints `
   *  //  overload for double at index 0: visited!
   *  //  overload for float at index 1: visited!
   *  //  overload for long at index 2: visited!
   *  //  overload for bool at index 3: visited!
   *  //  overload for int at index 4: visited!
   *  // `
   *  list::foreach(visitor(), "visited!");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename V, typename... VArgs>
  static constexpr bool foreach(V &&visitor, VArgs &&...args) {
    return detail::type_list_impl::foreach<0, Args...>::visit(
      std::forward<V>(visitor), std::forward<VArgs>(args)...
    ), sizeof...(Args) != 0;
  }

  /**
   * Calls the given visitor for the type at position `index` in the list.
   *
   * This is a way to obtain a given type from an index that's only known at
   * runtime.
   *
   * The first parameter given to the visitor is `indexed_type_tag`
   * with the list's type and its index, followed by a perfect forward
   * of `args`.
   *
   * This function returns `true` if the list has an element at `index` (visitor
   * has been called) or `false` if the list is shorter than `index` (visitor
   * hasn't beel called)`
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  template <typename> struct t2s;
   *  template <> struct t2s<int> { char const *s() { return "int"; } };
   *  template <> struct t2s<long> { char const *s() { return "long"; } };
   *  template <> struct t2s<double> { char const *s() { return "double"; } };
   *  template <> struct t2s<float> { char const *s() { return "float"; } };
   *  template <> struct t2s<bool> { char const *s() { return "bool"; } };
   *
   *  struct visitor {
   *    template <typename T, std::size_t Index>
   *    void operator ()(indexed_type_tag<T, Index>, char const *message) {
   *      std::cout << "overload for " << t2s<T>::s() << " at index " << Index
   *        << ": " << message << std::endl;
   *    }
   *  };
   *
   *  using list = type_list<double, float, long, bool, int>;
   *
   *  // yields `true` and prints `overload for double at index 0: visited!`
   *  list::visit(0, visitor(), "visited!");
   *
   *  // yields `true` and prints `overload for float at index 1: visited!`
   *  list::visit(1, visitor(), "visited!");
   *
   *  // yields `true` and prints `overload for long at index 2: visited!`
   *  list::visit(2, visitor(), "visited!");
   *
   *  // yields `true` and prints `overload for bool at index 3: visited!`
   *  list::visit(3, visitor(), "visited!");
   *
   *  // yields `true` and prints `overload for int at index 4: visited!`
   *  list::visit(4, visitor(), "visited!");
   *
   *  // yields `false`
   *  list::visit(5, visitor(), "visited!");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename V, typename... VArgs>
  static constexpr bool visit(std::size_t index, V &&visitor, VArgs &&...args) {
    return detail::type_list_impl::visit<0, Args...>::at(
      index,
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  }

  /**
   * Applies the transform `Transform` to every element of this list.
   *
   * Example:
   *
   *  template <typename> struct T {};
   *  using types = type_list<A, B, C>;
   *
   *  // yields `type_list<T<A>, T<B>, T<C>>`
   *  using result1 = types::transform<T>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Transform>
  using transform = type_list<fatal::apply<Transform, Args>...>;

  /**
   * Applies each transform `Transforms`, in the order they are given,
   * to every element of this list.
   *
   * This is a convenient alternative to calling `transform` multiple times.
   *
   * Example:
   *
   *  template <typename> struct T {};
   *  template <typename> struct U {};
   *  using types = type_list<A, B, C>;
   *
   *  // yields `type_list<T<A>, T<B>, T<C>>`
   *  using result1 = types::multi_transform<T>;
   *
   *  // yields `type_list<U<T<A>>, U<T<B>>, U<T<C>>>`
   *  using result2 = types::multi_transform<T, U>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class... Transforms>
  using multi_transform = type_list<
    typename compose<Transforms...>::template apply<Args>...
  >;

  /**
   * TODO: TEST
   * Applies `Transform` to each element of this list that's accepted
   * by the predicate. Every element rejected by the predicate remains
   * untouched.
   *
   * Example:
   *
   *  using types = type_list<int, void, double, std::string, long>;
   *
   *  // yields `type_list<unsigned, void, double, std::string, unsigned long>`
   *  types::transform_if<std::is_integral, std::make_unsigned_t>
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class Predicate,
    template <typename...> class... Transforms
  >
  using transform_if = type_list<
    typename conditional_transform<
      Predicate,
      compose<Transforms...>::template apply
    >::template apply<Args>...
  >;

  /**
   * Applies T to each element/index pair of this list.
   *
   * Example:
   *
   *  template <typename, std::size_t> struct T {};
   *  using types = type_list<A, B, C>;
   *
   *  // yields `type_list<T<A, 0>, T<B, 1>, T<C, 2>>`
   *  types::transform<T>
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   *
   * TODO: Test additional args
   */
  template <
    template <typename, std::size_t, typename...> class Transform,
    typename... UArgs
  >
  using indexed_transform = typename detail::type_list_impl::indexed_transform<
    0, Args...
  >::template apply<Transform, UArgs...>;

  /**
   * TODO: DOCUMENT
   *
   * Example:
   *
   *  using list = type_list<A, B, C, D, E>;
   *
   *  // yields `type_list<
   *  //  std::tuple<>,
   *  //  std::tuple<A>,
   *  //  std::tuple<A, B>,
   *  //  std::tuple<A, B, C>,
   *  //  std::tuple<A, B, C, D>
   *  // >`
   *  using result1 = list::cumulative_transform<>::apply<std::tuple>;
   *
   *  // yields `type_list<
   *  //  std::tuple<A>,
   *  //  std::tuple<A, B>,
   *  //  std::tuple<A, B, C>,
   *  //  std::tuple<A, B, C, D>,
   *  //  std::tuple<A, B, C, D, E>
   *  // >`
   *  using result2 = list::cumulative_transform<true>::apply<std::tuple>;
   *
   *  template <int... Values>
   *  using int_seq = type_list<std::integral_constant<int, Values>...>;
   *
   *  // yields `int_seq<0, 1, 3, 6, 10, 15, 21, 28, 36, 45>`
   *  using result3 = int_seq<1, 2, 3, 4, 5, 6, 7, 8, 9, 10>
   *    ::cumulative_transform<>
   *    ::apply<arithmetic::add, std::integral_constant<int, 0>>;
   *
   *  // yields `int_seq<1, 3, 6, 10, 15, 21, 28, 36, 45, 55>`
   *  using result4 = int_seq<1, 2, 3, 4, 5, 6, 7, 8, 9, 10>
   *    ::cumulative_transform<true>::apply<arithmetic::add>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <bool Inclusive = false>
  struct cumulative_transform {
    template <template <typename...> class Transform, typename... UArgs>
    using apply = typename detail::type_list_impl::cumulative_transform<
      Inclusive, Transform, type_list<>, type_list<>, Args...
    >::template apply<UArgs...>;
  };

  /**
   * TODO: DOCUMENT
   *
   * Example:
   *
   *  template <int... Values>
   *  using int_seq = type_list<std::integral_constant<int, Values>...>;
   *
   *  template <typename L, typename R>
   *  using add = std::integral_constant<int, L::value + R::value>;
   *
   *  // yields `std::integral_constant<int, 28>`
   *  using result1 = int_seq<1, 2, 3, 4, 5, 6, 7>::accumulate<add>;
   *
   *  template <typename L, typename R>
   *  using mul = std::integral_constant<int, L::value * R::value>;
   *
   *  // yields `std::integral_constant<int, 120>`
   *  using result2 = int_seq<1, 2, 3, 4, 5>::accumulate<mul>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Transform, typename... Seed>
  using accumulate = typename detail::type_list_impl::accumulate<
    Transform, Seed..., Args...
  >::type;

  /**
   * TODO: DOCUMENT
   *
   * Example:
   *
   *  template <int... Values>
   *  using int_seq = type_list<std::integral_constant<int, Values>...>;
   *
   *  template <typename L, typename R>
   *  using lt = std::integral_constant<bool, (L::value < R::value)>;
   *
   *  // yields `std::integral_constant<int, 1>`
   *  using result1 = int_seq<1, 2, 3, 4, 5, 6, 7>::choose<lt>;
   *
   *  template <typename L, typename R>
   *  using gt = std::integral_constant<bool, (L::value > R::value)>;
   *
   *  // yields `std::integral_constant<int, 7>`
   *  using result2 = int_seq<1, 2, 3, 4, 5, 6, 7>::choose<gt>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class BinaryPredicate,
    template <typename...> class... Transforms
  >
  using choose = typename detail::type_list_impl::choose<
    BinaryPredicate,
    compose<Transforms...>::template apply,
    Args...
  >::type;

  /**
   * Replaces all occurences of `TFrom` with `TTo`.
   *
   * Example:
   *
   *  using list = type_list<int, void, int, bool>;
   *
   *  // yields `type_list<double, void, double, bool>`
   *  using result = list::replace<int, double>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TFrom, typename TTo>
  using replace = transform<
    detail::type_list_impl::replace_transform<TFrom, TTo>::template apply
  >;

  /**
   * Returns a list with the rightmost types of this list,
   * starting at the index `Offset`.
   *
   * This is the same as `right<size - Offset>` or `slice<Offse, size>`.
   *
   * Example:
   *
   *  using types = type_list<A, B, C, D>;
   *
   *  // yields `type_list<B, C, D>`
   *  using result1 = types::tail<1>;
   *
   *  // yields `type_list<A, B, C, D>`
   *  using result2 = types::tail<0>;
   *
   *  // yields `type_list<>`
   *  using result3 = types::tail<types::size>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Offset>
  using tail = typename detail::type_list_impl::tail<Offset, Args...>::type;

  /**
   * Returns a type pair with two sublists resulting from splitting this
   * list at `Index`.
   *
   * By default, when `Index` is not specified, the list is split in half.
   *
   * This is the same as `type_pair<left<Index>, tail<Index>>`.
   *
   * Example:
   *
   *  // yields `type_pair<type_list<A, B>, type_list<C, D>>`
   *  using result1 = type_list<A, B, C, D>::split<>;
   *
   *  // yields `type_pair<type_list<A, B>, type_list<C, D, E>>`
   *  using result2 = type_list<A, B, C, D, E>::split<>;
   *
   *  // yields `type_pair<type_list<A, B>, type_list<C, D>>`
   *  using result3 = type_list<A, B, C, D>::split<>;
   *
   *  // yields `type_pair<type_list<A, B>, type_list<C, D>>`
   *  using result4 = type_list<A, B, C, D>::split<2>;
   *
   *  // yields `type_pair<type_list<>, type_list<A, B, C, D>>`
   *  using result5 = type_list<A, B, C, D>::split<0>;
   *
   *  // yields `type_pair<type_list<A, B, C, D>, type_list<>>`
   *  using result6 = type_list<A, B, C, D>::split<types::size>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index = (size / 2)>
  using split = typename detail::type_list_impl::split<Index, Args...>::type;

  /**
   * Returns a sublist with the types whose positional
   * index is in the range [Begin, End).
   *
   * Example:
   *
   *  using types = type_list<A, B, C, D>;
   *
   *  // yields `type_list<>`
   *  using result1 = types::slice<2, 0>;
   *
   *  // yields `type_list<C>`
   *  using result2 = types::slice<2, 1>;
   *
   *  // yields `type_list<C, D>`
   *  using result3 = types::slice<2, 2>;
   *
   *  // yields `type_list<B, C, D>`
   *  using result4 = types::slice<1, 3>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Begin, std::size_t End>
  using slice = typename detail::type_list_impl::slice<
    Begin, End, Args...
  >::type;

  /**
   * Returns a list with the `Size` leftmost types of this list.
   *
   * Example:
   *
   *  using types = type_list<A, B, C, D>;
   *
   *  // yields `type_list<A, B>`
   *  using result1 = types::left<2>;
   *
   *  // yields `type_list<>`
   *  using result2 = types::left<0>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Size>
  using left = typename detail::type_list_impl::left<Size, Args...>::type;

  /**
   * Returns a list with the `Size` rightmost types of this list.
   *
   * Example:
   *
   *  using types = type_list<A, B, C, D>;
   *
   *  // yields `type_list<C, D>`
   *  using result1 = types::right<2>;
   *
   *  // yields `type_list<>`
   *  using result2 = types::right<0>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Size>
  using right = tail<size - Size>;

  /**
   * Returns a pair with two `type_list`s. One (first) with the types that
   * got accepted by the predicate and the other (second) with the types that
   * weren't accepted by it.
   *
   * `Predicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with an element from this list.
   *
   * Example:
   *
   *  using types = type_list<int, std::string, double, long>;
   *
   *  // yields `type_pair<
   *  //   type_list<int, long>,
   *  //   type_list<std::string, double>
   *  // >`
   *  using result = types::separate<std::is_integral>;
   *
   * TODO: update docs and tests for multiple predicates
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class... TPredicates>
  using separate = typename detail::type_list_impl::separate<
    compose<TPredicates...>::template apply, Args...
  >::type;

  /**
   * Returns a `type_list` containing only the types that got accepted
   * by the predicate.
   *
   * `Predicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with an element from this list.
   *
   * Example:
   *
   *  using types = type_list<int, std::string, double, long>;
   *
   *  // yields `type_list<int, long>`
   *  using result = types::filter<std::is_integral>;
   *
   * TODO: update docs and tests for multiple predicates
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class... TPredicates>
  using filter = typename separate<TPredicates...>::first;

  /**
   * Returns a `type_list` containing only the types that did not get
   * accepted by the predicate.
   *
   * `Predicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with an element from this list.
   *
   * Example:
   *
   *  using types = type_list<int, std::string, double, long>;
   *
   *  // yields `type_list<std::string, double>`
   *  using result = types::reject<std::is_integral>;
   *
   * TODO: update docs and tests for multiple predicates
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class... TPredicates>
  using reject = typename separate<TPredicates...>::second;

  /**
   * Removes all occurences of given types from the type list.
   *
   * Example:
   *
   *  using types = type_list<int, bool, int, float, void>;
   *
   *  // yields `type_list<bool, float>`
   *  using result = types::remove<int, void>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using remove = typename separate<
    type_list<UArgs...>::template contains
  >::second;

  /**
   * Interleaves the types from both lists.
   *
   * Example:
   *
   *  using l1 = type_list<E, D, G>;
   *
   *  // yields type_list<E, B, D, A, G>
   *  using result1 = l1::zip<B, A>;
   *
   *  using l2 = type_list<B, A>;
   *
   *  // yields type_list<B, E, A, D, G>
   *  using result2 = l2::zip<E, D, G>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... URHS>
  using zip = typename detail::type_list_impl::zip<
    type_list<Args...>, type_list<URHS...>
  >::type;

  /**
   * Returns a type_list with a pattern of types based on their position.
   * Picks types starting at `Offset` and picking every `Step` elements.
   *
   * Example:
   *
   *  using list = type_list<A, B, C, D, E, F, G, H, I>;
   *
   *  // yields type_list<A, D, G>
   *  using result1 = list::unzip<3>;
   *
   *  // yields type_list<C, E, G, I>
   *  using result2 = list::unzip<2, 2>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Step, std::size_t Offset = 0>
  using unzip = typename tail<
    (Offset < size) ? Offset : size
  >::template apply<
    detail::type_list_impl::curried_skip<Step>::template apply
  >::type;

  /**
   * Searches for the first type that satisfiest the given predicate.
   *
   * `Predicate` is a std::integral_constant-like template whose value
   * tells whether a given type satisfies the match or not.
   *
   * If there's no type in this list that satisfies the predicate, then
   * `Default` is returned (defaults to `type_not_found_tag` when omitted).
   *
   * Example:
   *
   *  using types = type_list<int, std::string, double, long>;
   *
   *  // yields `int`
   *  using result1 = types::search<std::is_integral, void>;
   *
   *  // yields `void`
   *  using result2 = types::search<std::is_unsigned, void>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class Predicate,
    typename Default = type_not_found_tag
  >
  using search = typename detail::type_list_impl::search<
    Predicate, Default, Args...
  >::type;

  // TODO: UPDATE DOCS AND SUPPORT MORE THAN TWO LISTS
  /**
   * Combines two type lists of equal size into a single type list using the
   * `TCombiner` template.
   *
   * `TCombiner` defaults to `type_pair`.
   *
   * Example:
   *
   *  using left = type_list<A, B, C>;
   *  using right = type_list<D, E, F>;
   *  template <typename, typename> struct combiner {};
   *
   *  // yields `type_list<combiner<A, D>, combiner<B, E>, combiner<C, F>>`
   *  using result = left::combine<right, combiner>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class TCombiner = type_pair>
  struct combine {
    template <typename... UArgs>
    using args = type_list<TCombiner<Args, UArgs>...>;

    template <typename UList>
    using list = typename UList::template apply<args>;
  };

  /**
   * Flattens elements from sublists into a single, topmost list.
   *
   * The topmost list can be customized with the template template parameter
   * `List`. when omitted, it defaults to `fatal::type_list`.
   *
   * Only a single level will be flattened. For recursive flattening,
   * use deep_flatten.
   *
   * This is equivalent to `fatal::flatten` or `deep_flatten<1>`.
   *
   * Example:
   *
   *  using list = type_list<
   *    int,
   *    type_list<
   *      double,
   *      float,
   *      type_list<short>
   *    >,
   *    bool
   *  >;
   *
   *  // yields `type_list<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   type_list<short>,
   *  //   bool
   *  // >`
   *  using result1 = list::flatten<>;
   *
   *  // yields `std::tuple<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   type_list<short>,
   *  //   bool
   *  // >`
   *  using result2 = list::flatten<std::tuple>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class List = fatal::type_list>
  using flatten = typename fatal::flatten<List, fatal::type_list>
    ::template apply<Args...>;

  /**
   * Recursively flattens elements from sublists into a single, topmost list.
   *
   * The topmost list's elements will be layed out as if traversing using a
   * recursive iterator on the lists.
   *
   * Up to `Depth` recursion levels are flattened. 0 means the original list.
   *
   * Example:
   *
   *  using list = type_list<
   *    int,
   *    type_list<
   *      double,
   *      float,
   *      type_list<
   *        type_list<std::string>,
   *        type_list<std::size_t, std::vector<int>>,
   *        short
   *      >,
   *    >,
   *    bool
   *  >;
   *
   *  // yields `type_list<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   std::string,
   *  //   std::size_t, std::vector<int>,
   *  //   short
   *  //   bool
   *  // >`
   *  using result = list::deep_flatten<>;
   *
   *  // yields the same as `list`
   *  using result0 = list::deep_flatten<0>;
   *
   *  // yields `type_list<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   type_list<
   *  //     type_list<std::string>,
   *  //     type_list<std::size_t, std::vector<int>>,
   *  //     short
   *  //   >,
   *  //   bool
   *  // >`
   *  using result1 = list::deep_flatten<1>;
   *
   *  // yields `type_list<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   type_list<std::string>,
   *  //   type_list<std::size_t, std::vector<int>>,
   *  //   short
   *  //   bool
   *  // >`
   *  using result2 = list::deep_flatten<2>;
   *
   *  // yields `type_list<
   *  //   int,
   *  //   double,
   *  //   float,
   *  //   std::string,
   *  //   std::size_t, std::vector<int>,
   *  //   short
   *  //   bool
   *  // >`
   *  using result2 = list::deep_flatten<2>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Depth = std::numeric_limits<std::size_t>::max()>
  using deep_flatten = typename detail::type_list_impl::deep_flatten<
    0, Depth, Args...
  >::type;

  /**
   * Tells whether this type_list is sorted according to the given type
   * comparer `LessComparer`.
   *
   * `LessComparer` receives two types, `LHS` and `RHS`, as template
   * arguments, and must provide a static constexpr boolean member `value`
   * telling whether `LHS` is smaller than `RHS` or not.
   *
   * `LessComparer` must represent a total order relation between all types.
   *
   * The default comparer is `comparison_transform::less_than`.
   *
   * Example:
   *
   *  template <int X> struct T {};
   *  template <typename, typename> struct comparer;
   *  template <int L, int R>
   *  struct comparer<T<L>, T<R>>:
   *    public std::integral_constant<bool, (L < R)>
   *  {};
   *
   *  // yields `false`
   *  type_list<T<1>, T<0>, T<2>>::is_sorted<comparer>::value
   *
   *  // yields `true`
   *  type_list<T<0>, T<1>, T<2>>::is_sorted<comparer>::value
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class LessComparer = comparison_transform::less_than
  >
  using is_sorted = typename detail::type_list_impl::is_sorted<
    LessComparer, Args...
  >::type;

  // TODO: Update docs
  /**
   * Merges two sorted type lists into a new sorted type list, according to
   * the given type comparer `LessComparer`.
   *
   * `LessComparer` must represent a total order relation between all types.
   *
   * The default comparer is `comparison_transform::less_than`.
   *
   * Example:
   *
   *  using lhs = type_list<T<0>, T<1>, T<2>>;
   *  using rhs = type_list<T<3>, T<4>, T<5>>;
   *
   *  // yields `type_list<T<0>, T<1>, T<2>, T<3>, T<4>, T<5>>`
   *  using result = lhs::merge<rhs, comparison_transform::less_than>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class LessComparer = comparison_transform::less_than
  >
  struct merge {
    template <typename List>
    using list = typename detail::type_list_impl::merge_entry_point<
      LessComparer, List, Args...
    >::type;

    template <typename... UArgs>
    using args = list<type_list<UArgs...>>;
  };

  /**
   * Sorts this `type_list` using the stable merge sort algorithm, according to
   * the given type comparer `LessComparer`.
   *
   * `LessComparer` must represent a total order relation between all types.
   *
   * The default comparer is `comparison_transform::less_than`.
   *
   * Example:
   *
   *  using list = type_list<T<0>, T<5>, T<4>, T<2>, T<1>, T<3>>;
   *
   *  // yields `type_list<T<0>, T<1>, T<2>, T<3>, T<4>, T<5>>`
   *  using result = list::sort<>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class LessComparer = comparison_transform::less_than
  >
  using sort = typename detail::type_list_impl::sort<
    LessComparer, type_list
  >::type;

  /**
   * Removes all duplicate elements from this list.
   *
   * This algorithm is stable: only the first occurence of a type is kept.
   *
   * An optional transform `Transform` can be applied to each element of this
   * list before processing the duplicates.
   *
   * Example:
   *
   *  using list1 = type_list<int, double, double, int, bool, int, bool, float>;
   *
   *  // yields `type_list<int, double, bool, float>`
   *  using result1 = list::unique<>;
   *
   *  template <int... Values> using int_seq = type_list<
   *    std::integral_constant<int, Values>...
   *  >;
   *
   *  using list2 = int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2, 3, 0, 1, 2>;
   *
   *  // yields `int_seq<0, 1, 4, 3, 2, 6>`
   *  using result2 = list2::unique<>;
   *
   *  template <typename T>
   *  using double_val = std::integral_constant<int, T::value * 2>;
   *
   *  // yields `int_seq<0, 2, 8, 6, 4, 12>`
   *  using result3 = list2::unique<double_val>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Transform = identity>
  using unique = typename detail::type_list_impl::unique<
    type_list<>, fatal::apply<Transform, Args>...
  >::type;

  /**
   * TODO: TEST
   *
   * Tells whether there are duplicated elements in this list or not.
   *
   * An optional transform `Transform` can be applied to each element of this
   * list before processing the duplicates.
   *
   * Example:
   *
   *  // yields `std::false_type`
   *  using result1 = type_list<int, double, double, int, float>::is_unique<>;
   *
   *  // yields `std::true_type`
   *  using result2 = type_list<int, double, float, bool>::is_unique<>;
   *
   *  template <int... Values> using int_seq = type_list<
   *    std::integral_constant<int, Values>...
   *  >;
   *
   *  // yields `std::false_type`
   *  using result3 = int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2>::is_unique<>;
   *
   *  // yields `std::true_type`
   *  using result4 = int_seq<0, 1, 2, 3, 4, 5, 6>::is_unique<>;
   *
   *  template <typename T>
   *  using halve_val = std::integral_constant<int, T::value / 2>;
   *
   *  // yields `std::true_type`
   *  using result5 = int_seq<0, 2, 8, 6, 4, 12>::is_unique<halve_val>;
   *
   *  // yields `std::false_type`
   *  using result6 = int_seq<0, 1, 2, 3, 4, 5, 6>::is_unique<halve_val>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Transform = identity>
  // TODO: OPTIMIZE
  using is_unique = typename caster<bool>::apply<
    std::is_same<
      typename type_list::template transform<Transform>,
      unique<Transform>
    >
  >;

  /**
   * Performs a binary search on this list's types (assumes the list is sorted),
   * comparing against the given `needle`.
   *
   * If a matching type is found, the visitor is called with the following
   * arguments:
   *  - an instance of `indexed_type_tag<MatchingType, Index>`
   *  - the `needle`
   *  - the list of additional arguments `args` given to
   *    the visitor
   *
   * in other words, with this general signature:
   *
   *  template <
   *    typename T,
   *    std::size_t Index,
   *    typename Needle,
   *    typename... VArgs
   *  >
   *  void operator ()(
   *    indexed_type_tag<T, Index>,
   *    Needle &&needle,
   *    VArgs &&...args
   *  );
   *
   * Returns `true` when found, `false` otherwise.
   *
   * The comparison is performed using the given `Comparer`'s method, whose
   * signature must follow this pattern:
   *
   *  template <typename Needle, typename T, std::size_t Index>
   *  static int compare(Needle &&needle, indexed_type_tag<T, Index>);
   *
   * which effectivelly compares `needle` against the type `T`. The result must
   * be < 0, > 0 or == 0 if `needle` is, respectively, less than, greather than
   * or equal to `T`. `Index` is the position of `T` in this type list and can
   * also be used in the comparison if needed.
   *
   * `Comparer` defaults to `type_value_comparer` which compares an
   * std::integral_constant-like value to a runtime value.
   *
   * The sole purpose of `args` is to be passed along to the visitor, it is not
   * used by this method in any way. It could also be safely omitted.
   *
   * The visitor will be called at most once, iff a match is found. Otherwise it
   * will not be called. The boolean returned by this method can be used to tell
   * whether the visitor has been called or not.
   *
   * See each operation's documentation below for further details.
   *
   * Note: this is a runtime facility.
   *
   * Example comparer used on examples below:
   *
   *  template <char c> using chr = std::integral_constant<char, c>;
   *  template <char... s> using str = type_list<chr<s>...>;
   *
   *  template <int n> using int_val = std::integral_constant<int, n>;
   *  template <int... s> using int_seq = type_list<int_val<s>...>;
   *
   *  struct cmp {
   *    template <char c, std::size_t Index>
   *    static int compare(char needle, indexed_type_tag<chr<c>, Index>) {
   *      return static_cast<int>(needle) - c;
   *    };
   *
   *    template <int n, std::size_t Index>
   *    static int compare(int needle, indexed_type_tag<int_val<n>, Index>) {
   *      return needle - n;
   *    };
   *  };
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Comparer = type_value_comparer>
  class binary_search {
    // TODO: ASSERT THE LIST IS SORTED

    using prepared = typename detail::type_list_impl::binary_search_prepare<
      Comparer, type_list<>, Args...
    >::type;

  public:
    /**
     * Performs a binary search for an element
     * that is an exact match of the `needle`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <char c, std::size_t Index>
     *    void operator ()(indexed_type_tag<chr<c>, Index>, char needle) {
     *      assert(c == needle);
     *      std::cout << "needle '" << needle << "' found at index " << Index
     *        << std::endl;
     *    };
     *  };
     *
     *  using list = str<'a', 'e', 'i', 'o', 'u'>;
     *
     *  // yields `false`
     *  list::binary_search<cmp>::exact('x', visitor());
     *
     *  // yields `true` and prints `"needle 'i' found at index 2"`
     *  list::binary_search<cmp>::exact('i', visitor());
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <typename Needle, typename Visitor, typename... VArgs>
    static constexpr bool exact(
      Needle &&needle, Visitor &&visitor, VArgs &&...args
    ) {
      return prepared::template apply_front<
        detail::type_list_impl::binary_search_exact, Comparer
      >::template search<0>(
        std::forward<Needle>(needle),
        std::forward<Visitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }

    /**
     * Performs a binary search for the greatest element that is less than
     * or equal to (<=) the `needle`. This is analogous to `std::lower_bound`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <int n, std::size_t Index>
     *    void operator ()(indexed_type_tag<int_val<n>, Index>, int needle) {
     *      assert(n <= needle);
     *      std::cout << needle << "'s lower bound " << n
     *        << " found at index " << Index
     *        << std::endl;
     *    };
     *  };
     *
     *  using list = int_seq<10, 30, 50, 70>;
     *
     *  // yields `false`
     *  list::binary_search<cmp>::lower_bound(5, visitor());
     *
     *  // yields `true` and prints `"11's lower bound 10 found at index 0"`
     *  list::binary_search<cmp>::lower_bound(11, visitor());
     *
     *  // yields `true` and prints `"68's lower bound 50 found at index 2"`
     *  list::binary_search<cmp>::lower_bound(68, visitor());
     *
     *  // yields `true` and prints `"70's lower bound 70 found at index 3"`
     *  list::binary_search<cmp>::lower_bound(70, visitor());
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <typename Needle, typename Visitor, typename... VArgs>
    static constexpr bool lower_bound(
      Needle &&needle, Visitor &&visitor, VArgs &&...args
    ) {
      return prepared::template apply<
        detail::type_list_impl::binary_search_lower_bound
      >::template search<Comparer>(
        std::forward<Needle>(needle),
        std::forward<Visitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }

    /**
     * Performs a binary search for the least element that is greater
     * than (>) the `needle`. This is analogous to `std::upper_bound`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <int n, std::size_t Index>
     *    void operator ()(indexed_type_tag<int_val<n>, Index>, int needle) {
     *      assert(n > needle);
     *      std::cout << needle << "'s upper bound " << n
     *        << " found at index " << Index
     *        << std::endl;
     *    };
     *  };
     *
     *  using list = int_seq<10, 30, 50, 70>;
     *
     *  // yields `false`
     *  list::binary_search<cmp>::upper_bound(70, visitor());
     *
     *  // yields `true` and prints `"5's upper bound 10 found at index 0"`
     *  list::binary_search<cmp>::upper_bound(5, visitor());
     *
     *  // yields `true` and prints `"31's upper bound 50 found at index 2"`
     *  list::binary_search<cmp>::upper_bound(31, visitor());
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <typename Needle, typename Visitor, typename... VArgs>
    static constexpr bool upper_bound(
      Needle &&needle, Visitor &&visitor, VArgs &&...args
    ) {
      return prepared::template apply<
        detail::type_list_impl::binary_search_upper_bound
      >::template search<Comparer, 0>(
        std::forward<Needle>(needle),
        std::forward<Visitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }
  };
};

// TODO: DOCUMENT AND TEST
template <std::size_t... Values>
using size_list = type_list<std::integral_constant<std::size_t, Values>...>;

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

#if FATAL_CPLUSPLUS < 201703L
template <typename... Args> constexpr std::size_t type_list<Args...>::size;
template <typename... Args> constexpr bool type_list<Args...>::empty;
#endif

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace type_list_impl {

////////
// at //
////////

template <std::size_t Distance, typename U, typename... UArgs>
struct at<Distance, U, UArgs...> {
  using type = typename at<Distance - 1, UArgs...>::type;
};

template <typename U, typename... UArgs>
struct at<0, U, UArgs...> {
  using type = U;
};

////////////
// try_at //
////////////

template <std::size_t Index, typename Default, typename... Args>
struct try_at<false, Index, Default, Args...> {
  using type = Default;
};

template <std::size_t Index, typename Default, typename... Args>
struct try_at<true, Index, Default, Args...> {
  using type = typename at<Index, Args...>::type;
};

//////////////
// index_of //
//////////////

template <std::size_t Size, typename T>
struct index_of<Size, T> {
  using type = std::integral_constant<std::size_t, Size>;
};

template <std::size_t Index, typename T, typename... Args>
struct index_of<Index, T, T, Args...> {
  using type = std::integral_constant<std::size_t, Index>;
};

template <std::size_t Index, typename T, typename U, typename... Args>
struct index_of<Index, T, U, Args...> {
  using type = typename index_of<Index + 1, T, Args...>::type;
};

//////////////////////
// checked_index_of //
//////////////////////

template <std::size_t Index, typename T, typename... Args>
struct checked_index_of<Index, T, T, Args...> {
  using type = std::integral_constant<std::size_t, Index>;
};

template <std::size_t Index, typename T, typename U, typename... Args>
struct checked_index_of<Index, T, U, Args...> {
  using type = typename checked_index_of<Index + 1, T, Args...>::type;
};

/////////////
// type_at //
/////////////

template <typename U> struct type_at<U> {
  static constexpr std::type_info const &at(std::size_t) {
    return typeid(U);
  }
};

// TODO: speed up search using interpolation search
template <typename U, typename... UArgs> struct type_at<U, UArgs...> {
  static constexpr std::type_info const &at(std::size_t index) {
    return index ? type_at<UArgs...>::at(index - 1) : type_at<U>::at(index);
  }
};

//////////////
// contains //
//////////////

template <typename, typename...>
struct contains {
  using type = std::false_type;
};

template <typename T, typename... Args>
struct contains<T, T, Args...> {
  using type = std::true_type;
};

template <typename T, typename THead, typename... Args>
struct contains<T, THead, Args...> {
  using type = typename contains<T, Args...>::type;
};

////////////////
// foreach_if //
////////////////

template <bool>
struct conditional_visit {
  template <typename V, typename... VArgs>
  static constexpr std::size_t visit(V &&, VArgs &&...) { return 0; }
};

template <>
struct conditional_visit<true> {
  template <typename V, typename... VArgs>
  static constexpr std::size_t visit(V &&visitor, VArgs &&...args) {
    // hack to accommodate C++11's constexpr constraints
    return visitor(std::forward<VArgs>(args)...), 1;
  }
};

template <
  template <typename...> class Predicate, std::size_t Index, typename...
>
struct foreach_if {
  template <typename V, typename... VArgs>
  static constexpr std::size_t visit(std::size_t result, V &&, VArgs &&...) {
    return result;
  }
};

template <
  template <typename...> class Predicate,
  std::size_t Index, typename U, typename... UArgs
>
struct foreach_if<Predicate, Index, U, UArgs...> {
  template <typename V, typename... VArgs>
  static constexpr std::size_t visit(
    std::size_t result, V &&visitor, VArgs &&...args
  ) {
    return foreach_if<Predicate, Index + 1, UArgs...>::visit(
      result + conditional_visit<fatal::apply<Predicate, U>::value>::visit(
        visitor, indexed_type_tag<U, Index>(), args...
      ), visitor, args...
    );
  }
};

/////////////
// foreach //
/////////////

template <std::size_t Index, typename...>
struct foreach {
  template <typename V, typename... VArgs>
  static void visit(V &&, VArgs &&...) {}
};

template <std::size_t Index, typename U, typename... UArgs>
struct foreach<Index, U, UArgs...> {
  template <typename V, typename... VArgs>
  static void visit(V &&visitor, VArgs &&...args) {
    visitor(indexed_type_tag<U, Index>(), args...);
    foreach<Index + 1, UArgs...>::visit(visitor, args...);
  }
};

///////////
// visit //
///////////

// TODO: binary search for the given index
// TODO: switch statement optimization

template <std::size_t Index, typename T, typename... Args>
struct visit<Index, T, Args...> {
  template <typename V, typename... VArgs>
  static constexpr bool at(std::size_t index, V &&visitor, VArgs &&...args) {
    return index == Index
      ? (
        visitor(
          indexed_type_tag<T, Index>(),
          std::forward<VArgs>(args)...
        ),
        true
      )
      : visit<Index + 1, Args...>::at(
        index,
        std::forward<V>(visitor),
        std::forward<VArgs>(args)...
      );
  }
};

template <std::size_t Index>
struct visit<Index> {
  template <typename V, typename... VArgs>
  static constexpr bool at(std::size_t, V &&, VArgs &&...) {
    return false;
  }
};

///////////////////////
// indexed_transform //
///////////////////////

template <std::size_t Index, typename T, typename... Args>
struct indexed_transform<Index, T, Args...> {
  template <
    template <typename, std::size_t, typename...> class Transform,
    typename... UArgs
  >
  using apply = typename indexed_transform<Index + 1, Args...>
    ::template apply<Transform, UArgs...>
    ::template push_front<Transform<T, Index>>;
};

template <std::size_t Index>
struct indexed_transform<Index> {
  template <template <typename, std::size_t, typename...> class, typename...>
  using apply = type_list<>;
};

//////////////////////////
// cumulative_transform //
//////////////////////////

template <
  template <typename...> class Transform,
  typename Result,
  typename Front,
  typename T, typename... Tail
>
// TODO: BENCHMARK THE TEST FILE BY EXPLOITING TYPE DEDUCTION ON SPECIALIZATIONS
//       FOR OTHER IMPLEMENTATIONS LIKE HERE
struct cumulative_transform<true, Transform, Result, Front, T, Tail...> {
  using front = typename Front::template push_back<T>;

  template <typename... UArgs>
  using apply = typename cumulative_transform<
    true,
    Transform,
    typename Result::template push_back<
      typename front::template push_front<UArgs...>::template apply<Transform>
    >,
    front,
    Tail...
  >::template apply<UArgs...>;
};

template <
  template <typename...> class Transform,
  typename Result,
  typename Front,
  typename T, typename... Tail
>
// TODO: BENCHMARK THE TEST FILE BY EXPLOITING TYPE DEDUCTION ON SPECIALIZATIONS
//       FOR OTHER IMPLEMENTATIONS LIKE HERE
struct cumulative_transform<false, Transform, Result, Front, T, Tail...> {
  template <typename... UArgs>
  using apply = typename cumulative_transform<
    false,
    Transform,
    typename Result::template push_back<
      typename Front::template push_front<UArgs...>::template apply<Transform>
    >,
    typename Front::template push_back<T>,
    Tail...
  >::template apply<UArgs...>;
};

template <
  bool Inclusive,
  template <typename...> class Transform,
  typename Result,
  typename Accumulator
>
struct cumulative_transform<Inclusive, Transform, Result, Accumulator> {
  template <typename...>
  using apply = Result;
};

////////////////
// accumulate //
////////////////

template <template <typename...> class, typename Accumulator, typename...>
struct accumulate {
  using type = Accumulator;
};

template <
  template <typename...> class Transform,
  typename Accumulator, typename T, typename... Args
>
struct accumulate<Transform, Accumulator, T, Args...> {
  using type = typename accumulate<
    Transform, Transform<Accumulator, T>, Args...
  >::type;
};

////////////
// choose //
////////////

template <
  template <typename...> class BinaryPredicate,
  template <typename...> class Transform,
  typename Chosen, typename Candidate, typename... Args
>
struct choose<BinaryPredicate, Transform, Chosen, Candidate, Args...> {
  using type = typename choose<
    BinaryPredicate,
    Transform,
    conditional<
      fatal::apply<
        BinaryPredicate,
        fatal::apply<Transform, Candidate>,
        fatal::apply<Transform, Chosen>
      >::value,
      Candidate,
      Chosen
    >,
    Args...
  >::type;
};

template <
  template <typename...> class BinaryPredicate,
  template <typename...> class Transform,
  typename Chosen
>
struct choose<BinaryPredicate, Transform, Chosen> {
  using type = Chosen;
};

///////////////////////
// replace_transform //
///////////////////////

template <typename TFrom, typename TTo>
struct replace_transform {
  template <typename U>
  using apply = conditional<
    std::is_same<U, TFrom>::value, TTo, U
  >;
};

//////////
// left //
//////////

template <typename... Args>
struct left<0, Args...> { using type = type_list<>; };

template <typename T,typename... Args>
struct left<0, T, Args...> { using type = type_list<>; };

template <std::size_t Size, typename T, typename... Args>
struct left<Size, T, Args...> {
  static_assert(Size <= sizeof...(Args) + 1, "index out of bounds");
  using type = typename left<Size - 1, Args...>::type::template push_front<T>;
};

///////////
// slice //
///////////

template <std::size_t End, typename... UArgs>
struct slice<0, End, UArgs...> {
  using type = typename left<End, UArgs...>::type;
};

template <std::size_t End, typename U, typename... UArgs>
struct slice<0, End, U, UArgs...> {
  using type = typename left<End, U, UArgs...>::type;
};

template <std::size_t Begin, std::size_t End, typename U, typename... UArgs>
struct slice<Begin, End, U, UArgs...> {
  static_assert(End >= Begin, "invalid range");
  using type = typename slice<Begin - 1, End - 1, UArgs...>::type;
};

//////////////
// separate //
//////////////

template <template <typename...> class Predicate>
struct separate<Predicate> {
  using type = type_pair<type_list<>, type_list<>>;
};

template <
  template <typename...> class Predicate,
  typename U, typename... UArgs
>
struct separate<Predicate, U, UArgs...> {
  using tail = typename separate<Predicate, UArgs...>::type;

  using type = conditional<
    fatal::apply<Predicate, U>::value,
    type_pair<
      typename tail::first::template push_front<U>,
      typename tail::second
    >,
    type_pair<
      typename tail::first,
      typename tail::second::template push_front<U>
    >
  >;
};

/////////
// zip //
/////////

template <>
struct zip<type_list<>, type_list<>> {
  using type = type_list<>;
};

template <typename T, typename... Args>
struct zip<type_list<T, Args...>, type_list<>> {
  using type = type_list<T, Args...>;
};

template <typename T, typename... Args>
struct zip<type_list<>, type_list<T, Args...>> {
  using type = type_list<T, Args...>;
};

template <
  typename LHS, typename... TLHSArgs,
  typename RHS, typename... TRHSArgs
>
struct zip<type_list<LHS, TLHSArgs...>, type_list<RHS, TRHSArgs...>> {
  using type = typename zip<
    type_list<TLHSArgs...>,
    type_list<TRHSArgs...>
  >::type::template push_front<LHS, RHS>;
};

//////////
// skip //
//////////

template <std::size_t Next, std::size_t Step>
struct skip<Next, Step> { using type = type_list<>; };

template <std::size_t Step, typename U, typename... UArgs>
struct skip<0, Step, U, UArgs...> {
  using type = typename skip<Step, Step, UArgs...>::type
    ::template push_front<U>;
};

template <std::size_t Next, std::size_t Step, typename U, typename... UArgs>
struct skip<Next, Step, U, UArgs...> {
  using type = typename skip<Next - 1, Step, UArgs...>::type;
};

template <std::size_t Step>
struct curried_skip {
  template <typename... UArgs>
  using apply = skip<0, Step ? Step - 1 : 0, UArgs...>;
};

////////////
// search //
////////////

template <template <typename...> class Predicate, typename Default>
struct search<Predicate, Default> { using type = Default; };

template <
  template <typename...> class Predicate,
  typename Default, typename U, typename... UArgs
>
struct search<Predicate, Default, U, UArgs...> {
  using type = conditional<
    fatal::apply<Predicate, U>::value,
    U,
    typename search<Predicate, Default, UArgs...>::type
  >;
};

//////////////////
// deep_flatten //
//////////////////

// max depth reached - base case
template <std::size_t MaxDepth, typename T, typename... Args>
struct deep_flatten<MaxDepth, MaxDepth, T, Args...> {
  using type = type_list<T, Args...>;
};

template <std::size_t Depth, std::size_t MaxDepth, typename T, typename... Args>
struct deep_flatten<Depth, MaxDepth, T, Args...> {
  using type = typename deep_flatten<Depth, MaxDepth, Args...>::type
    ::template push_front<T>;
};

template <std::size_t Depth, std::size_t MaxDepth>
struct deep_flatten<Depth, MaxDepth> {
  using type = type_list<>;
};

template <std::size_t MaxDepth, typename... Args, typename... UArgs>
struct deep_flatten<MaxDepth, MaxDepth, type_list<Args...>, UArgs...> {
  using type = type_list<type_list<Args...>, UArgs...>;
};

template <
  std::size_t Depth, std::size_t MaxDepth, typename... Args, typename... UArgs
>
struct deep_flatten<Depth, MaxDepth, type_list<Args...>, UArgs...> {
  using type = typename deep_flatten<Depth + 1, MaxDepth, Args...>::type
    ::template concat<typename deep_flatten<Depth, MaxDepth, UArgs...>::type>;
};

////////////////
// interleave //
////////////////

template <>
struct interleave<> {
  template <typename...>
  struct apply {
    template <typename... Prefix>
    using after = type_list<Prefix...>;
  };
};

template <typename T>
struct interleave<T> {
  template <typename...>
  struct apply {
    template <typename... Prefix>
    using after = type_list<Prefix..., T>;
  };
};

template <typename T, typename... Args>
struct interleave<T, Args...> {
  template <typename... Separator>
  struct apply {
    template <typename... Prefix>
    using after = typename interleave<Args...>::template apply<Separator...>
      :: template after<Prefix..., T, Separator...>;
  };
};

////////////
// concat //
////////////

template <> struct concat<> { using type = type_list<>; };

template <typename... Args, typename... TLists>
struct concat<type_list<Args...>, TLists...> {
  using type = typename concat<TLists...>::type::template push_front<Args...>;
};

///////////////////
// insert_sorted //
///////////////////

template <template <typename...> class LessComparer, typename T>
struct insert_sorted<LessComparer, T> { using type = type_list<T>; };

template <
  template <typename...> class LessComparer,
  typename T,
  typename THead,
  typename... TTail
>
struct insert_sorted<LessComparer, T, THead, TTail...> {
  using type = conditional<
    LessComparer<T, THead>::value,
    type_list<T, THead, TTail...>,
    typename insert_sorted<
      LessComparer, T, TTail...
    >::type::template push_front<THead>
  >;
};

//////////////
// multiply //
//////////////

template <std::size_t Multiplier, typename... Args>
struct multiply {
  static_assert(Multiplier > 0, "out of bounds");
  using type = typename multiply<Multiplier - 1, Args...>::type
    ::template push_back<Args...>;
};

template <typename... Args>
struct multiply<0, Args...> {
  using type = type_list<>;
};

//////////
// tail //
//////////

template <std::size_t Index> struct tail<Index> {
  static_assert(Index == 0, "index out of bounds");
  using type = type_list<>;
};

template <typename T, typename... Args>
struct tail<0, T, Args...> { using type = type_list<T, Args...>; };

template <std::size_t Index, typename T, typename... Args>
struct tail<Index, T, Args...> {
  static_assert(Index <= sizeof...(Args) + 1, "index out of bounds");
  using type = typename tail<Index - 1, Args...>::type;
};

///////////
// split //
///////////

template <>
struct split<0> {
  using type = type_pair<type_list<>, type_list<>>;
};

template <typename T, typename... Args>
struct split<0, T, Args...> {
  using type = type_pair<type_list<>, type_list<T, Args...>>;
};

template <std::size_t Index, typename T, typename... Args>
struct split<Index, T, Args...> {
  using tail = split<Index - 1, Args...>;

  using type = type_pair<
    typename tail::type::first::template push_front<T>,
    typename tail::type::second
  >;
};

///////////////
// is_sorted //
///////////////

template <template <typename...> class, typename...>
struct is_sorted: public std::true_type {};

template <
  template <typename...> class LessComparer,
  typename LHS, typename RHS, typename... Args
>
struct is_sorted<LessComparer, LHS, RHS, Args...>:
  public logical::all<
    logical::negate<LessComparer<RHS, LHS>>,
    is_sorted<LessComparer, RHS, Args...>
  >
{};

///////////
// merge //
///////////

template <template <typename...> class, typename, typename...> struct merge;

template <template <typename...> class LessComparer>
struct merge<LessComparer, type_list<>> { using type = type_list<>; };

template <template <typename...> class LessComparer, typename TRHSList>
struct merge<LessComparer, TRHSList> { using type = TRHSList; };

template <
  template <typename...> class LessComparer,
  typename LHS, typename... TLHSArgs
>
struct merge<LessComparer, type_list<>, LHS, TLHSArgs...> {
  using type = type_list<LHS, TLHSArgs...>;
};

template <
  template <typename...> class LessComparer,
  typename TRHSList, typename LHS, typename... TLHSArgs
>
struct merge<LessComparer, TRHSList, LHS, TLHSArgs...> {
  using rhs = typename TRHSList::template at<0>;
  using right_merge = LessComparer<rhs, LHS>;
  using head = conditional<right_merge::value, rhs, LHS>;
  using tail = typename conditional<
    right_merge::value,
    merge<
      LessComparer, typename TRHSList::template tail<1>, LHS, TLHSArgs...
    >,
    merge<LessComparer, TRHSList, TLHSArgs...>
  >::type;

  using type = typename tail::template push_front<head>;
};

template <
  template <typename...> class LessComparer,
  typename TRHSList, typename... TLHSArgs
>
struct merge_entry_point {
#   ifndef NDEBUG
  // debug only due to compilation times
  static_assert(
    type_list<TLHSArgs...>::template is_sorted<LessComparer>::value,
    "left hand side list is not sorted"
  );

  static_assert(
    TRHSList::template is_sorted<LessComparer>::value,
    "right hand side list is not sorted"
  );
#   endif // NDEBUG
  using type = typename merge<LessComparer, TRHSList, TLHSArgs...>::type;
};

//////////
// sort //
//////////

template <template <typename...> class LessComparer, typename List>
struct sort {
  using unsorted = typename List::template split<List::size / 2>;
  using sorted_left = typename sort<
    LessComparer, typename unsorted::first
  >::type;
  using sorted_right = typename sort<
    LessComparer, typename unsorted::second
  >::type;

  static_assert(List::size > 1, "invalid specialization");
  using type = typename sorted_left::template merge<LessComparer>
    ::template list<sorted_right>;
};

template <template <typename...> class LessComparer>
struct sort<LessComparer, type_list<>> { using type = type_list<>; };

template <template <typename...> class LessComparer, typename T>
struct sort<LessComparer, type_list<T>> { using type = type_list<T>; };

////////////
// unique //
////////////

template <typename TResult> struct unique<TResult> { using type = TResult; };

template <typename TResult, typename T, typename... Args>
struct unique<TResult, T, Args...> {
  using type = typename unique<
    conditional<
      TResult::template contains<T>::value,
      TResult,
      typename TResult::template push_back<T>
    >,
    Args...
  >::type;
};

///////////////////////////
// binary_search_prepare //
///////////////////////////

template <typename Comparer, typename... Args>
struct binary_search_prepare<Comparer, type_list<>, Args...> {
  using type = type_list<Args...>;
};

template <typename T>
struct binary_search_prepare<type_value_comparer, T> {
  using type = T;
};

template <>
struct binary_search_prepare<type_value_comparer, type_list<>> {
  using type = type_list<>;
};

template <typename Head, typename... Tail>
struct binary_search_prepare<type_value_comparer, type_list<>, Head, Tail...> {
  using type = typename binary_search_prepare<
    type_value_comparer, type_list<Head>, Tail...
  >::type;
};

template <typename... Args, typename Head, typename... Tail>
struct binary_search_prepare<
  type_value_comparer, type_list<Args...>,
  Head, Tail...
> {
  using list = type_list<Args...>;
  using previous = typename list::template at<list::size - 1>;

  static_assert(
    previous::value <= Head::value,
    "binary_search requires a sorted list as input"
  );

  using type = typename binary_search_prepare<
    type_value_comparer,
    conditional<
      previous::value == Head::value,
      list,
      typename list::template push_back<Head>
    >,
    Tail...
  >::type;
};

/////////////////////////
// binary_search_exact //
/////////////////////////

template <typename Comparer, typename... Args>
struct binary_search_exact {
  using list = type_list<Args...>;

  using split = typename list:: template split<list::size / 2>;
  static_assert(!split::second::empty, "invalid specialization");

  using left = typename split::first;
  using right = typename split::second::template tail<1>;

  template <std::size_t Offset> using pivot = indexed_type_tag<
    typename split::second::template at<0>,
    Offset + left::size
  >;

  template <
    std::size_t Offset, typename TComparison,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool impl(
    TComparison &&comparison, Needle &&needle,
    Visitor &&visitor, VArgs &&...args
  ) {
    // ternary needed due to C++11's constexpr restrictions
    return comparison < 0
      ? left::template apply_front<
          type_list_impl::binary_search_exact, Comparer
        >::template search<Offset>(
          std::forward<Needle>(needle),
          std::forward<Visitor>(visitor),
          std::forward<VArgs>(args)...
        )
      : (0 < comparison
        ? right::template apply_front<
            type_list_impl::binary_search_exact, Comparer
          >::template search<Offset + left::size + 1>(
            std::forward<Needle>(needle),
            std::forward<Visitor>(visitor),
            std::forward<VArgs>(args)...
          )
        : (
          // comma operator needed due to C++11's constexpr restrictions
          visitor(
            pivot<Offset>{},
            std::forward<Needle>(needle),
            std::forward<VArgs>(args)...
          ), true
        )
      );
  }

  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    return impl<Offset>(
      Comparer::compare(needle, pivot<Offset>{}),
      needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
    );
  }
};

template <typename Comparer>
struct binary_search_exact<Comparer> {
  template <
    std::size_t, typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(Needle &&, Visitor &&, VArgs &&...) {
    return false;
  }
};

// optimization for low cardinality lists //

#define FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(Index) \
  ( \
    visitor( \
      indexed_type_tag<T##Index, Offset + 0x##Index>(), \
      std::forward<Needle>(needle), \
      std::forward<VArgs>(args)... \
    ), true \
  )

template <typename T0>
struct binary_search_exact<type_value_comparer, T0> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    return needle == T0::value
      ? FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0)
      : false;
  }
};

template <typename T0, typename T1>
struct binary_search_exact<type_value_comparer, T0, T1> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      default: return false;
    }
  }
};

template <typename T0, typename T1, typename T2>
struct binary_search_exact<type_value_comparer, T0, T1, T2> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      default: return false;
    }
  }
};

template <typename T0, typename T1, typename T2, typename T3>
struct binary_search_exact<type_value_comparer, T0, T1, T2, T3> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      default: return false;
    }
  }
};

template <typename T0, typename T1, typename T2, typename T3, typename T4>
struct binary_search_exact<type_value_comparer, T0, T1, T2, T3, T4> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3, typename T4, typename T5
>
struct binary_search_exact<type_value_comparer, T0, T1, T2, T3, T4, T5> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6
>
struct binary_search_exact<type_value_comparer, T0, T1, T2, T3, T4, T5, T6> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7
>
struct binary_search_exact<
  type_value_comparer, T0, T1, T2, T3, T4, T5, T6, T7
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7, typename T8
>
struct binary_search_exact<
  type_value_comparer, T0, T1, T2, T3, T4, T5, T6, T7, T8
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
>
struct binary_search_exact<
  type_value_comparer, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA
>
struct binary_search_exact<
  type_value_comparer, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA, typename TB
>
struct binary_search_exact<
  type_value_comparer, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      case TB::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(B);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA, typename TB, typename TC
>
struct binary_search_exact<
  type_value_comparer,
  T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      case TB::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(B);
      case TC::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(C);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA, typename TB, typename TC, typename TD
>
struct binary_search_exact<
  type_value_comparer,
  T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      case TB::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(B);
      case TC::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(C);
      case TD::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(D);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA, typename TB,
  typename TC, typename TD, typename TE
>
struct binary_search_exact<
  type_value_comparer,
  T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD, TE
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      case TB::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(B);
      case TC::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(C);
      case TD::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(D);
      case TE::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(E);
      default: return false;
    }
  }
};

template <
  typename T0, typename T1, typename T2, typename T3,
  typename T4, typename T5, typename T6, typename T7,
  typename T8, typename T9, typename TA, typename TB,
  typename TC, typename TD, typename TE, typename TF
>
struct binary_search_exact<
  type_value_comparer,
  T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD, TE, TF
> {
  template <
    std::size_t Offset, typename Needle, typename Visitor, typename... VArgs
  >
  static bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    switch (needle) {
      case T0::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(0);
      case T1::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(1);
      case T2::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(2);
      case T3::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(3);
      case T4::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(4);
      case T5::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(5);
      case T6::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(6);
      case T7::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(7);
      case T8::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(8);
      case T9::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(9);
      case TA::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(A);
      case TB::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(B);
      case TC::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(C);
      case TD::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(D);
      case TE::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(E);
      case TF::value: return FATAL_BINARY_SEARCH_EXACT_VISIT_IMPL(F);
      default: return false;
    }
  }
};

///////////////////////////////
// binary_search_lower_bound //
///////////////////////////////

template <typename... Args>
struct binary_search_lower_bound {
  using list = type_list<Args...>;
  using split = typename list:: template split<list::size / 2>;
  using left = typename split::first;
  using right = typename split::second;
  template <std::size_t Offset> using pivot = indexed_type_tag<
    typename right::template at<0>,
    Offset + left::size
  >;

  template <
    typename Comparer, std::size_t Offset,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool impl(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    // ternary needed due to C++11's constexpr restrictions
    return Comparer::compare(needle, pivot<Offset>{}) < 0
      ? left::template apply<
          type_list_impl::binary_search_lower_bound
        >::template impl<Comparer, Offset>(
          needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
        )
      : right::template apply<
          type_list_impl::binary_search_lower_bound
        >::template impl<Comparer, Offset + left::size>(
          needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
        );
  }

  template <
    typename Comparer, typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    using first = indexed_type_tag<typename list::template at<0>, 0>;
    return Comparer::compare(needle, first{}) >= 0
      && impl<Comparer, 0>(
        needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
      );
  }
};

template <typename T>
struct binary_search_lower_bound<T> {
  template <
    typename Comparer, std::size_t Offset,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool impl(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    // comma operator needed due to C++11's constexpr restrictions
    return visitor(
      indexed_type_tag<T, Offset>{},
      std::forward<Needle>(needle),
      std::forward<VArgs>(args)...
    ), true;
  }

  template <
    typename Comparer, typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    return Comparer::compare(needle, indexed_type_tag<T, 0>{}) >= 0
      && impl<Comparer, 0>(
        needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
      );
  }
};

template <>
struct binary_search_lower_bound<> {
  template <
    typename, std::size_t,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool impl(Needle &&, Visitor &&, VArgs &&...) {
    return false;
  }

  template <typename, typename Needle, typename Visitor, typename... VArgs>
  static constexpr bool search(Needle &&, Visitor &&, VArgs &&...) {
    return false;
  }
};

///////////////////////////////
// binary_search_upper_bound //
///////////////////////////////

template <typename... Args>
struct binary_search_upper_bound {
  using list = type_list<Args...>;
  using split = typename list:: template split<(list::size + 1) / 2>;
  using left = typename split::first;
  using right = typename split::second;
  template <std::size_t Offset> using pivot = indexed_type_tag<
    typename left::template at<left::size - 1>,
    Offset + (left::size - 1)
  >;

  template <
    typename Comparer, std::size_t Offset,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    // ternary needed due to C++11's constexpr restrictions
    return Comparer::compare(needle, pivot<Offset>{}) < 0
      ? left::template apply<
          type_list_impl::binary_search_upper_bound
        >::template search<Comparer, Offset>(
          needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
        )
      : right::template apply<
          type_list_impl::binary_search_upper_bound
        >::template search<Comparer, Offset + left::size>(
          needle, std::forward<Visitor>(visitor), std::forward<VArgs>(args)...
        );
  }
};

template <typename T>
struct binary_search_upper_bound<T> {
  template <
    typename Comparer, std::size_t Offset,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(
    Needle &&needle, Visitor &&visitor, VArgs &&...args
  ) {
    return Comparer::compare(needle, indexed_type_tag<T, Offset>{}) < 0 && (
      // comma operator needed due to C++11's constexpr restrictions
      visitor(
        indexed_type_tag<T, Offset>{},
        needle,
        std::forward<VArgs>(args)...
      ), true
    );
  }
};

template <>
struct binary_search_upper_bound<> {
  template <
    typename, std::size_t,
    typename Needle, typename Visitor, typename... VArgs
  >
  static constexpr bool search(Needle &&, Visitor &&, VArgs &&...) {
    return false;
  }
};

} // namespace type_list_impl {
} // namespace detail {
} // namespace fatal {

// TODO: ADD TESTS TO AVOID DOUBLE MOVES

#endif // FATAL_INCLUDE_fatal_type_deprecated_list_h
