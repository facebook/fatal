/*
 *  Copyright (c) 2016, Facebook, p.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found p the
 *  LICENSE file p the root directory of this source tree. An additional grant
 *  of patent rights can be found p the PATENTS file p the same directory.
 */

#include <fatal/type/transform.h>

#include <fatal/type/identity.h>
#include <fatal/type/list.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

using id = get_identity;

template <int Value>
using v = std::integral_constant<int, Value>;

struct p {
  template <typename T>
  using apply = std::integral_constant<typename T::value_type, T::value + 1>;
};

struct m {
  template <typename T>
  using apply = std::integral_constant<typename T::value_type, T::value - 1>;
};

struct n {
  template <typename T>
  using apply = std::integral_constant<typename T::value_type, -T::value>;
};

struct d {
  template <typename T>
  using apply = std::integral_constant<typename T::value_type, T::value * 2>;
};

struct s {
  template <typename T>
  using apply = std::integral_constant<
    typename T::value_type, T::value * T::value
  >;
};

struct h {
  template <typename T>
  using apply = std::integral_constant<typename T::value_type, T::value / 2>;
};

FATAL_TEST(transform, 0 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>>>();

  FATAL_EXPECT_SAME<list<v<0>>, transform<list<v<0>>>>();

  FATAL_EXPECT_SAME<
    list<v<0>, v<1>, v<2>, v<3>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>>
  >();
}

FATAL_TEST(transform, 1 transform) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p>>();

  FATAL_EXPECT_SAME<list<v<0>>, transform<list<v<0>>, id>>();
  FATAL_EXPECT_SAME<list<v<1>>, transform<list<v<0>>, p>>();

  FATAL_EXPECT_SAME<
    list<v<0>, v<1>, v<2>, v<3>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id>
  >();

  FATAL_EXPECT_SAME<
    list<v<1>, v<2>, v<3>, v<4>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p>
  >();
}

FATAL_TEST(transform, 2 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, id>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, p>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p, d>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, d, p>>();

  FATAL_EXPECT_SAME<list<v<1>>, transform<list<v<1>>, id, id>>();
  FATAL_EXPECT_SAME<list<v<2>>, transform<list<v<1>>, id, p>>();
  FATAL_EXPECT_SAME<list<v<4>>, transform<list<v<1>>, p, d>>();
  FATAL_EXPECT_SAME<list<v<3>>, transform<list<v<1>>, d, p>>();

  FATAL_EXPECT_SAME<
    list<v<0>, v<1>, v<2>, v<3>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, id>
  >();

  FATAL_EXPECT_SAME<
    list<v<1>, v<2>, v<3>, v<4>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, p>
  >();

  FATAL_EXPECT_SAME<
    list<v<2>, v<4>, v<6>, v<8>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p, d>
  >();

  FATAL_EXPECT_SAME<
    list<v<1>, v<3>, v<5>, v<7>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, d, p>
  >();
}

FATAL_TEST(transform, 3 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, id, p>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, p, s>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p, d, s>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, d, p, s>>();

  FATAL_EXPECT_SAME<list<v<2>>, transform<list<v<1>>, id, id, p>>();
  FATAL_EXPECT_SAME<list<v<4>>, transform<list<v<1>>, id, p, s>>();
  FATAL_EXPECT_SAME<list<v<16>>, transform<list<v<1>>, p, d, s>>();
  FATAL_EXPECT_SAME<list<v<9>>, transform<list<v<1>>, d, p, s>>();

  FATAL_EXPECT_SAME<
    list<v<1>, v<2>, v<3>, v<4>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, id, p>
  >();

  FATAL_EXPECT_SAME<
    list<v<1>, v<4>, v<9>, v<16>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, p, s>
  >();

  FATAL_EXPECT_SAME<
    list<v<4>, v<16>, v<36>, v<64>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p, d, s>
  >();

  FATAL_EXPECT_SAME<
    list<v<1>, v<9>, v<25>, v<49>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, d, p, s>
  >();
}

FATAL_TEST(transform, 4 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, id, p, s>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, p, s, h>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p, d, s, h>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, d, p, s, h>>();

  FATAL_EXPECT_SAME<list<v<4>>, transform<list<v<1>>, id, id, p, s>>();
  FATAL_EXPECT_SAME<list<v<2>>, transform<list<v<1>>, id, p, s, h>>();
  FATAL_EXPECT_SAME<list<v<8>>, transform<list<v<1>>, p, d, s, h>>();
  FATAL_EXPECT_SAME<list<v<4>>, transform<list<v<1>>, d, p, s, h>>();

  FATAL_EXPECT_SAME<
    list<v<1>, v<4>, v<9>, v<16>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, id, p, s>
  >();

  FATAL_EXPECT_SAME<
    list<v<0>, v<2>, v<4>, v<8>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, p, s, h>
  >();

  FATAL_EXPECT_SAME<
    list<v<2>, v<8>, v<18>, v<32>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p, d, s, h>
  >();

  FATAL_EXPECT_SAME<
    list<v<0>, v<4>, v<12>, v<24>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, d, p, s, h>
  >();
}

FATAL_TEST(transform, 5 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, id, p, s, n>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, p, s, h, n>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p, d, s, h, n>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, d, p, s, h, n>>();

  FATAL_EXPECT_SAME<list<v<-4>>, transform<list<v<1>>, id, id, p, s, n>>();
  FATAL_EXPECT_SAME<list<v<-2>>, transform<list<v<1>>, id, p, s, h, n>>();
  FATAL_EXPECT_SAME<list<v<-8>>, transform<list<v<1>>, p, d, s, h, n>>();
  FATAL_EXPECT_SAME<list<v<-4>>, transform<list<v<1>>, d, p, s, h, n>>();

  FATAL_EXPECT_SAME<
    list<v<-1>, v<-4>, v<-9>, v<-16>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, id, p, s, n>
  >();

  FATAL_EXPECT_SAME<
    list<v<0>, v<-2>, v<-4>, v<-8>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, p, s, h, n>
  >();

  FATAL_EXPECT_SAME<
    list<v<-2>, v<-8>, v<-18>, v<-32>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p, d, s, h, n>
  >();

  FATAL_EXPECT_SAME<
    list<v<0>, v<-4>, v<-12>, v<-24>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, d, p, s, h, n>
  >();
}

FATAL_TEST(transform, 6 transforms) {
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, id, p, s, n, m, m>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, id, p, s, h, n, m>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, p, d, s, h, n, m>>();
  FATAL_EXPECT_SAME<list<>, transform<list<>, d, p, s, h, n, m>>();

  FATAL_EXPECT_SAME<list<v<-5>>, transform<list<v<1>>, id, id, p, s, n, m>>();
  FATAL_EXPECT_SAME<list<v<-3>>, transform<list<v<1>>, id, p, s, h, n, m>>();
  FATAL_EXPECT_SAME<list<v<-9>>, transform<list<v<1>>, p, d, s, h, n, m>>();
  FATAL_EXPECT_SAME<list<v<-5>>, transform<list<v<1>>, d, p, s, h, n, m>>();

  FATAL_EXPECT_SAME<
    list<v<-2>, v<-5>, v<-10>, v<-17>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, id, p, s, n, m>
  >();

  FATAL_EXPECT_SAME<
    list<v<-1>, v<-3>, v<-5>, v<-9>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, id, p, s, h, n, m>
  >();

  FATAL_EXPECT_SAME<
    list<v<-3>, v<-9>, v<-19>, v<-33>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, p, d, s, h, n, m>
  >();

  FATAL_EXPECT_SAME<
    list<v<-1>, v<-5>, v<-13>, v<-25>>,
    transform<list<v<0>, v<1>, v<2>, v<3>>, d, p, s, h, n, m>
  >();
}

FATAL_TEST(transform_if, transform_if) {
}

} // namespace fatal {
