/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/list.h>
#include <fatal/type/map.h>

#include <tuple>
#include <utility>

namespace fatal {

template <typename... TTags>
struct tuple_tags {
  /**
   * A `type_list` of tags.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  typedef type_list<TTags...> tags;

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
  using index_of = typename tags::template index_of<TTag>;

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
   *  typedef tags::tag_map<tuple> result;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TTuple>
  using tag_map = type_map<
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
};

} // namespace fatal {
