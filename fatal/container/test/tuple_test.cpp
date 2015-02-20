/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/tuple.h>

#include <fatal/test/driver.h>

#include <chrono>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

struct W {};
struct X {};
struct Y {};
struct Z {};

namespace fatal {

#define CHECK_IMPL(Fn, ...) \
  do { \
    Fn(); \
    Fn<Y>(std::string("hello")); \
    Fn<Z, X>('=', std::string("world")); \
    Fn<X, Y, Z>(5.6, 10, true); \
    Fn<X, Y, Z, W>( \
      std::vector<int>{1, 4, 7}, \
      std::unordered_map<long, long>{ \
        {0, 1}, {1, 2}, {3, 5}, {8, 13}, {21, 34} \
      }, \
      std::make_pair(std::string("this is a pair"), 999), \
      std::chrono::seconds(86400) \
    ); \
  } while (false)

template <typename... TTags, typename... TData>
void check_tags(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  using actual = tuple<type_pair<TTags, typename std::decay<TData>::type>...>;

  FATAL_EXPECT_SAME<
    tuple_tags<TTags...>,
    typename actual::tags
  >();
}

TEST(tuple, tags) {
  CHECK_IMPL(check_tags);
}

template <typename... TTags, typename... TData>
void check_type(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  using actual = tuple<type_pair<TTags, typename std::decay<TData>::type>...>;

  FATAL_EXPECT_SAME<
    std::tuple<typename std::decay<TData>::type...>,
    typename actual::type
  >();
}

TEST(tuple, type) {
  CHECK_IMPL(check_type);
}

template <typename TTypes, typename TActual>
struct check_type_of_visitor {
  template <typename TTag, std::size_t Index>
  void operator ()(fatal::indexed_type_tag<TTag, Index>) const {
    FATAL_EXPECT_SAME<
      typename TTypes::template at<Index>,
      typename TActual::template type_of<TTag>
    >();
  }
};

template <typename... TTags, typename... TData>
void check_type_of(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  using tags = type_list<typename std::decay<TTags>::type...>;
  using types = type_list<typename std::decay<TData>::type...>;

  static_assert(tags::size == types::size, "size mismatch");

  using actual = tuple<type_pair<TTags, typename std::decay<TData>::type>...>;

  tags::foreach(check_type_of_visitor<types, actual>());
}

TEST(tuple, type_of) {
  CHECK_IMPL(check_type_of);
}

template <typename... TTags, typename... TData>
void check_default_ctor(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  std::tuple<TData...> expected;

  tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual;

  EXPECT_EQ(expected, actual.data());
}

TEST(tuple, default_ctor) {
  CHECK_IMPL(check_default_ctor);
}

TEST(tuple, ctor) {
  auto tuple = make_tuple<W, X, Y, Z>(
    std::string("hello"),
    10,
    std::make_pair(true, 5.6),
    std::vector<int>{9, 100}
  );

  EXPECT_EQ("hello", tuple.get<W>());

  EXPECT_EQ(10, tuple.get<X>());

  EXPECT_EQ(true, tuple.get<Y>().first);
  EXPECT_EQ(5.6, tuple.get<Y>().second);

  EXPECT_EQ(2, tuple.get<Z>().size());
  EXPECT_EQ(9, tuple.get<Z>()[0]);
  EXPECT_EQ(100, tuple.get<Z>()[1]);
}

template <typename... TTags, typename... TData>
void check_forwarding_ctor(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  std::tuple<TData...> expected(data...);

  tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  EXPECT_EQ(expected, actual.data());
}

TEST(tuple, forwarding_ctor) {
  CHECK_IMPL(check_forwarding_ctor);
}

template <typename... TTags, typename... TData>
void check_forwarding_ctor_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  std::tuple<TData...> expected(data...);

  tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::make_tuple(std::forward<TData>(data)...)
  );

  EXPECT_EQ(expected, actual.data());
}

TEST(tuple, forwarding_ctor_tuple) {
  CHECK_IMPL(check_forwarding_ctor_tuple);
}

template <typename...> struct check_get_helper;

template <typename T, typename... Args>
struct check_get_helper<T, Args...> {
  template <typename TExpected, typename TActual>
  static void check(TExpected &&expected, TActual &&actual) {
    using tuple = typename std::decay<TExpected>::type;
    using size = std::tuple_size<tuple>;
    using tail_size = std::integral_constant<std::size_t, sizeof...(Args) + 1>;
    static_assert(tail_size::value <= size::value, "out of bounds");
    using index = std::integral_constant<
      std::size_t, size::value - tail_size::value
    >;

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
  fatal::tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  check_get_helper<TTags...>::check(tuple, actual);
}

TEST(tuple, get_const) {
  CHECK_IMPL(check_get_const);
}

template <typename... TTags, typename... TData>
void check_get(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  check_get_helper<TTags...>::check(tuple, actual);
}

TEST(tuple, get) {
  CHECK_IMPL(check_get);
}

template <typename... TTags, typename... TData>
void check_tuple_const(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<
    type_pair<TTags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  EXPECT_EQ(tuple, actual.data());
}

TEST(tuple, tuple_const) {
  CHECK_IMPL(check_tuple_const);
}

template <typename... TTags, typename... TData>
void check_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<type_pair<TTags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  EXPECT_EQ(tuple, actual.data());
}

TEST(tuple, tuple) {
  CHECK_IMPL(check_tuple);
}

template <typename... TTags, typename... TData>
void check_paired_tuple(TData &&...) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");

  using expected = tuple<type_pair<TTags, typename std::decay<TData>::type>...>;

  using actual = typename type_list<TTags...>::template zip<
    typename std::decay<TData>::type...
  >::template apply<build_tuple>;

  FATAL_EXPECT_SAME<
    expected,
    actual
  >();
}

TEST(build_tuple, build_tuple) {
  CHECK_IMPL(check_paired_tuple);
}

template <typename... TTags, typename... TData>
void check_make_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(data...);
  auto const actual = make_tuple<TTags...>(std::forward<TData>(data)...);

  FATAL_EXPECT_SAME<
    fatal::tuple<type_pair<TTags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  EXPECT_EQ(tuple, actual.data());
}

TEST(make_tuple, make_tuple) {
  CHECK_IMPL(check_make_tuple);
}

template <typename... TTags, typename... TData>
void check_make_tuple_tuple(TData &&...data) {
  static_assert(sizeof...(TTags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(std::forward<TData>(data)...);
  auto const actual = make_tuple<TTags...>(tuple);

  FATAL_EXPECT_SAME<
    fatal::tuple<type_pair<TTags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  EXPECT_EQ(tuple, actual.data());
}

TEST(make_tuple, make_tuple_tuple) {
  CHECK_IMPL(check_make_tuple_tuple);
}

} // namespace fatal {
