/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_apply_h
#define FATAL_INCLUDE_fatal_type_apply_h

#include <fatal/type/impl/apply.h>

namespace fatal {

template <typename T, template <typename...> class To, typename... Suffix>
using apply_to = typename impl_apply::ls<To, T, Suffix...>::type;

template <typename T, template <typename...> class To, typename... Prefix>
using apply_to_front = typename impl_apply::lsf<To, T, Prefix...>::type;

template <typename T>
using sequence_apply = impl_apply::sq<T>;

template <template <typename...> class T>
struct applier {
  template <typename... Args>
  using apply = typename impl_apply::app<T, Args...>::type;
};

template <template <typename...> class T, template <typename> class Filter>
struct filtered_applier {
  template <typename... Args>
  using apply = typename impl_apply::app<T, Filter<Args>...>::type;
};

// TODO: ADD FILTERED VARIANT
template <typename T, typename... Bound>
struct curry {
  template <typename... Args>
  using apply = typename impl_apply::app<
    T::template apply, Bound..., Args...
  >::type;
};

// TODO: ADD FILTERED VARIANT
template <typename T, typename... Bound>
struct curry_back {
  template <typename... Args>
  using apply = typename impl_apply::app<
    T::template apply, Args..., Bound...
  >::type;
};

} // namespace fatal {

#include <fatal/type/deprecated/apply.h>

#endif // FATAL_INCLUDE_fatal_type_apply_h
