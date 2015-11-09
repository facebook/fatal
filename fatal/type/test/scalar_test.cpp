/*
 *  Copyright (c) 2015, Facebook, Inc.
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

enum E: unsigned { Ea = 1, Eb = 2, Ec = 4 };
enum class EC: unsigned { a = 1, b = 2, c = 4 };
enum Ei: std::int16_t { Eia = 1, Eib = 2, Eic = 4 };
enum class ECi: std::int16_t { a = 1, b = 2, c = 4 };

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
  FATAL_EXPECT_EQ_SAME(E, static_cast<E>(7), bitwise_merge(static_cast<E>(7)));
  FATAL_EXPECT_EQ_SAME(E, static_cast<E>(7), bitwise_merge(Ea, Eb, Ec));
  FATAL_EXPECT_EQ_SAME(
    E, static_cast<E>(7), bitwise_merge(Ea, static_cast<E>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    E, static_cast<E>(7), bitwise_merge(Ea, static_cast<E>(7))
  );

  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_merge(Eia));
  FATAL_EXPECT_EQ_SAME(Ei, Eib, bitwise_merge(Eib));
  FATAL_EXPECT_EQ_SAME(Ei, Eic, bitwise_merge(Eic));
  FATAL_EXPECT_EQ_SAME(Ei, static_cast<Ei>(7), bitwise_merge(static_cast<Ei>(7)));
  FATAL_EXPECT_EQ_SAME(Ei, static_cast<Ei>(7), bitwise_merge(Eia, Eib, Eic));
  FATAL_EXPECT_EQ_SAME(
    Ei, static_cast<Ei>(7), bitwise_merge(Eia, static_cast<Ei>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    Ei, static_cast<Ei>(7), bitwise_merge(Eia, static_cast<Ei>(7))
  );
}

FATAL_TEST(bitwise_merge, enum class) {
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_merge(EC::a));
  FATAL_EXPECT_EQ_SAME(EC, EC::b, bitwise_merge(EC::b));
  FATAL_EXPECT_EQ_SAME(EC, EC::c, bitwise_merge(EC::c));
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(7), bitwise_merge(static_cast<EC>(7))
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(7), bitwise_merge(EC::a, EC::b, EC::c)
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(7), bitwise_merge(EC::a, static_cast<EC>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(7), bitwise_merge(EC::a, static_cast<EC>(7))
  );

  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_merge(ECi::a));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::b, bitwise_merge(ECi::b));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::c, bitwise_merge(ECi::c));
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(7), bitwise_merge(static_cast<ECi>(7))
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(7), bitwise_merge(ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(7), bitwise_merge(ECi::a, static_cast<ECi>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(7), bitwise_merge(ECi::a, static_cast<ECi>(7))
  );
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
  FATAL_EXPECT_EQ_SAME(E, static_cast<E>(7), bitwise_filter(static_cast<E>(7)));
  FATAL_EXPECT_EQ_SAME(E, static_cast<E>(0), bitwise_filter(Ea, Eb, Ec));
  FATAL_EXPECT_EQ_SAME(
    E, static_cast<E>(5), bitwise_filter(
      static_cast<E>(13), static_cast<E>(5), static_cast<E>(7)
    )
  );
  FATAL_EXPECT_EQ_SAME(
    E, static_cast<E>(0), bitwise_filter(Ea, static_cast<E>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    E, Ea, bitwise_filter(Ea, static_cast<E>(7))
  );

  FATAL_EXPECT_EQ_SAME(Ei, Eia, bitwise_filter(Eia));
  FATAL_EXPECT_EQ_SAME(Ei, Eib, bitwise_filter(Eib));
  FATAL_EXPECT_EQ_SAME(Ei, Eic, bitwise_filter(Eic));
  FATAL_EXPECT_EQ_SAME(
    Ei, static_cast<Ei>(7), bitwise_filter(static_cast<Ei>(7))
  );
  FATAL_EXPECT_EQ_SAME(Ei, static_cast<Ei>(0), bitwise_filter(Eia, Eib, Eic));
  FATAL_EXPECT_EQ_SAME(
    Ei, static_cast<Ei>(5), bitwise_filter(
      static_cast<Ei>(13), static_cast<Ei>(5), static_cast<Ei>(7)
    )
  );
  FATAL_EXPECT_EQ_SAME(
    Ei, static_cast<Ei>(0), bitwise_filter(Eia, static_cast<Ei>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    Ei, Eia, bitwise_filter(Eia, static_cast<Ei>(7))
  );
}

FATAL_TEST(bitwise_filter, enum class) {
  FATAL_EXPECT_EQ_SAME(EC, EC::a, bitwise_filter(EC::a));
  FATAL_EXPECT_EQ_SAME(EC, EC::b, bitwise_filter(EC::b));
  FATAL_EXPECT_EQ_SAME(EC, EC::c, bitwise_filter(EC::c));
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(7), bitwise_filter(static_cast<EC>(7))
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(0), bitwise_filter(EC::a, EC::b, EC::c)
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(5), bitwise_filter(
      static_cast<EC>(13), static_cast<EC>(5), static_cast<EC>(7)
    )
  );
  FATAL_EXPECT_EQ_SAME(
    EC, static_cast<EC>(0), bitwise_filter(EC::a, static_cast<EC>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    EC, EC::a, bitwise_filter(EC::a, static_cast<EC>(7))
  );

  FATAL_EXPECT_EQ_SAME(ECi, ECi::a, bitwise_filter(ECi::a));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::b, bitwise_filter(ECi::b));
  FATAL_EXPECT_EQ_SAME(ECi, ECi::c, bitwise_filter(ECi::c));
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(7), bitwise_filter(static_cast<ECi>(7))
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(0), bitwise_filter(ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(5), bitwise_filter(
      static_cast<ECi>(13), static_cast<ECi>(5), static_cast<ECi>(7)
    )
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, static_cast<ECi>(0), bitwise_filter(ECi::a, static_cast<ECi>(6))
  );
  FATAL_EXPECT_EQ_SAME(
    ECi, ECi::a, bitwise_filter(ECi::a, static_cast<ECi>(7))
  );
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
  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<E>(8), Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_all(static_cast<E>(7), Ea, Eb, Ec));
  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<E>(6), Ea, Eb, Ec));
  FATAL_EXPECT_FALSE(bitwise_has_all(Ea, Ea, Eb, Ec));

  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<Ei>(8), Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_all(static_cast<Ei>(7), Eia, Eib, Eic));
  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<Ei>(6), Eia, Eib, Eic));
  FATAL_EXPECT_FALSE(bitwise_has_all(Eia, Eia, Eib, Eic));
}

FATAL_TEST(bitwise_has_all, enum class) {
  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<EC>(8), EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_all(static_cast<EC>(7), EC::a, EC::b, EC::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(static_cast<EC>(6), EC::a, EC::b, EC::c));
  FATAL_EXPECT_FALSE(bitwise_has_all(EC::a, EC::a, EC::b, EC::c));

  FATAL_EXPECT_FALSE(
    bitwise_has_all(static_cast<ECi>(8), ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_TRUE(
    bitwise_has_all(static_cast<ECi>(7), ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_FALSE(
    bitwise_has_all(static_cast<ECi>(6), ECi::a, ECi::b, ECi::c)
  );
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
  FATAL_EXPECT_FALSE(bitwise_has_any(static_cast<E>(8), Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<E>(7), Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<E>(6), Ea, Eb, Ec));
  FATAL_EXPECT_TRUE(bitwise_has_any(Ea, Ea, Eb, Ec));

  FATAL_EXPECT_FALSE(bitwise_has_any(static_cast<Ei>(8), Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<Ei>(7), Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<Ei>(6), Eia, Eib, Eic));
  FATAL_EXPECT_TRUE(bitwise_has_any(Eia, Eia, Eib, Eic));
}

FATAL_TEST(bitwise_has_any, enum class) {
  FATAL_EXPECT_FALSE(bitwise_has_any(static_cast<EC>(8), EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<EC>(7), EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(static_cast<EC>(6), EC::a, EC::b, EC::c));
  FATAL_EXPECT_TRUE(bitwise_has_any(EC::a, EC::a, EC::b, EC::c));

  FATAL_EXPECT_FALSE(
    bitwise_has_any(static_cast<ECi>(8), ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_TRUE(
    bitwise_has_any(static_cast<ECi>(7), ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_TRUE(
    bitwise_has_any(static_cast<ECi>(6), ECi::a, ECi::b, ECi::c)
  );
  FATAL_EXPECT_TRUE(bitwise_has_any(ECi::a, ECi::a, ECi::b, ECi::c));
}

} // namespace fatal {
