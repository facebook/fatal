/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_deprecated_type_map_h
#define FATAL_INCLUDE_fatal_type_deprecated_type_map_h

#include <fatal/portability.h>
#include <fatal/type/conditional.h>
#include <fatal/type/deprecated/transform.h>
#include <fatal/type/deprecated/type_list.h>
#include <fatal/type/deprecated/type_pair.h>
#include <fatal/type/deprecated/type_tag.h>

#include <type_traits>
#include <utility>
#include <typeinfo>

namespace fatal {

////////////////////////////////////////
// IMPLEMENTATION FORWARD DECLARATION //
////////////////////////////////////////

namespace detail {
namespace type_map_impl {

template <typename...> struct build_pair_list;
template <typename, typename...> struct concat;
template <typename, template <typename...> class, template <typename...> class>
struct transform;
template <
  typename, typename, template <typename...> class, template <typename...> class
>
struct transform_at;
template <typename, template <typename...> class, template <typename...> class>
struct invert_transform;
template <typename, typename, typename...> struct find;
template <typename, typename...> struct get;
template <template <typename...> class, typename> struct separate;
template <typename...> struct cluster;
struct visit_not_found {};
template <typename, typename> struct visit_visitor;
template <typename> struct binary_search_comparer;

} // namespace type_map_impl {
} // namespace detail {

//////////////
// type_map //
//////////////

/**
 * Type map for template metaprogramming.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: operations that would
 * mutate the map upon which they are performed actually create a new map.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
class type_map {
  // private to `type_map` so that `contains` will never have false negatives
  struct not_found_tag_impl {};

public:
  /**
   * The underlying type_list of type_pair<TKey, TMapped> used for
   *  implementing this map.
   */
  using contents = type_list<Args...>;

  /**
   * The size of this map.
   */
  static constexpr std::size_t size = sizeof...(Args);
  static_assert(size == contents::size, "size mismatch");

  /**
   * A boolean telling whether this map is empty or not.
   * This is the same as `type_map::size == 0`.
   */
  static constexpr bool empty = sizeof...(Args) == 0;
  static_assert(empty == contents::empty, "empty property mismatch");

  /**
   * A list of all the keys in this map. The order of the keys are guaranteed
   * to follow `contents`'s order.
   */
  using keys = typename contents::template transform<type_get_first>;

  /**
   * A list of all the mapped types in this map. The order of the mapped types
   * are guaranteed to follow `contents`'s order.
   */
  using mapped = typename contents::template transform<type_get_second>;

  // TODO: DOCUMENT AND TEST
  template <
    template <typename...> class TMappedTransform = identity,
    template <typename...> class TKeyTransform = identity
  >
  using transform = type_map<
    typename detail::type_map_impl::transform<
      Args, TKeyTransform, TMappedTransform
    >::type...
  >;

  // TODO: DOCUMENT AND TEST
  template <
    typename TKey,
    template <typename...> class TMappedTransform = identity,
    template <typename...> class TKeyTransform = identity
  >
  using transform_at = type_map<
    typename detail::type_map_impl::transform_at<
      TKey, Args, TKeyTransform, TMappedTransform
    >::type...
  >;

  // TODO: DOCUMENT AND TEST
  template <
    template <typename...> class TTransform,
    template <typename...> class TMappedTransform = identity,
    template <typename...> class TKeyTransform = identity
  >
  using apply = typename transform<TMappedTransform, TKeyTransform>
    ::contents::template apply<TTransform>;

  // TODO: DOCUMENT AND TEST
  template <typename... UArgs>
  using append = type_map<Args..., UArgs...>;

  // TODO: DOCUMENT AND TEST
  template <typename T>
  using concat = typename detail::type_map_impl::concat<T, Args...>::type;

  /**
   * Inverts the key and mapped types of this `type_map`.
   *
   * Optionally, it can apply a transformation to the key (`TKeyTransform`)
   * and mapped value (`TMappedTransform`) before inverting.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<float, long>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<bool, int>,
   *  //   type_pair<long, float>
   *  // >`
   *  using result1 = map::invert<>;
   *
   *  // yields `type_map<
   *  //   type_pair<Bar<bool>, Foo<int>>,
   *  //   type_pair<Bar<long>, Foo<float>>
   *  // >`
   *  template <typename> struct Foo {};
   *  template <typename> struct Bar {};
   *  using result2 = map::invert<Foo, Bar>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TKeyTransform = identity,
    template <typename...> class TMappedTransform = identity
  >
  using invert = type_map<
    typename detail::type_map_impl::invert_transform<
      Args, TKeyTransform, TMappedTransform
    >::type...
  >;

  /**
   * Finds the first pair with key `TKey` and returns the mapped type.
   *
   * If there's no pair in this map with key `TKey` then `TDefault` is returned
   * (defaults to `type_not_found_tag` when omitted).
   *
   * Example:
   *
   *  using map = type_map<type_pair<int, double>, type_pair<bool, long>>;
   *
   *  // yields `double`
   *  using result1 = map::find<int>;
   *
   *  // yields `type_not_found_tag`
   *  using result2 = map::find<float>;
   *
   *  // yields `void`
   *  using result3 = map::find<float, void>;
   */
  template <typename TKey, typename TDefault = type_not_found_tag>
  using find = typename detail::type_map_impl::find<
    TDefault, TKey, Args...
  >::type;

  /**
   * Finds the first pair with key `TKey` and returns the mapped type.
   *
   * If there's no pair in this map with key `TKey` then it fails to compile.
   *
   * Example:
   *
   *  using map = type_map<type_pair<int, double>, type_pair<bool, long>>;
   *
   *  // yields `double`
   *  using result1 = map::find<int>;
   *
   *  // fails to compile
   *  using result2 = map::get<float>;
   */
  template <typename TKey>
  using get = typename detail::type_map_impl::get<TKey, Args...>::type;

  /**
   * Finds the first pair whose key is accepted by the predicate `TPredicate`,
   * and returns the pair as a `type_pair<Key, Mapped>`.
   *
   * If there's no pair in this map whose key is accepted by the predicate
   * then `TDefault` is returned (defaults to `type_not_found_tag` when
   * omitted).
   *
   * Example:
   *
   *  using map = type_map<type_pair<int, double>, type_pair<bool, long>>;
   *
   *  template <typename TLHS>
   *  struct predicate {
   *    template <typename TRHS>
   *    using apply = std::is_same<TLHS, TRHS>;
   *  };
   *
   *  // yields `double`
   *  using result1 = map::search<predicate<int>::apply>;
   *
   *  // yields `type_not_found_tag`
   *  using result2 = map::search<predicate<float>::apply>;
   *
   *  // yields `void`
   *  using result3 = map::search<predicate<float>::apply, void>;
   */
  template <
    template <typename...> class TPredicate,
    typename TDefault = type_not_found_tag
  >
  using search = typename contents::template search<
    compose<type_get_first, TPredicate>::template apply,
    TDefault
  >;

  /**
   * Tells whether there is a mapping where T is the key.
   *
   * Example:
   *
   *  using map = type_map<type_pair<int, double>>;
   *
   *  // yields `true`
   *  map::contains<int>::value
   *
   *  // yields `false`
   *  map::contains<bool>::value
   */
  template <typename T>
  using contains = typename keys::template contains<T>;

  /**
   * TODO: FIX DOCS
   *
   * Inserts the given `TKey` and `TValue` pair before all elements of this map.
   *
   * There are two overloads, one accepting the key and mapped type
   * (push_front<TKey, TMapped>) and another accepting a `type_pair`
   * (push_front<type_pair<TKey, TMapped>>).
   *
   * Note that keys are not necessarily unique. Inserting an existing key
   * will create duplicate entries for it.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<float, double>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<short, long>,
   *  //   type_pair<int, bool>,
   *  //   type_pair<float, double>
   *  // >`
   *  using result1 = map::push_front<short, long>;
   *
   *  // yields `type_map<
   *  //   type_pair<short, long>,
   *  //   type_pair<int, bool>,
   *  //   type_pair<float, double>
   *  // >`
   *  using result2 = map::push_front<type_pair<short, long>>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using push_front = typename detail::type_map_impl::build_pair_list<UArgs...>
    ::type::template apply_back<fatal::type_map, Args...>;

  /**
   * Inserts the given `TKey` and `TValue` pair after all elements of this map.
   *
   * Note that keys are not necessarily unique. Inserting an existing key
   * will create duplicate entries for it.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<float, double>
   *  >;
   *
   *  // yields `type_map<
   *  //  type_pair<int, bool>,
   *  //  type_pair<float, double>,
   *  //  type_pair<short, long>
   *  // >`
   *  using result1 = map::push_back<short, long>;
   *
   *  // yields `type_map<
   *  //  type_pair<int, bool>,
   *  //  type_pair<float, double>,
   *  //  type_pair<short, long>,
   *  //  type_pair<void, unsigned>
   *  // >`
   *  using result2 = map::push_back<
   *    short, long,
   *    void, unsigned
   *  >;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using push_back = typename detail::type_map_impl::build_pair_list<UArgs...>
    ::type::template apply_front<fatal::type_map, Args...>;

  /**
   * TODO: IT SHOULD INSERT AFTER A SPECIFIC KEY, OR END OF MAP IF NOT FOUND
   *
   * Inserts a new mapping into the type_map, in no specified order.
   *
   * Note that keys are not necessarily unique. Inserting an existing key
   * will create duplicate entries for it. Should you need unique mappings,
   * use `replace` instead.
   *
   * Example:
   *
   *  using map = type_map<type_pair<int, double>>;
   *
   *  // yields `type_map<
   *  //  type_pair<int, double>,
   *  //  type_pair<float, long>
   *  // >`
   *  using result1 = map::insert<float, long>;
   *
   *  // yields `type_map<
   *  //  type_pair<int, double>,
   *  //  type_pair<float, long>,
   *  //  type_pair<void, unsigned>
   *  // >`
   *  using result2 = map::insert<
   *    float, long,
   *    void, unsigned
   *  >;
   */
  template <typename... UArgs>
  using insert = push_back<UArgs...>;

  /**
   * Inserts a new `TKey`, `TValue` mapping into the type_map in its sorted
   * position according to the order provided by `TLessComparer` when applied
   * to the keys.
   *
   * `TLessComparer` defaults to `comparison_transform::less_than` when omitted.
   *
   * Example:
   *
   *  template <int Value>
   *  using val = std::integral_constant<int, Value>;
   *
   *  using map = type_map<
   *    type_pair<val<0>, bool>,
   *    type_pair<val<3>, double>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<val<0>, bool>,
   *  //   type_pair<val<2>, long>,
   *  //   type_pair<val<3>, double>
   *  // >`
   *  using result = map::insert_sorted<
   *    comparison_transform::less_than, val<2>, long
   *  >;
   */
  template <
    typename TKey, typename TValue,
    template <typename...> class TLessComparer = comparison_transform::less_than
  >
  using insert_sorted = typename contents::template insert_sorted<
    type_pair<TKey, TValue>,
    type_get_first_comparer<TLessComparer>::template compare
  >::template apply<fatal::type_map>;

  /**
   * Inserts a new `type_pair` mapping into the type_map in its sorted position
   * according to the order provided by `TLessComparer` when applied to the
   * keys.
   *
   * `TLessComparer` defaults to `comparison_transform::less_than` when omitted.
   *
   * Example:
   *
   *  template <int Value>
   *  using val = std::integral_constant<int, Value>;
   *
   *  using map = type_map<
   *    type_pair<val<0>, bool>,
   *    type_pair<val<3>, double>
   *  > map;
   *
   *  // yields `type_map<
   *  //   type_pair<val<0>, bool>,
   *  //   type_pair<val<1>, int>,
   *  //   type_pair<val<3>, double>
   *  // >`
   *  using result = map::insert_pair_sorted<
   *    comparison_transform::less_than,
   *    type_pair<val<1>, int>
   *  >;
   */
  template <
    typename TPair,
    template <typename...> class TLessComparer = comparison_transform::less_than
  >
  using insert_pair_sorted = typename contents::template insert_sorted<
    TPair,
    type_get_first_comparer<TLessComparer>::template compare
  >::template apply<fatal::type_map>;

  /**
   * Replaces with `TMapped` the mapped type of all pairs with key `TKey`.
   *
   * Example:
   *
   *  // yields `type_map<type_pair<int, double>>`
   *  using result1 = type_map<type_pair<int, long>>::replace<int, double>;
   *
   *  // yields `type_map<
   *  //   type_pair<int, double>,
   *  //   type_pair<float, short>,
   *  //   type_pair<int, double>
   *  // >`
   *  using result2 = type_map<
   *    type_pair<int, long>,
   *    type_pair<float, short>,
   *    type_pair<int, bool>
   *  >::replace<int, double>;
   */
  template <typename TKey, typename TMapped>
  using replace = typename type_map::template transform_at<
    TKey,
    fixed_transform<TMapped>::template apply
  >;

  /**
   * Removes all entries where given types are the key from the type map.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<int, float>,
   *    type_pair<void, std::string>,
   *    type_pair<float, double>,
   *    type_pair<bool, bool>
   *  >;
   *
   *  // yields `type_map<type_pair<float, double>, type_pair<bool, bool>>`
   *  using result = map::remove<int, void>;
   */
  template <typename... UArgs>
  using remove = typename contents::template reject<
    compose<
      type_get_first,
      type_list<UArgs...>::template contains
    >::template apply
  >::template apply<fatal::type_map>;

  /**
   * Returns a pair with two `type_map`s. One (first) with the pairs whose keys
   * got accepted by the predicate and the other (second) with the pairs whose
   * keys weren't accepted by it.
   *
   * `TPredicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with a key from this map.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<int, float>,
   *    type_pair<void, std::string>,
   *    type_pair<float, double>,
   *    type_pair<bool, bool>
   *  >;
   *
   *  // yields `type_pair<
   *  //   type_map<
   *  //     type_pair<int, bool>,
   *  //     type_pair<int, float>,
   *  //     type_pair<bool, bool>
   *  //   >,
   *  //   type_map<
   *  //     type_pair<void, std::string>,
   *  //     type_pair<float, double>
   *  //   >
   *  // >`
   *  using result = map::separate<std::is_integral>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class TPredicate>
  using separate = typename detail::type_map_impl::separate<
    TPredicate, contents
  >::type;

  /**
   * Returns a `type_map` containing the pairs whose keys got accepted by
   * the predicate.
   *
   * `TPredicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with a key from this map.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<int, float>,
   *    type_pair<void, std::string>,
   *    type_pair<float, double>,
   *    type_pair<bool, bool>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<int, bool>,
   *  //   type_pair<int, float>,
   *  //   type_pair<bool, bool>
   *  // >`
   *  using result = map::filter<std::is_integral>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class TPredicate>
  using filter = typename separate<TPredicate>::first;

  /**
   * Returns a `type_map` containing the pairs whose keys did not get
   * accepted by the predicate.
   *
   * `TPredicate` is a std::integral_constant-like template whose value
   * evaluates to a boolean when fed with a key from this map.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<int, bool>,
   *    type_pair<int, float>,
   *    type_pair<void, std::string>,
   *    type_pair<float, double>,
   *    type_pair<bool, bool>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<void, std::string>,
   *  //   type_pair<float, double>
   *  // >`
   *  using result = map::reject<std::is_integral>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class TPredicate>
  using reject = typename separate<TPredicate>::second;

  /**
   * Sorts this `type_map` by key using the stable merge sort algorithm,
   * according to the given type comparer `TLessComparer`.
   *
   * `TLessComparer` must represent a total order relation between all types.
   *
   * The default comparer is `comparison_transform::less_than`, which sorts
   * in a non-decreasing order.
   *
   * Example:
   *
   *  using map = type_map<
   *    type_pair<T<0>, void>,
   *    type_pair<T<1>, short>,
   *    type_pair<T<4>, double>,
   *    type_pair<T<2>, bool>,
   *    type_pair<T<1>, int>,
   *    type_pair<T<3>, float>
   *  >;
   *
   *  // yields `type_map<
   *  //   type_pair<T<0>, void>,
   *  //   type_pair<T<1>, short>,
   *  //   type_pair<T<1>, int>,
   *  //   type_pair<T<2>, bool>,
   *  //   type_pair<T<3>, float>,
   *  //   type_pair<T<4>, double>
   *  // >`
   *  using result = map::sort<>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TLessComparer = comparison_transform::less_than
  >
  using sort = typename contents::template sort<
    type_get_first_comparer<TLessComparer>::template compare
  >::template apply<fatal::type_map>;

  template <
    template <typename...> class TKeyTransform = identity,
    template <typename...> class TMappedTransform = identity
  >
  using cluster = typename detail::type_map_impl::cluster<
    typename detail::type_map_impl::transform<
      Args, TKeyTransform, TMappedTransform
    >::type...
  >::type;

  template <
    template <typename...> class TPredicate, typename V, typename... VArgs
  >
  static constexpr std::size_t foreach_if(V &&visitor, VArgs &&...args) {
    return contents::template foreach_if<TPredicate>(
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  }

  template <typename V, typename... VArgs>
  static constexpr bool foreach(V &&visitor, VArgs &&...args) {
    return contents::foreach(
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  }

  template <typename TKey, typename TVisitor, typename... VArgs>
  static constexpr bool visit(TVisitor &&visitor, VArgs &&...args) {
    using mapped = find<TKey, detail::type_map_impl::visit_not_found>;

    return detail::type_map_impl::visit_visitor<TKey, mapped>::visit(
      std::forward<TVisitor>(visitor),
      std::forward<VArgs>(args)...
    );
  }

  /**
   * Performs a binary search on this map's keys (assumes the map is sorted),
   * comparing against the given `needle`.
   *
   * If a matching key is found, the visitor is called with the following
   * arguments:
   *  - an instance of `indexed_type_tag<
   *      type_pair<MatchingKey, MappedType>,
   *      Index
   *    >`
   *  - the `needle`
   *  - the list of additional arguments `args` given to
   *    the visitor
   *
   * in other words, with this general signature:
   *
   *  template <
   *    typename TKey, typename TMapped, std::size_t Index,
   *    typename TNeedle,
   *    typename... VArgs
   *  >
   *  void operator ()(
   *    indexed_type_tag<type_pair<TKey, TMapped>, Index>,
   *    TNeedle &&needle,
   *    VArgs &&...args
   *  );
   *
   * Returns `true` when found, `false` otherwise.
   *
   * The comparison is performed using the given `TComparer`'s method, whose
   * signature must follow this pattern:
   *
   *  template <typename TNeedle, typename TKey, std::size_t Index>
   *  static int compare(TNeedle &&needle, indexed_type_tag<TKey, Index>);
   *
   * which effectivelly compares `needle` against the key type `TKey`. The
   * result must be < 0, > 0 or == 0 if `needle` is, respectively, less than,
   * greather than or equal to `TKey`. `Index` is the position of the pair with
   * key `TKey` in this type map and can also be used in the comparison if
   * needed.
   *
   * `TComparer` defaults to `type_value_comparer` which compares an
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
   *  template <int n> using int_val = std::integral_constant<int, n>;
   *
   *  template <char c> using chr = std::integral_constant<char, c>;
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
   */
  template <typename TComparer = type_value_comparer>
  struct binary_search {
    /**
     * Performs a binary search for a key that
     * is an exact match of the `needle`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <char Key, char Mapped, std::size_t Index>
     *    void operator ()(
     *      indexed_type_tag<type_pair<chr<Key>, chr<Mapped>>, Index>,
     *      char needle
     *    ) {
     *      assert(needle == Key);
     *      std::cout << "key '" << needle << "' found at index " << Index
     *        << " mapping '" << Mapped << '\''
     *        << std::endl;
     *    };
     *  };
     *
     *  using map = type_map<
     *    type_pair<chr<'a'>, chr<'A'>>,
     *    type_pair<chr<'e'>, chr<'E'>>,
     *    type_pair<chr<'i'>, chr<'I'>>,
     *    type_pair<chr<'o'>, chr<'O'>>,
     *    type_pair<chr<'u'>, chr<'U'>>
     *  >;
     *
     *  // yields `false`
     *  map::binary_search::exact<cmp>('x', visitor());
     *
     *  // yields `true` and prints `"key 'i' found at index 2 mapping 'I'"`
     *  map::binary_search<cmp>::exact('i', visitor());
    */
    template <typename TNeedle, typename TVisitor, typename... VArgs>
    static constexpr bool exact(
      TNeedle &&needle, TVisitor &&visitor, VArgs &&...args
    ) {
      return contents::template binary_search<
        detail::type_map_impl::binary_search_comparer<TComparer>
      >::exact(
        std::forward<TNeedle>(needle),
        std::forward<TVisitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }

    /**
     * Performs a binary search for the greatest key that is less than
     * or equal to (<=) the `needle`. This is analogous to `std::lower_bound`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <int Key, int Mapped, std::size_t Index>
     *    void operator ()(
     *      indexed_type_tag<type_pair<int_val<Key>, int_val<Mapped>>, Index>,
     *      int needle
     *    ) {
     *      assert(Key <= needle);
     *      std::cout << needle << "'s lower bound " << n
     *        << " found at index " << Index << " mapping " << Mapped
     *        << std::endl;
     *    };
     *  };
     *
     *  using map = type_map<
     *    type_pair<int_val<10>, int_val<100>>,
     *    type_pair<int_val<30>, int_val<300>>,
     *    type_pair<int_val<50>, int_val<500>>,
     *    type_pair<int_val<70>, int_val<700>>
     *  >;
     *
     *  // yields `false`
     *  map::binary_search<cmp>::lower_bound(5, visitor());
     *
     *  // yields `true` and prints
     *  // `"11's lower bound 10 found at index 0 mapping 100"`
     *  map::binary_search<cmp>::lower_bound(11, visitor());
     *
     *  // yields `true` and prints
     *  // `"68's lower bound 50 found at index 2 mapping 500"`
     *  map::binary_search<cmp>::lower_bound(68, visitor());
     *
     *  // yields `true` and prints
     *  // `"70's lower bound 70 found at index 3 mapping 700"`
     *  map::binary_search<cmp>::lower_bound(70, visitor());
     */
    template <typename TNeedle, typename TVisitor, typename... VArgs>
    static constexpr bool lower_bound(
      TNeedle &&needle, TVisitor &&visitor, VArgs &&...args
    ) {
      return contents::template binary_search<
        detail::type_map_impl::binary_search_comparer<TComparer>
      >::lower_bound(
        std::forward<TNeedle>(needle),
        std::forward<TVisitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }

    /**
     * Performs a binary search for the least key that is greater than (>)
     * the `needle`. This is analogous to `std::upper_bound`.
     *
     * Refer to the `binary_search` documentation above for more details.
     *
     * Note: this is a runtime facility.
     *
     * Example:
     *
     *  struct visitor {
     *    template <int Key, int Mapped, std::size_t Index>
     *    void operator ()(
     *      indexed_type_pair_tag<int_val<Key>, int_val<Mapped>, Index>,
     *      int needle
     *    ) {
     *      assert(Key > needle);
     *      std::cout << needle << "'s upper bound " << n
     *        << " found at index " << Index << " mapping " << Mapped
     *        << std::endl;
     *    };
     *  };
     *
     *  using list = int_seq<10, 30, 50, 70>;
     *
     *  // yields `false`
     *  map::binary_search<cmp>::upper_bound(70, visitor());
     *
     *  // yields `true` and prints
     *  // `"5's upper bound 10 found at index 0 mapping 100"`
     *  map::binary_search<cmp>::upper_bound(5, visitor());
     *
     *  // yields `true` and prints
     *  // `"31's upper bound 50 found at index 2 mapping 500"`
     *  map::binary_search<cmp>::upper_bound(31, visitor());
     */
    template <typename TNeedle, typename TVisitor, typename... VArgs>
    static constexpr bool upper_bound(
      TNeedle &&needle, TVisitor &&visitor, VArgs &&...args
    ) {
      return contents::template binary_search<
        detail::type_map_impl::binary_search_comparer<TComparer>
      >::upper_bound(
        std::forward<TNeedle>(needle),
        std::forward<TVisitor>(visitor),
        std::forward<VArgs>(args)...
      );
    }
  };
};

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

#if FATAL_CPLUSPLUS < 201703L
template <typename... Args> constexpr std::size_t type_map<Args...>::size;
template <typename... Args> constexpr bool type_map<Args...>::empty;
#endif

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

/////////////////////
// type_get_traits //
/////////////////////

/**
 * Specialization of `type_get_traits` so that `type_get` supports `type_map`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct type_get_traits<type_map<Args...>> {
  template <std::size_t Index>
  using supported = std::integral_constant<bool, (Index < sizeof...(Args))>;

  template <std::size_t Index>
  using type = typename type_map<Args...>::contents::template at<Index>;
};

////////////////////
// build_type_map //
////////////////////

/**
 * Convenience mechanism to construct new type maps.
 *
 * It receives a flat list of `TKey, TMapped` types.
 *
 * Example:
 *
 *  // yields an empty `type_map<>`
 *  using empty = build_type_map<>;
 *
 *  // yields `type_map<type_pair<int, double>>`
 *  using result1 = build_type_map<int, double>;
 *
 *  // yields
 *  //   type_map<
 *  //     type_pair<int, bool>,
 *  //     type_pair<double, float>,
 *  //     type_pair<void, std::string>,
 *  //   > map
 *  using result2 = build_type_map<int, bool, double, float, void, std::string>;
 */
template <typename... Args>
using build_type_map = typename detail::type_map_impl::build_pair_list<Args...>
  ::type::template apply<type_map>;

///////////////////
// type_map_from //
///////////////////

/**
 * Creates a `type_map` out of a `type_list`.
 *
 * For each element `T` of the list, a corresponding key/value pair will be
 * added to the map, by applying an optional `TKeyTransform` to obtain the key.
 * An optional transform can also be applied to `T` to obtain the mapped value.
 * The default transform uses `T` itself.
 *
 * Example:
 *
 *  using list = type_list<int, bool, double>;
 *
 *  template <typename> struct Foo {};
 *  template <typename> struct Bar {};
 *
 *  // yield `type_map<
 *  //    type_pair<Foo<int>, int>,
 *  //    type_pair<Foo<bool>, bool>,
 *  //    type_pair<Foo<double>, double>
 *  // >`
 *  using result1 = type_map_from<Foo>::list<list>;
 *
 *  // yield `type_map<
 *  //    type_pair<Foo<int>, Bar<int>>,
 *  //    type_pair<Foo<bool>, Bar<bool>>,
 *  //    type_pair<Foo<double>, Bar<double>>
 *  // >`
 *  using result2 = type_map_from<Foo, Bar>::list<list>;
 *
 *  // yield `type_map<
 *  //    type_pair<int, int>,
 *  //    type_pair<bool, bool>,
 *  //    type_pair<double, double>
 *  // >`
 *  using result3 = type_map_from<>::list<list>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TKeyTransform = identity,
  template <typename...> class TValueTransform = identity
>
struct type_map_from {
  template <typename... UArgs>
  using args = type_map<
    typename type_pair_from<
      TKeyTransform, TValueTransform
    >::template type<UArgs>...
  >;

  template <typename UList>
  using list = typename UList::template apply<args>;
};

/////////////////////
// clustered_index //
/////////////////////

namespace detail {

template <
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
struct clustered_index_impl {
  template <typename TList>
  using apply = typename type_map_from<TTransform>
    ::template list<TList>
    ::template cluster<>
    ::template transform<
      clustered_index_impl<TTransforms...>::template apply
    >;
};

template <template <typename...> class TTransform>
struct clustered_index_impl<TTransform> {
  template <typename TList>
  using apply = typename type_map_from<TTransform>::template list<TList>;
};

} // namespace detail {

/////////////////////
// clustered_index //
/////////////////////

template <
  typename TList,
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
using clustered_index = typename detail::clustered_index_impl<
  TTransform, TTransforms...
>::template apply<TList>;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

/////////////////////////
// recursive_type_sort //
/////////////////////////

/**
 * Specialization of `recursive_type_sort` for `type_map`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... T, std::size_t Depth>
struct recursive_type_sort_impl<type_map<T...>, Depth> {
  using type = conditional<
    (Depth > 0),
    typename type_map<T...>
      ::template sort<>
      ::template transform<
        recursive_type_sort<Depth - 1>::template apply
      >,
    type_map<T...>
  >;
};

namespace detail {
namespace type_map_impl {

/////////////////////
// build_pair_list //
/////////////////////

template <typename TKey, typename TValue, typename... Args>
struct build_pair_list<TKey, TValue, Args...> {
  using type = typename build_pair_list<Args...>::type::template push_front<
    type_pair<TKey, TValue>
  >;
};

template <>
struct build_pair_list<> {
  using type = type_list<>;
};

////////////
// concat //
////////////

template <
  template <typename...> class T, typename... Suffix, typename... Prefix
>
struct concat<T<Suffix...>, Prefix...> {
  using type = type_map<Prefix..., Suffix...>;
};

///////////////
// transform //
///////////////

template <
  typename TPair,
  template <typename...> class TKeyTransform,
  template <typename...> class TMappedTransform
>
struct transform {
  using type = typename TPair::template transform<
    TKeyTransform, TMappedTransform
  >;
};

//////////////////
// transform_at //
//////////////////

template <
  typename TKey,
  typename TPair,
  template <typename...> class TKeyTransform,
  template <typename...> class TMappedTransform
>
struct transform_at {
  using type = conditional<
    std::is_same<TKey, type_get_first<TPair>>::value,
    typename TPair::template transform<TKeyTransform, TMappedTransform>,
    TPair
  >;
};

////////////
// invert //
////////////

template <
  typename TPair,
  template <typename...> class TKeyTransform,
  template <typename...> class TMappedTransform
>
struct invert_transform {
  using type = typename TPair::template transform<
    TKeyTransform, TMappedTransform
  >::invert;
};

//////////
// find //
//////////

template <typename TDefault, typename TKey>
struct find<TDefault, TKey> {
  using type = TDefault;
};

template <typename TDefault, typename TKey, typename TValue, typename... Args>
struct find<TDefault, TKey, type_pair<TKey, TValue>, Args...> {
  using type = TValue;
};

template <typename TDefault, typename TKey, typename T, typename... Args>
struct find<TDefault, TKey, T, Args...> {
  using type = typename find<TDefault, TKey, Args...>::type;
};

/////////
// get //
/////////

template <typename TKey, typename TValue, typename... Args>
struct get<TKey, type_pair<TKey, TValue>, Args...> {
  using type = TValue;
};

template <typename TKey, typename T, typename... Args>
struct get<TKey, T, Args...> {
  using type = typename get<TKey, Args...>::type;
};

//////////////
// separate //
//////////////

template <template <typename...> class TPredicate, typename TList>
struct separate {
  using separated = typename TList::template separate<
    compose<type_get_first, TPredicate>::template apply
  >;

  using type = type_pair<
    typename type_get_first<separated>::template apply<type_map>,
    typename type_get_second<separated>::template apply<type_map>
  >;
};

/////////////
// cluster //
/////////////

template <typename T>
struct cluster_transform {
  template <typename U>
  using add = typename U::template push_back<T>;
};

template <typename... Args>
struct cluster {
  using type = type_map<>;
};

template <typename T, typename... Args>
struct cluster<T, Args...> {
  using tail = typename cluster<Args...>::type;
  using key = type_get_first<T>;
  using mapped = type_get_second<T>;

  using type = conditional<
    tail::template contains<key>::value,
    typename tail::template transform_at<
      key,
      cluster_transform<mapped>::template add
    >,
    typename tail::template push_back<key, type_list<mapped>>
  >;
};

///////////
// visit //
///////////

template <typename TKey, typename TMapped>
struct visit_visitor {
  template <typename TVisitor, typename... VArgs>
  static constexpr bool visit(TVisitor &&visitor, VArgs &&...args) {
    // comma operator needed due to C++11's constexpr restrictions
    return visitor(
      type_pair<TKey, TMapped>(),
      std::forward<VArgs>(args)...
    ), true;
  }
};

template <typename TKey>
struct visit_visitor<TKey, visit_not_found> {
  template <typename TVisitor, typename... VArgs>
  static constexpr bool visit(TVisitor &&, VArgs &&...) { return false; }
};

///////////////////
// binary_search //
///////////////////

template <typename TComparer>
struct binary_search_comparer {
  template <typename TNeedle, typename TKey, typename TValue, std::size_t Index>
  static constexpr int compare(
    TNeedle &&needle,
    indexed_type_tag<type_pair<TKey, TValue>, Index>
  ) {
    return TComparer::compare(
      std::forward<TNeedle>(needle),
      indexed_type_tag<TKey, Index>{}
    );
  }
};

} // namespace type_map_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_deprecated_type_map_h
