/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/find.h>
#include <fatal/type/foreach.h>
#include <fatal/type/list.h>
#include <fatal/type/reflect_template.h>
#include <fatal/type/search.h>

#include <tuple>
#include <utility>

namespace fatal {
namespace detail {
namespace tuple_tags_impl {

struct foreach_visitor {
  template <
    typename Tag, std::size_t Index,
    typename Tuple, typename V, typename... VArgs
  >
  void operator ()(
    indexed<Tag, Index> tag,
    Tuple &&tuple, V &&visitor, VArgs &&...args
  ) const {
    visitor(tag, std::get<Index>(tuple), args...);
  }
};

} // namespace tuple_tags_impl {
} // namespace detail {

template <typename... Tags>
struct tuple_tags {
  /**
   * A `list` of tags.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using list = fatal::list<Tags...>;

  /**
   *  TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Tag>
  using supports = contains<list, Tag>;

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
  template <typename Tag>
  using index_of = index_of<list, Tag>;

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
  template <typename Tag, typename Tuple>
  using type_of = typename std::tuple_element<
    index_of<Tag>::value, Tuple
  >::type;

  /**
   * A `map` of tags to the type of the tuple's element associated with it.
   *
   * Example:
   *
   *  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
   *  typedef std::tuple<int, double, bool, long> tuple;
   *
   *  // yields `list<
   *  //    pair<Foo, int>,
   *  //    pair<Bar, double>,
   *  //    pair<Baz, bool>,
   *  //    pair<Gaz, long>
   *  // >`
   *  typedef tags::map<tuple> result;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Tuple>
  using map = fatal::list<
    pair<Tags, type_of<Tags, Tuple>>...
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
  template <typename Tag, typename Tuple>
  static auto get(Tuple &&tuple)
    -> decltype(std::get<index_of<Tag>::value>(std::forward<Tuple>(tuple)))
  {
    return std::get<index_of<Tag>::value>(std::forward<Tuple>(tuple));
  }

  /**
   * Calls the given visitor for each element in the tuple.
   *
   * The first parameter given to the visitor is `indexed`
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
   *    template <typename Tag, std::size_t Index, typename T>
   *    void operator ()(indexed<Tag, Index>, T &&element) const {
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
  template <typename Tuple, typename V, typename... VArgs>
  static constexpr bool foreach(Tuple &&tuple, V &&visitor, VArgs &&...args) {
    return fatal::foreach<list>(
      detail::tuple_tags_impl::foreach_visitor(),
      std::forward<Tuple>(tuple),
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    ), !empty<list>::value;
  }
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
template <typename T, template <typename> class TagTransform = identity>
using tuple_tags_from = typename reflect_template<T>::types
  ::template transform<TagTransform>
  ::template apply<tuple_tags>;

} // namespace fatal {
