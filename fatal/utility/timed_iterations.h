/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <chrono>

namespace fatal {

template <
  typename Clock = std::chrono::system_clock,
  typename Counter = std::size_t
>
struct timed_iterations {
  using clock = Clock;
  using time_stamp = typename clock::time_point;
  using duration = typename clock::duration;
  using counter = Counter;

  template <typename Duration>  
  timed_iterations(
    Duration time,
    counter minimum,
    counter check_interval = 1000 // TODO: smarter default value
  ):
    minimum_(minimum),
    check_interval_(check_interval),
    start_(clock::now()),
    deadline_(std::chrono::time_point_cast<duration>(start_ + time))
  {}
  
  bool next() {
    ++i_;
    ++iterations_;
    
    if (iterations_ >= minimum_ && i_ >= check_interval_) {
      auto const now = clock::now();

      if (now >= deadline_) {
        deadline_ = now;
        return false;
      }

      i_ = 0;
    }
    
    return true;
  }
  
  time_stamp start() const { return start_; }
  time_stamp deadline() const { return deadline_; }

  counter count() const { return iterations_; }
  
  // only effective after next() returns false
  duration elapsed() const { return deadline_ - start_; }
  
private:
  counter i_ = 0;
  counter iterations_ = 0;
  counter const minimum_;
  counter const check_interval_;
  time_stamp const start_;
  time_stamp deadline_;
};

} // namespace std {
