/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

namespace fatal {

/**
 * A metafunction which, when partially applied to one type `C`, always
 * returns `C` when fully applied to any further type `T`.
 *
 * Example:
 *
 *  using get_constant_double = get_constant<double>;
 *
 *  // yields `double`
 *  using result = get_constant_double::apply<int>>();
 */
template <typename C>
struct get_constant {
  template <typename>
  using apply = C;
};

}
