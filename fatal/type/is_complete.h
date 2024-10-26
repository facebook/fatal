/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/impl/is_complete.h>

namespace fatal {

/**
 * Tells whether a type is complete or not.
 *
 * Example:
 *
 *  struct complete_type {}; // complete type
 *  struct forward_declared_type;
 *
 *  // yields `true`
 *  is_complete<int>::type::value
 *
 *  // yields `true`
 *  is_complete<complete_type>::type::value
 *
 *  // yields `false`
 *  is_complete<forward_declared_type>::type::value
 *
 *  // now it has been declared and is not incomplete anymore
 *  struct forward_declared_type {};
 *
 *  // yields `true`
 *  is_complete<forward_declared_type>::type::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using is_complete = decltype(
  impl_is_complete::sfinae(static_cast<T *>(nullptr))
);

} // namespace fatal {
