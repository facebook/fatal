/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/list.h>
#include <fatal/type/traits.h>

#include <fatal/test/driver.h>

#include <folly/Conv.h>

namespace ftl {

///////////////
// type_list //
///////////////

template <int> struct T {};
template <int> struct S {};
template <int> struct P {};

template <typename...> struct is_T: public std::false_type {};
template <int Value> struct is_T<T<Value>>: public std::true_type {};
template <typename...> struct is_S: public std::false_type {};
template <int Value> struct is_S<S<Value>>: public std::true_type {};
template <typename...> struct is_P: public std::false_type {};
template <int Value> struct is_P<P<Value>>: public std::true_type {};


typedef type_list<> el;
typedef type_list<T<0>> single;
typedef type_list<T<0>, T<1>, T<2>> tl;
typedef type_list<P<0>, P<1>, P<2>> tp;
typedef type_list<S<0>, S<1>, S<2>> ts;
typedef type_list<P<0>, P<1>, P<2>, T<0>, T<1>, T<2>> tpl;
typedef type_list<T<0>, T<1>, T<2>, S<0>, S<1>, S<2>> tls;
typedef type_list<P<0>, P<1>, P<2>, T<0>, T<1>, T<2>, S<0>, S<1>, S<2>> tpls;

template <char Value> using chr_val = std::integral_constant<char, Value>;
template <char... Values> using chr_seq = type_list<chr_val<Values>...>;
template <int Value> using int_val = std::integral_constant<int, Value>;
template <int... Values> using int_seq = type_list<int_val<Values>...>;

template <typename T, T Multiplier>
struct multiply_transform {
  template <typename U>
  using type = std::integral_constant<
    decltype(U::value * Multiplier),
    U::value * Multiplier
  >;
};

/////////////////////
// type_list::size //
/////////////////////

TEST(type_list, size) {
  EXPECT_EQ(3, tl::size);
  EXPECT_EQ(0, (el::size));
  EXPECT_EQ(6, (tls::size));
  EXPECT_EQ(5, (type_list<T<0>, T<1>, T<2>, P<0>, P<1>>::size));
}

//////////////////////
// type_list::empty //
//////////////////////

TEST(type_list, empty) {
  EXPECT_FALSE(tl::empty);
  EXPECT_TRUE((el::empty));
  EXPECT_FALSE((tls::empty));
  EXPECT_FALSE((type_list<T<0>, T<1>, T<2>, P<0>, P<1>>::empty));
}

///////////////////
// type_list::at //
///////////////////

TEST(type_list, at) {
  expect_same<T<0>, tl::at<0>>();
  expect_same<T<1>, tl::at<1>>();
  expect_same<T<2>, tl::at<2>>();
}

///////////////////////
// type_list::try_at //
///////////////////////

TEST(type_list, try_at) {
  expect_same<type_list<T<0>>, tl::try_at<0>>();
  expect_same<type_list<T<1>>, tl::try_at<1>>();
  expect_same<type_list<T<2>>, tl::try_at<2>>();
  expect_same<type_list<>, tl::try_at<3>>();
}

/////////////////////////
// type_list::index_of //
/////////////////////////

TEST(type_list, index_of) {
  EXPECT_EQ(el::size, el::index_of<T<0>>::value);
  EXPECT_EQ(el::size, el::index_of<T<1>>::value);
  EXPECT_EQ(el::size, el::index_of<T<2>>::value);
  EXPECT_EQ(el::size, el::index_of<P<0>>::value);
  EXPECT_EQ(el::size, el::index_of<P<1>>::value);
  EXPECT_EQ(el::size, el::index_of<P<2>>::value);
  EXPECT_EQ(el::size, el::index_of<S<0>>::value);
  EXPECT_EQ(el::size, el::index_of<S<1>>::value);
  EXPECT_EQ(el::size, el::index_of<S<2>>::value);

  EXPECT_EQ(0, tl::index_of<T<0>>::value);
  EXPECT_EQ(1, tl::index_of<T<1>>::value);
  EXPECT_EQ(2, tl::index_of<T<2>>::value);
  EXPECT_EQ(tl::size, tl::index_of<P<0>>::value);
  EXPECT_EQ(tl::size, tl::index_of<P<1>>::value);
  EXPECT_EQ(tl::size, tl::index_of<P<2>>::value);
  EXPECT_EQ(tl::size, tl::index_of<S<0>>::value);
  EXPECT_EQ(tl::size, tl::index_of<S<1>>::value);
  EXPECT_EQ(tl::size, tl::index_of<S<2>>::value);

  EXPECT_EQ(0, tpl::index_of<P<0>>::value);
  EXPECT_EQ(1, tpl::index_of<P<1>>::value);
  EXPECT_EQ(2, tpl::index_of<P<2>>::value);
  EXPECT_EQ(3, tpl::index_of<T<0>>::value);
  EXPECT_EQ(4, tpl::index_of<T<1>>::value);
  EXPECT_EQ(5, tpl::index_of<T<2>>::value);
  EXPECT_EQ(tpl::size, tpl::index_of<S<0>>::value);
  EXPECT_EQ(tpl::size, tpl::index_of<S<1>>::value);
  EXPECT_EQ(tpl::size, tpl::index_of<S<2>>::value);

  EXPECT_EQ(tls::size, tls::index_of<P<0>>::value);
  EXPECT_EQ(tls::size, tls::index_of<P<1>>::value);
  EXPECT_EQ(tls::size, tls::index_of<P<2>>::value);
  EXPECT_EQ(0, tls::index_of<T<0>>::value);
  EXPECT_EQ(1, tls::index_of<T<1>>::value);
  EXPECT_EQ(2, tls::index_of<T<2>>::value);
  EXPECT_EQ(3, tls::index_of<S<0>>::value);
  EXPECT_EQ(4, tls::index_of<S<1>>::value);
  EXPECT_EQ(5, tls::index_of<S<2>>::value);

  EXPECT_EQ(0, tpls::index_of<P<0>>::value);
  EXPECT_EQ(1, tpls::index_of<P<1>>::value);
  EXPECT_EQ(2, tpls::index_of<P<2>>::value);
  EXPECT_EQ(3, tpls::index_of<T<0>>::value);
  EXPECT_EQ(4, tpls::index_of<T<1>>::value);
  EXPECT_EQ(5, tpls::index_of<T<2>>::value);
  EXPECT_EQ(6, tpls::index_of<S<0>>::value);
  EXPECT_EQ(7, tpls::index_of<S<1>>::value);
  EXPECT_EQ(8, tpls::index_of<S<2>>::value);
}

////////////////////////
// type_list::type_at //
////////////////////////

TEST(type_list, type_at) {
  EXPECT_EQ(typeid(P<0>), tp::type_at(0));
  EXPECT_EQ(typeid(P<1>), tp::type_at(1));
  EXPECT_EQ(typeid(P<2>), tp::type_at(2));

  EXPECT_EQ(typeid(T<0>), tl::type_at(0));
  EXPECT_EQ(typeid(T<1>), tl::type_at(1));
  EXPECT_EQ(typeid(T<2>), tl::type_at(2));

  EXPECT_EQ(typeid(S<0>), ts::type_at(0));
  EXPECT_EQ(typeid(S<1>), ts::type_at(1));
  EXPECT_EQ(typeid(S<2>), ts::type_at(2));

  EXPECT_EQ(typeid(P<0>), tpl::type_at(0));
  EXPECT_EQ(typeid(P<1>), tpl::type_at(1));
  EXPECT_EQ(typeid(P<2>), tpl::type_at(2));
  EXPECT_EQ(typeid(T<0>), tpl::type_at(3));
  EXPECT_EQ(typeid(T<1>), tpl::type_at(4));
  EXPECT_EQ(typeid(T<2>), tpl::type_at(5));

  EXPECT_EQ(typeid(T<0>), tls::type_at(0));
  EXPECT_EQ(typeid(T<1>), tls::type_at(1));
  EXPECT_EQ(typeid(T<2>), tls::type_at(2));
  EXPECT_EQ(typeid(S<0>), tls::type_at(3));
  EXPECT_EQ(typeid(S<1>), tls::type_at(4));
  EXPECT_EQ(typeid(S<2>), tls::type_at(5));

  EXPECT_EQ(typeid(P<0>), tpls::type_at(0));
  EXPECT_EQ(typeid(P<1>), tpls::type_at(1));
  EXPECT_EQ(typeid(P<2>), tpls::type_at(2));
  EXPECT_EQ(typeid(T<0>), tpls::type_at(3));
  EXPECT_EQ(typeid(T<1>), tpls::type_at(4));
  EXPECT_EQ(typeid(T<2>), tpls::type_at(5));
  EXPECT_EQ(typeid(S<0>), tpls::type_at(6));
  EXPECT_EQ(typeid(S<1>), tpls::type_at(7));
  EXPECT_EQ(typeid(S<2>), tpls::type_at(8));
}

/////////////////////////
// type_list::contains //
/////////////////////////

TEST(type_list, contains) {
  EXPECT_FALSE(el::contains<T<0>>::value);
  EXPECT_FALSE(el::contains<T<1>>::value);
  EXPECT_FALSE(el::contains<T<2>>::value);
  EXPECT_FALSE(el::contains<P<0>>::value);
  EXPECT_FALSE(el::contains<P<1>>::value);
  EXPECT_FALSE(el::contains<P<2>>::value);
  EXPECT_FALSE(el::contains<S<0>>::value);
  EXPECT_FALSE(el::contains<S<1>>::value);
  EXPECT_FALSE(el::contains<S<2>>::value);

  EXPECT_TRUE(tl::contains<T<0>>::value);
  EXPECT_TRUE(tl::contains<T<1>>::value);
  EXPECT_TRUE(tl::contains<T<2>>::value);
  EXPECT_FALSE(tl::contains<P<0>>::value);
  EXPECT_FALSE(tl::contains<P<1>>::value);
  EXPECT_FALSE(tl::contains<P<2>>::value);
  EXPECT_FALSE(tl::contains<S<0>>::value);
  EXPECT_FALSE(tl::contains<S<1>>::value);
  EXPECT_FALSE(tl::contains<S<2>>::value);
}

//////////////////////////
// type_list::push_back //
//////////////////////////

TEST(type_list, push_back) {
  expect_same<tl::push_back<>, tl>();
  expect_same<tl::push_back<S<0>, S<1>, S<2>>, tls>();
}

///////////////////////////
// type_list::push_front //
///////////////////////////

TEST(type_list, push_front) {
  expect_same<tl::push_front<>, tl>();
  expect_same<tl::push_front<P<0>, P<1>, P<2>>, tpl>();
}

///////////////////////
// type_list::concat //
///////////////////////

TEST(type_list, concat) {
  expect_same<tp::concat<tl>, tpl>();
  expect_same<tl::concat<ts>, tls>();
  expect_same<tl::concat<el>, tl>();
  expect_same<el::concat<tl>, tl>();
  expect_same<el::concat<el>, el>();
}

//////////////////////////////
// type_list::insert_sorted //
//////////////////////////////

template <typename, typename> using false_comparison = std::false_type;
template <typename, typename> using true_comparison = std::true_type;

TEST(type_list, insert_sorted) {
  expect_same<
    type_list<int_val<0>>,
    el::insert_sorted<int_val<0>>
  >();

  expect_same<
    type_list<int_val<0>, int_val<0>>,
    type_list<int_val<0>>::insert_sorted<int_val<0>>
  >();

  expect_same<
    type_list<int_val<0>, int_val<1>, int_val<2>, int_val<3>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>>
  >();

  expect_same<
    type_list<int_val<5>, int_val<4>, int_val<3>, int_val<1>>,
    type_list<int_val<5>, int_val<3>, int_val<1>>
      ::insert_sorted<int_val<4>, constants_comparison_gt>
  >();

  expect_same<
    type_list<int_val<0>, int_val<1>, int_val<3>, int_val<2>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>, false_comparison>
  >();

  expect_same<
    type_list<int_val<2>, int_val<0>, int_val<1>, int_val<3>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>, true_comparison>
  >();
}

//////////////////////
// type_list::apply //
//////////////////////

TEST(type_list, apply) {
  expect_same<tl::apply<std::tuple>, std::tuple<T<0>, T<1>, T<2>>>();
}

///////////////////////////
// type_list::apply_back //
///////////////////////////

TEST(type_list, apply_back) {
  expect_same<
    tl::apply_back<std::tuple, S<0>, S<1>, S<2>>,
    std::tuple<T<0>, T<1>, T<2>, S<0>, S<1>, S<2>>
  >();
}

////////////////////////////
// type_list::apply_front //
////////////////////////////

TEST(type_list, apply_front) {
  expect_same<
    tl::apply_front<std::tuple, P<0>, P<1>, P<2>>,
    std::tuple<P<0>, P<1>, P<2>, T<0>, T<1>, T<2>>
  >();
}

///////////////////////////
// type_list::foreach_if //
///////////////////////////

template <template <int...> class TTemplate>
struct foreach_if_filter {
  template <int... UArgs> static std::true_type sfinae(TTemplate<UArgs...> *);
  static std::false_type sfinae(...);

  template <typename U>
  using type = decltype(sfinae(static_cast<U *>(nullptr)));
};

typedef std::vector<std::pair<std::size_t, std::size_t>> foreach_if_expected;

template <template <int...> class TTemplate>
struct foreach_if_test_visitor {
  explicit foreach_if_test_visitor(foreach_if_expected expected):
    expected_(std::move(expected))
  {}

  ~foreach_if_test_visitor() {
    EXPECT_EQ(expected_.size(), index_);
  }

  template <int Id, std::size_t Index>
  void operator ()(indexed_type_tag<TTemplate<Id>, Index>) {
    ASSERT_LE(index_, expected_.size());
    auto const &e = expected_[index_++];
    EXPECT_EQ(e.first, Index);
    EXPECT_EQ(e.second, Id);
  }

private:
  std::size_t index_ = 0;
  foreach_if_expected const expected_;
};

template <template <int...> class TTemplate, typename TList, typename... Args>
void check_foreach_if(foreach_if_expected expected) {
  foreach_if_test_visitor<TTemplate> visitor(expected);

  auto const visited = TList::template foreach_if<
    foreach_if_filter<TTemplate>::template type
  >(visitor);

  EXPECT_EQ(expected.size(), visited);
}

TEST(type_list, foreach_if) {
  typedef type_list<
    //     1      2      3      4      5      6      7      8
    T<10>, P<22>, S<67>, T<41>, S<97>, P<83>, T<16>, P<32>, S<75>
  > list1;

  check_foreach_if<T, list1>(foreach_if_expected{{0, 10}, {3, 41}, {6, 16}});
  check_foreach_if<S, list1>(foreach_if_expected{{2, 67}, {4, 97}, {8, 75}});
  check_foreach_if<P, list1>(foreach_if_expected{{1, 22}, {5, 83}, {7, 32}});
}

////////////////////////
// type_list::foreach //
////////////////////////

template <typename TExpectedList>
struct foreach_test_visitor {
  typedef TExpectedList expected;

  template <typename U, std::size_t Index>
  void operator ()(indexed_type_tag<U, Index> type) {
    expect_same<
      typename expected::template at<Index>,
      indexed_type_tag<U, Index>
    >();

    auto const index = index_++;
    EXPECT_EQ(index, Index);

    auto const &actual = typeid(type);
    auto const &expected = expected::type_at(index);

    if (expected != actual) {
      EXPECT_EQ(
        folly::to<std::string>(
          "expected type '", folly::demangle(expected.name()), '\''
        ),
        folly::to<std::string>(
          "actual type '", folly::demangle(actual.name()), '\''
        )
      );
    }
  }

private:
  std::size_t index_ = 0;
};

template <typename TList>
void check_foreach() {
  typedef typename TList::template indexed_transform<
    indexed_type_tag
  > expected;

  TList::foreach(foreach_test_visitor<expected>());
}

TEST(type_list, foreach) {
  check_foreach<el>();
  check_foreach<tl>();
  check_foreach<tp>();
  check_foreach<ts>();
  check_foreach<tpl>();
  check_foreach<tls>();
  check_foreach<tpls>();
}

//////////////////////
// type_list::visit //
//////////////////////

template <typename TExpected>
struct visit_test_visitor {
  template <typename T, std::size_t Index>
  void operator ()(indexed_type_tag<T, Index>, std::size_t expected_index) {
    EXPECT_FALSE(visited_);
    visited_ = true;
    EXPECT_SAME(TExpected, T);
    EXPECT_EQ(expected_index, Index);
  }

private:
  bool visited_ = false;
};

TEST(type_list, visit) {
  auto const no_visit = [](auto tag, auto index) { EXPECT_TRUE(false); };

  EXPECT_FALSE((el::visit(0, no_visit)));

  using list = type_list<double, float, long, bool, int>;

  EXPECT_TRUE(list::visit(0, visit_test_visitor<double>(), 0));
  EXPECT_TRUE(list::visit(1, visit_test_visitor<float>(), 1));
  EXPECT_TRUE(list::visit(2, visit_test_visitor<long>(), 2));
  EXPECT_TRUE(list::visit(3, visit_test_visitor<bool>(), 3));
  EXPECT_TRUE(list::visit(4, visit_test_visitor<int>(), 4));
  EXPECT_FALSE(list::visit(5, no_visit, 5));
}

//////////////////////////
// type_list::transform //
//////////////////////////

TEST(type_list, transform) {
  expect_same<
    type_list<std::tuple<T<0>>, std::tuple<T<1>>, std::tuple<T<2>>>,
    tl::transform<std::tuple>
  >();

  expect_same<
    type_list<std::tuple<T<0>>>,
    type_list<T<0>>::transform<std::tuple>
  >();

  expect_same<el, el::transform<std::tuple>>();
}

//////////////////////////////////
// type_list::indexed_transform //
//////////////////////////////////

template <typename T, std::size_t N>
using indexed_transform_test_impl = indexed_type_tag<T, N>;

TEST(type_list, indexed_transform) {
  expect_same<
    type_list<
      indexed_type_tag<T<0>, 0>,
      indexed_type_tag<T<1>, 1>,
      indexed_type_tag<T<2>, 2>
    >,
    tl::indexed_transform<indexed_transform_test_impl>
  >();

  expect_same<
    type_list<indexed_type_tag<T<0>, 0>>,
    single::indexed_transform<indexed_transform_test_impl>
  >();

  expect_same<
    el,
    el::indexed_transform<indexed_transform_test_impl>
  >();
}

////////////////////////
// type_list::replace //
////////////////////////

TEST(type_list, replace) {
  expect_same<el, el::replace<int, double>>();
  expect_same<tl, tl::replace<int, double>>();
  expect_same<tl, tl::replace<T<1>, T<1>>>();

  expect_same<
    type_list<T<0>, int, T<2>>,
    tl::replace<T<1>, int>
  >();

  expect_same<
    tp,
    tl::replace<T<0>, P<0>>
      ::replace<T<1>, P<1>>
      ::replace<T<2>, P<2>>
  >();
}

/////////////////////
// type_list::tail //
/////////////////////

TEST(type_list, tail) {
  expect_same<tl, tl::tail<0>>();
  expect_same<type_list<T<1>, T<2>>, tl::tail<1>>();
  expect_same<type_list<T<2>>, tl::tail<2>>();
  expect_same<el, tl::tail<3>>();
  expect_same<el, tl::tail<tl::size>>();
}

//////////////////////
// type_list::split //
//////////////////////

template <typename TList, std::size_t Index>
void check_split_impl() {
  expect_same<
    type_pair<
      typename TList::template left<Index>,
      typename TList::template tail<Index>
    >,
    typename TList::template split<Index>
  >();
}

template <typename... Args>
void check_split() {
  typedef type_list<Args...> list;

  check_split_impl<list, 0>();
  check_split_impl<list, list::size / 4>();
  check_split_impl<list, list::size / 3>();
  check_split_impl<list, list::size / 2>();
  check_split_impl<list, list::size * 2 / 3>();
  check_split_impl<list, list::size * 3 / 4>();
  check_split_impl<list, list::size>();
}

TEST(type_list, split) {
  check_split<>();
  check_split<int>();
  check_split<int, bool>();
  check_split<int, bool, float>();
  check_split<int, bool, float, long>();
  check_split<int, bool, float, long, double>();
  check_split<int, bool, float, long, double, short>();
  check_split<int, bool, float, long, double, short, void>();
  check_split<int, bool, float, long, double, short, void, unsigned>();
}

/////////////////////
// type_list::left //
/////////////////////

TEST(type_list, left) {
  expect_same<el, tl::left<0>>();
  expect_same<type_list<T<0>>, tl::left<1>>();
  expect_same<type_list<T<0>, T<1>>, tl::left<2>>();
  expect_same<tl, tl::left<3>>();
  expect_same<tl, tl::left<tl::size>>();
}

//////////////////////
// type_list::slice //
//////////////////////

TEST(type_list, slice) {
  expect_same<el, tl::slice<0, 0>>();

  expect_same<type_list<T<0>>, tl::slice<0, 1>>();
  expect_same<type_list<>, tl::slice<1, 1>>();

  expect_same<type_list<T<0>, T<1>>, tl::slice<0, 2>>();
  expect_same<type_list<T<1>>, tl::slice<1, 2>>();
  expect_same<type_list<>, tl::slice<2, 2>>();

  expect_same<tl, tl::slice<0, tl::size>>();
  expect_same<type_list<T<1>, T<2>>, tl::slice<1, tl::size>>();
  expect_same<type_list<T<2>>, tl::slice<2, tl::size>>();
  expect_same<type_list<>, tl::slice<tl::size, tl::size>>();
}

//////////////////////
// type_list::right //
//////////////////////

TEST(type_list, right) {
  expect_same<el, tl::right<0>>();
  expect_same<type_list<T<2>>, tl::right<1>>();
  expect_same<type_list<T<1>, T<2>>, tl::right<2>>();
  expect_same<tl, tl::right<3>>();
  expect_same<tl, tl::right<tl::size>>();
}

///////////////////////
// type_list::filter //
///////////////////////

template <
  typename TList,
  template <typename, typename...> class TFilter,
  typename TExpectedFirst,
  typename TExpectedSecond
>
void check_filter()  {
  typedef typename TList::template filter<TFilter> actual;
  expect_same<TExpectedFirst, typename actual::first>();
  expect_same<TExpectedSecond, typename actual::second>();
};

template <typename> using all_filter = std::true_type;
template <typename> using none_filter = std::false_type;

TEST(type_list, filter) {
  check_filter<el, none_filter, el, el>();
  check_filter<el, all_filter, el,el>();
  check_filter<tl, none_filter, el, tl>();
  check_filter<tl, all_filter, tl, el>();
}

///////////////////////
// type_list::remove //
///////////////////////

TEST(type_list, remove) {
  expect_same<type_list<>, type_list<>::remove<>>();
  expect_same<type_list<>, type_list<int>::remove<int>>();
  expect_same<
    type_list<>,
    type_list<int, bool, float>::remove<bool, int, float>
  >();

  expect_same<type_list<int>, type_list<int>::remove<>>();
  expect_same<type_list<int>, type_list<int>::remove<void>>();
  expect_same<type_list<int>, type_list<int>::remove<void, float>>();

  expect_same<type_list<int, float>, type_list<int, float>::remove<>>();
  expect_same<
    type_list<int, float>,
    type_list<int, float>::remove<void>
  >();
  expect_same<
    type_list<int, float>,
    type_list<int, float>::remove<void, bool>
  >();

  expect_same<
    type_list<bool, float>,
    type_list<int, bool, int, float, void>::remove<int, void>
  >();
}

////////////////////
// type_list::zip //
////////////////////

template <typename TLHS, typename TRHS, typename TExpected>
void check_zip()  {
  typedef typename TRHS::template apply<TLHS::template zip> actual;
  expect_same<TExpected, actual>();
};

TEST(type_list, zip) {
  check_zip<el, el, el>();
  check_zip<tl, el, tl>();
  check_zip<el, tl, tl>();
  check_zip<ts, single, type_list<S<0>, T<0>, S<1>, S<2>>>();
  check_zip<single, ts, type_list<T<0>, S<0>, S<1>, S<2>>>();
  check_zip<single, single, type_list<T<0>, T<0>>>();
  check_zip<tp, ts, type_list<P<0>, S<0>, P<1>, S<1>, P<2>, S<2>>>();
}

//////////////////////
// type_list::unzip //
//////////////////////

template <
  std::size_t Step, std::size_t Offset, typename TList, typename TExpected
>
void check_unzip()  {
  typedef typename TList::template unzip<Step, Offset> actual;
  expect_same<TExpected, actual>();
};

template <typename TList>
void check_full_unzip() { check_unzip<0, 0, TList, TList>(); }

template <typename TList>
void check_empty_unzip() {
  check_unzip<0, TList::size, TList, el>();
  check_unzip<TList::size, TList::size, TList, el>();
}

TEST(type_list, unzip) {
  check_full_unzip<el>();
  check_full_unzip<tl>();
  check_full_unzip<single>();
  check_full_unzip<tpls>();

  check_empty_unzip<el>();
  check_empty_unzip<tl>();
  check_empty_unzip<single>();
  check_empty_unzip<tpls>();

  check_unzip<0, 1, single, el>();
  check_unzip<tl::size, 0, tl, single>();
  check_unzip<2, 0, tpls, type_list<P<0>, T<0>, S<0>>>();
  check_unzip<2, 1, tpls, type_list<P<1>, T<1>, S<1>>>();
  check_unzip<2, 2, tpls, type_list<P<2>, T<2>, S<2>>>();
}

///////////////////////
// type_list::search //
///////////////////////

template <
  typename TList,
  template <typename...> class TFilter,
  typename TDefault,
  typename TExpected
>
void check_search() {
  typedef typename TList::template search<TFilter, TDefault> actual;
  expect_same<TExpected, actual>();
}

TEST(type_list, search) {
  typedef transform::alias<std::is_same, T<0>> same_as_t0;
  check_search<el, same_as_t0::template type, void, void>();
  check_search<tl, same_as_t0::template type, void, T<0>>();
  check_search<tp, same_as_t0::template type, void, void>();

  typedef transform::alias<std::is_same, T<1>> same_as_t1;
  check_search<el, same_as_t1::template type, void, void>();
  check_search<tl, same_as_t1::template type, void, T<1>>();
  check_search<tp, same_as_t1::template type, void, void>();

  typedef transform::alias<std::is_same, T<2>> same_as_t2;
  check_search<el, same_as_t2::template type, void, void>();
  check_search<tl, same_as_t2::template type, void, T<2>>();
  check_search<tp, same_as_t2::template type, void, void>();

  typedef is_template<std::basic_string> is_std_string;
  typedef is_template<std::tuple> is_std_tuple;

  typedef type_list<int, std::string, double, long> types1;
  check_search<types1, is_std_string::instantiation, void, std::string>();
  check_search<types1, is_std_tuple::instantiation, void, void>();

  typedef type_list<int, float, double, std::tuple<long, bool>> types2;
  check_search<types2, is_std_string::instantiation, void, void>();
  check_search<
    types2, is_std_tuple::instantiation, void, std::tuple<long, bool>
  >();
}

////////////////////////
// type_list::combine //
////////////////////////

TEST(type_list, combine) {
  expect_same<
    type_list<>,
    type_list<>::combine<type_list<>, std::pair>
  >();

  expect_same<
    type_list<std::pair<int, bool>>,
    type_list<int>::combine<
      type_list<bool>,
      std::pair
    >
  >();

  expect_same<
    type_list<
      std::pair<int, bool>,
      std::pair<double, long>
    >,
    type_list<int, double>::combine<
      type_list<bool, long>,
      std::pair
    >
  >();

  expect_same<
    type_list<
      std::pair<int, bool>,
      std::pair<double, long>,
      std::pair<short, float>
    >,
    type_list<int, double, short>::combine<
      type_list<bool, long, float>,
      std::pair
    >
  >();
}

/////////////
// flatten //
/////////////

#define CHECK_FLATTEN(Expected, ...) \
  do { \
    typedef type_list<__VA_ARGS__> list; \
    EXPECT_SAME(list, typename list::template flatten<>); \
    EXPECT_SAME(list, typename list::template flatten<0>); \
    EXPECT_SAME(list, typename list::template flatten<1>); \
    EXPECT_SAME(list, typename list::template flatten<2>); \
    EXPECT_SAME(list, typename list::template flatten<3>); \
  } while (false)

TEST(type_list, flatten) {
  CHECK_FLATTEN();
  CHECK_FLATTEN(void);
  CHECK_FLATTEN(int);
  CHECK_FLATTEN(int, void);
  CHECK_FLATTEN(int, void, bool);
  CHECK_FLATTEN(int, void, bool, double);

  {
    using list = type_list<type_list<type_list<type_list<type_list<void>>>>>;

    using expected1 = type_list<type_list<type_list<type_list<void>>>>;
    using expected2 = type_list<type_list<type_list<void>>>;
    using expected3 = type_list<type_list<void>>;
    using expected_unlimited = type_list<void>;

    EXPECT_SAME(expected_unlimited, list::flatten<>);
    EXPECT_SAME(list, list::flatten<0>);
    EXPECT_SAME(expected1, list::flatten<1>);
    EXPECT_SAME(expected2, list::flatten<2>);
    EXPECT_SAME(expected3, list::flatten<3>);
    EXPECT_SAME(expected_unlimited, list::flatten<4>);
    EXPECT_SAME(expected_unlimited, list::flatten<5>);
  }

  {
    using list = type_list<
      int,
      type_list<
        void,
        type_list<
          bool,
          double,
          type_list<
            std::string,
            type_list<
              std::vector<int>
            >,
            std::wstring
          >,
          short
        >
      >
    >;

    using expected1 = type_list<
      int,
      void,
      type_list<bool, double, type_list<
        std::string, type_list<std::vector<int>>, std::wstring
      >, short>
    >;

    using expected2 = type_list<
      int, void, bool, double, type_list<
        std::string, type_list<std::vector<int>>, std::wstring
      >,
      short
    >;

    using expected3 = type_list<
      int, void, bool, double, std::string,
      type_list<std::vector<int>>,
      std::wstring, short
    >;

    using expected_unlimited = type_list<
      int, void, bool, double, std::string,
      std::vector<int>, std::wstring, short
    >;

    EXPECT_SAME(expected_unlimited, list::flatten<>);
    EXPECT_SAME(list, list::flatten<0>);
    EXPECT_SAME(expected1, list::flatten<1>);
    EXPECT_SAME(expected2, list::flatten<2>);
    EXPECT_SAME(expected3, list::flatten<3>);
    EXPECT_SAME(expected_unlimited, list::flatten<4>);
    EXPECT_SAME(expected_unlimited, list::flatten<5>);
  }
}

#undef CHECK_FLATTEN

//////////////////////////
// type_list::is_sorted //
//////////////////////////

template <bool IsSorted, int... Values>
void check_is_sorted() {
  typedef int_seq<Values...> list;

  if (list::template is_sorted<>::value != IsSorted) {
    LOG(ERROR) << "list: '" << folly::demangle(typeid(list).name()) << '\'';
    bool b = IsSorted;
    EXPECT_EQ(
      b,
      (list::template is_sorted<>::value)
    );
  }
}

TEST(type_list, is_sorted) {
  check_is_sorted<true>();
  check_is_sorted<true, 0>();
  check_is_sorted<true, 0, 1>();
  check_is_sorted<true, 1, 1>();
  check_is_sorted<true, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>();
  check_is_sorted<true, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>();

  check_is_sorted<false, 1, 0>();
  check_is_sorted<false, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>();
}

//////////////////////
// type_list::merge //
//////////////////////

template <typename TLHS, typename TRHS, int... Expected>
void check_merge() {
  typedef int_seq<Expected...> expected;
  typedef typename TLHS::template merge<
    TRHS
  >::template apply_values<int, int_seq> actual;

  if (!std::is_same<expected, actual>::value) {
    LOG(INFO) << "lhs: '" << folly::demangle(typeid(TLHS).name()) << '\'';
    LOG(INFO) << "rhs: '" << folly::demangle(typeid(TRHS).name()) << '\'';
  }

  expect_same<expected, actual>();
}

TEST(type_list, merge) {
  check_merge<int_seq<>, int_seq<>>();
  check_merge<int_seq<>, int_seq<1>, 1>();
  check_merge<int_seq<1>, int_seq<>, 1>();
  check_merge<int_seq<1>, int_seq<1>, 1, 1>();
  check_merge<int_seq<1, 1>, int_seq<>, 1, 1>();
  check_merge<int_seq<>, int_seq<1, 1>, 1, 1>();
  check_merge<int_seq<1>, int_seq<2>, 1, 2>();
  check_merge<int_seq<2>, int_seq<1>, 1, 2>();

  check_merge<int_seq<1>, int_seq<1, 2>, 1, 1, 2>();
  check_merge<int_seq<1, 2>, int_seq<1>, 1, 1, 2>();

  check_merge<int_seq<0>, int_seq<1, 2>, 0, 1, 2>();
  check_merge<int_seq<1>, int_seq<0, 2>, 0, 1, 2>();
  check_merge<int_seq<2>, int_seq<0, 1>, 0, 1, 2>();
  check_merge<int_seq<0, 1>, int_seq<2>, 0, 1, 2>();
  check_merge<int_seq<0, 2>, int_seq<1>, 0, 1, 2>();
  check_merge<int_seq<1, 2>, int_seq<0>, 0, 1, 2>();

  check_merge<
    int_seq<0, 2, 4, 6, 8>, int_seq<1, 3, 5, 7, 9>,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  >();
}

///////////////////////////
// type_list::merge_sort //
///////////////////////////

template <typename TExpectedList, int... Values>
void check_merge_sort() {
  typedef TExpectedList expected;

  static_assert(
    expected::template is_sorted<>::value,
    "expected list should be sorted"
  );

  typedef int_seq<Values...> input;
  typedef typename input::template merge_sort<> sorted;
  auto is_sorted = sorted::template is_sorted<>::value;
  typedef typename sorted::template apply_values<int, int_seq> actual;

  if (!is_sorted || !std::is_same<expected, actual>::value) {
    LOG(INFO) << "input: '" << folly::demangle(typeid(input).name()) << '\'';
    LOG(INFO) << "result: '" << folly::demangle(typeid(actual).name()) << '\'';
  }

  EXPECT_TRUE(is_sorted);
  expect_same<expected, actual>();
}

TEST(type_list, merge_sort) {
  check_merge_sort<int_seq<>>();
  check_merge_sort<int_seq<1>, 1>();

  check_merge_sort<int_seq<1, 1>, 1, 1>();
  check_merge_sort<int_seq<1, 2>, 1, 2>();
  check_merge_sort<int_seq<1, 2>, 2, 1>();

  check_merge_sort<int_seq<0, 1, 2>, 0, 1, 2>();
  check_merge_sort<int_seq<0, 1, 2>, 0, 2, 1>();
  check_merge_sort<int_seq<0, 1, 2>, 1, 0, 2>();
  check_merge_sort<int_seq<0, 1, 2>, 1, 2, 0>();
  check_merge_sort<int_seq<0, 1, 2>, 2, 0, 1>();
  check_merge_sort<int_seq<0, 1, 2>, 2, 1, 0>();

  check_merge_sort<int_seq<0, 1, 1>, 0, 1, 1>();
  check_merge_sort<int_seq<0, 1, 1>, 0, 1, 1>();
  check_merge_sort<int_seq<0, 1, 1>, 1, 0, 1>();
  check_merge_sort<int_seq<0, 1, 1>, 1, 1, 0>();
  check_merge_sort<int_seq<0, 1, 1>, 1, 0, 1>();
  check_merge_sort<int_seq<0, 1, 1>, 1, 1, 0>();

  check_merge_sort<int_seq<1, 1, 1>, 1, 1, 1>();

  check_merge_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    0, 2, 4, 6, 8, 1, 3, 5, 7, 9
  >();
  check_merge_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  >();
  check_merge_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0
  >();
}

///////////////////////
// type_list::unique //
///////////////////////

TEST(type_list, unique) {
  expect_same<
    type_list<int, double, bool, float>,
    type_list<int, double, double, int, bool, int, bool, float>::unique<>
  >();

  expect_same<
    int_seq<0, 1, 4, 3, 2, 6>,
    int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2, 3, 0, 1, 2>::unique<>
  >();

  expect_same<
    int_seq<0, 2, 8, 6, 4, 12>,
    int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2, 3, 0, 1, 2>::unique<
      multiply_transform<int, 2>::type
    >
  >();
}

/////////////////////////////////////
// type_list::binary_search::exact //
/////////////////////////////////////

template <typename T>
struct bs_visitor {
  template <T Value, std::size_t Index>
  void operator ()(
    indexed_type_tag<std::integral_constant<T, Value>, Index>,
    T const &needle,
    T &out,
    std::size_t &index
  ) {
    out = Value;
    index = Index;
  };
};

template <
  typename T,
  bool Result, T Needle, std::size_t ExpectedIndex,
  typename TList, T Empty
>
void check_bs_exact() {
  auto actual = Empty;
  std::size_t index = TList::size;

  auto result = TList::template binary_search<type_value_comparer>::exact(
    Needle, bs_visitor<T>(), actual, index
  );

  auto const expectedResult = Result;
  EXPECT_EQ(expectedResult, result);
  auto const expectedValue = Result ? Needle : Empty;
  EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  EXPECT_EQ(expectedIndex, index);
}

TEST(type_list, binary_search_exact) {
  typedef chr_seq<> empty;

  LOG(INFO) << "empty";
  check_bs_exact<char, false, '-', empty::size, empty, '\0'>();
  check_bs_exact<int,  false, 3,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  LOG(INFO) << "one";
  check_bs_exact<char, false, '-', one::size, one, '\0'>();
  check_bs_exact<char, true,  'x', 0, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  LOG(INFO) << "two";
  check_bs_exact<char, false, '-', two::size, two, '\0'>();
  check_bs_exact<char, true,  'x', 0, two, '\0'>();
  check_bs_exact<char, true,  'y', 1, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  LOG(INFO) << "aeiou";
  check_bs_exact<char, false, 'x', aeiou::size, aeiou, '\0'>();

  check_bs_exact<char, true, 'a', 0, aeiou, '\0'>();
  check_bs_exact<char, true, 'e', 1, aeiou, '\0'>();
  check_bs_exact<char, true, 'i', 2, aeiou, '\0'>();
  check_bs_exact<char, true, 'o', 3, aeiou, '\0'>();
  check_bs_exact<char, true, 'u', 4, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287, 2147483647> mp;

  LOG(INFO) << "mp";
  check_bs_exact<int, false, -1,         mp::size, mp, -1>();
  check_bs_exact<int, false, 0,          mp::size, mp, -1>();
  check_bs_exact<int, false, 63,         mp::size, mp, -1>();
  check_bs_exact<int, false, 63,         mp::size, mp, -1>();
  check_bs_exact<int, true,  3,          0, mp, -1>();
  check_bs_exact<int, true,  7,          1, mp, -1>();
  check_bs_exact<int, true,  31,         2, mp, -1>();
  check_bs_exact<int, true,  127,        3, mp, -1>();
  check_bs_exact<int, true,  8191,       4, mp, -1>();
  check_bs_exact<int, true,  131071,     5, mp, -1>();
  check_bs_exact<int, true,  524287,     6, mp, -1>();
  check_bs_exact<int, true,  2147483647, 7, mp, -1>();
}

///////////////////////////////////////////
// type_list::binary_search::lower_bound //
///////////////////////////////////////////

template <
  typename T,
  bool Result, T Needle, T ExpectedValue, std::size_t ExpectedIndex,
  typename TList, T Empty
>
void check_bs_lower_bound() {
  auto actual = Empty;
  std::size_t index = TList::size;

  auto result = TList::template binary_search<type_value_comparer>::lower_bound(
    Needle, bs_visitor<T>(), actual, index
  );

  auto const expectedResult = Result;
  EXPECT_EQ(expectedResult, result);
  auto const expectedValue = ExpectedValue;
  EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  EXPECT_EQ(expectedIndex, index);
}

TEST(type_list, binary_search_lower_bound) {
  typedef chr_seq<> empty;

  LOG(INFO) << "empty";
  check_bs_lower_bound<char, false, '-', '\0', empty::size, empty, '\0'>();
  check_bs_lower_bound<int,  false, 3,   -1,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  LOG(INFO) << "one";
  check_bs_lower_bound<char, false, 'w', '\0', one::size, one, '\0'>();
  check_bs_lower_bound<char, true,  'x', 'x',  0, one, '\0'>();
  check_bs_lower_bound<char, true,  'y', 'x',  0, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  LOG(INFO) << "two";
  check_bs_lower_bound<char, false, 'w', '\0', two::size, two, '\0'>();
  check_bs_lower_bound<char, true,  'x', 'x',  0, two, '\0'>();
  check_bs_lower_bound<char, true,  'y', 'y',  1, two, '\0'>();
  check_bs_lower_bound<char, true,  'z', 'y',  1, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  LOG(INFO) << "aeiou";
  check_bs_lower_bound<char, false, 'a' - 1, '\0', aeiou::size, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'a',     'a',  0, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'e',     'e',  1, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'i',     'i',  2, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'o',     'o',  3, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'u',     'u',  4, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'x',     'u',  4, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287> mp;

  LOG(INFO) << "mp";
  check_bs_lower_bound<int, false, -1,        -1,     mp::size, mp, -1>();
  check_bs_lower_bound<int, false, 0,         -1,     mp::size, mp, -1>();
  check_bs_lower_bound<int, false, 2,         -1,     mp::size, mp, -1>();
  check_bs_lower_bound<int, true,  3,         3,      0, mp, -1>();
  check_bs_lower_bound<int, true,  4,         3,      0, mp, -1>();
  check_bs_lower_bound<int, true,  6,         3,      0, mp, -1>();
  check_bs_lower_bound<int, true,  7,         7,      1, mp, -1>();
  check_bs_lower_bound<int, true,  8,         7,      1, mp, -1>();
  check_bs_lower_bound<int, true,  30,        7,      1, mp, -1>();
  check_bs_lower_bound<int, true,  31,        31,     2, mp, -1>();
  check_bs_lower_bound<int, true,  32,        31,     2, mp, -1>();
  check_bs_lower_bound<int, true,  126,       31,     2, mp, -1>();
  check_bs_lower_bound<int, true,  127,       127,    3, mp, -1>();
  check_bs_lower_bound<int, true,  128,       127,    3, mp, -1>();
  check_bs_lower_bound<int, true,  8190,      127,    3, mp, -1>();
  check_bs_lower_bound<int, true,  8191,      8191,   4, mp, -1>();
  check_bs_lower_bound<int, true,  8192,      8191,   4, mp, -1>();
  check_bs_lower_bound<int, true,  131070,    8191,   4, mp, -1>();
  check_bs_lower_bound<int, true,  131071,    131071, 5, mp, -1>();
  check_bs_lower_bound<int, true,  131072,    131071, 5, mp, -1>();
  check_bs_lower_bound<int, true,  524286,    131071, 5, mp, -1>();
  check_bs_lower_bound<int, true,  524287,    524287, 6, mp, -1>();
  check_bs_lower_bound<int, true,  524288,    524287, 6, mp, -1>();
}

///////////////////////////////////////////
// type_list::binary_search::upper_bound //
///////////////////////////////////////////

template <
  typename T,
  bool Result, T Needle, T ExpectedValue, std::size_t ExpectedIndex,
  typename TList, T Empty
>
void check_bs_upper_bound() {
  auto actual = Empty;
  std::size_t index = TList::size;

  auto result = TList::template binary_search<type_value_comparer>::upper_bound(
    Needle, bs_visitor<T>(), actual, index
  );

  auto const expectedResult = Result;
  EXPECT_EQ(expectedResult, result);
  auto const expectedValue = ExpectedValue;
  EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  EXPECT_EQ(expectedIndex, index);
}

TEST(type_list, binary_search_upper_bound) {
  typedef chr_seq<> empty;

  LOG(INFO) << "empty";
  check_bs_upper_bound<char, false, '-', '\0', empty::size, empty, '\0'>();
  check_bs_upper_bound<int,  false, 3,   -1,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  LOG(INFO) << "one";
  check_bs_upper_bound<char, true,  'w', 'x',  0, one, '\0'>();
  check_bs_upper_bound<char, false, 'x', '\0', one::size, one, '\0'>();
  check_bs_upper_bound<char, false, 'y', '\0', one::size, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  LOG(INFO) << "two";
  check_bs_upper_bound<char, true,  'w', 'x',  0, two, '\0'>();
  check_bs_upper_bound<char, true,  'x', 'y',  1, two, '\0'>();
  check_bs_upper_bound<char, false, 'y', '\0', two::size, two, '\0'>();
  check_bs_upper_bound<char, false, 'z', '\0', two::size, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  LOG(INFO) << "aeiou";
  check_bs_upper_bound<char, true, 'a' - 1, 'a',  0, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'a',     'e',  1, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'e',     'i',  2, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'i',     'o',  3, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'o',     'u',  4, aeiou, '\0'>();
  check_bs_upper_bound<char, false, 'u',    '\0', aeiou::size, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287> mp;

  LOG(INFO) << "mp";
  check_bs_upper_bound<int, true,  -1,         3,      0, mp, -1>();
  check_bs_upper_bound<int, true,  0,          3,      0, mp, -1>();
  check_bs_upper_bound<int, true,  2,          3,      0, mp, -1>();
  check_bs_upper_bound<int, true,  3,          7,      1, mp, -1>();
  check_bs_upper_bound<int, true,  4,          7,      1, mp, -1>();
  check_bs_upper_bound<int, true,  6,          7,      1, mp, -1>();
  check_bs_upper_bound<int, true,  7,          31,     2, mp, -1>();
  check_bs_upper_bound<int, true,  8,          31,     2, mp, -1>();
  check_bs_upper_bound<int, true,  30,         31,     2, mp, -1>();
  check_bs_upper_bound<int, true,  31,         127,    3, mp, -1>();
  check_bs_upper_bound<int, true,  32,         127,    3, mp, -1>();
  check_bs_upper_bound<int, true,  126,        127,    3, mp, -1>();
  check_bs_upper_bound<int, true,  127,        8191,   4, mp, -1>();
  check_bs_upper_bound<int, true,  128,        8191,   4, mp, -1>();
  check_bs_upper_bound<int, true,  8190,       8191,   4, mp, -1>();
  check_bs_upper_bound<int, true,  8191,       131071, 5, mp, -1>();
  check_bs_upper_bound<int, true,  8192,       131071, 5, mp, -1>();
  check_bs_upper_bound<int, true,  131070,     131071, 5, mp, -1>();
  check_bs_upper_bound<int, true,  131071,     524287, 6, mp, -1>();
  check_bs_upper_bound<int, true,  131072,     524287, 6, mp, -1>();
  check_bs_upper_bound<int, true,  524286,     524287, 6, mp, -1>();
  check_bs_upper_bound<int, false, 524287,     -1,     mp::size, mp, -1>();
  check_bs_upper_bound<int, false, 524288,     -1,     mp::size, mp, -1>();
}

//////////////
// type_get //
//////////////

template <typename TList>
struct check_type_get_visitor {
  template <typename T, std::size_t Index>
  void operator ()(indexed_type_tag<T, Index> const &) {
    expect_same<T, typename type_get<Index>::template type<TList>>();

    expect_same<
      typename TList::template at<Index>,
      typename type_get<Index>::template type<TList>
    >();
  }
};

template <typename... Args>
void check_type_get() {
  typedef type_list<Args...> list;
  list::foreach(check_type_get_visitor<list>());
}

TEST(type_get, type_list) {
  check_type_get<>();
  check_type_get<void>();
  check_type_get<int, double>();
  check_type_get<int, int, float>();
  check_type_get<
    type_list<bool, int>,
    type_list<std::string, std::string>,
    type_list<std::string, std::string, void>
  >();
}

//////////////////////////
// type_list_from::type //
//////////////////////////

TEST(type_list_from, type) {
  expect_same<type_list<>, type_list_from<>::type<void>>();

  expect_same<
    type_list<
      void
    >,
    type_list_from<
      transform::identity
    >::type<void>
  >();

  expect_same<
    type_list<
      double,
      std::string
    >,
    type_list_from<
      type_get_first,
      type_get_second
    >::type<std::pair<double, std::string>>
  >();

  expect_same<
    type_list<
      std::pair<double, std::string>,
      double,
      std::string
    >,
    type_list_from<
      transform::identity,
      type_get_first,
      type_get_second
    >::type<std::pair<double, std::string>>
  >();
}

} // namespace ftl {
