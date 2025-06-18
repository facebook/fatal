/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/math/statistical_moments.h>
#include <fatal/type/scalar.h>

#include <fatal/test/driver.h>

#include <random>
#include <ratio>
#include <vector>

namespace fatal {

using value_type = long double;

// number of iterations for some tests
using iterations = size_constant<100>;

// how many rounds to run on each randomized test
using rounds = size_constant<100>;

// the number of samples for each randomized test
using samples = size_constant<100000>;

// the relative error allowed when comparing results
using relative_error = std::ratio<1, 100>;

// the absolute error to use when comparing results
using absolute_error = std::ratio<1, 1000>;

// the precision to use when comparing results that should be equal
using equality_precision = std::ratio<1, 10000000>;

////////////////////
// TEST UTILITIES //
////////////////////

template <typename T, typename TRNG, typename TDistribution>
std::vector<T> random_samples(
  std::size_t case_samples,
  TRNG &&rng,
  TDistribution &&distribution
) {
  std::vector<T> result;
  result.reserve(case_samples);

  while (case_samples--) {
    result.push_back(distribution(rng));
  }

  return result;
}

template <typename T, typename TRNG, typename TDistribution>
void statistical_moments_test_round(
  std::size_t const case_samples,
  TRNG &&rng,
  TDistribution &&distribution
) {
  using case_value_type = T;

  FATAL_ASSERT_GT(case_samples, 0);

  auto const v1 = random_samples<case_value_type>(
    case_samples / 2, rng, distribution
  );
  auto const v2 = random_samples<case_value_type>(
    case_samples / 2 + (case_samples & 1),
    rng,
    distribution
  );
  FATAL_ASSERT_EQ(case_samples, v1.size() + v2.size());

  statistical_moments<case_value_type> moments; // all samples
  statistical_moments<case_value_type> moments1; // samples in v1
  statistical_moments<case_value_type> moments2; // samples in v2

  FATAL_EXPECT_TRUE(moments.empty());
  FATAL_EXPECT_TRUE(moments1.empty());
  FATAL_EXPECT_TRUE(moments2.empty());

  // calculates the moments using the streaming algorithm

  case_value_type sum = 0;

  for (auto i: v1) {
    sum += i;
    moments.add(i);
    moments1.add(i);
  }

  for (auto i: v2) {
    sum += i;
    moments.add(i);
    moments2.add(i);
  }

  FATAL_EXPECT_FALSE(moments.empty());
  FATAL_EXPECT_FALSE(moments1.empty());
  FATAL_EXPECT_FALSE(moments2.empty());

  FATAL_EXPECT_EQ(case_samples, moments.size());
  FATAL_EXPECT_EQ(v1.size(), moments1.size());
  FATAL_EXPECT_EQ(v2.size(), moments2.size());

  // calculates the moments using the traditional approach, for later comparison

  auto const mean = sum / case_samples;

  case_value_type cumulant_1 = 0;
  case_value_type cumulant_3 = 0;
  case_value_type cumulant_4 = 0;

  for (auto i: v1) {
    auto const x = i - mean;
    cumulant_1 += x * x;
    cumulant_3 += x * x * x;
    cumulant_4 += x * x * x * x;
  }

  for (auto i: v2) {
    auto const x = i - mean;
    cumulant_1 += x * x;
    cumulant_3 += x * x * x;
    cumulant_4 += x * x * x * x;
  }

  auto const variance = cumulant_1 / case_samples;
  auto const standard_deviation = std::sqrt(variance);
  auto const standard_deviation_3 = variance * standard_deviation;
  auto const kurtosis = cumulant_4 / case_samples / (variance * variance) - 3;
  auto const skewness = cumulant_3 / case_samples / standard_deviation_3;

  // checks the results

// checks if results are different by no more than precision
# define TEST_ABSOLUTE_ERROR_IMPL( \
  expected, \
  actual, \
  relative_error, \
  absolute_error \
) \
  do { \
    FATAL_ASSERT_GE(absolute_error, 0); \
    FATAL_EXPECT_LE(expected - absolute_error, actual); \
    FATAL_EXPECT_GE(expected + absolute_error, actual); \
  } while (false)

// checks if results are different by no more than threshold percent,
// or precision if such percentage of the expected result is smaller
// than the precision
# define TEST_RELATIVE_ERROR_IMPL( \
  expected, \
  actual, \
  relative_error, \
  absolute_error \
) \
  do { \
    auto const margin = std::abs(expected * relative_error); \
    \
    if (margin < absolute_error) { \
      TEST_ABSOLUTE_ERROR_IMPL( \
        expected, \
        actual, \
        relative_error, \
        absolute_error \
      ); \
    } else { \
      FATAL_EXPECT_LE(expected - margin, actual); \
      FATAL_EXPECT_GE(expected + margin, actual); \
    } \
  } while (false)

# define TEST_ALL_IMPL(Fn, ...) \
  do { \
    Fn(TEST_RELATIVE_ERROR_IMPL, mean, __VA_ARGS__); \
    Fn(TEST_RELATIVE_ERROR_IMPL, variance, __VA_ARGS__); \
    Fn(TEST_RELATIVE_ERROR_IMPL, standard_deviation, __VA_ARGS__); \
    Fn(TEST_ABSOLUTE_ERROR_IMPL, skewness, __VA_ARGS__); \
    Fn(TEST_ABSOLUTE_ERROR_IMPL, kurtosis, __VA_ARGS__); \
  } while (false)

# define TEST_IMPL(Fn, what, subject) \
  Fn( \
    what, \
    subject.what(), \
    (to_scalar<relative_error, long double>()), \
    (to_scalar<absolute_error, long double>()) \
  )

  // compares `moments` against the expected results
  TEST_ALL_IMPL(TEST_IMPL, moments);

# undef TEST_IMPL
# define TEST_IMPL(Fn, what, expected, actual) \
  Fn( \
    expected.what(), \
    actual.what(), \
    (to_scalar<relative_error, long double>()), \
    (to_scalar<equality_precision, long double>()) \
  )

  // tests merging into an empty instance
  statistical_moments<case_value_type> merge_into_empty;
  merge_into_empty.merge(moments);
  TEST_ALL_IMPL(TEST_IMPL, moments, merge_into_empty);

  // tests merging two sets of samples
  auto merged_subsets(moments1);
  merged_subsets.merge(moments2);
  TEST_ALL_IMPL(TEST_IMPL, moments, merged_subsets);

# undef TEST_IMPL
# undef TEST_ALL_IMPL
}

template <typename T, typename TRNG, typename TDistribution>
void test_statistical_moments(
  std::size_t const case_samples,
  TRNG &&rng,
  TDistribution &&distribution
) {
  for (auto i = rounds::value; i--; ) {
    statistical_moments_test_round<T>(case_samples, rng, distribution);
  }
}

///////////
// TESTS //
///////////

// the minimum value of a sample on uniform distribution randomized tests
using uniform_min = std::ratio<-1000>;

// the maximum value of a sample on uniform distribution randomized tests
using uniform_max = std::ratio<1000>;

static_assert(std::ratio_less_equal<uniform_min, uniform_max>::value);

FATAL_TEST(statistical_moments, uniform_distribution) {
  test_statistical_moments<value_type>(
    samples::value,
    random_data(),
    std::uniform_real_distribution<value_type>(
      to_scalar<uniform_min, long double>(),
      to_scalar<uniform_max, long double>()
    )
  );
}

// the mean to use for normal distribution randomized tests
using normal_mean = std::ratio<0>;

// the standard deviation to use for normal distribution randomized tests
using normal_stddev = std::ratio<10>;

FATAL_TEST(statistical_moments, normal_distribution) {
  test_statistical_moments<value_type>(
    samples::value,
    random_data(),
    std::normal_distribution<value_type>(
      to_scalar<normal_mean, long double>(),
      to_scalar<normal_stddev, long double>()
    )
  );
}

// the rate (lambda) to use for exponential distribution randomized tests
using exponential_lambda = std::ratio<10>;

FATAL_TEST(statistical_moments, exponential_distribution) {
  test_statistical_moments<value_type>(
    samples::value,
    random_data(),
    std::exponential_distribution<value_type>(
      to_scalar<exponential_lambda, long double>()
    )
  );
}

FATAL_TEST(statistical_moments, state) {
  random_data rng;
  std::normal_distribution<value_type> distribution(
    to_scalar<normal_mean, long double>(),
    to_scalar<normal_stddev, long double>()
  );

  statistical_moments<value_type> moments;
  statistical_moments<value_type> empty_copy(moments.state());
  FATAL_EXPECT_EQ(moments, empty_copy);

  for (auto i = iterations::value; i--; ) {
    moments.add(distribution(rng));
    statistical_moments<value_type> copy(moments.state());
    FATAL_EXPECT_EQ(moments, copy);
  }
}

} // namespace fatal {
