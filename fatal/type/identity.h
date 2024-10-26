/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {

/**
 * Helper alias similar to std::decay_t, that resolves to the type
 * given as the template parameter.
 *
 * Example:
 *
 *  // yields `int`
 *  using result2 = identity<int>;
 *
 *  // yields `std::string`
 *  using result2 = identity<std::string>;
 *
 *  // yields `double`
 *  using result3 = identity<identity<double>>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using identity = T;

struct get_identity {
  template <typename T>
  using apply = T;
};

} // namespace fatal {
