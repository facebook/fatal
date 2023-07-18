/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/tuple.h>
#include <fatal/type/deprecated/type_map.h>

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

template <typename... Tags, typename... TData>
void check_tags(TData &&...) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  using actual = tuple<pair<Tags, typename std::decay<TData>::type>...>;

  FATAL_EXPECT_SAME<
    tuple_tags<Tags...>,
    typename actual::tags
  >();
}

FATAL_TEST(tuple, tags) {
  CHECK_IMPL(check_tags);
}

template <typename... Tags, typename... TData>
void check_type(TData &&...) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  using actual = tuple<pair<Tags, typename std::decay<TData>::type>...>;

  FATAL_EXPECT_SAME<
    std::tuple<typename std::decay<TData>::type...>,
    typename actual::type
  >();
}

FATAL_TEST(tuple, type) {
  CHECK_IMPL(check_type);
}

template <typename Types, typename TActual>
struct check_type_of_visitor {
  template <typename Tag, std::size_t Index>
  void operator ()(fatal::indexed<Tag, Index>) const {
    FATAL_EXPECT_SAME<
      at<Types, Index>,
      typename TActual::template type_of<Tag>
    >();
  }
};

template <typename... Tags, typename... TData>
void check_type_of(TData &&...) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  using tags = list<typename std::decay<Tags>::type...>;
  using types = list<typename std::decay<TData>::type...>;

  static_assert(size<tags>::value == size<types>::value, "size mismatch");

  using actual = tuple<pair<Tags, typename std::decay<TData>::type>...>;

  foreach<tags>(check_type_of_visitor<types, actual>());
}

FATAL_TEST(tuple, type_of) {
  CHECK_IMPL(check_type_of);
}

FATAL_TEST(tuple, ctor) {
  auto tuple = make_tuple<W, X, Y, Z>(
    std::string("hello"),
    10,
    std::make_pair(true, 5.6),
    std::vector<int>{9, 100}
  );

  FATAL_EXPECT_EQ("hello", tuple.get<W>());

  FATAL_EXPECT_EQ(10, tuple.get<X>());

  FATAL_EXPECT_EQ(true, tuple.get<Y>().first);
  FATAL_EXPECT_EQ(5.6, tuple.get<Y>().second);

  FATAL_EXPECT_EQ(2, tuple.get<Z>().size());
  FATAL_EXPECT_EQ(9, tuple.get<Z>()[0]);
  FATAL_EXPECT_EQ(100, tuple.get<Z>()[1]);
}

template <typename... Tags, typename... TData>
void check_forwarding_ctor(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  std::tuple<TData...> expected(data...);

  tuple<pair<Tags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  FATAL_EXPECT_EQ(expected, actual.data());
}

FATAL_TEST(tuple, forwarding_ctor) {
  CHECK_IMPL(check_forwarding_ctor);
}

template <typename... Tags, typename... TData>
void check_forwarding_ctor_tuple(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  std::tuple<TData...> expected(data...);

  tuple<pair<Tags, typename std::decay<TData>::type>...> actual(
    std::make_tuple(std::forward<TData>(data)...)
  );

  FATAL_EXPECT_EQ(expected, actual.data());
}

FATAL_TEST(tuple, forwarding_ctor_tuple) {
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

    using actual_type = typename std::decay<TActual>::type;
    static_assert(sizeof(actual_type) >= 0, "");
    FATAL_EXPECT_EQ(
      std::get<index::value>(expected),
      actual.actual_type::template get<T>()
    );

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

template <typename... Tags, typename... TData>
void check_get_const(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<
    pair<Tags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  check_get_helper<Tags...>::check(tuple, actual);
}

FATAL_TEST(tuple, get_const) {
  CHECK_IMPL(check_get_const);
}

template <typename... Tags, typename... TData>
void check_get(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<pair<Tags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  check_get_helper<Tags...>::check(tuple, actual);
}

FATAL_TEST(tuple, get) {
  CHECK_IMPL(check_get);
}

template <typename... Tags, typename... TData>
void check_tuple_const(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<
    pair<Tags, typename std::decay<TData>::type>...
  > const actual(std::forward<TData>(data)...);

  FATAL_EXPECT_EQ(tuple, actual.data());
}

FATAL_TEST(tuple, tuple_const) {
  CHECK_IMPL(check_tuple_const);
}

template <typename... Tags, typename... TData>
void check_tuple(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  auto const tuple = std::make_tuple(data...);
  fatal::tuple<pair<Tags, typename std::decay<TData>::type>...> actual(
    std::forward<TData>(data)...
  );

  FATAL_EXPECT_EQ(tuple, actual.data());
}

FATAL_TEST(tuple, tuple) {
  CHECK_IMPL(check_tuple);
}

FATAL_TEST(tuple, push_back) {
  using tup = tuple<pair<W, std::string>>;
  FATAL_EXPECT_SAME<
      typename tup::push_back<pair<X, std::string>>,
    tuple<pair<W, std::string>, pair<X, std::string>>>();
}

FATAL_TEST(tuple, push_front) {
  using tup = tuple<pair<W, std::string>>;
  FATAL_EXPECT_SAME<
      typename tup::push_front<pair<X, std::string>>,
    tuple<pair<X, std::string>, pair<W, std::string>>>();
}

template <typename... Tags, typename... TData>
void check_paired_tuple(TData &&...) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");

  using expected = tuple<pair<Tags, typename std::decay<TData>::type>...>;

  using actual = typename type_list<Tags...>::template zip<
    typename std::decay<TData>::type...
  >::template apply<build_tuple>;

  FATAL_EXPECT_SAME<
    expected,
    actual
  >();
}

FATAL_TEST(build_tuple, build_tuple) {
  CHECK_IMPL(check_paired_tuple);
}

template <typename... Tags, typename... TData>
void check_make_tuple(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(data...);
  auto const actual = make_tuple<Tags...>(std::forward<TData>(data)...);

  FATAL_EXPECT_SAME<
    fatal::tuple<pair<Tags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  FATAL_EXPECT_EQ(tuple, actual.data());
}

FATAL_TEST(make_tuple, make_tuple) {
  CHECK_IMPL(check_make_tuple);
}

template <typename... Tags, typename... TData>
void check_make_tuple_tuple(TData &&...data) {
  static_assert(sizeof...(Tags) == sizeof...(TData), "size mismatch");
  auto const tuple = std::make_tuple(std::forward<TData>(data)...);
  auto const actual = make_tuple<Tags...>(tuple);

  FATAL_EXPECT_SAME<
    fatal::tuple<pair<Tags, typename std::decay<TData>::type>...>,
    typename std::decay<decltype(actual)>::type
  >();

  FATAL_EXPECT_EQ(tuple, actual.data());
}

FATAL_TEST(make_tuple, make_tuple_tuple) {
  CHECK_IMPL(check_make_tuple_tuple);
}

template <typename> struct Foo {};
template <typename> struct Bar {};

FATAL_TEST(tuple_from, args) {
  FATAL_EXPECT_SAME<tuple<>, tuple_from<>::args<>>();
  FATAL_EXPECT_SAME<
    tuple<
      pair<int, int>,
      pair<double, double>,
      pair<float, float>,
      pair<bool, bool>
    >,
    tuple_from<int, double, float, bool>::args<>
  >();

  FATAL_EXPECT_SAME<tuple<>, tuple_from<>::args<Foo, Bar>>();
  FATAL_EXPECT_SAME<
    tuple<
      pair<Bar<int>, Foo<int>>,
      pair<Bar<double>, Foo<double>>,
      pair<Bar<float>, Foo<float>>,
      pair<Bar<bool>, Foo<bool>>
    >,
    tuple_from<int, double, float, bool>::args<Foo, Bar>
  >();
}

FATAL_TEST(tuple_from, list) {
  FATAL_EXPECT_SAME<tuple<>, tuple_from<list<>>::list<>>();
  FATAL_EXPECT_SAME<
    tuple<
      pair<int, int>,
      pair<double, double>,
      pair<float, float>,
      pair<bool, bool>
    >,
    tuple_from<list<int, double, float, bool>>::list<>
  >();

  FATAL_EXPECT_SAME<tuple<>, tuple_from<list<>>::list<Foo, Bar>>();
  FATAL_EXPECT_SAME<
    tuple<
      pair<Bar<int>, Foo<int>>,
      pair<Bar<double>, Foo<double>>,
      pair<Bar<float>, Foo<float>>,
      pair<Bar<bool>, Foo<bool>>
    >,
    tuple_from<list<int, double, float, bool>>::list<Foo, Bar>
  >();
}

FATAL_TEST(tuple_from, map) {
  FATAL_EXPECT_SAME<tuple<>, tuple_from<type_map<>>::map<>>();
  FATAL_EXPECT_SAME<
    tuple<pair<int, double>, pair<float, bool>>,
    tuple_from<list<pair<int, double>, pair<float, bool>>>::map<>
  >();

  FATAL_EXPECT_SAME<tuple<>, tuple_from<type_map<>>::map<Foo, Bar>>();
  FATAL_EXPECT_SAME<
    tuple<pair<Bar<int>, Foo<double>>, pair<Bar<float>, Foo<bool>>>,
    tuple_from<list<pair<int, double>, pair<float, bool>>>::map<Foo, Bar>
  >();
}

} // namespace fatal {
