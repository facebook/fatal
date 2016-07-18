/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/deprecated/apply.h>

#include <fatal/test/driver.h>

namespace fatal {

#define TEST_IMPL(T, ...) \
  FATAL_EXPECT_SAME<T<__VA_ARGS__>, apply<T, __VA_ARGS__>>()

template <typename... Args>
struct args {};

FATAL_TEST(apply, args) {
  FATAL_EXPECT_SAME<args<>, apply<args>>();
  TEST_IMPL(args, int);
  TEST_IMPL(args, int, bool);
  TEST_IMPL(args, int, bool, double);
  TEST_IMPL(args, int, bool, double, float);
  TEST_IMPL(args, int, bool, double, float, long);
}

template <typename T>
struct t {};

template <typename T, typename... Args>
struct t_args {};

FATAL_TEST(apply, t) {
  TEST_IMPL(t, int);
  TEST_IMPL(t_args, int);
  TEST_IMPL(t_args, int, bool);
  TEST_IMPL(t_args, int, bool, double);
  TEST_IMPL(t_args, int, bool, double, float);
  TEST_IMPL(t_args, int, bool, double, float, long);
}

template <typename T, typename U>
struct t_u {};

template <typename T, typename U, typename... Args>
struct t_u_args {};

FATAL_TEST(apply, t_u) {
  TEST_IMPL(t_u, int, bool);
  TEST_IMPL(t_u_args, int, bool);
  TEST_IMPL(t_u_args, int, bool, double);
  TEST_IMPL(t_u_args, int, bool, double, float);
  TEST_IMPL(t_u_args, int, bool, double, float, long);
}

template <typename T, typename U, typename V>
struct t_u_v {};

template <typename T, typename U, typename V, typename... Args>
struct t_u_v_args {};

FATAL_TEST(apply, t_u_v) {
  TEST_IMPL(t_u_v, int, bool, double);
  TEST_IMPL(t_u_v_args, int, bool, double);
  TEST_IMPL(t_u_v_args, int, bool, double, float);
  TEST_IMPL(t_u_v_args, int, bool, double, float, long);
}

template <typename T, typename U, typename V, typename W>
struct t_u_v_w {};

template <typename T, typename U, typename V, typename W, typename... Args>
struct t_u_v_w_args {};

FATAL_TEST(apply, t_u_v_w) {
  TEST_IMPL(t_u_v_w, int, bool, double, float);
  TEST_IMPL(t_u_v_w_args, int, bool, double, float);
  TEST_IMPL(t_u_v_w_args, int, bool, double, float, long);
}

template <typename T, typename U, typename V, typename W, typename Z>
struct t_u_v_w_z {};

template <
  typename T, typename U, typename V, typename W, typename Z, typename... Args
> struct t_u_v_w_z_args {};

FATAL_TEST(apply, t_u_v_w_z) {
  TEST_IMPL(t_u_v_w_z, int, bool, double, float, long);
  TEST_IMPL(t_u_v_w_z_args, int, bool, double, float, long);
}

} // namespace fatal {
