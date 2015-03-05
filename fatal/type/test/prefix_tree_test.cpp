/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/prefix_tree.h>
#include <fatal/type/string.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

FATAL_STR(a, "a");
FATAL_STR(ab, "ab");
FATAL_STR(abc, "abc");
FATAL_STR(abcd, "abcd");
FATAL_STR(abcde, "abcde");
FATAL_STR(abcdef, "abcdef");
FATAL_STR(abcx, "abcx");
FATAL_STR(abcxy, "abcxy");
FATAL_STR(abcxyz, "abcxyz");

using abc_tree = build_type_prefix_tree<>::from<
  a, ab, abc, abcd, abcde, abcdef, abcx, abcxy, abcxyz
>;

FATAL_STR(h, "h");
FATAL_STR(ha, "ha");
FATAL_STR(hat, "hat");
FATAL_STR(hi, "hi");
FATAL_STR(hit, "hit");
FATAL_STR(hint, "hint");
FATAL_STR(ho, "ho");
FATAL_STR(hot, "hot");

using hs_tree = build_type_prefix_tree<>::from<
  h, ha, hat, hi, hint, hit, ho, hot
>;

template <char C> using chr = std::integral_constant<char, C>;

////////////////////////////
// build_type_prefix_tree //
////////////////////////////

FATAL_TEST(type_prefix_tree, build_type_prefix_tree) {
  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag>,
    build_type_prefix_tree<>::from<>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'a'>, type_prefix_tree<a>>
    >,
    build_type_prefix_tree<>::from<a>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'b'>, type_prefix_tree<ab>>
      >>
    >,
    build_type_prefix_tree<>::from<ab>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hit>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
            type_pair<chr<'t'>, type_prefix_tree<hint>>
          >>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hint>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'o'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hot>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hot>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hit>,
    build_type_prefix_tree<>::from<hit, hit>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hit>,
    build_type_prefix_tree<>::from<hit, hit, hit>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hat, hit>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hit, hat>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hat, hit>,
    build_type_prefix_tree<>::from<hit, hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
            type_pair<chr<'t'>, type_prefix_tree<hint>>
          >>,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hint, hit>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
            type_pair<chr<'t'>, type_prefix_tree<hint>>
          >>,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hit, hint>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hint, hit>,
    build_type_prefix_tree<>::from<hit, hint>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
            type_pair<chr<'t'>, type_prefix_tree<hint>>
          >>,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>,
        type_pair<chr<'o'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hot>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hat, hint, hit, hot>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'n'>, type_prefix_tree<non_terminal_tag,
            type_pair<chr<'t'>, type_prefix_tree<hint>>
          >>,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>,
        type_pair<chr<'o'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hot>>
        >>
      >>
    >,
    build_type_prefix_tree<>::from<hot, hit, hint, hat>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hat, hint, hit, hot>,
    build_type_prefix_tree<>::from<hot, hit, hint, hat>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hat, hint, hit, hot>,
    build_type_prefix_tree<>::from<hot, hit, hint, hot, hat>
  >();

  FATAL_EXPECT_SAME<
    build_type_prefix_tree<>::from<hat, hint, hit, hot>,
    build_type_prefix_tree<>::from<hot, hit, hint, hat, hot, hat, hint, hat>
  >();
}

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

template <typename TExpectedList>
struct check_match_prefixes_visitor {
  template <typename TString, typename TNeedleChar>
  void operator ()(
    type_tag<TString>,
    std::basic_string<TNeedleChar> const &needle,
    std::size_t &matches
  ) {
    FATAL_EXPECT_TRUE(TExpectedList::template contains<TString>::value);
    FATAL_EXPECT_LT(matches++, TExpectedList::size);

    FATAL_EXPECT_LE(TString::size, needle.size());
    FATAL_EXPECT_EQ(0, needle.compare(0, TString::size, TString::string()));
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

  template <typename TTree, typename... TExpected>
  static void prefixes(std::string const &needle) {
    typedef type_list<TExpected...> expected;

    check_match_prefixes_visitor<expected> visitor;

    std::size_t matches = 0;
    auto result = TTree::template match<TComparer>::prefixes(
      needle.begin(), needle.end(), visitor, needle, matches
    );

    auto const expectedResult = expected::size;
    FATAL_EXPECT_EQ(expectedResult, result);
    auto const expectedMatches = expected::size;
    FATAL_EXPECT_EQ(expectedMatches, matches);
  }
};

/////////////////
// match_exact //
/////////////////

FATAL_TEST(type_prefix_tree, match_exact_h_empty) {
  check_match<>::exact<false, hs_tree>("");
}

FATAL_TEST(type_prefix_tree, match_exact_h_h) {
  check_match<>::exact<true, hs_tree>("h");
}

FATAL_TEST(type_prefix_tree, match_exact_h_H) {
  check_match<>::exact<false, hs_tree>("H");
}

FATAL_TEST(type_prefix_tree, match_exact_h_ha) {
  check_match<>::exact<true, hs_tree>("ha");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Ha) {
  check_match<>::exact<false, hs_tree>("Ha");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hA) {
  check_match<>::exact<false, hs_tree>("hA");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hat) {
  check_match<>::exact<true, hs_tree>("hat");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Hat) {
  check_match<>::exact<false, hs_tree>("Hat");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hAt) {
  check_match<>::exact<false, hs_tree>("hAt");
}

FATAL_TEST(type_prefix_tree, match_exact_h_haT) {
  check_match<>::exact<false, hs_tree>("haT");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hi) {
  check_match<>::exact<true, hs_tree>("hi");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Hi) {
  check_match<>::exact<false, hs_tree>("Hi");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hI) {
  check_match<>::exact<false, hs_tree>("hI");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hint) {
  check_match<>::exact<true, hs_tree>("hint");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Hint) {
  check_match<>::exact<false, hs_tree>("Hint");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hInt) {
  check_match<>::exact<false, hs_tree>("hInt");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hiNt) {
  check_match<>::exact<false, hs_tree>("hiNt");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hinT) {
  check_match<>::exact<false, hs_tree>("hinT");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hit) {
  check_match<>::exact<true, hs_tree>("hit");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Hit) {
  check_match<>::exact<false, hs_tree>("Hit");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hIt) {
  check_match<>::exact<false, hs_tree>("hIt");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hiT) {
  check_match<>::exact<false, hs_tree>("hiT");
}

FATAL_TEST(type_prefix_tree, match_exact_h_ho) {
  check_match<>::exact<true, hs_tree>("ho");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Ho) {
  check_match<>::exact<false, hs_tree>("Ho");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hO) {
  check_match<>::exact<false, hs_tree>("hO");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hot) {
  check_match<>::exact<true, hs_tree>("hot");
}

FATAL_TEST(type_prefix_tree, match_exact_h_Hot) {
  check_match<>::exact<false, hs_tree>("Hot");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hOt) {
  check_match<>::exact<false, hs_tree>("hOt");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hoT) {
  check_match<>::exact<false, hs_tree>("hoT");
}

FATAL_TEST(type_prefix_tree, match_exact_h_hut) {
  check_match<>::exact<false, hs_tree>("hut");
}

////////////////////
// match_prefixes //
////////////////////

FATAL_TEST(type_prefix_tree, match_prefixes_h_empty) {
  check_match<>::prefixes<hs_tree>("");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_h) {
  check_match<>::prefixes<hs_tree, h>("h");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_H) {
  check_match<>::prefixes<hs_tree>("H");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hi) {
  check_match<>::prefixes<hs_tree, h, hi>("hi");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hI) {
  check_match<>::prefixes<hs_tree, h>("hI");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_Hi) {
  check_match<>::prefixes<hs_tree>("Hi");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hit) {
  check_match<>::prefixes<hs_tree, h, hi, hit>("hit");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hint) {
  check_match<>::prefixes<hs_tree, h, hi, hint>("hint");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hinter) {
  check_match<>::prefixes<hs_tree, h, hi, hint>("hinter");
}

FATAL_TEST(type_prefix_tree, match_prefixes_h_hinTer) {
  check_match<>::prefixes<hs_tree, h, hi>("hinTer");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_empty) {
  check_match<>::prefixes<abc_tree>("");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abc) {
  check_match<>::prefixes<abc_tree, a, ab, abc>("abc");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abcmno) {
  check_match<>::prefixes<abc_tree, a, ab, abc>("abcmno");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abcdef) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcd, abcde, abcdef>("abcdef");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abcdEF) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcd>("abcdEF");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abcxyz) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcx, abcxy, abcxyz>("abcxyz");
}

FATAL_TEST(type_prefix_tree, match_prefixes_abc_abcxYZ) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcx>("abcxYZ");
}

} // namespace fatal {
