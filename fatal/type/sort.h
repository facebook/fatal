/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/apply.h>
#include <fatal/type/compare.h>
#include <fatal/type/logical.h>
#include <fatal/type/pair.h>
#include <fatal/type/slice.h>

#include <fatal/type/impl/sort.h>

namespace fatal {

// `Pair` is guaranteed to be instantiated at most once
template <typename T, typename Filter, template <typename...> class Pair = pair>
using partition = typename i_s::P<T>::template apply<Filter, Pair>;

template <typename T, typename Predicate>
using filter = typename i_s::F<Predicate, T>::type;

template <typename T, typename Predicate>
using reject = filter<T, negation<Predicate>>;

// TODO: ACCEPT A CUSTOM PREDICATE
template  <typename LHS, typename RHS>
using merge = typename i_s::M<LHS, RHS>::type;

template <typename T, typename Less = less, typename... By>
using sort = typename i_s::Q<T, Less, By...>::type;

template <typename T, typename... By>
using sort_by = sort<T, less, By...>;

// TODO: REVIEW / MOVE SOMEWHERE ELSE / GENERALIZE / ...
template <typename T>
using invert = typename i_s::i<T>::type;

} // namespace fatal {
