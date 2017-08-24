/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_math_statistical_moments_h
#define FATAL_INCLUDE_fatal_math_statistical_moments_h

#include <tuple>
#include <utility>

#include <cmath>

#include <fatal/portability.h>

namespace fatal {

/**
 * Online calculation of statistical moments:
 * - mean
 * - variance
 * - standard deviation
 * - skewness
 * - kurtosis
 *
 * It allows, for instance, calculating these moments from a stream of samples,
 * without the need to hold all samples in memory at the same time. The partial
 * results can also be queried at any given time.
 *
 * The arithmetic has been worked out to make the calculation numerical stable.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T = double>
struct statistical_moments {
  /**
   * The type of the samples.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using value_type = T;

  /**
   * The type representing the number of samples seen at any given moment.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using size_type = std::size_t;

  /**
   * Constructors.
   */
  statistical_moments() = default;
  statistical_moments(statistical_moments const &rhs) = default;
  statistical_moments(statistical_moments &&rhs) = default;

  /**
   * Adds a new sample from the stream.
   *
   * Example:
   *
   *  statistical_moments<> moments;
   *
   *  for (decltype(moments)::value_type sample; std::cin >> sample; ) {
   *    moments.add(sample);
   *  }
   *
   *  std::cout << "samples observed: " << moments.size()
   *    << " mean: " << moments.mean()
   *    << " variance: " << moments.variance()
   *    << std::endl;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  void add(value_type const &sample) {
    auto const n = samples_++;

    auto const delta = sample - moment_1_;
    auto const normalized_delta = delta / samples_;
    auto const normalized_delta_squared = normalized_delta * normalized_delta;

    auto const term1 = delta * normalized_delta * n;

    moment_4_ += term1 * normalized_delta_squared
      * (samples_ * samples_ - 3 * samples_ + 3)
      + 6 * normalized_delta_squared * moment_2_
      - 4 * normalized_delta * moment_3_;

    moment_3_ += term1 * normalized_delta * (n - 1)
      - 3 * normalized_delta * moment_2_;

    moment_2_ += term1;

    moment_1_ += normalized_delta;
  }

  /**
   * Returns the mean of the samples added so far.
   *
   * See: http://en.wikipedia.org/wiki/Arithmetic_mean
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  value_type const &mean() const { return moment_1_; }

  /**
   * Calculates the variance of the samples added so far.
   *
   * See: http://en.wikipedia.org/wiki/Variance
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  value_type variance() const {
    return samples_ < 2 ? 0 : moment_2_ / (samples_ - 1);
  }

  /**
   * Calculates the standard deviation of the samples added so far.
   *
   * This is slower than working with the variance since this function calls
   * `std::sqrt`.
   *
   * See: http://en.wikipedia.org/wiki/Standard_deviation
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  value_type standard_deviation() const { return std::sqrt(variance()); }

  /**
   * Calculates the skewness of the samples added so far.
   *
   * This function calls `std::sqrt` and `std::pow`.
   *
   * See: http://en.wikipedia.org/wiki/Skewness
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  value_type skewness() const {
    return std::sqrt(static_cast<value_type>(samples_)) * moment_3_
      / std::pow(moment_2_, 1.5);
  }

  /**
   * Calculates the kurtosis of the samples added so far.
   *
   * See: http://en.wikipedia.org/wiki/Kurtosis
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  value_type kurtosis() const {
    return (samples_ * moment_4_) / (moment_2_ * moment_2_) - 3;
  }

  /**
   * Tells how many samples have been added so far.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  size_type size() const { return samples_; }

  /**
   * True if no sample has been added so far, false otherwise.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  bool empty() const { return !samples_; }

  /**
   * Clears the internal state as if no sample had been added so far.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  void clear() {
    samples_ = 0;
    moment_1_ = 0;
    moment_2_ = 0;
    moment_3_ = 0;
    moment_4_ = 0;
  }

  bool operator != (statistical_moments const &rhs) const {
    return !(*this == rhs);
  }

  bool operator == (statistical_moments const &rhs) const {
    return rhs.samples_ == samples_
      && rhs.moment_1_ == moment_1_
      && rhs.moment_2_ == moment_2_
      && rhs.moment_3_ == moment_3_
      && rhs.moment_4_ == moment_4_;
  }

  /**
   * Merges the samples from the given instance `rhs` into this one.
   *
   * Example:
   *
   *  template <typename T, typename Stream>
   *  void gather(statistical_moments<T> &moments, Stream &&in) {
   *    for (T sample; in >> sample; ) {
   *      moments.add(sample);
   *    }
   *  }
   *
   *  template <typename T>
   *  void print(statistical_moments<T> &moments, char const *text) {
   *    std::cout << text << " - samples observed: " << moments.size()
   *      << " mean: " << moments.mean()
   *      << " variance: " << moments.variance()
   *      << std::endl;
   *  }
   *
   *  int main() {
   *    statistical_moments<> moments1;
   *    statistical_moments<> moments2;
   *
   *    gather(moments1, std::cin);
   *    gather(moments2, std::ifstream("/tmp/samples"));
   *
   *    print("<stdin>", moments1);
   *    print("/tmp/samples", moments2);
   *    print("both", moments1.merge(moments2));
   *  }
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  statistical_moments &merge(statistical_moments const &rhs) {
    auto samples = samples_ + rhs.samples_;

    auto const delta_1 = rhs.moment_1_ - moment_1_;
    auto const delta_2 = delta_1 * delta_1;
    auto const delta_3 = delta_1 * delta_2;
    auto const delta_4 = delta_2 * delta_2;

    auto moment_1 = (
      samples_ * moment_1_ + rhs.samples_ * rhs.moment_1_
    ) / samples;

    auto moment_2 = moment_2_ + rhs.moment_2_
      + delta_2 * samples_ * rhs.samples_ / samples;

    auto moment_3 = (
      moment_3_ + rhs.moment_3_
        + delta_3 * samples_ * rhs.samples_ * (samples_ - rhs.samples_)
        / (samples * samples)
      ) + (
        3 * delta_1 * (
          samples_ * rhs.moment_2_ - rhs.samples_ * moment_2_
        ) / samples
      );

    moment_4_ = (
      moment_4_ + rhs.moment_4_
        + delta_4 * samples_ * rhs.samples_ * (
          samples_ * samples_
            - samples_ * rhs.samples_
            + rhs.samples_ * rhs.samples_
        ) / (samples * samples * samples)
      ) + (
        6 * delta_2 * (
          samples_ * samples_ * rhs.moment_2_
            + rhs.samples_ * rhs.samples_ * moment_2_
        ) / (samples * samples)
        + 4 * delta_1 * (
          samples_ * rhs.moment_3_ - rhs.samples_ * moment_3_
        ) / samples
      );

    samples_ = samples;
    moment_1_ = std::move(moment_1);
    moment_2_ = std::move(moment_2);
    moment_3_ = std::move(moment_3);

    return *this;
  }

  /**
   * A representation of the internal state.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using internal_state = std::tuple<
    size_type,
    value_type,
    value_type,
    value_type,
    value_type
  >;

  /**
   * Gets the representation of the internal state.
   *
   * Useful for serialization.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  internal_state state() const {
    return internal_state(
      samples_,
      moment_1_,
      moment_2_,
      moment_3_,
      moment_4_
    );
  }

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

  /**
   * Constructs an instance by restoring the given internal state.
   *
   * Useful for deserialization.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  explicit statistical_moments(internal_state const &state):
    samples_(std::get<0>(state)),
    moment_1_(std::get<1>(state)),
    moment_2_(std::get<2>(state)),
    moment_3_(std::get<3>(state)),
    moment_4_(std::get<4>(state))
  {}

FATAL_DIAGNOSTIC_POP

private:
  size_type samples_ = 0;
  value_type moment_1_ = 0;
  value_type moment_2_ = 0;
  value_type moment_3_ = 0;
  value_type moment_4_ = 0;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_math_statistical_moments_h
