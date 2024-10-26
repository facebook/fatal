/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <atomic>
#include <vector>

#include <cassert>
#include <cstdint>

namespace fatal {
namespace detail {
namespace ref_counter_impl {

struct counters;

static std::vector<counters *> &all_counters() {
  static std::vector<counters *> instance;
  return instance;
}

struct counters {
  counters():
    created(0),
    alive(0),
    valid(0)
  {}

  std::atomic<std::intmax_t> created;
  std::atomic<std::intmax_t> alive;
  std::atomic<std::intmax_t> valid;

  void reset() {
    created = 0;
    alive = 0;
    valid = 0;
  }

  void operator -=(counters const &rhs) {
    // TODO: FIX, NOT THREAD SAFE
    created -= rhs.created;
    alive -= rhs.alive;
    valid -= rhs.valid;
  }

  void init() {
    if (registered_.test_and_set()) {
      return;
    }

    // TODO: FIX, NOT THREAD SAFE
    all_counters().push_back(this);
  }

private:
  std::atomic_flag registered_ = ATOMIC_FLAG_INIT;
};

static counters &global() {
  static counters instance;
  return instance;
}

} // namespace ref_counter_impl {
} // namespace detail {


/**
 * Tracks the number of instances created and the number of instances still
 * alive.
 *
 * Example:
 *
 *  ref_counter::reset();
 *
 *  // yields `0`
 *  ref_counter::created();
 *  // yields `0`
 *  ref_counter::alive();
 *  // yields `0`
 *  ref_counter::valid();
 *
 *  ref_counter<> g1;
 *
 *  // yields `1`
 *  ref_counter::created();
 *  // yields `1`
 *  ref_counter::alive();
 *  // yields `1`
 *  ref_counter::valid();
 *
 *  {
 *    ref_counter<> g2;
 *
 *    // yields `2`
 *    ref_counter::created();
 *    // yields `2`
 *    ref_counter::alive();
 *    // yields `2`
 *    ref_counter::valid();
 *
 *    ref_counter<> g3(std::move(g2));
 *
 *    // yields `3`
 *    ref_counter::created();
 *    // yields `3`
 *    ref_counter::alive();
 *    // yields `2`
 *    ref_counter::valid();
 *  }
 *
 *  // yields `3`
 *  ref_counter::created();
 *  // yields `1`
 *  ref_counter::alive();
 *  // yields `1`
 *  ref_counter::valid();
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: ADD UNIT TESTS
template <typename...>
struct ref_counter {
  ref_counter():
    valid_(true)
  {
    increase_created();
    increase_alive();
    increase_valid();
  }

  ref_counter(ref_counter const &rhs):
    valid_(rhs.valid_)
  {
    increase_created();
    increase_alive();

    if (valid_) {
      increase_valid();
    }
  }

  ref_counter(ref_counter &&rhs) noexcept:
    valid_(rhs.valid_)
  {
    increase_created();
    increase_alive();
    rhs.valid_ = false;
  }

  template <
    typename T,
    typename... Args,
    typename = typename std::enable_if<
      !std::is_same<ref_counter, typename std::decay<T>::type>::value
    >::type
  >
  explicit ref_counter(T &&, Args &&...):
    valid_(true)
  {
    increase_created();
    increase_alive();
    increase_valid();
  }

  ref_counter &operator =(ref_counter const &rhs) {
    if (!valid_ && rhs.valid_) {
      increase_valid();
    }

    valid_ = rhs.valid_;

    return *this;
  }

  ref_counter &operator =(ref_counter &&rhs) {
    if (valid_) {
      decrease_valid();
    }

    valid_ = rhs.valid_;
    rhs.valid_ = false;

    return *this;
  }

  template <typename T>
  ref_counter &operator =(T &&) {
    if (!valid_) {
      valid_ = true;
      increase_valid();
    }

    return *this;
  }

  ~ref_counter() {
    decrease_alive();

    if (valid_) {
      decrease_valid();
    }
  }

  /**
   * Upon construction, calls `ref_counter_reset()`.
   *
   * Upon destruction, asserts that `alive` and `valid` counters are both 0.
   *
   * Usually constructed at the beginning of a method that uses `ref_counter`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  struct guard {
    guard() {
      auto duplicate = singleton().test_and_set();
      assert(!duplicate);
      (void)duplicate;
      reset();
    }

    guard(guard const &) = delete;
    guard(guard &&) = delete;

    ~guard() {
      assert(counters_().alive == 0);
      assert(counters_().valid == 0);
      singleton().clear();
    }

  private:
    static std::atomic_flag &singleton() {
      static std::atomic_flag instance = ATOMIC_FLAG_INIT;
      return instance;
    }
  };

  /**
   * Resets all counters to 0.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static void reset() {
    detail::ref_counter_impl::global() -= counters_();
    counters_().reset();
  }

  /**
   * The number of instances created.
   *
   * This number increases once a constructor is called.
   *
   * The only way to decrease this number is through `reset()`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static std::intmax_t created() { return counters_().created; }

  /**
   * The number of instances alive.
   *
   * This number increases once a constructor is called and decreases once the
   * destructor is called.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static std::intmax_t alive() { return counters_().alive; }

  /**
   * The number of instances in a valid state.
   *
   * This number increases once a constructor other than the move constructor is
   * called.
   *
   * Once an instance is moved from, it enters an `invalid` state that can only
   * be restored through assignment, and its previous valid state is transfered
   * to the instance it is being moved into.
   *
   * This counter decreases once the destructor is called, as long as the
   * instance being destroyed is in a valid state.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static std::intmax_t valid() { return counters_().valid; }

private:
  bool valid_;

  static detail::ref_counter_impl::counters &counters_() {
    static detail::ref_counter_impl::counters instance;
    instance.init();
    return instance;
  }

  static void increase_created() {
    ++counters_().created;
    ++detail::ref_counter_impl::global().created;
  }

  static void increase_alive() {
    ++counters_().alive;
    ++detail::ref_counter_impl::global().alive;
  }

  static void decrease_alive() {
    --counters_().alive;
    // TODO: FIX, NOT THREAD SAFE
    assert(counters_().alive >= 0);
    --detail::ref_counter_impl::global().alive;
    // TODO: FIX, NOT THREAD SAFE
    assert(detail::ref_counter_impl::global().alive >= 0);
  }

  static void increase_valid() {
    ++counters_().valid;
    ++detail::ref_counter_impl::global().valid;
  }

  static void decrease_valid() {
    --counters_().valid;
    // TODO: FIX, NOT THREAD SAFE
    assert(counters_().valid >= 0);
    --detail::ref_counter_impl::global().valid;
    // TODO: FIX, NOT THREAD SAFE
    assert(detail::ref_counter_impl::global().valid >= 0);
  }
};

/**
 * Resets all global counters to 0.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
void ref_counter_reset() {
  detail::ref_counter_impl::global().reset();

  // TODO: FIX, NOT THREAD SAFE
  for (auto &i: detail::ref_counter_impl::all_counters()) {
    i->reset();
  }
}

/**
 * The global number of instances created for all template instantiations of
 * ref_counter.
 *
 * This number increases once a constructor is called.
 *
 * The only way to decrease this number is through `reset()`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
std::intmax_t ref_counter_created() {
  return detail::ref_counter_impl::global().created;
}

/**
 * The number of instances alive for all template instantiations of ref_counter.
 *
 * This number increases once a constructor is called and decreases once the
 * destructor is called.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
std::intmax_t ref_counter_alive() {
  return detail::ref_counter_impl::global().alive;
}

/**
 * The number of instances in a valid state for all template instantiations of
 * ref_counter.
 *
 * This number increases once a constructor other than the move constructor is
 * called.
 *
 * Once an instance is moved from, it enters an `invalid` state that can only
 * be restored through assignment, and its previous valid state is transfered
 * to the instance it is being moved into.
 *
 * This counter decreases once the destructor is called, as long as the
 * instance being destroyed is in a valid state.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
std::intmax_t ref_counter_valid() {
  return detail::ref_counter_impl::global().valid;
}

/**
 * Upon construction, calls `ref_counter_reset()`.
 *
 * Upon destruction, asserts that `alive` and `valid` counters are both 0.
 *
 * Usually constructed at the beginning of a method that uses `ref_counter` and
 * checks global counters with `ref_counter_*()`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct ref_counter_guard {
  ref_counter_guard() {
    auto duplicate = singleton().test_and_set();
    assert(!duplicate);
    (void)duplicate;
    ref_counter_reset();
  }

  ref_counter_guard(ref_counter_guard const &) = delete;
  ref_counter_guard(ref_counter_guard &&) = delete;

  ~ref_counter_guard() {
    assert(detail::ref_counter_impl::global().alive == 0);
    assert(detail::ref_counter_impl::global().valid == 0);
    singleton().clear();
  }

private:
  static std::atomic_flag &singleton() {
    static std::atomic_flag instance = ATOMIC_FLAG_INIT;
    return instance;
  }
};

} // namespace fatal {
