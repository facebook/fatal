/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/prefix_tree.h>
#include <fatal/type/string.h>

#include <fatal/test/driver.h>

#include <folly/Conv.h>

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

typedef type_prefix_tree_builder<>::build<
  a, ab, abc, abcd, abcde, abcdef, abcx, abcxy, abcxyz
> abc_tree;

FATAL_STR(h, "h");
FATAL_STR(ha, "ha");
FATAL_STR(hat, "hat");
FATAL_STR(hi, "hi");
FATAL_STR(hit, "hit");
FATAL_STR(hint, "hint");
FATAL_STR(ho, "ho");
FATAL_STR(hot, "hot");

typedef type_prefix_tree_builder<>::build<
  h, ha, hat, hi, hint, hit, ho, hot
> hs_tree;

template <char C> using chr = std::integral_constant<char, C>;

////////////////////////////
// build_type_prefix_tree //
////////////////////////////

TEST(type_prefix_tree, build_type_prefix_tree) {
  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag>,
    type_prefix_tree_builder<>::build<>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'a'>, type_prefix_tree<a>>
    >,
    type_prefix_tree_builder<>::build<a>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'b'>, type_prefix_tree<ab>>
      >>
    >,
    type_prefix_tree_builder<>::build<ab>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'a'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hat>>
        >>
      >>
    >,
    type_prefix_tree_builder<>::build<hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'i'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hit>>
        >>
      >>
    >,
    type_prefix_tree_builder<>::build<hit>
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
    type_prefix_tree_builder<>::build<hint>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree<non_terminal_tag,
      type_pair<chr<'h'>, type_prefix_tree<non_terminal_tag,
        type_pair<chr<'o'>, type_prefix_tree<non_terminal_tag,
          type_pair<chr<'t'>, type_prefix_tree<hot>>
        >>
      >>
    >,
    type_prefix_tree_builder<>::build<hot>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hit>,
    type_prefix_tree_builder<>::build<hit, hit>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hit>,
    type_prefix_tree_builder<>::build<hit, hit, hit>
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
    type_prefix_tree_builder<>::build<hat, hit>
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
    type_prefix_tree_builder<>::build<hit, hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hat, hit>,
    type_prefix_tree_builder<>::build<hit, hat>
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
    type_prefix_tree_builder<>::build<hint, hit>
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
    type_prefix_tree_builder<>::build<hit, hint>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hint, hit>,
    type_prefix_tree_builder<>::build<hit, hint>
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
    type_prefix_tree_builder<>::build<hat, hint, hit, hot>
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
    type_prefix_tree_builder<>::build<hot, hit, hint, hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hat, hint, hit, hot>,
    type_prefix_tree_builder<>::build<hot, hit, hint, hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hat, hint, hit, hot>,
    type_prefix_tree_builder<>::build<hot, hit, hint, hot, hat>
  >();

  FATAL_EXPECT_SAME<
    type_prefix_tree_builder<>::build<hat, hint, hit, hot>,
    type_prefix_tree_builder<>::build<hot, hit, hint, hat, hot, hat, hint, hat>
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
    EXPECT_EQ(TString::string(), needle);
    EXPECT_EQ(0, matches++);
  }
};

template <> struct check_match_exact_visitor<false> {
  template <typename TString, typename... VArgs>
  void operator ()(type_tag<TString>, VArgs &&...) {
    EXPECT_EQ(
      "no match expected",
      folly::to<std::string>("got '", TString::string(), '\'')
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
    EXPECT_TRUE(TExpectedList::template contains<TString>::value);
    EXPECT_LT(matches++, TExpectedList::size);

    EXPECT_LE(TString::size, needle.size());
    EXPECT_EQ(0, needle.compare(0, TString::size, TString::string()));
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
    EXPECT_EQ(expectedResult, result);
    auto const expectedMatches = TExpectMatch ? 1 : 0;
    EXPECT_EQ(expectedMatches, matches);
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
    EXPECT_EQ(expectedResult, result);
    auto const expectedMatches = expected::size;
    EXPECT_EQ(expectedMatches, matches);
  }
};

/////////////////
// match_exact //
/////////////////

TEST(type_prefix_tree, match_exact_h_empty) {
  check_match<>::exact<false, hs_tree>("");
}

TEST(type_prefix_tree, match_exact_h_h) {
  check_match<>::exact<true, hs_tree>("h");
}

TEST(type_prefix_tree, match_exact_h_H) {
  check_match<>::exact<false, hs_tree>("H");
}

TEST(type_prefix_tree, match_exact_h_ha) {
  check_match<>::exact<true, hs_tree>("ha");
}

TEST(type_prefix_tree, match_exact_h_Ha) {
  check_match<>::exact<false, hs_tree>("Ha");
}

TEST(type_prefix_tree, match_exact_h_hA) {
  check_match<>::exact<false, hs_tree>("hA");
}

TEST(type_prefix_tree, match_exact_h_hat) {
  check_match<>::exact<true, hs_tree>("hat");
}

TEST(type_prefix_tree, match_exact_h_Hat) {
  check_match<>::exact<false, hs_tree>("Hat");
}

TEST(type_prefix_tree, match_exact_h_hAt) {
  check_match<>::exact<false, hs_tree>("hAt");
}

TEST(type_prefix_tree, match_exact_h_haT) {
  check_match<>::exact<false, hs_tree>("haT");
}

TEST(type_prefix_tree, match_exact_h_hi) {
  check_match<>::exact<true, hs_tree>("hi");
}

TEST(type_prefix_tree, match_exact_h_Hi) {
  check_match<>::exact<false, hs_tree>("Hi");
}

TEST(type_prefix_tree, match_exact_h_hI) {
  check_match<>::exact<false, hs_tree>("hI");
}

TEST(type_prefix_tree, match_exact_h_hint) {
  check_match<>::exact<true, hs_tree>("hint");
}

TEST(type_prefix_tree, match_exact_h_Hint) {
  check_match<>::exact<false, hs_tree>("Hint");
}

TEST(type_prefix_tree, match_exact_h_hInt) {
  check_match<>::exact<false, hs_tree>("hInt");
}

TEST(type_prefix_tree, match_exact_h_hiNt) {
  check_match<>::exact<false, hs_tree>("hiNt");
}

TEST(type_prefix_tree, match_exact_h_hinT) {
  check_match<>::exact<false, hs_tree>("hinT");
}

TEST(type_prefix_tree, match_exact_h_hit) {
  check_match<>::exact<true, hs_tree>("hit");
}

TEST(type_prefix_tree, match_exact_h_Hit) {
  check_match<>::exact<false, hs_tree>("Hit");
}

TEST(type_prefix_tree, match_exact_h_hIt) {
  check_match<>::exact<false, hs_tree>("hIt");
}

TEST(type_prefix_tree, match_exact_h_hiT) {
  check_match<>::exact<false, hs_tree>("hiT");
}

TEST(type_prefix_tree, match_exact_h_ho) {
  check_match<>::exact<true, hs_tree>("ho");
}

TEST(type_prefix_tree, match_exact_h_Ho) {
  check_match<>::exact<false, hs_tree>("Ho");
}

TEST(type_prefix_tree, match_exact_h_hO) {
  check_match<>::exact<false, hs_tree>("hO");
}

TEST(type_prefix_tree, match_exact_h_hot) {
  check_match<>::exact<true, hs_tree>("hot");
}

TEST(type_prefix_tree, match_exact_h_Hot) {
  check_match<>::exact<false, hs_tree>("Hot");
}

TEST(type_prefix_tree, match_exact_h_hOt) {
  check_match<>::exact<false, hs_tree>("hOt");
}

TEST(type_prefix_tree, match_exact_h_hoT) {
  check_match<>::exact<false, hs_tree>("hoT");
}

TEST(type_prefix_tree, match_exact_h_hut) {
  check_match<>::exact<false, hs_tree>("hut");
}

////////////////////
// match_prefixes //
////////////////////

TEST(type_prefix_tree, match_prefixes_h_empty) {
  check_match<>::prefixes<hs_tree>("");
}

TEST(type_prefix_tree, match_prefixes_h_h) {
  check_match<>::prefixes<hs_tree, h>("h");
}

TEST(type_prefix_tree, match_prefixes_h_H) {
  check_match<>::prefixes<hs_tree>("H");
}

TEST(type_prefix_tree, match_prefixes_h_hi) {
  check_match<>::prefixes<hs_tree, h, hi>("hi");
}

TEST(type_prefix_tree, match_prefixes_h_hI) {
  check_match<>::prefixes<hs_tree, h>("hI");
}

TEST(type_prefix_tree, match_prefixes_h_Hi) {
  check_match<>::prefixes<hs_tree>("Hi");
}

TEST(type_prefix_tree, match_prefixes_h_hit) {
  check_match<>::prefixes<hs_tree, h, hi, hit>("hit");
}

TEST(type_prefix_tree, match_prefixes_h_hint) {
  check_match<>::prefixes<hs_tree, h, hi, hint>("hint");
}

TEST(type_prefix_tree, match_prefixes_h_hinter) {
  check_match<>::prefixes<hs_tree, h, hi, hint>("hinter");
}

TEST(type_prefix_tree, match_prefixes_h_hinTer) {
  check_match<>::prefixes<hs_tree, h, hi>("hinTer");
}

TEST(type_prefix_tree, match_prefixes_abc_empty) {
  check_match<>::prefixes<abc_tree>("");
}

TEST(type_prefix_tree, match_prefixes_abc_abc) {
  check_match<>::prefixes<abc_tree, a, ab, abc>("abc");
}

TEST(type_prefix_tree, match_prefixes_abc_abcmno) {
  check_match<>::prefixes<abc_tree, a, ab, abc>("abcmno");
}

TEST(type_prefix_tree, match_prefixes_abc_abcdef) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcd, abcde, abcdef>("abcdef");
}

TEST(type_prefix_tree, match_prefixes_abc_abcdEF) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcd>("abcdEF");
}

TEST(type_prefix_tree, match_prefixes_abc_abcxyz) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcx, abcxy, abcxyz>("abcxyz");
}

TEST(type_prefix_tree, match_prefixes_abc_abcxYZ) {
  check_match<>::prefixes<abc_tree, a, ab, abc, abcx>("abcxYZ");
}

} // namespace fatal {
