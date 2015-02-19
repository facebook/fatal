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

#include <fatal/benchmark/driver.h>

#include <string>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <utility>

namespace fatal {

//////////////////////////////
// BENCHMARK IMPLEMENTATION //
//////////////////////////////

template <typename...> struct sequential_ifs_impl;

template <>
struct sequential_ifs_impl<> {
  template <typename TInput>
  static void match(TInput &&, unsigned &) {}
};

template <typename TString, typename... TStrings>
struct sequential_ifs_impl<TString, TStrings...> {
  template <typename TInput>
  static void match(TInput &&input, unsigned &count) {
    if (input == TString::z_data()) {
      count += input.size();
    } else {
      sequential_ifs_impl<TStrings...>::match(
        std::forward<TInput>(input),
        count
      );
    }
  }
};

struct visitor {
  template <typename TString>
  void operator ()(
    type_tag<TString>,
    std::string const &s,
    unsigned &count
  ) {
    count += s.size();
  }
};

template <typename... TStrings>
struct benchmark_impl {
  using list = type_list<TStrings...>;

  static std::array<std::string, sizeof...(TStrings)> const str;

  using prefix_tree = typename list::template apply<
    build_type_prefix_tree<>::template from
  >;

  static void prefix_tree_benchmark() {
    unsigned count = 0;

    BENCHMARK_SUSPEND {}

    for (auto const &s: str) {
      prefix_tree::template match<>::exact(
        s.begin(), s.end(), visitor{}, s, count
      );
    }

    folly::doNotOptimizeAway(count);
  }

  static void sequential_ifs_benchmark() {
    unsigned count = 0;

    BENCHMARK_SUSPEND {}

    for (auto const &s: str) {
      sequential_ifs_impl<TStrings...>::match(s, count);
    }

    folly::doNotOptimizeAway(count);
  }

  static void sorted_std_array_benchmark() {
    std::array<std::string, list::size> c = str;
    unsigned count = 0;

    BENCHMARK_SUSPEND {
      std::sort(c.begin(), c.end());
    }

    for (auto const &s: str) {
      count += std::binary_search(c.begin(), c.end(), s);
    }

    folly::doNotOptimizeAway(count);
  }

  static void sorted_std_vector_benchmark() {
    std::vector<std::string> c;
    unsigned count = 0;

    BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.push_back(s);
      }

      std::sort(c.begin(), c.end());
    }

    for (auto const &s: str) {
      count += std::binary_search(c.begin(), c.end(), s);
    }

    folly::doNotOptimizeAway(count);
  }

  static void std_set_benchmark() {
    std::set<std::string> c;
    unsigned count = 0;

    BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.insert(s);
      }
    }

    for (auto const &s: str) {
      auto const i = c.find(s);
      count += i->size();
    }

    folly::doNotOptimizeAway(count);
  }

  static void std_unordered_set_benchmark() {
    std::unordered_set<std::string> c;
    unsigned count = 0;

    BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.insert(s);
      }
    }

    for (auto const &s: str) {
      auto const i = c.find(s);
      count += i->size();
    }

    folly::doNotOptimizeAway(count);
  }
};

/////////////////////////////////
// STATIC VARIABLES DEFINITION //
/////////////////////////////////

template <typename... TStrings>
std::array<std::string, sizeof...(TStrings)> const benchmark_impl<
  TStrings...
>::str = {{ TStrings::string()... }};

//////////////////////////////
// BENCHMARKS INSTANTIATION //
//////////////////////////////

#define CREATE_BENCHMARK(name, ...) \
  typedef benchmark_impl<__VA_ARGS__> name##_impl; \
  auto name##_warmup = []() { \
    unsigned count = 0; \
    for (auto const &i: name##_impl::str) { \
      count += i.size(); \
    } \
    folly::doNotOptimizeAway(count); \
    return count; \
  }(); \
  BENCHMARK(name##_type_prefix_tree) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::prefix_tree_benchmark(); \
  } \
  BENCHMARK_RELATIVE(name##_sorted_std_array) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::sorted_std_array_benchmark(); \
  } \
  BENCHMARK_RELATIVE(name##_sorted_std_vector) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::sorted_std_vector_benchmark(); \
  } \
  BENCHMARK_RELATIVE(name##_std_unordered_set) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::std_unordered_set_benchmark(); \
  } \
  BENCHMARK_RELATIVE(name##_std_set) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::std_set_benchmark(); \
  } \
  BENCHMARK_RELATIVE(name##_sequential_ifs) { \
    folly::doNotOptimizeAway(name##_warmup); \
    name##_impl::sequential_ifs_benchmark(); \
  }

/////////////////////////
// STRINGS DECLARATION //
/////////////////////////

FATAL_STR(s5_00, "00000");
FATAL_STR(s5_01, "00001");
FATAL_STR(s5_02, "00002");
FATAL_STR(s5_03, "00003");
FATAL_STR(s5_04, "00004");
FATAL_STR(s5_05, "00005");
FATAL_STR(s5_06, "00006");
FATAL_STR(s5_07, "00007");
FATAL_STR(s5_08, "00008");
FATAL_STR(s5_09, "00009");

FATAL_STR(s5_10, "00010");
FATAL_STR(s5_11, "00011");
FATAL_STR(s5_12, "00012");
FATAL_STR(s5_13, "00013");
FATAL_STR(s5_14, "00014");
FATAL_STR(s5_15, "00015");
FATAL_STR(s5_16, "00016");
FATAL_STR(s5_17, "00017");
FATAL_STR(s5_18, "00018");
FATAL_STR(s5_19, "00019");

FATAL_STR(s5_20, "00020");
FATAL_STR(s5_21, "00021");
FATAL_STR(s5_22, "00022");
FATAL_STR(s5_23, "00023");
FATAL_STR(s5_24, "00024");
FATAL_STR(s5_25, "00025");
FATAL_STR(s5_26, "00026");
FATAL_STR(s5_27, "00027");
FATAL_STR(s5_28, "00028");
FATAL_STR(s5_29, "00029");

FATAL_STR(s10_00, "0000000000");
FATAL_STR(s10_01, "0000000001");
FATAL_STR(s10_02, "0000000002");
FATAL_STR(s10_03, "0000000003");
FATAL_STR(s10_04, "0000000004");
FATAL_STR(s10_05, "0000000005");
FATAL_STR(s10_06, "0000000006");
FATAL_STR(s10_07, "0000000007");
FATAL_STR(s10_08, "0000000008");
FATAL_STR(s10_09, "0000000009");

FATAL_STR(s10_10, "0000000010");
FATAL_STR(s10_11, "0000000011");
FATAL_STR(s10_12, "0000000012");
FATAL_STR(s10_13, "0000000013");
FATAL_STR(s10_14, "0000000014");
FATAL_STR(s10_15, "0000000015");
FATAL_STR(s10_16, "0000000016");
FATAL_STR(s10_17, "0000000017");
FATAL_STR(s10_18, "0000000018");
FATAL_STR(s10_19, "0000000019");

FATAL_STR(s10_20, "0000000020");
FATAL_STR(s10_21, "0000000021");
FATAL_STR(s10_22, "0000000022");
FATAL_STR(s10_23, "0000000023");
FATAL_STR(s10_24, "0000000024");
FATAL_STR(s10_25, "0000000025");
FATAL_STR(s10_26, "0000000026");
FATAL_STR(s10_27, "0000000027");
FATAL_STR(s10_28, "0000000028");
FATAL_STR(s10_29, "0000000029");

FATAL_STR(s20_00, "00000000000000000000");
FATAL_STR(s20_01, "00000000000000000001");
FATAL_STR(s20_02, "00000000000000000002");
FATAL_STR(s20_03, "00000000000000000003");
FATAL_STR(s20_04, "00000000000000000004");
FATAL_STR(s20_05, "00000000000000000005");
FATAL_STR(s20_06, "00000000000000000006");
FATAL_STR(s20_07, "00000000000000000007");
FATAL_STR(s20_08, "00000000000000000008");
FATAL_STR(s20_09, "00000000000000000009");

FATAL_STR(s20_10, "00000000000000000010");
FATAL_STR(s20_11, "00000000000000000011");
FATAL_STR(s20_12, "00000000000000000012");
FATAL_STR(s20_13, "00000000000000000013");
FATAL_STR(s20_14, "00000000000000000014");
FATAL_STR(s20_15, "00000000000000000015");
FATAL_STR(s20_16, "00000000000000000016");
FATAL_STR(s20_17, "00000000000000000017");
FATAL_STR(s20_18, "00000000000000000018");
FATAL_STR(s20_19, "00000000000000000019");

FATAL_STR(s20_20, "00000000000000000020");
FATAL_STR(s20_21, "00000000000000000021");
FATAL_STR(s20_22, "00000000000000000022");
FATAL_STR(s20_23, "00000000000000000023");
FATAL_STR(s20_24, "00000000000000000024");
FATAL_STR(s20_25, "00000000000000000025");
FATAL_STR(s20_26, "00000000000000000026");
FATAL_STR(s20_27, "00000000000000000027");
FATAL_STR(s20_28, "00000000000000000028");
FATAL_STR(s20_29, "00000000000000000029");

FATAL_STR(s30_00, "000000000000000000000000000000");
FATAL_STR(s30_01, "000000000000000000000000000001");
FATAL_STR(s30_02, "000000000000000000000000000002");
FATAL_STR(s30_03, "000000000000000000000000000003");
FATAL_STR(s30_04, "000000000000000000000000000004");
FATAL_STR(s30_05, "000000000000000000000000000005");
FATAL_STR(s30_06, "000000000000000000000000000006");
FATAL_STR(s30_07, "000000000000000000000000000007");
FATAL_STR(s30_08, "000000000000000000000000000008");
FATAL_STR(s30_09, "000000000000000000000000000009");

FATAL_STR(s30_10, "000000000000000000000000000010");
FATAL_STR(s30_11, "000000000000000000000000000011");
FATAL_STR(s30_12, "000000000000000000000000000012");
FATAL_STR(s30_13, "000000000000000000000000000013");
FATAL_STR(s30_14, "000000000000000000000000000014");
FATAL_STR(s30_15, "000000000000000000000000000015");
FATAL_STR(s30_16, "000000000000000000000000000016");
FATAL_STR(s30_17, "000000000000000000000000000017");
FATAL_STR(s30_18, "000000000000000000000000000018");
FATAL_STR(s30_19, "000000000000000000000000000019");

FATAL_STR(s30_20, "000000000000000000000000000020");
FATAL_STR(s30_21, "000000000000000000000000000021");
FATAL_STR(s30_22, "000000000000000000000000000022");
FATAL_STR(s30_23, "000000000000000000000000000023");
FATAL_STR(s30_24, "000000000000000000000000000024");
FATAL_STR(s30_25, "000000000000000000000000000025");
FATAL_STR(s30_26, "000000000000000000000000000026");
FATAL_STR(s30_27, "000000000000000000000000000027");
FATAL_STR(s30_28, "000000000000000000000000000028");
FATAL_STR(s30_29, "000000000000000000000000000029");

///////////////////////////
// BENCHMARKS DEFINITION //
///////////////////////////

///////////
// n = 1 //
///////////

CREATE_BENCHMARK(n1_len5, s5_00);
CREATE_BENCHMARK(n1_len10, s10_00);
CREATE_BENCHMARK(n1_len20, s20_00);
CREATE_BENCHMARK(n1_len30, s30_00);
BENCHMARK_DRAW_LINE();

///////////
// n = 2 //
///////////

CREATE_BENCHMARK(n2_len5, s5_00, s5_01);
CREATE_BENCHMARK(n2_len10, s10_00, s10_01);
CREATE_BENCHMARK(n2_len20, s20_00, s20_01);
CREATE_BENCHMARK(n2_len30, s30_00, s30_01);
BENCHMARK_DRAW_LINE();

///////////
// n = 3 //
///////////

CREATE_BENCHMARK(n3_len5, s5_00, s5_01, s5_02);
CREATE_BENCHMARK(n3_len10, s10_00, s10_01, s10_02);
CREATE_BENCHMARK(n3_len20, s20_00, s20_01, s20_02);
CREATE_BENCHMARK(n3_len30, s30_00, s30_01, s30_02);
BENCHMARK_DRAW_LINE();

///////////
// n = 4 //
///////////

CREATE_BENCHMARK(n4_len5, s5_00, s5_01, s5_02, s5_03);
CREATE_BENCHMARK(n4_len10, s10_00, s10_01, s10_02, s10_03);
CREATE_BENCHMARK(n4_len20, s20_00, s20_01, s20_02, s20_03);
CREATE_BENCHMARK(n4_len30, s30_00, s30_01, s30_02, s30_03);
BENCHMARK_DRAW_LINE();

///////////
// n = 5 //
///////////

CREATE_BENCHMARK(n5_len5, s5_00, s5_01, s5_02, s5_03, s5_04);
CREATE_BENCHMARK(n5_len10, s10_00, s10_01, s10_02, s10_03, s10_04);
CREATE_BENCHMARK(n5_len20, s20_00, s20_01, s20_02, s20_03, s20_04);
CREATE_BENCHMARK(n5_len30, s30_00, s30_01, s30_02, s30_03, s30_04);
BENCHMARK_DRAW_LINE();

////////////
// n = 10 //
////////////

CREATE_BENCHMARK(n10_len5,
  s5_00, s5_01, s5_02, s5_03, s5_04,
  s5_05, s5_06, s5_07, s5_08, s5_09
);

CREATE_BENCHMARK(n10_len10,
  s10_00, s10_01, s10_02, s10_03, s10_04,
  s10_05, s10_06, s10_07, s10_08, s10_09
);

CREATE_BENCHMARK(n10_len20,
  s20_00, s20_01, s20_02, s20_03, s20_04,
  s20_05, s20_06, s20_07, s20_08, s20_09
);

CREATE_BENCHMARK(n10_len30,
  s30_00, s30_01, s30_02, s30_03, s30_04,
  s30_05, s30_06, s30_07, s30_08, s30_09
);

BENCHMARK_DRAW_LINE();

////////////
// n = 20 //
////////////

CREATE_BENCHMARK(n20_len5,
  s5_00, s5_01, s5_02, s5_03, s5_04,
  s5_05, s5_06, s5_07, s5_08, s5_09,
  s5_10, s5_11, s5_12, s5_13, s5_14,
  s5_15, s5_16, s5_17, s5_18, s5_19
);

CREATE_BENCHMARK(n20_len10,
  s10_00, s10_01, s10_02, s10_03, s10_04,
  s10_05, s10_06, s10_07, s10_08, s10_09,
  s10_10, s10_11, s10_12, s10_13, s10_14,
  s10_15, s10_16, s10_17, s10_18, s10_19
);

CREATE_BENCHMARK(n20_len20,
  s20_00, s20_01, s20_02, s20_03, s20_04,
  s20_05, s20_06, s20_07, s20_08, s20_09,
  s20_10, s20_11, s20_12, s20_13, s20_14,
  s20_15, s20_16, s20_17, s20_18, s20_19
);

CREATE_BENCHMARK(n20_len30,
  s30_00, s30_01, s30_02, s30_03, s30_04,
  s30_05, s30_06, s30_07, s30_08, s30_09,
  s30_10, s30_11, s30_12, s30_13, s30_14,
  s30_15, s30_16, s30_17, s30_18, s30_19
);

BENCHMARK_DRAW_LINE();

////////////
// n = 30 //
////////////

CREATE_BENCHMARK(n30_len5,
  s5_00, s5_01, s5_02, s5_03, s5_04,
  s5_05, s5_06, s5_07, s5_08, s5_09,
  s5_10, s5_11, s5_12, s5_13, s5_14,
  s5_15, s5_16, s5_17, s5_18, s5_19,
  s5_20, s5_21, s5_22, s5_23, s5_24,
  s5_25, s5_26, s5_27, s5_28, s5_29
);

CREATE_BENCHMARK(n30_len10,
  s10_00, s10_01, s10_02, s10_03, s10_04,
  s10_05, s10_06, s10_07, s10_08, s10_09,
  s10_10, s10_11, s10_12, s10_13, s10_14,
  s10_15, s10_16, s10_17, s10_18, s10_19,
  s10_20, s10_21, s10_22, s10_23, s10_24,
  s10_25, s10_26, s10_27, s10_28, s10_29
);

CREATE_BENCHMARK(n30_len20,
  s20_00, s20_01, s20_02, s20_03, s20_04,
  s20_05, s20_06, s20_07, s20_08, s20_09,
  s20_10, s20_11, s20_12, s20_13, s20_14,
  s20_15, s20_16, s20_17, s20_18, s20_19,
  s20_20, s20_21, s20_22, s20_23, s20_24,
  s20_25, s20_26, s20_27, s20_28, s20_29
);

CREATE_BENCHMARK(n30_len30,
  s30_00, s30_01, s30_02, s30_03, s30_04,
  s30_05, s30_06, s30_07, s30_08, s30_09,
  s30_10, s30_11, s30_12, s30_13, s30_14,
  s30_15, s30_16, s30_17, s30_18, s30_19,
  s30_20, s30_21, s30_22, s30_23, s30_24,
  s30_25, s30_26, s30_27, s30_28, s30_29
);

} // namespace fatal {
