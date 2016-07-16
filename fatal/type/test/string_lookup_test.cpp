/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/string_lookup.h>
#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

FATAL_STR(h, "h");
FATAL_STR(ha, "ha");
FATAL_STR(hat, "hat");
FATAL_STR(hi, "hi");
FATAL_STR(hit, "hit");
FATAL_STR(hint, "hint");
FATAL_STR(ho, "ho");
FATAL_STR(hot, "hot");

using hs_tree = string_lookup<
  h, ha, hat, hi, hint, hit, ho, hot
>;

/////////////////////
// match_* helpers //
/////////////////////

template <bool TExpectMatch>
struct check_match_exact_visitor {
  template <typename TString, typename TNeedleChar>
  void operator ()(
    type_tag<TString>,
    std::basic_string<TNeedleChar> const &needle,
    std::size_t &matches
  ) {
    FATAL_EXPECT_EQ(TString::string(), needle);
    FATAL_EXPECT_EQ(0, matches++);
  }
};

template <> struct check_match_exact_visitor<false> {
  template <typename TString, typename... VArgs>
  void operator ()(type_tag<TString>, VArgs &&...) {
    FATAL_EXPECT_EQ(
      "no match expected",
      to_string("got '", TString::string(), '\'')
    );
  }
};

template <typename TComparer = type_value_comparer>
struct check_match {
  template <bool TExpectMatch, typename TTree>
  static void exact(std::string const &needle) {
    check_match_exact_visitor<TExpectMatch> visitor;

    std::size_t matches = 0;
    auto result = TTree::template match<TComparer>::exact(
      needle.begin(), needle.end(), visitor, needle, matches
    );

    auto const expectedResult = TExpectMatch;
    FATAL_EXPECT_EQ(expectedResult, result);
    auto const expectedMatches = TExpectMatch ? 1 : 0;
    FATAL_EXPECT_EQ(expectedMatches, matches);
  }
};

/////////////////
// match_exact //
/////////////////

FATAL_TEST(string_lookup, match_exact_h_empty) {
  check_match<>::exact<false, hs_tree>("");
}

FATAL_TEST(string_lookup, match_exact_h_h) {
  check_match<>::exact<true, hs_tree>("h");
}

FATAL_TEST(string_lookup, match_exact_h_H) {
  check_match<>::exact<false, hs_tree>("H");
}

FATAL_TEST(string_lookup, match_exact_h_ha) {
  check_match<>::exact<true, hs_tree>("ha");
}

FATAL_TEST(string_lookup, match_exact_h_Ha) {
  check_match<>::exact<false, hs_tree>("Ha");
}

FATAL_TEST(string_lookup, match_exact_h_hA) {
  check_match<>::exact<false, hs_tree>("hA");
}

FATAL_TEST(string_lookup, match_exact_h_hat) {
  check_match<>::exact<true, hs_tree>("hat");
}

FATAL_TEST(string_lookup, match_exact_h_Hat) {
  check_match<>::exact<false, hs_tree>("Hat");
}

FATAL_TEST(string_lookup, match_exact_h_hAt) {
  check_match<>::exact<false, hs_tree>("hAt");
}

FATAL_TEST(string_lookup, match_exact_h_haT) {
  check_match<>::exact<false, hs_tree>("haT");
}

FATAL_TEST(string_lookup, match_exact_h_hi) {
  check_match<>::exact<true, hs_tree>("hi");
}

FATAL_TEST(string_lookup, match_exact_h_Hi) {
  check_match<>::exact<false, hs_tree>("Hi");
}

FATAL_TEST(string_lookup, match_exact_h_hI) {
  check_match<>::exact<false, hs_tree>("hI");
}

FATAL_TEST(string_lookup, match_exact_h_hint) {
  check_match<>::exact<true, hs_tree>("hint");
}

FATAL_TEST(string_lookup, match_exact_h_Hint) {
  check_match<>::exact<false, hs_tree>("Hint");
}

FATAL_TEST(string_lookup, match_exact_h_hInt) {
  check_match<>::exact<false, hs_tree>("hInt");
}

FATAL_TEST(string_lookup, match_exact_h_hiNt) {
  check_match<>::exact<false, hs_tree>("hiNt");
}

FATAL_TEST(string_lookup, match_exact_h_hinT) {
  check_match<>::exact<false, hs_tree>("hinT");
}

FATAL_TEST(string_lookup, match_exact_h_hit) {
  check_match<>::exact<true, hs_tree>("hit");
}

FATAL_TEST(string_lookup, match_exact_h_Hit) {
  check_match<>::exact<false, hs_tree>("Hit");
}

FATAL_TEST(string_lookup, match_exact_h_hIt) {
  check_match<>::exact<false, hs_tree>("hIt");
}

FATAL_TEST(string_lookup, match_exact_h_hiT) {
  check_match<>::exact<false, hs_tree>("hiT");
}

FATAL_TEST(string_lookup, match_exact_h_ho) {
  check_match<>::exact<true, hs_tree>("ho");
}

FATAL_TEST(string_lookup, match_exact_h_Ho) {
  check_match<>::exact<false, hs_tree>("Ho");
}

FATAL_TEST(string_lookup, match_exact_h_hO) {
  check_match<>::exact<false, hs_tree>("hO");
}

FATAL_TEST(string_lookup, match_exact_h_hot) {
  check_match<>::exact<true, hs_tree>("hot");
}

FATAL_TEST(string_lookup, match_exact_h_Hot) {
  check_match<>::exact<false, hs_tree>("Hot");
}

FATAL_TEST(string_lookup, match_exact_h_hOt) {
  check_match<>::exact<false, hs_tree>("hOt");
}

FATAL_TEST(string_lookup, match_exact_h_hoT) {
  check_match<>::exact<false, hs_tree>("hoT");
}

FATAL_TEST(string_lookup, match_exact_h_hut) {
  check_match<>::exact<false, hs_tree>("hut");
}

} // namespace fatal {
