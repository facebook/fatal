/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/tagged_tuple.h>

#include <fatal/test/driver.h>

struct W {};
struct X {};
struct Y {};
struct Z {};

namespace fatal {

template <typename... TTags, typename... TData>
void check_tags(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  typedef tagged_tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > actual;

  FATAL_EXPECT_SAME<
    tuple_tags<TTags...>,
    typename actual::tags
  >();
}

TEST(tagged_tuple, tags) {
  check_tags();
  check_tags<Y>("hello");
  check_tags<Z, X>('=', "world");
  check_tags<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_tuple_type(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  typedef tagged_tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > actual;

  FATAL_EXPECT_SAME<
    std::tuple<typename std::decay<TData>::type...>,
    typename actual::tuple_type
  >();
}

TEST(tagged_tuple, tuple_type) {
  check_tuple_type();
  check_tuple_type<Y>("hello");
  check_tuple_type<Z, X>('=', "world");
  check_tuple_type<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_default_ctor(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual;

  EXPECT_NE(nullptr, std::addressof(actual));
}

TEST(tagged_tuple, default_ctor) {
  check_default_ctor();
  check_default_ctor<Y>("hello");
  check_default_ctor<Z, X>('=', "world");
  check_default_ctor<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_forwarding_ctor(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  EXPECT_NE(nullptr, std::addressof(actual));
}

TEST(tagged_tuple, forwarding_ctor) {
  check_forwarding_ctor();
  check_forwarding_ctor<Y>("hello");
  check_forwarding_ctor<Z, X>('=', "world");
  check_forwarding_ctor<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_forwarding_ctor_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::make_tuple(std::forward<TData>(data)...)
  );

  EXPECT_NE(nullptr, std::addressof(actual));
}

TEST(tagged_tuple, forwarding_ctor_tuple) {
  check_forwarding_ctor_tuple();
  check_forwarding_ctor_tuple<Y>("hello");
  check_forwarding_ctor_tuple<Z, X>('=', "world");
  check_forwarding_ctor_tuple<X, Y, Z>(5.6, 10, true);
}

template <typename...> struct check_get_helper;

template <typename T, typename... Args>
struct check_get_helper<T, Args...> {
  template <typename TExpected, typename TActual>
  static void check(TExpected &&expected, TActual &&actual) {
    typedef typename std::decay<TExpected>::type tuple;
    typedef std::tuple_size<tuple> size;
    typedef std::integral_constant<std::size_t, sizeof...(Args) + 1> tail_size;
    static_assert(tail_size::value <= size::value, "out of bounds");
    typedef std::integral_constant<
      std::size_t, size::value - tail_size::value
    > index;

    EXPECT_EQ(std::get<index::value>(expected), actual.template get<T>());

    check_get_helper<Args...>::check(
      std::forward<TExpected>(expected),
      std::forward<TActual>(actual)
    );
  }
};

template <>
struct check_get_helper<> {
  template <typename TExpected, typename TActual>
  static void check(TExpected &&, TActual &&) {}
};

template <typename... TTags, typename... TData>
void check_get_const(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  tagged_tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  check_get_helper<TTags...>::check(tuple, actual);
}

TEST(tagged_tuple, get_const) {
  check_get_const();
  check_get_const<Y>("hello");
  check_get_const<Z, X>('=', "world");
  check_get_const<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_get(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  check_get_helper<TTags...>::check(tuple, actual);
}

TEST(tagged_tuple, get) {
  check_get();
  check_get<Y>("hello");
  check_get<Z, X>('=', "world");
  check_get<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_tuple_const(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  tagged_tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  EXPECT_EQ(tuple, actual.tuple());
}

TEST(tagged_tuple, tuple_const) {
  check_tuple_const();
  check_tuple_const<Y>("hello");
  check_tuple_const<Z, X>('=', "world");
  check_tuple_const<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  EXPECT_EQ(tuple, actual.tuple());
}

TEST(tagged_tuple, tuple) {
  check_tuple();
  check_tuple<Y>("hello");
  check_tuple<Z, X>('=', "world");
  check_tuple<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_paired_tagged_tuple(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  typedef tagged_tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > expected;

  typedef typename type_list<TTags...>::template zip<
    typename std::decay<TData>::type...
  >::template apply<build_tagged_tuple> actual;

  FATAL_EXPECT_SAME<
    expected,
    actual
  >();
}

TEST(build_tagged_tuple, build_tagged_tuple) {
  check_paired_tagged_tuple();
  check_paired_tagged_tuple<Y>("hello");
  check_paired_tagged_tuple<Z, X>('=', "world");
  check_paired_tagged_tuple<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_make_tagged_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(data...);
  auto const actual = make_tagged_tuple<TTags...>(std::forward<TData>(data)...);

  FATAL_EXPECT_SAME<
    tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  EXPECT_EQ(tuple, actual.tuple());
}

TEST(make_tagged_tuple, make_tagged_tuple) {
  check_make_tagged_tuple();
  check_make_tagged_tuple<Y>("hello");
  check_make_tagged_tuple<Z, X>('=', "world");
  check_make_tagged_tuple<X, Y, Z>(5.6, 10, true);
}

template <typename... TTags, typename... TData>
void check_make_tagged_tuple_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(std::forward<TData>(data)...);
  auto const actual = make_tagged_tuple<TTags...>(tuple);

  FATAL_EXPECT_SAME<
    tagged_tuple<type_pair<TTags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  EXPECT_EQ(tuple, actual.tuple());
}

TEST(make_tagged_tuple, make_tagged_tuple_tuple) {
  check_make_tagged_tuple_tuple();
  check_make_tagged_tuple_tuple<Y>("hello");
  check_make_tagged_tuple_tuple<Z, X>('=', "world");
  check_make_tagged_tuple_tuple<X, Y, Z>(5.6, 10, true);
}

} // namespace fatal {
