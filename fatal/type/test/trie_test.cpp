/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/array.h>
#include <fatal/type/convert.h>
#include <fatal/type/get_type.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>
#include <fatal/type/transform.h>
#include <fatal/type/trie.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

FATAL_S(a, "a");
FATAL_S(ab, "ab");
FATAL_S(abc, "abc");
FATAL_S(abcd, "abcd");
FATAL_S(abcde, "abcde");
FATAL_S(abcdef, "abcdef");
FATAL_S(abcx, "abcx");
FATAL_S(abcxy, "abcxy");
FATAL_S(abcxyz, "abcxyz");

using abc_tree = list<a, ab, abc, abcd, abcde, abcdef, abcx, abcxy, abcxyz>;

FATAL_S(h, "h");
FATAL_S(ha, "ha");
FATAL_S(hat, "hat");
FATAL_S(hi, "hi");
FATAL_S(hit, "hit");
FATAL_S(hint, "hint");
FATAL_S(ho, "ho");
FATAL_S(hot, "hot");

using hs_tree = list<h, ha, hat, hi, hint, hit, ho, hot>;

template <typename T>
struct wrapper {
  using value = T;
};

struct seq {
  using gooey = sequence<char, 'g', 'o', 'o', 'e', 'y'>;
  using fast = sequence<char, 'f', 'a', 's', 't'>;
  using granite = sequence<char, 'g', 'r', 'a', 'n', 'i', 't', 'e'>;
  using fastest = sequence<char, 'f', 'a', 's', 't', 'e', 's', 't'>;
  using fart = sequence<char, 'f', 'a', 'r', 't'>;
  using far = sequence<char, 'f', 'a', 'r'>;
  using good = sequence<char, 'g', 'o', 'o', 'd'>;
  using great = sequence<char, 'g', 'r', 'e', 'a', 't'>;
  using grok = sequence<char, 'g', 'r', 'o', 'k'>;
  using faster = sequence<char, 'f', 'a', 's', 't', 'e', 'r'>;
  using green = sequence<char, 'g', 'r', 'e', 'e', 'n'>;
  using gold = sequence<char, 'g', 'o', 'l', 'd'>;
  using farther = sequence<char, 'f', 'a', 'r', 't', 'h', 'e', 'r'>;
  using groove = sequence<char, 'g', 'r', 'o', 'o', 'v', 'e'>;
  using fat = sequence<char, 'f', 'a', 't'>;
  using fist = sequence<char, 'f', 'i', 's', 't'>;

  using empty = sequence<char>;
  using x = sequence<char, 'x'>;
  using notfound = sequence<char, 'n', 'o', 't', 'f', 'o', 'u', 'n', 'd'>;

  using shuffled = list<
    gooey, fast, granite, fastest, fart, far, good, great,
    grok, faster, green, gold, farther, groove, fat, fist
  >;
};

struct lst {
  using gooey = as_list<seq::gooey>;
  using fast = as_list<seq::fast>;
  using granite = as_list<seq::granite>;
  using fastest = as_list<seq::fastest>;
  using fart = as_list<seq::fart>;
  using far = as_list<seq::far>;
  using good = as_list<seq::good>;
  using great = as_list<seq::great>;
  using grok = as_list<seq::grok>;
  using faster = as_list<seq::faster>;
  using green = as_list<seq::green>;
  using gold = as_list<seq::gold>;
  using farther = as_list<seq::farther>;
  using groove = as_list<seq::groove>;
  using fat = as_list<seq::fat>;
  using fist = as_list<seq::fist>;

  using empty = as_list<seq::empty>;
  using x = as_list<seq::x>;
  using notfound = as_list<seq::notfound>;

  using shuffled = list<
    gooey, fast, granite, fastest, fart, far, good, great,
    grok, faster, green, gold, farther, groove, fat, fist
  >;
};

/////////////////////
// match_* helpers //
/////////////////////

template <typename...> struct check_trie_find_visitor;

template <typename Expected, typename Filter>
struct check_trie_find_visitor<Expected, Filter> {
  template <typename Match, typename NeedleChar>
  void operator ()(
    tag<Match>,
    std::basic_string<NeedleChar> const &needle,
    std::size_t &matches
  ) {
    using actual = typename Filter::template apply<Match>;
    FATAL_EXPECT_SAME<Expected, actual>();
    FATAL_EXPECT_EQ((to_instance<std::string, actual>()), needle);
    ++matches;
  }
};

template <typename Expected>
struct check_trie_find_visitor<Expected>:
  check_trie_find_visitor<Expected, get_identity>
{};

template <>
struct check_trie_find_visitor<void> {
  template <typename String, typename... VArgs>
  void operator ()(tag<String>, VArgs &&...) {
    FATAL_EXPECT_EQ(
      "no match expected",
      to_string("got '", to_instance<std::string, String>(), '\'')
    );
  }
};

template <typename Expected, typename Tree, typename... Filter>
void check_trie_find_impl(std::string const &needle) {
  check_trie_find_visitor<Expected, Filter...> visitor;

  std::size_t matches = 0;
  bool const result = trie_find<Tree,  less, Filter...>(
    needle.begin(), needle.end(), visitor, needle, matches
  );

  bool const expected = !std::is_same<void, Expected>::value;
  FATAL_EXPECT_EQ(expected, result);
  FATAL_EXPECT_EQ(expected, matches);
}
template <typename Tree, typename Expected = void>
void check_trie_find(std::string const &needle) {
  check_trie_find_impl<Expected, Tree>(needle);
  check_trie_find_impl<
    Expected,
    transform<Tree, wrapper>,
    get_type::value
  >(needle);
}

/////////////////
// match_exact //
/////////////////

FATAL_TEST(trie, find h_empty) {
  check_trie_find<hs_tree>("");
}

FATAL_TEST(trie, find h_h) {
  check_trie_find<hs_tree, h>("h");
}

FATAL_TEST(trie, find h_H) {
  check_trie_find<hs_tree>("H");
}

FATAL_TEST(trie, find h_ha) {
  check_trie_find<hs_tree, ha>("ha");
}

FATAL_TEST(trie, find h_Ha) {
  check_trie_find<hs_tree>("Ha");
}

FATAL_TEST(trie, find h_hA) {
  check_trie_find<hs_tree>("hA");
}

FATAL_TEST(trie, find h_hat) {
  check_trie_find<hs_tree, hat>("hat");
}

FATAL_TEST(trie, find h_Hat) {
  check_trie_find<hs_tree>("Hat");
}

FATAL_TEST(trie, find h_hAt) {
  check_trie_find<hs_tree>("hAt");
}

FATAL_TEST(trie, find h_haT) {
  check_trie_find<hs_tree>("haT");
}

FATAL_TEST(trie, find h_hi) {
  check_trie_find<hs_tree, hi>("hi");
}

FATAL_TEST(trie, find h_Hi) {
  check_trie_find<hs_tree>("Hi");
}

FATAL_TEST(trie, find h_hI) {
  check_trie_find<hs_tree>("hI");
}

FATAL_TEST(trie, find h_hint) {
  check_trie_find<hs_tree, hint>("hint");
}

FATAL_TEST(trie, find h_Hint) {
  check_trie_find<hs_tree>("Hint");
}

FATAL_TEST(trie, find h_hInt) {
  check_trie_find<hs_tree>("hInt");
}

FATAL_TEST(trie, find h_hiNt) {
  check_trie_find<hs_tree>("hiNt");
}

FATAL_TEST(trie, find h_hinT) {
  check_trie_find<hs_tree>("hinT");
}

FATAL_TEST(trie, find h_hit) {
  check_trie_find<hs_tree, hit>("hit");
}

FATAL_TEST(trie, find h_Hit) {
  check_trie_find<hs_tree>("Hit");
}

FATAL_TEST(trie, find h_hIt) {
  check_trie_find<hs_tree>("hIt");
}

FATAL_TEST(trie, find h_hiT) {
  check_trie_find<hs_tree>("hiT");
}

FATAL_TEST(trie, find h_ho) {
  check_trie_find<hs_tree, ho>("ho");
}

FATAL_TEST(trie, find h_Ho) {
  check_trie_find<hs_tree>("Ho");
}

FATAL_TEST(trie, find h_hO) {
  check_trie_find<hs_tree>("hO");
}

FATAL_TEST(trie, find h_hot) {
  check_trie_find<hs_tree, hot>("hot");
}

FATAL_TEST(trie, find h_Hot) {
  check_trie_find<hs_tree>("Hot");
}

FATAL_TEST(trie, find h_hOt) {
  check_trie_find<hs_tree>("hOt");
}

FATAL_TEST(trie, find h_hoT) {
  check_trie_find<hs_tree>("hoT");
}

FATAL_TEST(trie, find h_hut) {
  check_trie_find<hs_tree>("hut");
}

#define TEST_TRIE_FIND(Expected, Needle, ...) \
  do { \
    using needle = as_array<Needle, char>; \
    bool const actual = trie_find<__VA_ARGS__>( \
      static_cast<char const *>(needle::data), \
      std::next(needle::data, needle::size::value), \
      test_trie_find_visitor<Needle>() \
    ); \
    FATAL_EXPECT_EQ(Expected, actual); \
  } while (false)

template <typename Expected>
struct test_trie_find_visitor {
  template <typename Needle>
  void operator ()(tag<Needle>) const {
    FATAL_EXPECT_SAME<Expected, Needle>();
  }
};

FATAL_TEST(trie, find) {
  TEST_TRIE_FIND(false, seq::fat, list<>);

  TEST_TRIE_FIND(true, seq::empty, list<seq::empty>);
  TEST_TRIE_FIND(false, seq::fat, list<seq::empty>);

  TEST_TRIE_FIND(true, seq::fat, list<seq::fat>);
  TEST_TRIE_FIND(false, seq::gooey, list<seq::fat>);

  TEST_TRIE_FIND(false, seq::fat, list<seq::fastest>);
  TEST_TRIE_FIND(false, seq::fast, list<seq::fastest>);
  TEST_TRIE_FIND(false, seq::faster, list<seq::fastest>);
  TEST_TRIE_FIND(true, seq::fastest, list<seq::fastest>);

  TEST_TRIE_FIND(true, seq::fat, list<seq::fastest, seq::fat>);
  TEST_TRIE_FIND(false, seq::fast, list<seq::fastest, seq::fat>);
  TEST_TRIE_FIND(false, seq::gooey, list<seq::fastest, seq::fat>);
  TEST_TRIE_FIND(false, seq::faster, list<seq::fastest, seq::fat>);
  TEST_TRIE_FIND(true, seq::fastest, list<seq::fastest, seq::fat>);

  TEST_TRIE_FIND(true, seq::gooey, seq::shuffled);
  TEST_TRIE_FIND(true, seq::fast, seq::shuffled);
  TEST_TRIE_FIND(true, seq::granite, seq::shuffled);
  TEST_TRIE_FIND(true, seq::fastest, seq::shuffled);
  TEST_TRIE_FIND(true, seq::fart, seq::shuffled);
  TEST_TRIE_FIND(true, seq::far, seq::shuffled);
  TEST_TRIE_FIND(true, seq::good, seq::shuffled);
  TEST_TRIE_FIND(true, seq::great, seq::shuffled);
  TEST_TRIE_FIND(true, seq::grok, seq::shuffled);
  TEST_TRIE_FIND(true, seq::faster, seq::shuffled);
  TEST_TRIE_FIND(true, seq::green, seq::shuffled);
  TEST_TRIE_FIND(true, seq::gold, seq::shuffled);
  TEST_TRIE_FIND(true, seq::farther, seq::shuffled);
  TEST_TRIE_FIND(true, seq::groove, seq::shuffled);
  TEST_TRIE_FIND(true, seq::fat, seq::shuffled);
  TEST_TRIE_FIND(true, seq::fist, seq::shuffled);
  TEST_TRIE_FIND(false, seq::empty, seq::shuffled);
  TEST_TRIE_FIND(false, seq::x, seq::shuffled);
  TEST_TRIE_FIND(false, seq::notfound, seq::shuffled);

  TEST_TRIE_FIND(false, lst::fat, list<>);

  TEST_TRIE_FIND(true, lst::empty, list<lst::empty>);
  TEST_TRIE_FIND(false, lst::fat, list<lst::empty>);

  TEST_TRIE_FIND(true, lst::fat, list<lst::fat>);
  TEST_TRIE_FIND(false, lst::gooey, list<lst::fat>);

  TEST_TRIE_FIND(false, lst::fat, list<lst::fastest>);
  TEST_TRIE_FIND(false, lst::fast, list<lst::fastest>);
  TEST_TRIE_FIND(false, lst::faster, list<lst::fastest>);
  TEST_TRIE_FIND(true, lst::fastest, list<lst::fastest>);

  TEST_TRIE_FIND(true, lst::fat, list<lst::fastest, lst::fat>);
  TEST_TRIE_FIND(false, lst::fast, list<lst::fastest, lst::fat>);
  TEST_TRIE_FIND(false, lst::gooey, list<lst::fastest, lst::fat>);
  TEST_TRIE_FIND(false, lst::faster, list<lst::fastest, lst::fat>);
  TEST_TRIE_FIND(true, lst::fastest, list<lst::fastest, lst::fat>);

  TEST_TRIE_FIND(true, lst::gooey, lst::shuffled);
  TEST_TRIE_FIND(true, lst::fast, lst::shuffled);
  TEST_TRIE_FIND(true, lst::granite, lst::shuffled);
  TEST_TRIE_FIND(true, lst::fastest, lst::shuffled);
  TEST_TRIE_FIND(true, lst::fart, lst::shuffled);
  TEST_TRIE_FIND(true, lst::far, lst::shuffled);
  TEST_TRIE_FIND(true, lst::good, lst::shuffled);
  TEST_TRIE_FIND(true, lst::great, lst::shuffled);
  TEST_TRIE_FIND(true, lst::grok, lst::shuffled);
  TEST_TRIE_FIND(true, lst::faster, lst::shuffled);
  TEST_TRIE_FIND(true, lst::green, lst::shuffled);
  TEST_TRIE_FIND(true, lst::gold, lst::shuffled);
  TEST_TRIE_FIND(true, lst::farther, lst::shuffled);
  TEST_TRIE_FIND(true, lst::groove, lst::shuffled);
  TEST_TRIE_FIND(true, lst::fat, lst::shuffled);
  TEST_TRIE_FIND(true, lst::fist, lst::shuffled);
  TEST_TRIE_FIND(false, lst::empty, lst::shuffled);
  TEST_TRIE_FIND(false, lst::x, lst::shuffled);
  TEST_TRIE_FIND(false, lst::notfound, lst::shuffled);
}

} // namespace fatal {
