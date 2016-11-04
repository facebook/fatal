/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/trie.h>
#include <fatal/type/sequence.h>

#include <fatal/benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fatal {

//////////////////////////////
// BENCHMARK IMPLEMENTATION //
//////////////////////////////

template <typename...> struct sequential_ifs_impl;

template <>
struct sequential_ifs_impl<> {
  template <typename Input>
  static void match(Input &&, unsigned &) {}
};

template <typename String, typename... Strings>
struct sequential_ifs_impl<String, Strings...> {
  template <typename Input>
  static void match(Input &&input, unsigned &count) {
    if (input == z_data<String>()) {
      count += input.size();
    } else {
      sequential_ifs_impl<Strings...>::match(
        std::forward<Input>(input),
        count
      );
    }
  }
};

struct visitor {
  template <typename String>
  void operator ()(
    tag<String>,
    std::string const &s,
    unsigned &count
  ) {
    count += s.size();
  }
};

template <typename... Strings>
struct benchmark_impl {
  static std::array<std::string, sizeof...(Strings)> const str;

  template <typename Controller>
  static void trie_benchmark(Controller &benchmark) {
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {}

    for (auto const &s: str) {
      trie_find<list<Strings...>>(s.begin(), s.end(), visitor{}, s, count);
    }

    prevent_optimization(count);
  }

  template <typename Controller>
  static void sequential_ifs_benchmark(Controller &benchmark) {
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {}

    for (auto const &s: str) {
      sequential_ifs_impl<Strings...>::match(s, count);
    }

    prevent_optimization(count);
  }

  template <typename Controller>
  static void sorted_std_array_benchmark(Controller &benchmark) {
    std::array<std::string, sizeof...(Strings)> c = str;
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {
      std::sort(c.begin(), c.end());
    }

    for (auto const &s: str) {
      count += std::binary_search(c.begin(), c.end(), s);
    }

    prevent_optimization(count);
  }

  template <typename Controller>
  static void sorted_std_vector_benchmark(Controller &benchmark) {
    std::vector<std::string> c;
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.push_back(s);
      }

      std::sort(c.begin(), c.end());
    }

    for (auto const &s: str) {
      count += std::binary_search(c.begin(), c.end(), s);
    }

    prevent_optimization(count);
  }

  template <typename Controller>
  static void std_set_benchmark(Controller &benchmark) {
    std::set<std::string> c;
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.insert(s);
      }
    }

    for (auto const &s: str) {
      auto const i = c.find(s);
      count += i->size();
    }

    prevent_optimization(count);
  }

  template <typename Controller>
  static void std_unordered_set_benchmark(Controller &benchmark) {
    std::unordered_set<std::string> c;
    unsigned count = 0;

    FATAL_BENCHMARK_SUSPEND {
      for (auto const &s: str) {
        c.insert(s);
      }
    }

    for (auto const &s: str) {
      auto const i = c.find(s);
      count += i->size();
    }

    prevent_optimization(count);
  }
};

/////////////////////////////////
// STATIC VARIABLES DEFINITION //
/////////////////////////////////

template <typename> struct as_str;

template <template <typename V, V...> class Sequence, typename T, T... Values>
struct as_str<Sequence<T, Values...>> {
  static std::string get() { return {Values...}; }
};

template <typename... Strings>
std::array<std::string, sizeof...(Strings)> const benchmark_impl<
  Strings...
>::str = {{ as_str<Strings>::get()... }};

//////////////////////////////
// BENCHMARKS INSTANTIATION //
//////////////////////////////

#define CREATE_BENCHMARK(Name, ...) \
  typedef benchmark_impl<__VA_ARGS__> Name##_impl; \
  auto Name##_warmup = []() { \
    unsigned count = 0; \
    for (auto const &i: Name##_impl::str) { \
      count += i.size(); \
    } \
    prevent_optimization(count); \
    return count; \
  }(); \
  FATAL_BENCHMARK(Name, type_prefix_tree) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::trie_benchmark(benchmark); \
  } \
  FATAL_BENCHMARK(Name, sorted_std_array) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::sorted_std_array_benchmark(benchmark); \
  } \
  FATAL_BENCHMARK(Name, sorted_std_vector) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::sorted_std_vector_benchmark(benchmark); \
  } \
  FATAL_BENCHMARK(Name, std_unordered_set) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::std_unordered_set_benchmark(benchmark); \
  } \
  FATAL_BENCHMARK(Name, std_set) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::std_set_benchmark(benchmark); \
  } \
  FATAL_BENCHMARK(Name, sequential_ifs) { \
    prevent_optimization(Name##_warmup); \
    Name##_impl::sequential_ifs_benchmark(benchmark); \
  }

/////////////////////////
// STRINGS DECLARATION //
/////////////////////////

FATAL_S(s5_00, "00000");
FATAL_S(s5_01, "00001");
FATAL_S(s5_02, "00002");
FATAL_S(s5_03, "00003");
FATAL_S(s5_04, "00004");
FATAL_S(s5_05, "00005");
FATAL_S(s5_06, "00006");
FATAL_S(s5_07, "00007");
FATAL_S(s5_08, "00008");
FATAL_S(s5_09, "00009");

FATAL_S(s5_10, "00010");
FATAL_S(s5_11, "00011");
FATAL_S(s5_12, "00012");
FATAL_S(s5_13, "00013");
FATAL_S(s5_14, "00014");
FATAL_S(s5_15, "00015");
FATAL_S(s5_16, "00016");
FATAL_S(s5_17, "00017");
FATAL_S(s5_18, "00018");
FATAL_S(s5_19, "00019");

FATAL_S(s5_20, "00020");
FATAL_S(s5_21, "00021");
FATAL_S(s5_22, "00022");
FATAL_S(s5_23, "00023");
FATAL_S(s5_24, "00024");
FATAL_S(s5_25, "00025");
FATAL_S(s5_26, "00026");
FATAL_S(s5_27, "00027");
FATAL_S(s5_28, "00028");
FATAL_S(s5_29, "00029");

FATAL_S(s10_00, "0000000000");
FATAL_S(s10_01, "0000000001");
FATAL_S(s10_02, "0000000002");
FATAL_S(s10_03, "0000000003");
FATAL_S(s10_04, "0000000004");
FATAL_S(s10_05, "0000000005");
FATAL_S(s10_06, "0000000006");
FATAL_S(s10_07, "0000000007");
FATAL_S(s10_08, "0000000008");
FATAL_S(s10_09, "0000000009");

FATAL_S(s10_10, "0000000010");
FATAL_S(s10_11, "0000000011");
FATAL_S(s10_12, "0000000012");
FATAL_S(s10_13, "0000000013");
FATAL_S(s10_14, "0000000014");
FATAL_S(s10_15, "0000000015");
FATAL_S(s10_16, "0000000016");
FATAL_S(s10_17, "0000000017");
FATAL_S(s10_18, "0000000018");
FATAL_S(s10_19, "0000000019");

FATAL_S(s10_20, "0000000020");
FATAL_S(s10_21, "0000000021");
FATAL_S(s10_22, "0000000022");
FATAL_S(s10_23, "0000000023");
FATAL_S(s10_24, "0000000024");
FATAL_S(s10_25, "0000000025");
FATAL_S(s10_26, "0000000026");
FATAL_S(s10_27, "0000000027");
FATAL_S(s10_28, "0000000028");
FATAL_S(s10_29, "0000000029");

FATAL_S(s20_00, "00000000000000000000");
FATAL_S(s20_01, "00000000000000000001");
FATAL_S(s20_02, "00000000000000000002");
FATAL_S(s20_03, "00000000000000000003");
FATAL_S(s20_04, "00000000000000000004");
FATAL_S(s20_05, "00000000000000000005");
FATAL_S(s20_06, "00000000000000000006");
FATAL_S(s20_07, "00000000000000000007");
FATAL_S(s20_08, "00000000000000000008");
FATAL_S(s20_09, "00000000000000000009");

FATAL_S(s20_10, "00000000000000000010");
FATAL_S(s20_11, "00000000000000000011");
FATAL_S(s20_12, "00000000000000000012");
FATAL_S(s20_13, "00000000000000000013");
FATAL_S(s20_14, "00000000000000000014");
FATAL_S(s20_15, "00000000000000000015");
FATAL_S(s20_16, "00000000000000000016");
FATAL_S(s20_17, "00000000000000000017");
FATAL_S(s20_18, "00000000000000000018");
FATAL_S(s20_19, "00000000000000000019");

FATAL_S(s20_20, "00000000000000000020");
FATAL_S(s20_21, "00000000000000000021");
FATAL_S(s20_22, "00000000000000000022");
FATAL_S(s20_23, "00000000000000000023");
FATAL_S(s20_24, "00000000000000000024");
FATAL_S(s20_25, "00000000000000000025");
FATAL_S(s20_26, "00000000000000000026");
FATAL_S(s20_27, "00000000000000000027");
FATAL_S(s20_28, "00000000000000000028");
FATAL_S(s20_29, "00000000000000000029");

FATAL_S(s30_00, "000000000000000000000000000000");
FATAL_S(s30_01, "000000000000000000000000000001");
FATAL_S(s30_02, "000000000000000000000000000002");
FATAL_S(s30_03, "000000000000000000000000000003");
FATAL_S(s30_04, "000000000000000000000000000004");
FATAL_S(s30_05, "000000000000000000000000000005");
FATAL_S(s30_06, "000000000000000000000000000006");
FATAL_S(s30_07, "000000000000000000000000000007");
FATAL_S(s30_08, "000000000000000000000000000008");
FATAL_S(s30_09, "000000000000000000000000000009");

FATAL_S(s30_10, "000000000000000000000000000010");
FATAL_S(s30_11, "000000000000000000000000000011");
FATAL_S(s30_12, "000000000000000000000000000012");
FATAL_S(s30_13, "000000000000000000000000000013");
FATAL_S(s30_14, "000000000000000000000000000014");
FATAL_S(s30_15, "000000000000000000000000000015");
FATAL_S(s30_16, "000000000000000000000000000016");
FATAL_S(s30_17, "000000000000000000000000000017");
FATAL_S(s30_18, "000000000000000000000000000018");
FATAL_S(s30_19, "000000000000000000000000000019");

FATAL_S(s30_20, "000000000000000000000000000020");
FATAL_S(s30_21, "000000000000000000000000000021");
FATAL_S(s30_22, "000000000000000000000000000022");
FATAL_S(s30_23, "000000000000000000000000000023");
FATAL_S(s30_24, "000000000000000000000000000024");
FATAL_S(s30_25, "000000000000000000000000000025");
FATAL_S(s30_26, "000000000000000000000000000026");
FATAL_S(s30_27, "000000000000000000000000000027");
FATAL_S(s30_28, "000000000000000000000000000028");
FATAL_S(s30_29, "000000000000000000000000000029");

} // namespace fatal {
