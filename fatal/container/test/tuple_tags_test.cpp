/*
 *  Copyright (c) 2014, Facebook, Inc.
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

TEST(tuple_tags, tags) {
  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;

  expect_same<type_list<Foo, Bar, Baz, Gaz>, tags::tags>();
}

TEST(tuple_tags, tag_map) {
  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
  typedef std::tuple<int, double, bool, long> tuple;

  expect_same<
    type_map<
      type_pair<Foo, int>,
      type_pair<Bar, double>,
      type_pair<Baz, bool>,
      type_pair<Gaz, long>
    >,
    tags::tag_map<tuple>
  >();
}

TEST(tuple_tags, index_of) {
  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);

  EXPECT_EQ(10, std::get<tags::index_of<Foo>::value>(tuple));
  EXPECT_EQ(5.6, std::get<tags::index_of<Bar>::value>(tuple));
  EXPECT_EQ(true, std::get<tags::index_of<Baz>::value>(tuple));
  EXPECT_EQ(999, std::get<tags::index_of<Gaz>::value>(tuple));
}

TEST(tuple_tags, type_of) {
  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
  typedef std::tuple<int, double, bool, long> tuple;

  expect_same<int, tags::type_of<Foo, tuple>>();
  expect_same<double, tags::type_of<Bar, tuple>>();
  expect_same<bool, tags::type_of<Baz, tuple>>();
  expect_same<long, tags::type_of<Gaz, tuple>>();
}

TEST(tuple_tags, get) {
  typedef tuple_tags<Foo, Bar, Baz, Gaz> tags;
  std::tuple<int, double, bool, long> tuple(10, 5.6, true, 999);

  EXPECT_EQ(10, tags::get<Foo>(tuple));
  EXPECT_EQ(5.6, tags::get<Bar>(tuple));
  EXPECT_EQ(true, tags::get<Baz>(tuple));
  EXPECT_EQ(999, tags::get<Gaz>(tuple));
}

} // namespace fatal {
