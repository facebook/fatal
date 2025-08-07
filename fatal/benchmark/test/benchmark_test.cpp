/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/test/driver.h>

#include <fatal/benchmark/benchmark.h>

#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include <cassert>

namespace fatal {
namespace benchmark {

std::chrono::milliseconds const big_delay(5);
std::chrono::microseconds const small_delay(50);

FATAL_BENCHMARK(group_1, benchmark_1_1) {
  FATAL_BENCHMARK_SUSPEND {
    std::this_thread::sleep_for(big_delay);
  }
}

FATAL_BENCHMARK(group_1, benchmark_1_2) {
  FATAL_BENCHMARK_SUSPEND {
  }

  std::this_thread::sleep_for(big_delay);
}

FATAL_BENCHMARK(group_1, benchmark_1_3, n) {
  auto token = benchmark.suspend();

  std::this_thread::sleep_for(big_delay);

  token.resume();

  while (n--) {}
}

FATAL_BENCHMARK(group_1, benchmark_1_4, n) {
  auto token = benchmark.suspend();

  token.resume();

  std::this_thread::sleep_for(big_delay);

  while (n--) {}
}

FATAL_BENCHMARK(group_1, benchmark_1_5) {
}

FATAL_BENCHMARK(group_2, benchmark_2_1, n) {
  auto token = benchmark.suspend();

  token.resume();

  while (n--) {
    std::this_thread::sleep_for(small_delay);
  }
}

FATAL_BENCHMARK(group_2, benchmark_2_2) {
}

FATAL_BENCHMARK(group_2, benchmark_2_3, n) {
  std::this_thread::sleep_for(big_delay);

  while (n--) {}
}

FATAL_BENCHMARK(group_2, benchmark_2_4) {
}

FATAL_TEST(benchmark, sanity_check) {
  struct state {
    std::ostringstream cout;
    results metrics;
    std::thread thread;

    void run_one() {
      metrics = run(cout);
    }
  };
  auto states = std::vector<state>(12);

  for (auto &st : states) {
    st.thread = std::thread(std::bind(&state::run_one, std::ref(st)));
  }
  for (auto &st : states) {
    st.thread.join();
  }

  for (auto &st : states) {
    std::cout << st.cout.str();

    for (auto const &i: st.metrics) {
      auto const &grp = i.second;
      std::vector<std::string> names;
      auto const txfm = std::bind(&result_entry::name, std::placeholders::_1);
      std::transform(grp.begin(), grp.end(), std::back_inserter(names), txfm);
      std::sort(names.begin(), names.end());
      auto const adj = std::adjacent_find(names.begin(), names.end());
      FATAL_ASSERT_EQ(adj, names.end());
    }
  }

  auto get_min = [&](std::string const &group, std::string const &name) {
    auto min = duration{};
    for (auto const &st : states) {
      auto const i = st.metrics.find(group);
      assert(i != st.metrics.end());
      auto const &grp = i->second;

      auto const pred = [&](auto const &it) { return it.name() == name; };
      auto const j = std::find_if(grp.begin(), grp.end(), pred);
      assert(j != i->second.end());

      min = std::max(min, j->period());
    }
    return min;
  };

  auto get_med = [&](std::string const &group, std::string const &name) {
    std::vector<duration> periods;
    for (auto const &st : states) {
      auto const i = st.metrics.find(group);
      assert(i != st.metrics.end());
      auto const &grp = i->second;

      auto const pred = [&](auto const &it) { return it.name() == name; };
      auto const j = std::find_if(grp.begin(), grp.end(), pred);
      assert(j != i->second.end());

      periods.push_back(j->period());
    }

    assert(!periods.empty());
    std::sort(periods.begin(), periods.end());
    if (periods.size() % 2 == 1) {
      return periods[periods.size() / 2];
    } else {
      auto const m0 = periods[periods.size() / 2 - 1];
      auto const m1 = periods[periods.size() / 2 - 0];
      return (m0 + m1) / 2;
    }
  };

  FATAL_ASSERT_LT(small_delay, big_delay);

  FATAL_EXPECT_LT(get_med("group_1", "benchmark_1_1"), small_delay);
  FATAL_EXPECT_GE(get_min("group_1", "benchmark_1_2"), big_delay);
  FATAL_EXPECT_LT(get_med("group_1", "benchmark_1_3"), small_delay);
  FATAL_EXPECT_GE(get_min("group_1", "benchmark_1_4"), big_delay);
  FATAL_EXPECT_LT(get_med("group_1", "benchmark_1_5"), small_delay);

  FATAL_EXPECT_LT(get_med("group_2", "benchmark_2_1"), big_delay);
  FATAL_EXPECT_GE(get_min("group_2", "benchmark_2_1"), small_delay);
  FATAL_EXPECT_LT(get_med("group_2", "benchmark_2_2"), small_delay);
  FATAL_EXPECT_GE(get_min("group_2", "benchmark_2_3"), big_delay);
  FATAL_EXPECT_LT(get_med("group_2", "benchmark_2_4"), small_delay);
}

} // namespace benchmark {
} // namespace fatal {
