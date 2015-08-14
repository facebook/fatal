/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_tuple_tags_h
#define FATAL_INCLUDE_fatal_container_tuple_tags_h

#include <fatal/type/list.h>
#include <fatal/type/map.h>
#include <fatal/type/reflect_template.h>

#include <tuple>
#include <utility>

namespace fatal {
namespace detail {
namespace tuple_tags_impl {

struct foreach_visitor {
  template <
    typename TTag, std::size_t Index,
    typename TTuple, typename V, typename... VArgs
  >
  void operator ()(
    indexed_type_tag<TTag, Index> tag,
    TTuple &&tuple, V &&visitor, VArgs &&...args
  ) const {
    visitor(tag, std::get<Index>(tuple), args...);
  }
};

} // namespace tuple_tags_impl {
} // namespace detail {

template <typename... TTags>
struct tuple_tags {
  /**
   * A `type_list` of tags.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using list = type_list<TTags...>;

  /**
   *  TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTag>
  using supports = typename list::template contains<TTag>;

  /**
   * Gets the index associated with a given tag.
   *
   * Example:
   *
   *  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
   *  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);
   *
   *  // yields `10`
   *  auto result1 = std::get<tags::index_of<Foo>::value>(tuple);
   *
   *  // yields `5.6`
   *  auto result2 = std::get<tags::index_of<Bar>::value>(tuple);
   *
   *  // yields `true`
   *  auto result3 = std::get<tags::index_of<Baz>::value>(tuple);
   *
   *  // yields `999`
   *  auto result4 = std::get<tags::index_of<Gaz>::value>(tuple);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTag>
  using index_of = typename list::template checked_index_of<TTag>;

  /**
   * Gets the type of the tuple's element associated with a given tag.
   *
   * Example:
   *
   *  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
   *  typedef std::tuple<int, double, bool, long> tuple;
   *
   *  // yields `int`
   *  typedef tags::type_of<Foo, tuple> result1;
   *
   *  // yields `double`
   *  typedef tags::type_of<Bar, tuple> result2;
   *
   *  // yields `bool`
   *  typedef tags::type_of<Baz, tuple> result3;
   *
   *  // yields `long`
   *  typedef tags::type_of<Gaz, tuple> result4;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTag, typename TTuple>
  using type_of = typename std::tuple_element<
    index_of<TTag>::value, TTuple
  >::type;

  /**
   * A `type_map` of tags to the type of the tuple's element associated with it.
   *
   * Example:
   *
   *  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
   *  typedef std::tuple<int, double, bool, long> tuple;
   *
   *  // yields `type_map<
   *  //    type_pair<Foo, int>,
   *  //    type_pair<Bar, double>,
   *  //    type_pair<Baz, bool>,
   *  //    type_pair<Gaz, long>
   *  // >`
   *  typedef tags::map<tuple> result;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTuple>
  using map = type_map<
    type_pair<TTags, type_of<TTags, TTuple>>...
  >;

  /**
   * Gets the tuple's element associated with a given tag.
   *
   * Example:
   *
   *  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
   *  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);
   *
   *  // yields `10`
   *  auto result1 = tags::get<Foo>(tuple);
   *
   *  // yields `5.6`
   *  auto result2 = tags::get<Bar>(tuple);
   *
   *  // yields `true`
   *  auto result3 = tags::get<Baz>(tuple);
   *
   *  // yields `999`
   *  auto result4 = tags::get<Gaz>(tuple);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTag, typename TTuple>
  static auto get(TTuple &&tuple)
    -> decltype(std::get<index_of<TTag>::value>(std::forward<TTuple>(tuple)))
  {
    return std::get<index_of<TTag>::value>(std::forward<TTuple>(tuple));
  }

  /**
   * Calls the given visitor for each element in the tuple whose tag is
   * accepted by the given predicate.
   *
   * The first parameter given to the visitor is `indexed_type_tag`
   * with the tag and its index, followed by `args`.
   *
   * The predicate must accept a tag as its single template parameter
   * and evaluate to a `std::integral_constant<bool>` like type with
   * the result. A value of `true` means the element should be visited.
   * A value of `false` means it should be ignored.
   *
   * This function returns the amount of elements visited (i.e.: the amount of
   * calls to the visitor or the amount of tags accepted by the predicate).
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  struct visitor {
   *    template <typename TTag, std::size_t Index, typename T>
   *    void operator ()(indexed_type_tag<TTag, Index>, T &&element) const {
   *      std::cout << "visiting '" << element << '\'' << std::endl;
   *    }
   *  };
   *
   *  template <std::size_t Id>
   *  using tag = std::integral_constant<std::size_t, Id>;
   *
   *  using tags = tuple_tags<tag<0>, tag<1>, tag<2>>;
   *
   *  template <typename T>
   *  using predicate = std::integral_constant<bool, T::value % 2 == 0>;
   *
   *  auto tuple = std::make_tuple("hello", "world", '!');
   *
   *  // yields `2` and prints `
   *  //  visiting 'hello'
   *  //  visiting '!'
   *  // `
   *  auto result = tags::foreach_if<predicate>(tuple, visitor());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TPredicate,
    typename TTuple, typename V, typename... VArgs
  >
  static constexpr std::size_t foreach_if(
    TTuple &&tuple, V &&visitor, VArgs &&...args
  ) {
    return list::template foreach_if<TPredicate>(
      detail::tuple_tags_impl::foreach_visitor(),
      std::forward<TTuple>(tuple),
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };

  /**
   * Calls the given visitor for each element in the tuple.
   *
   * The first parameter given to the visitor is `indexed_type_tag`
   * with the tag and its index. The second parameter is the tuple
   * element, followed by `args`, if any.
   *
   * This function returns `true` if the tuple has at least one element
   * and `false` otherwise (i.e.: tells whether the visitor has been
   * called at least once or not).
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  struct visitor {
   *    template <typename TTag, std::size_t Index, typename T>
   *    void operator ()(indexed_type_tag<TTag, Index>, T &&element) const {
   *      std::cout << "visiting '" << element << '\'' << std::endl;
   *    }
   *  };
   *
   *  template <std::size_t Id>
   *  using tag = std::integral_constant<std::size_t, Id>;
   *
   *  using tags = tuple_tags<tag<0>, tag<1>, tag<2>>;
   *
   *  auto tuple = std::make_tuple("hello", "world", '!');
   *
   *  // yields `true` and prints `
   *  //  visiting 'hello'
   *  //  visiting 'world'
   *  //  visiting '!'
   *  // `
   *  auto result = tags::foreach(tuple, visitor());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTuple, typename V, typename... VArgs>
  static constexpr bool foreach(TTuple &&tuple, V &&visitor, VArgs &&...args) {
    return list::foreach(
      detail::tuple_tags_impl::foreach_visitor(),
      std::forward<TTuple>(tuple),
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };
};

/**
 * Derives a `tuple_tags` type out of the given type's template parameters.
 *
 * An optional transform for the tags can be given. If no transform is given,
 * the tags are used as is.
 *
 * Example:
 *
 *  // `yields tuple_tags<>`
 *  using result1 = tuple_tags_from<std::tuple<>>;
 *
 *  // `yields tuple_tags<int, double>`
 *  using result2 = tuple_tags_from<std::tuple<int, double>>;
 *
 *  template <typename> struct Tag {};
 *
 *  // `yields tuple_tags<Tag<int>, Tag<double>>`
 *  using result3 = tuple_tags_from<std::tuple<int, double>, Tag>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename T,
  template <typename> class TTagTransform = fatal::identity_transform
>
using tuple_tags_from = typename fatal::reflect_template<T>::types
  ::template transform<TTagTransform>
  ::template apply<tuple_tags>;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_tuple_tags_h
