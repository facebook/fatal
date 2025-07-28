/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/tuple_tags.h>

#include <fatal/test/driver.h>

namespace fatal {

struct Foo {};
struct Bar {};
struct Baz {};
struct Gaz {};

template <typename> struct Tag {};

template <std::size_t Id>
using itag = std::integral_constant<std::size_t, Id>;

FATAL_TEST(tuple_tags, tags) {
  using tags = tuple_tags<Foo, Bar, Baz, Gaz>;

  FATAL_EXPECT_SAME<list<Foo, Bar, Baz, Gaz>, tags::list>();
}

FATAL_TEST(tuple_tags, map) {
  using tags = tuple_tags<Foo, Bar, Baz, Gaz>;
  using tuple = std::tuple<int, double, bool, long>;

  FATAL_EXPECT_SAME<
    list<
      pair<Foo, int>,
      pair<Bar, double>,
      pair<Baz, bool>,
      pair<Gaz, long>
    >,
    tags::map<tuple>
  >();
}

FATAL_TEST(tuple_tags, index_of) {
  using tags = tuple_tags<Foo, Bar, Baz, Gaz>;
  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);

  FATAL_EXPECT_EQ(10, std::get<tags::index_of<Foo>::value>(tuple));
  FATAL_EXPECT_EQ(5.6, std::get<tags::index_of<Bar>::value>(tuple));
  FATAL_EXPECT_EQ(true, std::get<tags::index_of<Baz>::value>(tuple));
  FATAL_EXPECT_EQ(999, std::get<tags::index_of<Gaz>::value>(tuple));
}

FATAL_TEST(tuple_tags, type_of) {
  using tags = tuple_tags<Foo, Bar, Baz, Gaz>;
  using tuple = std::tuple<int, double, bool, long>;

  FATAL_EXPECT_SAME<int, tags::type_of<Foo, tuple>>();
  FATAL_EXPECT_SAME<double, tags::type_of<Bar, tuple>>();
  FATAL_EXPECT_SAME<bool, tags::type_of<Baz, tuple>>();
  FATAL_EXPECT_SAME<long, tags::type_of<Gaz, tuple>>();
}

FATAL_TEST(tuple_tags, get) {
  using tags = tuple_tags<Foo, Bar, Baz, Gaz>;
  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);

  FATAL_EXPECT_EQ(10, tags::get<Foo>(tuple));
  FATAL_EXPECT_EQ(5.6, tags::get<Bar>(tuple));
  FATAL_EXPECT_EQ(true, tags::get<Baz>(tuple));
  FATAL_EXPECT_EQ(999, tags::get<Gaz>(tuple));
}

struct foreach_visitor {
  template <typename TTag, std::size_t Index, typename T>
  void operator ()(
    indexed<TTag, Index>,
    T &&element,
    std::vector<std::size_t> &indexes,
    std::vector<std::string> &elements
  ) const {
    indexes.push_back(Index);
    elements.push_back(element);
  }
};

template <typename T>
using is_even_predicate = std::integral_constant<bool, T::value % 2 == 0>;

FATAL_TEST(tuple_tags, foreach) {
  using tags = tuple_tags<itag<0>, itag<1>, itag<2>>;

  auto tuple = std::make_tuple("hello", "world", "!");

  std::vector<std::size_t> indexes;
  std::vector<std::string> elements;

  FATAL_EXPECT_TRUE(tags::foreach(tuple, foreach_visitor(), indexes, elements));

  FATAL_EXPECT_EQ((std::vector<std::size_t>{0, 1, 2}), indexes);
  FATAL_EXPECT_EQ((std::vector<std::string>{"hello", "world", "!"}), elements);
}

FATAL_TEST(tuple_tags, tuple_tags_from) {
  FATAL_EXPECT_SAME<tuple_tags<>, tuple_tags_from<std::tuple<>>>();

  FATAL_EXPECT_SAME<
    tuple_tags<int, double>,
    tuple_tags_from<std::tuple<int, double>>
  >();

  FATAL_EXPECT_SAME<
    tuple_tags<Tag<int>, Tag<double>>,
    tuple_tags_from<std::tuple<int, double>, Tag>
  >();
}

} // namespace fatal {
