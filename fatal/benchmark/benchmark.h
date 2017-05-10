/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_benchmark_benchmark_h
#define FATAL_INCLUDE_fatal_benchmark_benchmark_h

#include <fatal/benchmark/prevent_optimization.h>
#include <fatal/container/optional.h>
#include <fatal/portability.h>
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
#include <cstdint>

namespace fatal {
namespace benchmark {

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

#define FATAL_IMPL_BENCHMARK(Class, Group, Name, UserLoop, Iterations) \
  class Class { \
    using timer = ::fatal::benchmark::detail::registry::timer; \
    \
    ::fatal::benchmark::controller<timer> benchmark; \
    \
  public: \
    Class(timer &result): benchmark(result) {} \
    \
    void operator ()(::fatal::benchmark::iterations Iterations) \
    FATAL_CONDITIONAL(UserLoop)(;)( \
      { \
        while (Iterations--) { \
          (*this)(); \
        } \
      } \
      \
      void operator ()(); \
    ) \
  }; \
  \
  namespace { \
  static auto const FATAL_UID( \
    FATAL_CAT(Class, FATAL_CAT(_, benchmark_registry)) \
  ) = ::fatal::benchmark::detail::registry::get().add<Class>(); \
  } \
  \
  char const *operator <<(::fatal::benchmark::detail::group_tag, Class *) { \
    return FATAL_TO_STR(Group); \
  } \
  \
  char const *operator <<(::fatal::benchmark::detail::name_tag, Class *) { \
    return FATAL_TO_STR(Name); \
  } \
  \
  void Class::operator ()(FATAL_CONDITIONAL(UserLoop)( \
    ::fatal::benchmark::iterations Iterations \
  )())

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_BENCHMARK_SUSPEND \
  FATAL_IMPL_BENCHMARK_SUSPEND(FATAL_UID(benchmark_suspend_scope))

#define FATAL_IMPL_BENCHMARK_SUSPEND(Scope) \
  for (auto Scope = benchmark.suspend(); Scope; Scope.resume())

using clock = std::chrono::high_resolution_clock;
using duration = clock::duration;
using iterations = std::uint_fast32_t;

struct result_entry {

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

  result_entry(
    duration net_duration,
    duration gross_duration,
    iterations n,
    std::string name
  ):
    net_duration_(net_duration),
    gross_duration_(gross_duration),
    n_(n),
    period_(net_duration_ / n_),
    name_(std::move(name))
  {}

FATAL_DIAGNOSTIC_POP

  duration net_duration() const { return net_duration_; }
  duration gross_duration() const { return gross_duration_; }
  iterations n() const { return n_; }
  duration period() const { return period_; }
  std::string const &name() const { return name_; }

  bool operator <(result_entry const &rhs) const {
    return period_ < rhs.period_ || (
      period_ == rhs.period_ && (
        n_ > rhs.n_ || (
          n_ == rhs.n_ && name_ < rhs.name_
        )
      )
    );
  }

private:
  duration net_duration_;
  duration gross_duration_;
  iterations n_;
  duration period_;
  std::string name_;
};

using duration_index = std::integral_constant<std::size_t, 0>;
using iterations_index = std::integral_constant<std::size_t, 1>;
using name_index = std::integral_constant<std::size_t, 2>;

using results = std::unordered_map<
  std::string,
  std::vector<result_entry>
>;

namespace detail {

struct group_tag {};
struct name_tag {};

struct registry {
  using time_point = clock::time_point;

  struct timer {
    timer():
      elapsed_(0),
      running_(false)
    {}

    void start() {
      assert(!running_);
      running_ = true;
      start_ = clock::now();
    }

    void stop() {
      auto const end = clock::now();
      assert(running_);
      assert(start_ <= end);
      elapsed_ += end - start_;
      running_ = false;
    }

    duration elapsed() const { return elapsed_; }

  private:
    time_point start_;
    duration elapsed_;
    bool running_;
  };

private:
  struct entry {
    virtual ~entry() {}

    virtual duration run(iterations) = 0;
    virtual char const *group() = 0;
    virtual char const *name() = 0;
  };

  template <typename T>
  struct entry_impl:
    public entry
  {
    using type = T;

    duration run(iterations iterations) override {
      timer result;
      type benchmark(result);

      result.start();
      benchmark(iterations);
      result.stop();

      return result.elapsed();
    }

    char const *group() override {
      return group_tag() << static_cast<type *>(nullptr);
    }

    char const *name() override {
      return name_tag() << static_cast<type *>(nullptr);
    }
  };

public:
  template <typename T>
  bool add() {
    entries_.emplace_back(new entry_impl<T>());

    return entries_.empty();
  }

  results run() const {
    results result;

    for (auto const &i: entries_) {
      auto measured = measure(*i);

      result[i->group()].emplace_back(
        std::get<0>(measured),
        std::get<1>(measured),
        std::get<2>(measured),
        i->name()
      );
    }

    for (auto &group: result) {
      std::sort(group.second.begin(), group.second.end());
    }

    return result;
  }

  static registry &get() {
    static registry instance;
    return instance;
  }

private:
  std::tuple<duration, duration, iterations> measure(entry &i) const {
    iterations iterations = 1;
    duration net_duration(0);
    duration gross_duration(0);

    for (std::size_t tries = 0; tries < tries_threshold_; ++tries) {
      if (tries) {
        iterations *= 2;
      }

      net_duration = i.run(iterations);
      gross_duration += net_duration;

      if (net_duration >= duration_threshold_) {
        break;
      }
    }

    return std::make_tuple(net_duration, gross_duration, iterations);
  }

  std::vector<std::unique_ptr<entry>> entries_;
  // TODO: MAKE IT CONFIGURABLE
  std::size_t tries_threshold_ = 10;
  // TODO: MAKE IT CONFIGURABLE
  duration duration_threshold_ = std::chrono::milliseconds(1);
};

} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct controller {
  using type = T;

  explicit controller(type &run): run_(run) {}

  struct scoped_suspend {
    explicit scoped_suspend(type *run): run_(run) {}

    scoped_suspend(scoped_suspend const &) = delete;
    scoped_suspend(scoped_suspend &&rhs) noexcept:
      run_(rhs.run_)
    {
      rhs.run_ = nullptr;
    }

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

struct default_printer {
  template <typename TOut>
  void operator ()(
    TOut &out,
    results const &result,
    duration running_time
  ) const {
    for (auto const &group: result) {
      out << "-- group: " << group.first << " --\n";

      bool first = true;
      duration previous_period(0);

      for (auto const &i: group.second) {
        auto const period = i.period();
        bool const inf = period.count() == 0;
        auto const frequency = inf
          ? 0
          : decltype(period)(std::chrono::seconds(1)).count() / period.count();

        out << i.name() << ": period = " << period.count()
          << ' ' << time::suffix(period) << ", frequency = ";

        if (inf) {
          out << "inf";
        } else {
          out << frequency;
        }

        out << " Hz";

        if (first) {
          first = false;
        } else {
          assert(previous_period <= period);

          out << ", diff = ";

          if (previous_period.count()) {
            auto const diff = period.count() * 10000 / previous_period.count();
            assert(diff >= 10000);

            out << (diff / 100) << '.' << (diff % 100) << '%';
          } else {
            out << (period.count() ? "inf" : "0.0%");
          }
        }

        out << '\n';

        previous_period = period;
      }

      out << '\n';
    }

    out << "total running time: " << running_time.count()
      << ' ' << time::suffix(running_time) << '\n';
  }
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TPrinter = default_printer, typename TOut>
results run(TOut &out) {
  auto const start = clock::now();
  auto result = detail::registry::get().run();
  auto const running_time = clock::now() - start;

  TPrinter printer;
  printer(out, result, running_time);

  return result;
}

} // namespace benchmark {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_benchmark_benchmark_h
