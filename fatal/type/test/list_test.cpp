/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/list.h>
#include <fatal/type/traits.h>

#include <fatal/test/driver.h>

namespace fatal {

///////////////
// type_list //
///////////////

template <int> struct T {};
template <int> struct P {};
template <int> struct S {};

using T0 = T<0>; using T1 = T<1>; using T2 = T<2>;
using P0 = P<0>; using P1 = P<1>; using P2 = P<2>;
using S0 = S<0>; using S1 = S<1>; using S2 = S<2>;

template <typename...> struct is_T: public std::false_type {};
template <int Value> struct is_T<T<Value>>: public std::true_type {};
template <typename...> struct is_P: public std::false_type {};
template <int Value> struct is_P<P<Value>>: public std::true_type {};
template <typename...> struct is_S: public std::false_type {};
template <int Value> struct is_S<S<Value>>: public std::true_type {};


using vl = type_list<void>;
using ivfl = type_list<int, void, float>;

using el = type_list<>;
using single = type_list<T0>;
using tl = type_list<T0, T1, T2>;
using tp = type_list<P0, P1, P2>;
using ts = type_list<S0, S1, S2>;
using tpl = type_list<P0, P1, P2, T0, T1, T2>;
using tls = type_list<T0, T1, T2, S0, S1, S2>;
using tpls = type_list<P0, P1, P2, T0, T1, T2, S0, S1, S2>;

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

template <typename T>
using halve_val = std::integral_constant<int, T::value / 2>;

struct xyz { int x; int y; int z; };

/////////////////////
// type_list::size //
/////////////////////

FATAL_TEST(size, size) {
  FATAL_EXPECT_EQ(3, tl::size);
  FATAL_EXPECT_EQ(0, (el::size));
  FATAL_EXPECT_EQ(6, (tls::size));
  FATAL_EXPECT_EQ(5, (type_list<T0, T1, T2, P0, P1>::size));
}

//////////////////////
// type_list::empty //
//////////////////////

FATAL_TEST(empty, empty) {
  FATAL_EXPECT_FALSE(tl::empty);
  FATAL_EXPECT_TRUE((el::empty));
  FATAL_EXPECT_FALSE((tls::empty));
  FATAL_EXPECT_FALSE((type_list<T0, T1, T2, P0, P1>::empty));
}

///////////////////
// type_list::at //
///////////////////

FATAL_TEST(at, at) {
  FATAL_EXPECT_SAME<T0, tl::at<0>>();
  FATAL_EXPECT_SAME<T1, tl::at<1>>();
  FATAL_EXPECT_SAME<T2, tl::at<2>>();
}

///////////////////////
// type_list::try_at //
///////////////////////

FATAL_TEST(try_at, try_at) {
  FATAL_EXPECT_SAME<el, el::try_at<0>>();
  FATAL_EXPECT_SAME<el, el::try_at<1>>();
  FATAL_EXPECT_SAME<el, el::try_at<2>>();
  FATAL_EXPECT_SAME<el, el::try_at<3>>();

  FATAL_EXPECT_SAME<tp, el::try_at<0, P0, P1, P2>>();
  FATAL_EXPECT_SAME<tp, el::try_at<1, P0, P1, P2>>();
  FATAL_EXPECT_SAME<tp, el::try_at<2, P0, P1, P2>>();
  FATAL_EXPECT_SAME<tp, el::try_at<3, P0, P1, P2>>();

  FATAL_EXPECT_SAME<type_list<T0>, tl::try_at<0>>();
  FATAL_EXPECT_SAME<type_list<T1>, tl::try_at<1>>();
  FATAL_EXPECT_SAME<type_list<T2>, tl::try_at<2>>();
  FATAL_EXPECT_SAME<el, tl::try_at<3>>();

  FATAL_EXPECT_SAME<type_list<T0>, tl::try_at<0, P0, P1, P2>>();
  FATAL_EXPECT_SAME<type_list<T1>, tl::try_at<1, P0, P1, P2>>();
  FATAL_EXPECT_SAME<type_list<T2>, tl::try_at<2, P0, P1, P2>>();
  FATAL_EXPECT_SAME<tp, tl::try_at<3, P0, P1, P2>>();
}

/////////////////////////
// type_list::index_of //
/////////////////////////

FATAL_TEST(index_of, index_of) {
  FATAL_EXPECT_EQ(el::size, el::index_of<T0>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<T1>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<T2>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<P0>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<P1>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<P2>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<S0>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<S1>::value);
  FATAL_EXPECT_EQ(el::size, el::index_of<S2>::value);

  FATAL_EXPECT_EQ(0, tl::index_of<T0>::value);
  FATAL_EXPECT_EQ(1, tl::index_of<T1>::value);
  FATAL_EXPECT_EQ(2, tl::index_of<T2>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<P0>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<P1>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<P2>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<S0>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<S1>::value);
  FATAL_EXPECT_EQ(tl::size, tl::index_of<S2>::value);

  FATAL_EXPECT_EQ(0, tpl::index_of<P0>::value);
  FATAL_EXPECT_EQ(1, tpl::index_of<P1>::value);
  FATAL_EXPECT_EQ(2, tpl::index_of<P2>::value);
  FATAL_EXPECT_EQ(3, tpl::index_of<T0>::value);
  FATAL_EXPECT_EQ(4, tpl::index_of<T1>::value);
  FATAL_EXPECT_EQ(5, tpl::index_of<T2>::value);
  FATAL_EXPECT_EQ(tpl::size, tpl::index_of<S0>::value);
  FATAL_EXPECT_EQ(tpl::size, tpl::index_of<S1>::value);
  FATAL_EXPECT_EQ(tpl::size, tpl::index_of<S2>::value);

  FATAL_EXPECT_EQ(tls::size, tls::index_of<P0>::value);
  FATAL_EXPECT_EQ(tls::size, tls::index_of<P1>::value);
  FATAL_EXPECT_EQ(tls::size, tls::index_of<P2>::value);
  FATAL_EXPECT_EQ(0, tls::index_of<T0>::value);
  FATAL_EXPECT_EQ(1, tls::index_of<T1>::value);
  FATAL_EXPECT_EQ(2, tls::index_of<T2>::value);
  FATAL_EXPECT_EQ(3, tls::index_of<S0>::value);
  FATAL_EXPECT_EQ(4, tls::index_of<S1>::value);
  FATAL_EXPECT_EQ(5, tls::index_of<S2>::value);

  FATAL_EXPECT_EQ(0, tpls::index_of<P0>::value);
  FATAL_EXPECT_EQ(1, tpls::index_of<P1>::value);
  FATAL_EXPECT_EQ(2, tpls::index_of<P2>::value);
  FATAL_EXPECT_EQ(3, tpls::index_of<T0>::value);
  FATAL_EXPECT_EQ(4, tpls::index_of<T1>::value);
  FATAL_EXPECT_EQ(5, tpls::index_of<T2>::value);
  FATAL_EXPECT_EQ(6, tpls::index_of<S0>::value);
  FATAL_EXPECT_EQ(7, tpls::index_of<S1>::value);
  FATAL_EXPECT_EQ(8, tpls::index_of<S2>::value);
}

/////////////////////////////////
// type_list::checked_index_of //
/////////////////////////////////

FATAL_TEST(checked_index_of, checked_index_of) {
  FATAL_EXPECT_EQ(0, tl::checked_index_of<T0>::value);
  FATAL_EXPECT_EQ(1, tl::checked_index_of<T1>::value);
  FATAL_EXPECT_EQ(2, tl::checked_index_of<T2>::value);

  FATAL_EXPECT_EQ(0, tpl::checked_index_of<P0>::value);
  FATAL_EXPECT_EQ(1, tpl::checked_index_of<P1>::value);
  FATAL_EXPECT_EQ(2, tpl::checked_index_of<P2>::value);
  FATAL_EXPECT_EQ(3, tpl::checked_index_of<T0>::value);
  FATAL_EXPECT_EQ(4, tpl::checked_index_of<T1>::value);
  FATAL_EXPECT_EQ(5, tpl::checked_index_of<T2>::value);

  FATAL_EXPECT_EQ(0, tls::checked_index_of<T0>::value);
  FATAL_EXPECT_EQ(1, tls::checked_index_of<T1>::value);
  FATAL_EXPECT_EQ(2, tls::checked_index_of<T2>::value);
  FATAL_EXPECT_EQ(3, tls::checked_index_of<S0>::value);
  FATAL_EXPECT_EQ(4, tls::checked_index_of<S1>::value);
  FATAL_EXPECT_EQ(5, tls::checked_index_of<S2>::value);

  FATAL_EXPECT_EQ(0, tpls::checked_index_of<P0>::value);
  FATAL_EXPECT_EQ(1, tpls::checked_index_of<P1>::value);
  FATAL_EXPECT_EQ(2, tpls::checked_index_of<P2>::value);
  FATAL_EXPECT_EQ(3, tpls::checked_index_of<T0>::value);
  FATAL_EXPECT_EQ(4, tpls::checked_index_of<T1>::value);
  FATAL_EXPECT_EQ(5, tpls::checked_index_of<T2>::value);
  FATAL_EXPECT_EQ(6, tpls::checked_index_of<S0>::value);
  FATAL_EXPECT_EQ(7, tpls::checked_index_of<S1>::value);
  FATAL_EXPECT_EQ(8, tpls::checked_index_of<S2>::value);
}

////////////////////////
// type_list::type_at //
////////////////////////

FATAL_TEST(type_at, type_at) {
  FATAL_EXPECT_EQ(typeid(P0), tp::type_at(0));
  FATAL_EXPECT_EQ(typeid(P1), tp::type_at(1));
  FATAL_EXPECT_EQ(typeid(P2), tp::type_at(2));

  FATAL_EXPECT_EQ(typeid(T0), tl::type_at(0));
  FATAL_EXPECT_EQ(typeid(T1), tl::type_at(1));
  FATAL_EXPECT_EQ(typeid(T2), tl::type_at(2));

  FATAL_EXPECT_EQ(typeid(S0), ts::type_at(0));
  FATAL_EXPECT_EQ(typeid(S1), ts::type_at(1));
  FATAL_EXPECT_EQ(typeid(S2), ts::type_at(2));

  FATAL_EXPECT_EQ(typeid(P0), tpl::type_at(0));
  FATAL_EXPECT_EQ(typeid(P1), tpl::type_at(1));
  FATAL_EXPECT_EQ(typeid(P2), tpl::type_at(2));
  FATAL_EXPECT_EQ(typeid(T0), tpl::type_at(3));
  FATAL_EXPECT_EQ(typeid(T1), tpl::type_at(4));
  FATAL_EXPECT_EQ(typeid(T2), tpl::type_at(5));

  FATAL_EXPECT_EQ(typeid(T0), tls::type_at(0));
  FATAL_EXPECT_EQ(typeid(T1), tls::type_at(1));
  FATAL_EXPECT_EQ(typeid(T2), tls::type_at(2));
  FATAL_EXPECT_EQ(typeid(S0), tls::type_at(3));
  FATAL_EXPECT_EQ(typeid(S1), tls::type_at(4));
  FATAL_EXPECT_EQ(typeid(S2), tls::type_at(5));

  FATAL_EXPECT_EQ(typeid(P0), tpls::type_at(0));
  FATAL_EXPECT_EQ(typeid(P1), tpls::type_at(1));
  FATAL_EXPECT_EQ(typeid(P2), tpls::type_at(2));
  FATAL_EXPECT_EQ(typeid(T0), tpls::type_at(3));
  FATAL_EXPECT_EQ(typeid(T1), tpls::type_at(4));
  FATAL_EXPECT_EQ(typeid(T2), tpls::type_at(5));
  FATAL_EXPECT_EQ(typeid(S0), tpls::type_at(6));
  FATAL_EXPECT_EQ(typeid(S1), tpls::type_at(7));
  FATAL_EXPECT_EQ(typeid(S2), tpls::type_at(8));
}

/////////////////////////
// type_list::contains //
/////////////////////////

FATAL_TEST(contains, contains) {
  FATAL_EXPECT_FALSE(el::contains<T0>::value);
  FATAL_EXPECT_FALSE(el::contains<T1>::value);
  FATAL_EXPECT_FALSE(el::contains<T2>::value);
  FATAL_EXPECT_FALSE(el::contains<P0>::value);
  FATAL_EXPECT_FALSE(el::contains<P1>::value);
  FATAL_EXPECT_FALSE(el::contains<P2>::value);
  FATAL_EXPECT_FALSE(el::contains<S0>::value);
  FATAL_EXPECT_FALSE(el::contains<S1>::value);
  FATAL_EXPECT_FALSE(el::contains<S2>::value);

  FATAL_EXPECT_TRUE(tl::contains<T0>::value);
  FATAL_EXPECT_TRUE(tl::contains<T1>::value);
  FATAL_EXPECT_TRUE(tl::contains<T2>::value);
  FATAL_EXPECT_FALSE(tl::contains<P0>::value);
  FATAL_EXPECT_FALSE(tl::contains<P1>::value);
  FATAL_EXPECT_FALSE(tl::contains<P2>::value);
  FATAL_EXPECT_FALSE(tl::contains<S0>::value);
  FATAL_EXPECT_FALSE(tl::contains<S1>::value);
  FATAL_EXPECT_FALSE(tl::contains<S2>::value);
}

//////////////////////////
// type_list::push_back //
//////////////////////////

FATAL_TEST(push_back, push_back) {
  FATAL_EXPECT_SAME<tl::push_back<>, tl>();
  FATAL_EXPECT_SAME<tl::push_back<S0, S1, S2>, tls>();
}

///////////////////////////
// type_list::push_front //
///////////////////////////

FATAL_TEST(push_front, push_front) {
  FATAL_EXPECT_SAME<tl::push_front<>, tl>();
  FATAL_EXPECT_SAME<tl::push_front<P0, P1, P2>, tpl>();
}

///////////////////////
// type_list::concat //
///////////////////////

FATAL_TEST(concat, concat) {
  FATAL_EXPECT_SAME<tp::concat<tl>, tpl>();
  FATAL_EXPECT_SAME<tl::concat<ts>, tls>();
  FATAL_EXPECT_SAME<tl::concat<el>, tl>();
  FATAL_EXPECT_SAME<el::concat<tl>, tl>();
  FATAL_EXPECT_SAME<el::concat<el>, el>();
}

//////////////////////////////
// type_list::insert_sorted //
//////////////////////////////

FATAL_TEST(insert_sorted, insert_sorted) {
  FATAL_EXPECT_SAME<
    type_list<int_val<0>>,
    el::insert_sorted<int_val<0>>
  >();

  FATAL_EXPECT_SAME<
    type_list<int_val<0>, int_val<0>>,
    type_list<int_val<0>>::insert_sorted<int_val<0>>
  >();

  FATAL_EXPECT_SAME<
    type_list<int_val<0>, int_val<1>, int_val<2>, int_val<3>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>>
  >();

  FATAL_EXPECT_SAME<
    type_list<int_val<5>, int_val<4>, int_val<3>, int_val<1>>,
    type_list<int_val<5>, int_val<3>, int_val<1>>
      ::insert_sorted<int_val<4>, comparison_transform::greater_than>
  >();

  FATAL_EXPECT_SAME<
    type_list<int_val<0>, int_val<1>, int_val<3>, int_val<2>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>, false_predicate>
  >();

  FATAL_EXPECT_SAME<
    type_list<int_val<2>, int_val<0>, int_val<1>, int_val<3>>,
    type_list<int_val<0>, int_val<1>, int_val<3>>
      ::insert_sorted<int_val<2>, true_predicate>
  >();
}

/////////////////////////
// type_list::multiply //
/////////////////////////

FATAL_TEST(multiply, multiply) {
  FATAL_EXPECT_SAME<el, el::multiply<0>>();
  FATAL_EXPECT_SAME<el, el::multiply<1>>();
  FATAL_EXPECT_SAME<el, el::multiply<2>>();
  FATAL_EXPECT_SAME<el, el::multiply<3>>();
  FATAL_EXPECT_SAME<el, el::multiply<4>>();
  FATAL_EXPECT_SAME<el, el::multiply<5>>();

  FATAL_EXPECT_SAME<el, vl::multiply<0>>();
  FATAL_EXPECT_SAME<vl, vl::multiply<1>>();
  FATAL_EXPECT_SAME<type_list<void, void>, vl::multiply<2>>();
  FATAL_EXPECT_SAME<type_list<void, void, void>, vl::multiply<3>>();
  FATAL_EXPECT_SAME<type_list<void, void, void, void>, vl::multiply<4>>();
  FATAL_EXPECT_SAME<type_list<void, void, void, void, void>, vl::multiply<5>>();

  FATAL_EXPECT_SAME<el, ivfl::multiply<0>>();
  FATAL_EXPECT_SAME<ivfl, ivfl::multiply<1>>();
  FATAL_EXPECT_SAME<
    type_list<
      int, void, float,
      int, void, float
    >,
    ivfl::multiply<2>
  >();
  FATAL_EXPECT_SAME<
    type_list<
      int, void, float,
      int, void, float,
      int, void, float
    >,
    ivfl::multiply<3>
  >();
  FATAL_EXPECT_SAME<
    type_list<
      int, void, float,
      int, void, float,
      int, void, float,
      int, void, float
    >,
    ivfl::multiply<4>
  >();
  FATAL_EXPECT_SAME<
    type_list<
      int, void, float,
      int, void, float,
      int, void, float,
      int, void, float,
      int, void, float
    >,
    ivfl::multiply<5>
  >();
}

//////////////////////
// type_list::apply //
//////////////////////

FATAL_TEST(apply, apply) {
  FATAL_EXPECT_SAME<tl::apply<std::tuple>, std::tuple<T0, T1, T2>>();
}

///////////////////////////
// type_list::apply_back //
///////////////////////////

FATAL_TEST(apply_back, apply_back) {
  FATAL_EXPECT_SAME<
    tl::apply_back<std::tuple, S0, S1, S2>,
    std::tuple<T0, T1, T2, S0, S1, S2>
  >();
}

////////////////////////////
// type_list::apply_front //
////////////////////////////

FATAL_TEST(apply_front, apply_front) {
  FATAL_EXPECT_SAME<
    tl::apply_front<std::tuple, P0, P1, P2>,
    std::tuple<P0, P1, P2, T0, T1, T2>
  >();
}

///////////////////////////
// type_list::foreach_if //
///////////////////////////

template <template <int...> class TTemplate>
struct foreach_if_predicate {
  template <int... UArgs> static std::true_type sfinae(TTemplate<UArgs...> *);
  static std::false_type sfinae(...);

  template <typename U>
  using apply = decltype(sfinae(static_cast<U *>(nullptr)));
};

typedef std::vector<std::pair<std::size_t, std::size_t>> foreach_if_expected;

template <template <int...> class TTemplate>
struct foreach_if_test_visitor {
  explicit foreach_if_test_visitor(foreach_if_expected expected):
    expected_(std::move(expected))
  {}

  ~foreach_if_test_visitor() {
    FATAL_EXPECT_EQ(expected_.size(), index_);
  }

  template <int Id, std::size_t Index>
  void operator ()(indexed_type_tag<TTemplate<Id>, Index>) {
    FATAL_ASSERT_LE(index_, expected_.size());
    auto const &e = expected_[index_++];
    FATAL_EXPECT_EQ(e.first, Index);
    FATAL_EXPECT_EQ(e.second, Id);
  }

private:
  std::size_t index_ = 0;
  foreach_if_expected const expected_;
};

template <template <int...> class TTemplate, typename TList, typename... Args>
void check_foreach_if(foreach_if_expected expected) {
  foreach_if_test_visitor<TTemplate> visitor(expected);

  auto const visited = TList::template foreach_if<
    foreach_if_predicate<TTemplate>::template apply
  >(visitor);

  FATAL_EXPECT_EQ(expected.size(), visited);
}

FATAL_TEST(foreach_if, foreach_if) {
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
  using expected_list = TExpectedList;

  template <typename U, std::size_t Index>
  void operator ()(indexed_type_tag<U, Index>) {
    FATAL_EXPECT_SAME<typename expected_list::template at<Index>, U>();

    visited.push_back(Index);
  }

  std::vector<std::size_t> visited;
};

template <typename TList>
void check_foreach() {
  foreach_test_visitor<TList> visitor;

  TList::foreach(visitor);

  FATAL_EXPECT_EQ(TList::size, visitor.visited.size());
  for (auto i = visitor.visited.size(); i--; ) {
    FATAL_EXPECT_EQ(i, visitor.visited[i]);
  }
}

FATAL_TEST(foreach, foreach) {
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
    FATAL_EXPECT_FALSE(visited_);
    visited_ = true;
    FATAL_EXPECT_SAME<TExpected, T>();
    FATAL_EXPECT_EQ(expected_index, Index);
  }

private:
  bool visited_ = false;
};

struct visit_no_visit_test_visitor {
  template <typename T, std::size_t Index>
  void operator ()(indexed_type_tag<T, Index>, std::size_t) {
    FATAL_EXPECT_TRUE(false);
  }
};

FATAL_TEST(visit, visit) {
  FATAL_EXPECT_FALSE((el::visit(0, visit_no_visit_test_visitor())));

  using list = type_list<double, float, long, bool, int>;

  FATAL_EXPECT_TRUE(list::visit(0, visit_test_visitor<double>(), 0));
  FATAL_EXPECT_TRUE(list::visit(1, visit_test_visitor<float>(), 1));
  FATAL_EXPECT_TRUE(list::visit(2, visit_test_visitor<long>(), 2));
  FATAL_EXPECT_TRUE(list::visit(3, visit_test_visitor<bool>(), 3));
  FATAL_EXPECT_TRUE(list::visit(4, visit_test_visitor<int>(), 4));
  FATAL_EXPECT_FALSE(list::visit(5, visit_no_visit_test_visitor(), 5));
}

//////////////////////////
// type_list::transform //
//////////////////////////

FATAL_TEST(transform, transform) {
  FATAL_EXPECT_SAME<
    type_list<std::tuple<T0>, std::tuple<T1>, std::tuple<T2>>,
    tl::transform<std::tuple>
  >();

  FATAL_EXPECT_SAME<
    type_list<std::tuple<T0>>,
    type_list<T0>::transform<std::tuple>
  >();

  FATAL_EXPECT_SAME<el, el::transform<std::tuple>>();

  FATAL_EXPECT_SAME<
    type_list<
      std::vector<std::tuple<T0>>,
      std::vector<std::tuple<T1>>,
      std::vector<std::tuple<T2>>
    >,
    tl::transform<std::tuple, std::vector>
  >();

  FATAL_EXPECT_SAME<
    type_list<std::vector<std::tuple<T0>>>,
    type_list<T0>::transform<std::tuple, std::vector>
  >();

  FATAL_EXPECT_SAME<el, el::transform<std::tuple, std::vector>>();
}

//////////////////////////////////
// type_list::indexed_transform //
//////////////////////////////////

template <typename T, std::size_t N>
using indexed_transform_test_impl = indexed_type_tag<T, N>;

FATAL_TEST(indexed_transform, indexed_transform) {
  FATAL_EXPECT_SAME<
    type_list<
      indexed_type_tag<T0, 0>,
      indexed_type_tag<T1, 1>,
      indexed_type_tag<T2, 2>
    >,
    tl::indexed_transform<indexed_transform_test_impl>
  >();

  FATAL_EXPECT_SAME<
    type_list<indexed_type_tag<T0, 0>>,
    single::indexed_transform<indexed_transform_test_impl>
  >();

  FATAL_EXPECT_SAME<
    el,
    el::indexed_transform<indexed_transform_test_impl>
  >();
}

///////////////////////////
// type_list::accumulate //
///////////////////////////

FATAL_TEST(accumulate, accumulate) {
# define FATAL_TEST_IMPL(TExpected, TTransform, ...) \
  do { \
    FATAL_EXPECT_EQ( \
      TExpected, \
      (int_seq<__VA_ARGS__>::accumulate<TTransform>::value) \
    ); \
  } while (false)

  FATAL_TEST_IMPL(7, arithmetic_transform::add, 7);
  FATAL_TEST_IMPL(8, arithmetic_transform::add, 1, 7);
  FATAL_TEST_IMPL(8, arithmetic_transform::add, 7, 1);

  FATAL_TEST_IMPL(28, arithmetic_transform::add, 1, 2, 3, 4, 5, 6, 7);
  FATAL_TEST_IMPL(28, arithmetic_transform::add, 7, 6, 5, 4, 3, 2, 1);

  FATAL_TEST_IMPL(28, arithmetic_transform::add, 1, 6, 2, 7, 5, 3, 4);

# undef FATAL_TEST_IMPL

# define FATAL_TEST_IMPL(TExpected, TTransform, TSeed, ...) \
  do { \
    FATAL_EXPECT_EQ( \
      TExpected, \
      (int_seq<__VA_ARGS__>::accumulate<TTransform, int_val<TSeed>>::value) \
    ); \
  } while (false)

  FATAL_TEST_IMPL(107, arithmetic_transform::add, 100, 7);
  FATAL_TEST_IMPL(108, arithmetic_transform::add, 100, 1, 7);
  FATAL_TEST_IMPL(108, arithmetic_transform::add, 100, 7, 1);

  FATAL_TEST_IMPL(128, arithmetic_transform::add, 100, 1, 2, 3, 4, 5, 6, 7);
  FATAL_TEST_IMPL(128, arithmetic_transform::add, 100, 7, 6, 5, 4, 3, 2, 1);

  FATAL_TEST_IMPL(128, arithmetic_transform::add, 100, 1, 6, 2, 7, 5, 3, 4);

# undef FATAL_TEST_IMPL
}

///////////////////////
// type_list::choose //
///////////////////////

FATAL_TEST(choose, choose) {
# define FATAL_TEST_IMPL(TExpected, TPredicate, ...) \
  do { \
    FATAL_EXPECT_SAME< \
      int_val<TExpected>, \
      int_seq<__VA_ARGS__>::choose<TPredicate> \
    >(); \
  } while (false)

  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 7);
  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 1, 7);
  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 7, 1);

  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 1, 2, 3, 4, 5, 6, 7);
  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 7, 6, 5, 4, 3, 2, 1);

  FATAL_TEST_IMPL(7, comparison_transform::greater_than, 1, 6, 2, 7, 5, 3, 4);

# undef FATAL_TEST_IMPL
}

////////////////////////
// type_list::replace //
////////////////////////

FATAL_TEST(replace, replace) {
  FATAL_EXPECT_SAME<el, el::replace<int, double>>();
  FATAL_EXPECT_SAME<tl, tl::replace<int, double>>();
  FATAL_EXPECT_SAME<tl, tl::replace<T1, T1>>();

  FATAL_EXPECT_SAME<
    type_list<T0, int, T2>,
    tl::replace<T1, int>
  >();

  FATAL_EXPECT_SAME<
    tp,
    tl::replace<T0, P0>
      ::replace<T1, P1>
      ::replace<T2, P2>
  >();
}

/////////////////////
// type_list::tail //
/////////////////////

FATAL_TEST(tail, tail) {
  FATAL_EXPECT_SAME<tl, tl::tail<0>>();
  FATAL_EXPECT_SAME<type_list<T1, T2>, tl::tail<1>>();
  FATAL_EXPECT_SAME<type_list<T2>, tl::tail<2>>();
  FATAL_EXPECT_SAME<el, tl::tail<3>>();
  FATAL_EXPECT_SAME<el, tl::tail<tl::size>>();
}

//////////////////////
// type_list::split //
//////////////////////

template <typename TList, std::size_t Index>
void check_split_impl() {
  FATAL_EXPECT_SAME<
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

FATAL_TEST(split, split) {
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

//////////////////////
// type_list::slice //
//////////////////////

FATAL_TEST(slice, slice) {
  FATAL_EXPECT_SAME<el, tl::slice<0, 0>>();

  FATAL_EXPECT_SAME<type_list<T0>, tl::slice<0, 1>>();
  FATAL_EXPECT_SAME<type_list<>, tl::slice<1, 1>>();

  FATAL_EXPECT_SAME<type_list<T0, T1>, tl::slice<0, 2>>();
  FATAL_EXPECT_SAME<type_list<T1>, tl::slice<1, 2>>();
  FATAL_EXPECT_SAME<type_list<>, tl::slice<2, 2>>();

  FATAL_EXPECT_SAME<tl, tl::slice<0, tl::size>>();
  FATAL_EXPECT_SAME<type_list<T1, T2>, tl::slice<1, tl::size>>();
  FATAL_EXPECT_SAME<type_list<T2>, tl::slice<2, tl::size>>();
  FATAL_EXPECT_SAME<type_list<>, tl::slice<tl::size, tl::size>>();
}

/////////////////////
// type_list::left //
/////////////////////

FATAL_TEST(left, left) {
  FATAL_EXPECT_SAME<el, tl::left<0>>();
  FATAL_EXPECT_SAME<type_list<T0>, tl::left<1>>();
  FATAL_EXPECT_SAME<type_list<T0, T1>, tl::left<2>>();
  FATAL_EXPECT_SAME<tl, tl::left<3>>();
  FATAL_EXPECT_SAME<tl, tl::left<tl::size>>();
}

//////////////////////
// type_list::right //
//////////////////////

FATAL_TEST(right, right) {
  FATAL_EXPECT_SAME<el, tl::right<0>>();
  FATAL_EXPECT_SAME<type_list<T2>, tl::right<1>>();
  FATAL_EXPECT_SAME<type_list<T1, T2>, tl::right<2>>();
  FATAL_EXPECT_SAME<tl, tl::right<3>>();
  FATAL_EXPECT_SAME<tl, tl::right<tl::size>>();
}

/////////////////////////
// type_list::separate //
/////////////////////////

template <
  typename TList,
  template <typename...> class TPredicate,
  typename TExpectedFirst,
  typename TExpectedSecond
>
void check_separate()  {
  typedef typename TList::template separate<TPredicate> actual;
  FATAL_EXPECT_SAME<TExpectedFirst, typename actual::first>();
  FATAL_EXPECT_SAME<TExpectedSecond, typename actual::second>();
};

FATAL_TEST(separate, separate) {
  check_separate<el, false_predicate, el, el>();
  check_separate<el, true_predicate, el, el>();
  check_separate<tl, false_predicate, el, tl>();
  check_separate<tl, true_predicate, tl, el>();
}

///////////////////////
// type_list::filter //
///////////////////////

template <
  typename TList,
  template <typename...> class TPredicate,
  typename TExpected
>
void check_filter()  {
  FATAL_EXPECT_SAME<TExpected, typename TList::template filter<TPredicate>>();
};

FATAL_TEST(filter, filter) {
  check_filter<el, false_predicate, el>();
  check_filter<el, true_predicate, el>();
  check_filter<tl, false_predicate, el>();
  check_filter<tl, true_predicate, tl>();
}

///////////////////////
// type_list::reject //
///////////////////////

template <
  typename TList,
  template <typename...> class TPredicate,
  typename TExpected
>
void check_inverse_filter()  {
  FATAL_EXPECT_SAME<
    TExpected,
    typename TList::template reject<TPredicate>
  >();
};

FATAL_TEST(reject, reject) {
  check_inverse_filter<el, false_predicate, el>();
  check_inverse_filter<el, true_predicate, el>();
  check_inverse_filter<tl, false_predicate, tl>();
  check_inverse_filter<tl, true_predicate, el>();
}

///////////////////////
// type_list::remove //
///////////////////////

FATAL_TEST(remove, remove) {
  FATAL_EXPECT_SAME<type_list<>, type_list<>::remove<>>();
  FATAL_EXPECT_SAME<type_list<>, type_list<int>::remove<int>>();
  FATAL_EXPECT_SAME<
    type_list<>,
    type_list<int, bool, float>::remove<bool, int, float>
  >();

  FATAL_EXPECT_SAME<type_list<int>, type_list<int>::remove<>>();
  FATAL_EXPECT_SAME<type_list<int>, type_list<int>::remove<void>>();
  FATAL_EXPECT_SAME<type_list<int>, type_list<int>::remove<void, float>>();

  FATAL_EXPECT_SAME<type_list<int, float>, type_list<int, float>::remove<>>();
  FATAL_EXPECT_SAME<
    type_list<int, float>,
    type_list<int, float>::remove<void>
  >();
  FATAL_EXPECT_SAME<
    type_list<int, float>,
    type_list<int, float>::remove<void, bool>
  >();

  FATAL_EXPECT_SAME<
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
  FATAL_EXPECT_SAME<TExpected, actual>();
};

FATAL_TEST(zip, zip) {
  check_zip<el, el, el>();
  check_zip<tl, el, tl>();
  check_zip<el, tl, tl>();
  check_zip<ts, single, type_list<S0, T0, S1, S2>>();
  check_zip<single, ts, type_list<T0, S0, S1, S2>>();
  check_zip<single, single, type_list<T0, T0>>();
  check_zip<tp, ts, type_list<P0, S0, P1, S1, P2, S2>>();
}

//////////////////////
// type_list::unzip //
//////////////////////

#define CHECK_UNZIP(Step, Offset, TList, ...) \
  do { \
    using expected = __VA_ARGS__; \
    using list = TList; \
    using actual = typename list::template unzip<Step, Offset>; \
    FATAL_EXPECT_SAME<expected, actual>(); \
  } while (false)

FATAL_TEST(unzip, unzip) {
  CHECK_UNZIP(1, 0, single, single);
  CHECK_UNZIP(1, 1, single, el);

  CHECK_UNZIP(tl::size, 0, tl, type_list<T0>);
  CHECK_UNZIP(tl::size, 1, tl, type_list<T1>);
  CHECK_UNZIP(tl::size, 2, tl, type_list<T2>);

  CHECK_UNZIP(3, 0, tpls, type_list<P0, T0, S0>);
  CHECK_UNZIP(3, 1, tpls, type_list<P1, T1, S1>);
  CHECK_UNZIP(3, 2, tpls, type_list<P2, T2, S2>);
}

FATAL_TEST(unzip, empty) {
# define CHECK_EMPTY_UNZIP(...) \
  do { \
    using elist = __VA_ARGS__; \
    CHECK_UNZIP(1, elist::size, elist, el); \
    CHECK_UNZIP(elist::size, elist::size, elist, el); \
  } while (false)

  CHECK_EMPTY_UNZIP(el);
  CHECK_EMPTY_UNZIP(tl);
  CHECK_EMPTY_UNZIP(single);
  CHECK_EMPTY_UNZIP(tpls);

# undef CHECK_EMPTY_UNZIP
}

FATAL_TEST(unzip, full) {
# define CHECK_FULL_UNZIP(...) \
  do { \
    using flist = __VA_ARGS__; \
    CHECK_UNZIP(1, 0, flist, flist); \
  } while (false)

  CHECK_FULL_UNZIP(el);
  CHECK_FULL_UNZIP(tl);
  CHECK_FULL_UNZIP(single);
  CHECK_FULL_UNZIP(tpls);

# undef CHECK_FULL_UNZIP
}

#undef CHECK_UNZIP

///////////////////////
// type_list::search //
///////////////////////

template <
  typename TList,
  template <typename...> class TPredicate,
  typename TDefault,
  typename TExpected
>
void check_search() {
  typedef typename TList::template search<TPredicate, TDefault> actual;
  FATAL_EXPECT_SAME<TExpected, actual>();
}

FATAL_TEST(search, search) {
  typedef transform_alias<std::is_same, T0> same_as_t0;
  check_search<el, same_as_t0::template apply, void, void>();
  check_search<tl, same_as_t0::template apply, void, T0>();
  check_search<tp, same_as_t0::template apply, void, void>();

  typedef transform_alias<std::is_same, T1> same_as_t1;
  check_search<el, same_as_t1::template apply, void, void>();
  check_search<tl, same_as_t1::template apply, void, T1>();
  check_search<tp, same_as_t1::template apply, void, void>();

  typedef transform_alias<std::is_same, T2> same_as_t2;
  check_search<el, same_as_t2::template apply, void, void>();
  check_search<tl, same_as_t2::template apply, void, T2>();
  check_search<tp, same_as_t2::template apply, void, void>();

  typedef is_template<std::basic_string> is_std_string;
  typedef is_template<std::tuple> is_std_tuple;

  typedef type_list<int, std::string, double, long> types1;
  check_search<types1, is_std_string::type, void, std::string>();
  check_search<types1, is_std_tuple::type, void, void>();

  typedef type_list<int, float, double, std::tuple<long, bool>> types2;
  check_search<types2, is_std_string::type, void, void>();
  check_search<types2, is_std_tuple::type, void, std::tuple<long, bool>>();
}

////////////////////////
// type_list::combine //
////////////////////////

template <typename... TInput>
struct check_combine {
  template <template <typename...> class TCombiner, typename... Args>
  struct test {
    template <typename... TExpected>
    static void expect() {
      using expected = type_list<TExpected...>;
      using input = type_list<TInput...>;
      FATAL_EXPECT_SAME<
        expected,
        typename input::template combine<TCombiner>::template args<Args...>
      >();
      using rhs = type_list<Args...>;
      FATAL_EXPECT_SAME<
        expected,
        typename input::template combine<TCombiner>::template list<rhs>
      >();
    }
  };
};

FATAL_TEST(combine, combine) {
  check_combine<>::test<std::pair>::expect<>();
  check_combine<int>::test<std::pair, bool>::expect<std::pair<int, bool>>();
  check_combine<int, double>::test<std::pair, bool, long>::expect<
    std::pair<int, bool>, std::pair<double, long>
  >();
  check_combine<int, double, short>::test<std::pair, bool, long, float>::expect<
    std::pair<int, bool>, std::pair<double, long>, std::pair<short, float>
  >();
}

/////////////
// flatten //
/////////////

#define CHECK_FLATTEN(Expected, ...) \
  do { \
    typedef type_list<__VA_ARGS__> list; \
    FATAL_EXPECT_SAME<list, typename list::template flatten<>>(); \
    FATAL_EXPECT_SAME<list, typename list::template flatten<0>>(); \
    FATAL_EXPECT_SAME<list, typename list::template flatten<1>>(); \
    FATAL_EXPECT_SAME<list, typename list::template flatten<2>>(); \
    FATAL_EXPECT_SAME<list, typename list::template flatten<3>>(); \
  } while (false)

FATAL_TEST(flatten, flatten) {
  CHECK_FLATTEN();
  CHECK_FLATTEN(void);
  CHECK_FLATTEN(int);
  CHECK_FLATTEN(int, void);
  CHECK_FLATTEN(int, void, bool);
  CHECK_FLATTEN(int, void, bool, double);

  {
    using list = type_list<type_list<type_list<type_list<vl>>>>;

    using expected1 = type_list<type_list<type_list<vl>>>;
    using expected2 = type_list<type_list<vl>>;
    using expected3 = type_list<vl>;
    using expected_unlimited = vl;

    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<>>();
    FATAL_EXPECT_SAME<list, list::flatten<0>>();
    FATAL_EXPECT_SAME<expected1, list::flatten<1>>();
    FATAL_EXPECT_SAME<expected2, list::flatten<2>>();
    FATAL_EXPECT_SAME<expected3, list::flatten<3>>();
    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<4>>();
    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<5>>();
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

    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<>>();
    FATAL_EXPECT_SAME<list, list::flatten<0>>();
    FATAL_EXPECT_SAME<expected1, list::flatten<1>>();
    FATAL_EXPECT_SAME<expected2, list::flatten<2>>();
    FATAL_EXPECT_SAME<expected3, list::flatten<3>>();
    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<4>>();
    FATAL_EXPECT_SAME<expected_unlimited, list::flatten<5>>();
  }
}

#undef CHECK_FLATTEN

//////////////////////////
// type_list::is_sorted //
//////////////////////////

#define CHECK_IS_SORTED(Expected, ...) \
  do { \
    FATAL_EXPECT_SAME< \
      std::integral_constant<bool, Expected>, \
      int_seq<__VA_ARGS__>::is_sorted<> \
    >(); \
  } while (false)

FATAL_TEST(is_sorted, is_sorted) {
  CHECK_IS_SORTED(true);
  CHECK_IS_SORTED(true, 0);
  CHECK_IS_SORTED(true, 0, 1);
  CHECK_IS_SORTED(true, 1, 1);
  CHECK_IS_SORTED(true, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  CHECK_IS_SORTED(true, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

  CHECK_IS_SORTED(false, 1, 0);
  CHECK_IS_SORTED(false, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
}

#undef CHECK_IS_SORTED

//////////////////////
// type_list::merge //
//////////////////////

template <typename TLHS, typename TRHS, typename TActual, int... Expected>
void check_merge_impl() {
  using expected = int_seq<Expected...> ;

  if (!std::is_same<expected, TActual>::value) {
    FATAL_VLOG(1) << "lhs: '" << type_str<TLHS>() << '\'';
    FATAL_VLOG(1) << "rhs: '" << type_str<TRHS>() << '\'';
  }

  FATAL_EXPECT_SAME<expected, TActual>();
}

template <typename TLHS, typename TRHS, int... Expected>
void check_merge() {
  check_merge_impl<
    TLHS, TRHS,
    typename TLHS::template merge<>::template list<TRHS>
      ::template apply_values<int, int_seq>,
    Expected...
  >();

  check_merge_impl<
    TLHS, TRHS,
    typename TRHS::template apply<TLHS::template merge<>::template args>
      ::template apply_values<int, int_seq>,
    Expected...
  >();
}

FATAL_TEST(merge, merge) {
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

/////////////////////
// type_list::sort //
/////////////////////

template <typename TExpectedList, int... Values>
void check_sort() {
  typedef TExpectedList expected;

  static_assert(
    expected::template is_sorted<>::value,
    "expected list should be sorted"
  );

  using input = int_seq<Values...>;
  using sorted = typename input::template sort<>;
  auto is_sorted = sorted::template is_sorted<>::value;
  using actual = typename sorted::template apply_values<int, int_seq>;

  if (!is_sorted || !std::is_same<expected, actual>::value) {
    FATAL_VLOG(1) << "input: '" << type_str<input>() << '\'';
    FATAL_VLOG(1) << "result: '" << type_str<actual>() << '\'';
  }

  FATAL_EXPECT_TRUE(is_sorted);
  FATAL_EXPECT_SAME<expected, actual>();
}

FATAL_TEST(sort, sort) {
  check_sort<int_seq<>>();
  check_sort<int_seq<1>, 1>();

  check_sort<int_seq<1, 1>, 1, 1>();
  check_sort<int_seq<1, 2>, 1, 2>();
  check_sort<int_seq<1, 2>, 2, 1>();

  check_sort<int_seq<0, 1, 2>, 0, 1, 2>();
  check_sort<int_seq<0, 1, 2>, 0, 2, 1>();
  check_sort<int_seq<0, 1, 2>, 1, 0, 2>();
  check_sort<int_seq<0, 1, 2>, 1, 2, 0>();
  check_sort<int_seq<0, 1, 2>, 2, 0, 1>();
  check_sort<int_seq<0, 1, 2>, 2, 1, 0>();

  check_sort<int_seq<0, 1, 1>, 0, 1, 1>();
  check_sort<int_seq<0, 1, 1>, 0, 1, 1>();
  check_sort<int_seq<0, 1, 1>, 1, 0, 1>();
  check_sort<int_seq<0, 1, 1>, 1, 1, 0>();
  check_sort<int_seq<0, 1, 1>, 1, 0, 1>();
  check_sort<int_seq<0, 1, 1>, 1, 1, 0>();

  check_sort<int_seq<1, 1, 1>, 1, 1, 1>();

  check_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    0, 2, 4, 6, 8, 1, 3, 5, 7, 9
  >();
  check_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  >();
  check_sort<
    int_seq<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>,
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0
  >();
}

///////////////////////
// type_list::unique //
///////////////////////

FATAL_TEST(unique, unique) {
  FATAL_EXPECT_SAME<
    type_list<int, double, bool, float>,
    type_list<int, double, double, int, bool, int, bool, float>::unique<>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 1, 4, 3, 2, 6>,
    int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2, 3, 0, 1, 2>::unique<>
  >();

  FATAL_EXPECT_SAME<
    int_seq<0, 2, 8, 6, 4, 12>,
    int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2, 3, 0, 1, 2>::unique<
      multiply_transform<int, 2>::type
    >
  >();
}

//////////////////////////
// type_list::is_unique //
//////////////////////////

FATAL_TEST(is_unique, is_unique) {
  FATAL_EXPECT_SAME<
    std::false_type,
    type_list<int, double, double, int, float>::is_unique<>
  >();

  FATAL_EXPECT_SAME<
    std::true_type,
    type_list<int, double, float, bool>::is_unique<>
  >();

  FATAL_EXPECT_SAME<
    std::false_type,
    int_seq<0, 1, 4, 3, 2, 6, 1, 2, 4, 3, 1, 2>::is_unique<>
  >();

  FATAL_EXPECT_SAME<
    std::true_type,
    int_seq<0, 1, 2, 3, 4, 5, 6>::is_unique<>
  >();

  FATAL_EXPECT_SAME<
    std::true_type,
    int_seq<0, 2, 8, 6, 4, 12>::is_unique<halve_val>
  >();

  FATAL_EXPECT_SAME<
    std::false_type,
    int_seq<0, 1, 2, 3, 4, 5, 6>::is_unique<halve_val>
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
  FATAL_EXPECT_EQ(expectedResult, result);
  auto const expectedValue = Result ? Needle : Empty;
  FATAL_EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  FATAL_EXPECT_EQ(expectedIndex, index);
}

FATAL_TEST(binary_search, exact) {
  typedef chr_seq<> empty;

  FATAL_VLOG(1) << "empty";
  check_bs_exact<char, false, '-', empty::size, empty, '\0'>();
  check_bs_exact<int,  false, 3,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  FATAL_VLOG(1) << "one";
  check_bs_exact<char, false, '-', one::size, one, '\0'>();
  check_bs_exact<char, true,  'x', 0, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  FATAL_VLOG(1) << "two";
  check_bs_exact<char, false, '-', two::size, two, '\0'>();
  check_bs_exact<char, true,  'x', 0, two, '\0'>();
  check_bs_exact<char, true,  'y', 1, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  FATAL_VLOG(1) << "aeiou";
  check_bs_exact<char, false, 'x', aeiou::size, aeiou, '\0'>();

  check_bs_exact<char, true, 'a', 0, aeiou, '\0'>();
  check_bs_exact<char, true, 'e', 1, aeiou, '\0'>();
  check_bs_exact<char, true, 'i', 2, aeiou, '\0'>();
  check_bs_exact<char, true, 'o', 3, aeiou, '\0'>();
  check_bs_exact<char, true, 'u', 4, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287, 2147483647> mp;

  FATAL_VLOG(1) << "mp";
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
  FATAL_EXPECT_EQ(expectedResult, result);
  auto const expectedValue = ExpectedValue;
  FATAL_EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  FATAL_EXPECT_EQ(expectedIndex, index);
}

FATAL_TEST(binary_search, lower_bound) {
  typedef chr_seq<> empty;

  FATAL_VLOG(1) << "empty";
  check_bs_lower_bound<char, false, '-', '\0', empty::size, empty, '\0'>();
  check_bs_lower_bound<int,  false, 3,   -1,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  FATAL_VLOG(1) << "one";
  check_bs_lower_bound<char, false, 'w', '\0', one::size, one, '\0'>();
  check_bs_lower_bound<char, true,  'x', 'x',  0, one, '\0'>();
  check_bs_lower_bound<char, true,  'y', 'x',  0, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  FATAL_VLOG(1) << "two";
  check_bs_lower_bound<char, false, 'w', '\0', two::size, two, '\0'>();
  check_bs_lower_bound<char, true,  'x', 'x',  0, two, '\0'>();
  check_bs_lower_bound<char, true,  'y', 'y',  1, two, '\0'>();
  check_bs_lower_bound<char, true,  'z', 'y',  1, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  FATAL_VLOG(1) << "aeiou";
  check_bs_lower_bound<char, false, 'a' - 1, '\0', aeiou::size, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'a',     'a',  0, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'e',     'e',  1, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'i',     'i',  2, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'o',     'o',  3, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'u',     'u',  4, aeiou, '\0'>();
  check_bs_lower_bound<char, true,  'x',     'u',  4, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287> mp;

  FATAL_VLOG(1) << "mp";
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
  FATAL_EXPECT_EQ(expectedResult, result);
  auto const expectedValue = ExpectedValue;
  FATAL_EXPECT_EQ(expectedValue, actual);
  auto const expectedIndex = ExpectedIndex;
  FATAL_EXPECT_EQ(expectedIndex, index);
}

FATAL_TEST(binary_search, upper_bound) {
  typedef chr_seq<> empty;

  FATAL_VLOG(1) << "empty";
  check_bs_upper_bound<char, false, '-', '\0', empty::size, empty, '\0'>();
  check_bs_upper_bound<int,  false, 3,   -1,   empty::size, empty, -1>();

  typedef chr_seq<'x'> one;

  FATAL_VLOG(1) << "one";
  check_bs_upper_bound<char, true,  'w', 'x',  0, one, '\0'>();
  check_bs_upper_bound<char, false, 'x', '\0', one::size, one, '\0'>();
  check_bs_upper_bound<char, false, 'y', '\0', one::size, one, '\0'>();

  typedef chr_seq<'x', 'y'> two;

  FATAL_VLOG(1) << "two";
  check_bs_upper_bound<char, true,  'w', 'x',  0, two, '\0'>();
  check_bs_upper_bound<char, true,  'x', 'y',  1, two, '\0'>();
  check_bs_upper_bound<char, false, 'y', '\0', two::size, two, '\0'>();
  check_bs_upper_bound<char, false, 'z', '\0', two::size, two, '\0'>();

  typedef chr_seq<'a', 'e', 'i', 'o', 'u'> aeiou;

  FATAL_VLOG(1) << "aeiou";
  check_bs_upper_bound<char, true, 'a' - 1, 'a',  0, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'a',     'e',  1, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'e',     'i',  2, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'i',     'o',  3, aeiou, '\0'>();
  check_bs_upper_bound<char, true, 'o',     'u',  4, aeiou, '\0'>();
  check_bs_upper_bound<char, false, 'u',    '\0', aeiou::size, aeiou, '\0'>();

  typedef int_seq<3, 7, 31, 127, 8191, 131071, 524287> mp;

  FATAL_VLOG(1) << "mp";
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
    FATAL_EXPECT_SAME<T, typename type_get<Index>::template from<TList>>();

    FATAL_EXPECT_SAME<
      typename TList::template at<Index>,
      typename type_get<Index>::template from<TList>
    >();
  }
};

template <typename... Args>
void check_type_get() {
  typedef type_list<Args...> list;
  list::foreach(check_type_get_visitor<list>());
}

FATAL_TEST(type_get, type_list) {
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

FATAL_TEST(type_list_from, type) {
  FATAL_EXPECT_SAME<type_list<>, type_list_from<>::type<void>>();

  FATAL_EXPECT_SAME<vl, type_list_from<identity_transform>::type<void>>();

  FATAL_EXPECT_SAME<
    type_list<double, std::string>,
    type_list_from<
      type_get_first,
      type_get_second
    >::type<std::pair<double, std::string>>
  >();

  FATAL_EXPECT_SAME<
    type_list<
      std::pair<double, std::string>,
      double,
      std::string
    >,
    type_list_from<
      identity_transform,
      type_get_first,
      type_get_second
    >::type<std::pair<double, std::string>>
  >();
}

} // namespace fatal {
