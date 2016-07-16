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
#include <map>
#include <string>
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
  std::map<std::string, std::map<std::string, duration>> metrics;

  for (auto const &i: run(std::cout)) {
    auto &group = metrics[i.first];

    for (auto const &j: i.second) {
      FATAL_ASSERT_EQ(group.end(), group.find(j.name()));

      group[j.name()] = j.period();
    }
  }

  auto get = [&](std::string group, std::string name) {
    auto i = metrics.find(group);
    assert(i != metrics.end());

    auto j = i->second.find(name);
    assert(j != i->second.end());

    return j->second;
  };

  FATAL_ASSERT_LT(small_delay, big_delay);

  FATAL_EXPECT_LT(get("group_1", "benchmark_1_1"), small_delay);
  FATAL_EXPECT_GE(get("group_1", "benchmark_1_2"), big_delay);
  FATAL_EXPECT_LT(get("group_1", "benchmark_1_3"), small_delay);
  FATAL_EXPECT_GE(get("group_1", "benchmark_1_4"), big_delay);
  FATAL_EXPECT_LT(get("group_1", "benchmark_1_5"), small_delay);

  FATAL_EXPECT_LT(get("group_2", "benchmark_2_1"), big_delay);
  FATAL_EXPECT_GE(get("group_2", "benchmark_2_1"), small_delay);
  FATAL_EXPECT_LT(get("group_2", "benchmark_2_2"), small_delay);
  FATAL_EXPECT_GE(get("group_2", "benchmark_2_3"), big_delay);
  FATAL_EXPECT_LT(get("group_2", "benchmark_2_4"), small_delay);
}

} // namespace benchmark {
} // namespace fatal {
