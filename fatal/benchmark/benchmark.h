/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_benchmark_benchmark_h
#define FATAL_INCLUDE_fatal_benchmark_benchmark_h

#include <fatal/preprocessor.h>
#include <fatal/time/time.h>

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>

namespace fatal {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_BENCHMARK(Group, Name, ...) \
  FATAL_IMPL_BENCHMARK( \
    FATAL_CAT(Group, FATAL_CAT(_, Name)), \
    Group, \
    Name, \
    FATAL_HAS_ARGS(__VA_ARGS__), \
    FATAL_CONDITIONAL(FATAL_HAS_ARGS(__VA_ARGS__)) \
      (__VA_ARGS__) \
      (FATAL_UID(iterations)) \
  )

// TODO: bike-shed HasN
#define FATAL_IMPL_BENCHMARK(Class, Group, Name, HasN, Iterations) \
  class Class { \
    using timer = ::fatal::detail::benchmark_registry::timer; \
    \
    ::fatal::benchmark_controller<timer> benchmark; \
    \
  public: \
    Class(timer &result): benchmark(result) {} \
    \
    void run(benchmark_iterations Iterations) \
    FATAL_CONDITIONAL(HasN)(;)( \
      { \
        while (Iterations--) { \
          run(); \
        } \
      } \
      \
      void run(); \
    ) \
    \
    static char const *group() { return FATAL_TO_STR(Group); } \
    static char const *name() { return FATAL_TO_STR(Name); } \
  }; \
  \
  namespace { \
  static auto const FATAL_UID(FATAL_CAT(Class, FATAL_CAT(_, registry))) \
    = ::fatal::detail::benchmark_registry::get().add<Class>(); \
    \
  } \
  \
  \
  void Class::run(FATAL_CONDITIONAL(HasN)(benchmark_iterations Iterations)())

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_BENCHMARK_SUSPEND \
  FATAL_IMPL_BENCHMARK_SUSPEND(FATAL_UID(benchmark_suspend_scope))

#define FATAL_IMPL_BENCHMARK_SUSPEND(Scope) \
  for (auto Scope = benchmark.suspend(); Scope; Scope.resume())

using benchmark_clock = std::chrono::high_resolution_clock;
using benchmark_duration = benchmark_clock::duration;
using benchmark_iterations = std::uint_fast32_t;

using benchmark_result_entry = std::tuple<
  benchmark_duration,
  benchmark_iterations,
  std::string
>;

using benchmark_results = std::unordered_map<
  std::string,
  std::vector<benchmark_result_entry>
>;

namespace detail {

struct benchmark_registry {
  using time_point = benchmark_clock::time_point;

  struct timer {
    timer():
      elapsed_(0),
      running_(false)
    {}

    void start() {
      assert(!running_);
      running_ = true;
      start_ = benchmark_clock::now();
    }

    void stop() {
      auto const end = benchmark_clock::now();
      assert(running_);
      assert(start_ <= end);
      elapsed_ += end - start_;
      running_ = false;
    }

    benchmark_duration elapsed() const { return elapsed_; }

  private:
    time_point start_;
    benchmark_duration elapsed_;
    bool running_;
  };

private:
  struct entry {
    virtual benchmark_duration run(benchmark_iterations) = 0;
    virtual char const *group() = 0;
    virtual char const *name() = 0;
  };

  template <typename T>
  struct entry_impl:
    entry
  {
    using type = T;

    benchmark_duration run(benchmark_iterations iterations) override {
      timer result;
      type benchmark(result);

      result.start();
      benchmark.run(iterations);
      result.stop();

      return result.elapsed();
    }

    char const *group() override { return type::group(); }
    char const *name() override { return type::name(); }
  };

  struct comparer {
    bool operator ()(
      benchmark_result_entry const &lhs,
      benchmark_result_entry const &rhs
    ) const {
      return std::get<0>(lhs) < std::get<0>(rhs)
        || (
          std::get<0>(lhs) == std::get<0>(rhs)
          && (
            std::get<1>(lhs) > std::get<1>(rhs)
            || (
              std::get<1>(lhs) == std::get<1>(rhs)
              && std::get<2>(lhs) < std::get<2>(rhs)
            )
          )
        );
    }
  };

public:
  template <typename T>
  std::size_t add() {
    entries_.emplace_back(new entry_impl<T>());

    return entries_.size();
  }

  benchmark_results run() const {
    benchmark_results result;

    for (auto const &i: entries_) {
      auto measured = measure(*i);

      result[i->group()].emplace_back(
        measured.first, measured.second, i->name()
      );
    }

    for (auto &group: result) {
      std::sort(group.second.begin(), group.second.end(), comparer());
    }

    return result;
  }

  static benchmark_registry &get() {
    static benchmark_registry instance;
    return instance;
  }

private:
  std::pair<benchmark_duration, benchmark_iterations> measure(entry &i) const {
    benchmark_iterations iterations = 1;
    benchmark_duration elapsed(0);

    for (std::size_t tries = 0; tries < tries_threshold_; ++tries) {
      if (tries) {
        iterations *= 2;
      }

      elapsed = i.run(iterations);

      if (elapsed >= duration_threshold_) {
        break;
      }
    }

    return std::make_pair(elapsed, iterations);
  }

  std::vector<std::unique_ptr<entry>> entries_;
  // TODO: MAKE IT CONFIGURABLE
  std::size_t tries_threshold_ = 10;
  // TODO: MAKE IT CONFIGURABLE
  benchmark_duration duration_threshold_ = std::chrono::microseconds(1);
};

} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct benchmark_controller {
  using type = T;

  explicit benchmark_controller(type &run): run_(run) {}

  struct scoped_suspend {
    explicit scoped_suspend(type *run): run_(run) {}

    scoped_suspend(scoped_suspend const &) = delete;
    scoped_suspend(scoped_suspend &&rhs): run_(rhs.run_) { rhs.run_ = nullptr; }

    ~scoped_suspend() {
      if (run_) {
        run_->start();
      }
    }

    void resume() {
      assert(run_);
      run_->start();
      run_ = nullptr;
    }

    explicit operator bool() const { return run_ != nullptr; }
    bool operator !() const { return !run_; }

  private:
    type *run_;
  };

  scoped_suspend suspend() {
    run_.stop();

    return scoped_suspend(std::addressof(run_));
  }

private:
  type &run_;
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TOut>
benchmark_results run_benchmarks(TOut &out) {
  auto result = detail::benchmark_registry::get().run();

  for (auto const &group: result) {
    out << "-- group: " << group.first << " --\n";

    // TODO: PRETTY PRINT
    // TODO: PRINT RELATIVE VALUES
    for (auto const &i: group.second) {
      out << std::get<2>(i) << ": " << std::get<1>(i) << " iterations in "
        << std::get<0>(i).count() << time::suffix(std::get<0>(i)) << '\n';
    }

    out << '\n';
  }

  return result;
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_benchmark_benchmark_h
