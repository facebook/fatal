/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/scalar.h>

#include <fatal/test/driver.h>

#include <ratio>
#include <type_traits>

#include <cstdint>

namespace fatal {

using i10 = std::integral_constant<int, 10>;
using d5_6 = std::ratio<56, 10>;

enum E: unsigned {
  Ez = 0, Ea = 1, Eb = 2, Eab = 3,
  Ec = 4, Eac = 5, Ebc = 6, Eabc = 7,
  Ed = 8, Ead = 9, Ebd = 10, Eabd = 11,
  Ecd = 12, Eacd = 13, Ebcd = 14, Eabcd = 15,
  Ee = 16
};

enum class EC: unsigned {
  z = 0, a = 1, b = 2, ab = 3,
  c = 4, ac = 5, bc = 6, abc = 7,
  d = 8, ad = 9, bd = 10, abd = 11,
  cd = 12, acd = 13, bcd = 14, abcd = 15,
  e = 16
};

enum Ei: std::int16_t {
  Eiz = 0, Eia = 1, Eib = 2, Eiab = 3,
  Eic = 4, Eiac = 5, Eibc = 6, Eiabc = 7,
  Eid = 8, Eiad = 9, Eibd = 10, Eiabd = 11,
  Eicd = 12, Eiacd = 13, Eibcd = 14, Eiabcd = 15,
  Eie = 16
};

enum class ECi: std::int16_t {
  z = 0, a = 1, b = 2, ab = 3,
  c = 4, ac = 5, bc = 6, abc = 7,
  d = 8, ad = 9, bd = 10, abd = 11,
  cd = 12, acd = 13, bcd = 14, abcd = 15,
  e = 16
};

FATAL_TEST(to_scalar, integral) {
  FATAL_EXPECT_EQ_SAME(int, 10, to_scalar<i10>());
  FATAL_EXPECT_EQ_SAME(int, 10, to_scalar<i10,int>());
  FATAL_EXPECT_EQ_SAME(long, 10l, to_scalar<i10, long>());
  FATAL_EXPECT_EQ_SAME(unsigned, 10u, to_scalar<i10, unsigned>());
  FATAL_EXPECT_EQ_SAME(char, 10, to_scalar<i10, char>());
}

FATAL_TEST(to_scalar, ratio) {
  FATAL_EXPECT_EQ_SAME(double, 5.6, to_scalar<d5_6>());
  FATAL_EXPECT_EQ_SAME(double, 5.6, to_scalar<d5_6, double>());
  FATAL_EXPECT_EQ_SAME(float, 5.6f, to_scalar<d5_6, float>());
  FATAL_EXPECT_EQ_SAME(long double, 5.6L, to_scalar<d5_6, long double>());
  FATAL_EXPECT_EQ_SAME(int, 5, to_scalar<d5_6, int>());
}

FATAL_TEST(to_integral, enum) {
  FATAL_EXPECT_EQ_SAME(unsigned, 1, to_integral(Ea));
  FATAL_EXPECT_EQ_SAME(unsigned, 2, to_integral(Eb));
  FATAL_EXPECT_EQ_SAME(unsigned, 4, to_integral(Ec));

  FATAL_EXPECT_EQ_SAME(std::int16_t, 1, to_integral(Eia));
  FATAL_EXPECT_EQ_SAME(std::int16_t, 2, to_integral(Eib));
  FATAL_EXPECT_EQ_SAME(std::int16_t, 4, to_integral(Eic));
}

FATAL_TEST(to_integral, enum class) {
  FATAL_EXPECT_EQ_SAME(unsigned, 1, to_integral(EC::a));
  FATAL_EXPECT_EQ_SAME(unsigned, 2, to_integral(EC::b));
  FATAL_EXPECT_EQ_SAME(unsigned, 4, to_integral(EC::c));

  FATAL_EXPECT_EQ_SAME(std::int16_t, 1, to_integral(ECi::a));
  FATAL_EXPECT_EQ_SAME(std::int16_t, 2, to_integral(ECi::b));
  FATAL_EXPECT_EQ_SAME(std::int16_t, 4, to_integral(ECi::c));
}

FATAL_TEST(bitwise_merge, integer) {
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_merge(1));
  FATAL_EXPECT_EQ_SAME(int, 2, bitwise_merge(2));
  FATAL_EXPECT_EQ_SAME(int, 4, bitwise_merge(4));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_merge(7));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_merge(1, 2, 4));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_merge(1, 6));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_merge(1, 7));

  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_merge(1u));
  FATAL_EXPECT_EQ_SAME(unsigned, 2u, bitwise_merge(2u));
  FATAL_EXPECT_EQ_SAME(unsigned, 4u, bitwise_merge(4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_merge(7u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_merge(1u, 2u, 4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_merge(1u, 6u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_merge(1u, 7u));
}

FATAL_TEST(bitwise_merge, enum) {
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_merge(Ea));
  FATAL_EXPECT_EQ_SAME(E, Eb, bitwise_merge(Eb));
  FATAL_EXPECT_EQ_SAME(E, Ec, bitwise_merge(Ec));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_merge(Eabc));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_merge(Ea, Eb, Ec));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_merge(Ea, Ebc));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_merge(Ea, Eabc));

  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_merge(Eia));
  FATAL_EXPECT_EQ_SAME(Ei, Eib, bitwise_merge(Eib));
  FATAL_EXPECT_EQ_SAME(Ei, Eic, bitwise_merge(Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_merge(Eiabc));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_merge(Eia, Eib, Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_merge(Eia, Eibc));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_merge(Eia, Eiabc));
}

FATAL_TEST(bitwise_merge, enum class) {
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_merge(EC::a));
  FATAL_EXPECT_EQ_SAME(EC, EC::b, bitwise_merge(EC::b));
  FATAL_EXPECT_EQ_SAME(EC, EC::c, bitwise_merge(EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_merge(EC::abc));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_merge(EC::a, EC::b, EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_merge(EC::a, EC::bc));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_merge(EC::a, EC::abc));

  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_merge(ECi::a));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::b, bitwise_merge(ECi::b));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::c, bitwise_merge(ECi::c));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_merge(ECi::abc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_merge(ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_merge(ECi::a, ECi::bc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_merge(ECi::a, ECi::abc));
}

FATAL_TEST(bitwise_filter, integer) {
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_filter(1));
  FATAL_EXPECT_EQ_SAME(int, 2, bitwise_filter(2));
  FATAL_EXPECT_EQ_SAME(int, 4, bitwise_filter(4));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_filter(7));
  FATAL_EXPECT_EQ_SAME(int, 0, bitwise_filter(1, 2, 4));
  FATAL_EXPECT_EQ_SAME(int, 5, bitwise_filter(13, 5, 7));
  FATAL_EXPECT_EQ_SAME(int, 0, bitwise_filter(1, 6));
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_filter(1, 7));

  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_filter(1u));
  FATAL_EXPECT_EQ_SAME(unsigned, 2u, bitwise_filter(2u));
  FATAL_EXPECT_EQ_SAME(unsigned, 4u, bitwise_filter(4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_filter(7u));
  FATAL_EXPECT_EQ_SAME(unsigned, 0u, bitwise_filter(1u, 2u, 4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 5u, bitwise_filter(13u, 5u, 7u));
  FATAL_EXPECT_EQ_SAME(unsigned, 0u, bitwise_filter(1u, 6u));
  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_filter(1u, 7u));
}

FATAL_TEST(bitwise_filter, enum) {
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_filter(Ea));
  FATAL_EXPECT_EQ_SAME(E, Eb, bitwise_filter(Eb));
  FATAL_EXPECT_EQ_SAME(E, Ec, bitwise_filter(Ec));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_filter(Eabc));
  FATAL_EXPECT_EQ_SAME(E, Ez, bitwise_filter(Ea, Eb, Ec));
  FATAL_EXPECT_EQ_SAME(E, Eac, bitwise_filter(Eacd, Eac, Eabc));
  FATAL_EXPECT_EQ_SAME(E, Ez, bitwise_filter(Ea, Ebc));
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_filter(Ea, Eabc));

  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_filter(Eia));
  FATAL_EXPECT_EQ_SAME(Ei, Eib, bitwise_filter(Eib));
  FATAL_EXPECT_EQ_SAME(Ei, Eic, bitwise_filter(Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_filter(Eiabc));
  FATAL_EXPECT_EQ_SAME(Ei, Eiz, bitwise_filter(Eia, Eib, Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eiac, bitwise_filter(Eiacd, Eiac, Eiabc));
  FATAL_EXPECT_EQ_SAME(Ei, Eiz, bitwise_filter(Eia, Eibc));
  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_filter(Eia, Eiabc));
}

FATAL_TEST(bitwise_filter, enum class) {
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_filter(EC::a));
  FATAL_EXPECT_EQ_SAME(EC, EC::b, bitwise_filter(EC::b));
  FATAL_EXPECT_EQ_SAME(EC, EC::c, bitwise_filter(EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_filter(EC::abc));
  FATAL_EXPECT_EQ_SAME(EC, EC::z, bitwise_filter(EC::a, EC::b, EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::ac, bitwise_filter(EC::acd, EC::ac, EC::abc));
  FATAL_EXPECT_EQ_SAME(EC, EC::z, bitwise_filter(EC::a, EC::bc));
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_filter(EC::a, EC::abc));

  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_filter(ECi::a));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::b, bitwise_filter(ECi::b));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::c, bitwise_filter(ECi::c));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_filter(ECi::abc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::z, bitwise_filter(ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_EQ_SAME(
    ECi, ECi::ac, bitwise_filter(ECi::acd, ECi::ac, ECi::abc)
  );
  FATAL_EXPECT_EQ_SAME(ECi, ECi::z, bitwise_filter(ECi::a, ECi::bc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_filter(ECi::a, ECi::abc));
}

FATAL_TEST(bitwise_disable, integer) {
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_disable(1));
  FATAL_EXPECT_EQ_SAME(int, 2, bitwise_disable(2));
  FATAL_EXPECT_EQ_SAME(int, 4, bitwise_disable(4));
  FATAL_EXPECT_EQ_SAME(int, 7, bitwise_disable(7));
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_disable(1, 2, 4));
  FATAL_EXPECT_EQ_SAME(int, 8, bitwise_disable(13, 5, 7));
  FATAL_EXPECT_EQ_SAME(int, 1, bitwise_disable(1, 6));
  FATAL_EXPECT_EQ_SAME(int, 0, bitwise_disable(1, 7));

  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_disable(1u));
  FATAL_EXPECT_EQ_SAME(unsigned, 2u, bitwise_disable(2u));
  FATAL_EXPECT_EQ_SAME(unsigned, 4u, bitwise_disable(4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 7u, bitwise_disable(7u));
  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_disable(1u, 2u, 4u));
  FATAL_EXPECT_EQ_SAME(unsigned, 8u, bitwise_disable(13u, 5u, 7u));
  FATAL_EXPECT_EQ_SAME(unsigned, 1u, bitwise_disable(1u, 6u));
  FATAL_EXPECT_EQ_SAME(unsigned, 0u, bitwise_disable(1u, 7u));
}

FATAL_TEST(bitwise_disable, enum) {
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_disable(Ea));
  FATAL_EXPECT_EQ_SAME(E, Eb, bitwise_disable(Eb));
  FATAL_EXPECT_EQ_SAME(E, Ec, bitwise_disable(Ec));
  FATAL_EXPECT_EQ_SAME(E, Eabc, bitwise_disable(Eabc));
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_disable(Ea, Eb, Ec));
  FATAL_EXPECT_EQ_SAME(E, Ed, bitwise_disable(Eacd, Eac, Eabc));
  FATAL_EXPECT_EQ_SAME(E, Ea, bitwise_disable(Ea, Ebc));
  FATAL_EXPECT_EQ_SAME(E, Ez, bitwise_disable(Ea, Eabc));

  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_disable(Eia));
  FATAL_EXPECT_EQ_SAME(Ei, Eib, bitwise_disable(Eib));
  FATAL_EXPECT_EQ_SAME(Ei, Eic, bitwise_disable(Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eiabc, bitwise_disable(Eiabc));
  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_disable(Eia, Eib, Eic));
  FATAL_EXPECT_EQ_SAME(Ei, Eid, bitwise_disable(Eiacd, Eiac, Eiabc));
  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_disable(Eia, Eibc));
  FATAL_EXPECT_EQ_SAME(Ei, Eiz, bitwise_disable(Eia, Eiabc));
}

FATAL_TEST(bitwise_disable, enum class) {
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_disable(EC::a));
  FATAL_EXPECT_EQ_SAME(EC, EC::b, bitwise_disable(EC::b));
  FATAL_EXPECT_EQ_SAME(EC, EC::c, bitwise_disable(EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::abc, bitwise_disable(EC::abc));
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_disable(EC::a, EC::b, EC::c));
  FATAL_EXPECT_EQ_SAME(EC, EC::d, bitwise_disable(EC::acd, EC::ac, EC::abc));
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_disable(EC::a, EC::bc));
  FATAL_EXPECT_EQ_SAME(EC, EC::z, bitwise_disable(EC::a, EC::abc));

  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_disable(ECi::a));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::b, bitwise_disable(ECi::b));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::c, bitwise_disable(ECi::c));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::abc, bitwise_disable(ECi::abc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_disable(ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_EQ_SAME(
    ECi, ECi::d, bitwise_disable(ECi::acd, ECi::ac, ECi::abc)
  );
  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_disable(ECi::a, ECi::bc));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::z, bitwise_disable(ECi::a, ECi::abc));
}

FATAL_TEST(bitwise_has_all, integer) {
  FATAL_EXPECT_FALSE(bitwise_has_all(8, 1, 2, 4));
  FATAL_EXPECT_TRUE(bitwise_has_all(7, 1, 2, 4));
  FATAL_EXPECT_FALSE(bitwise_has_all(6, 1, 2, 4));
  FATAL_EXPECT_FALSE(bitwise_has_all(1, 1, 2, 4));

  FATAL_EXPECT_FALSE(bitwise_has_all(8u, 1u, 2u, 4u));
  FATAL_EXPECT_TRUE(bitwise_has_all(7u, 1u, 2u, 4u));
  FATAL_EXPECT_FALSE(bitwise_has_all(6u, 1u, 2u, 4u));
  FATAL_EXPECT_FALSE(bitwise_has_all(1u, 1u, 2u, 4u));
}

FATAL_TEST(bitwise_has_all, enum) {
  FATAL_EXPECT_FALSE(bitwise_has_all(Ed, Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_all(Eabc, Ea, Eb, Ec));
  FATAL_EXPECT_FALSE(bitwise_has_all(Ebc, Ea, Eb, Ec));
  FATAL_EXPECT_FALSE(bitwise_has_all(Ea, Ea, Eb, Ec));

  FATAL_EXPECT_FALSE(bitwise_has_all(Eid, Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_all(Eiabc, Eia, Eib, Eic));
  FATAL_EXPECT_FALSE(bitwise_has_all(Eibc, Eia, Eib, Eic));
  FATAL_EXPECT_FALSE(bitwise_has_all(Eia, Eia, Eib, Eic));
}

FATAL_TEST(bitwise_has_all, enum class) {
  FATAL_EXPECT_FALSE(bitwise_has_all(EC::d, EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_all(EC::abc, EC::a, EC::b, EC::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(EC::bc, EC::a, EC::b, EC::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(EC::a, EC::a, EC::b, EC::c));

  FATAL_EXPECT_FALSE(bitwise_has_all(ECi::d, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_TRUE(bitwise_has_all(ECi::abc, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(ECi::bc, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(ECi::a, ECi::a, ECi::b, ECi::c));
}

FATAL_TEST(bitwise_has_any, integer) {
  FATAL_EXPECT_FALSE(bitwise_has_any(8, 1, 2, 4));
  FATAL_EXPECT_TRUE(bitwise_has_any(7, 1, 2, 4));
  FATAL_EXPECT_TRUE(bitwise_has_any(6, 1, 2, 4));
  FATAL_EXPECT_TRUE(bitwise_has_any(1, 1, 2, 4));

  FATAL_EXPECT_FALSE(bitwise_has_any(8u, 1u, 2u, 4u));
  FATAL_EXPECT_TRUE(bitwise_has_any(7u, 1u, 2u, 4u));
  FATAL_EXPECT_TRUE(bitwise_has_any(6u, 1u, 2u, 4u));
  FATAL_EXPECT_TRUE(bitwise_has_any(1u, 1u, 2u, 4u));
}

FATAL_TEST(bitwise_has_any, enum) {
  FATAL_EXPECT_FALSE(bitwise_has_any(Ed, Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(Eabc, Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(Ebc, Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(Ea, Ea, Eb, Ec));

  FATAL_EXPECT_FALSE(bitwise_has_any(Eid, Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(Eiabc, Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(Eibc, Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(Eia, Eia, Eib, Eic));
}

FATAL_TEST(bitwise_has_any, enum class) {
  FATAL_EXPECT_FALSE(bitwise_has_any(EC::d, EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(EC::abc, EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(EC::bc, EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(EC::a, EC::a, EC::b, EC::c));

  FATAL_EXPECT_FALSE(bitwise_has_any(ECi::d, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(ECi::abc, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(ECi::bc, ECi::a, ECi::b, ECi::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(ECi::a, ECi::a, ECi::b, ECi::c));
}

} // namespace fatal {
