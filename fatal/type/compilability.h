/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/list.h>

#include <fatal/type/impl/compilability.h>

namespace fatal {

/**
 * Evaluates to either `std::true_type` or `std::false_type` telling whether
 * applying `Args` to the template `T` would result in a successful compilation
 * or not.
 *
 * It uses SFINAE under the hood, so the applications may be somewhat limited.
 * Due to the lack of robustness, this is not intended to be used outside of
 * unit tests.
 *
 * @author: Marcelo Juchem <juchem@gmail.com>
 */
template <template <typename...> class T, typename... Args>
using check_compilability = decltype(
  i_cpl::checker<T, sizeof...(Args)>::sfinae(
    static_cast<list<Args...> *>(nullptr)
  )
);

} // namespace fatal {
