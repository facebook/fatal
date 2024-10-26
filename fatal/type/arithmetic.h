/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/arithmetic.h>

namespace fatal {

struct add {
  template <typename... Args>
  using apply = typename impl_arithmetic::add<Args...>::type;
};

struct subtract {
  template <typename... Args>
  using apply = typename impl_arithmetic::sub<Args...>::type;
};

struct multiply {
  template <typename... Args>
  using apply = typename impl_arithmetic::mul<Args...>::type;
};

struct divide {
  template <typename... Args>
  using apply = typename impl_arithmetic::div<Args...>::type;
};

struct modulo {
  template <typename... Args>
  using apply = typename impl_arithmetic::mod<Args...>::type;
};

} // namespace fatal {
