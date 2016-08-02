/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_sort_h
#define FATAL_INCLUDE_fatal_type_sort_h

#include <fatal/type/apply.h>
#include <fatal/type/compare.h>
#include <fatal/type/logical.h>
#include <fatal/type/pair.h>
#include <fatal/type/slice.h>

#include <fatal/type/impl/sort.h>

namespace fatal {

template <typename T, typename Less, template <typename...> class Pair = pair>
using partition = typename impl_srt::part<Pair, T, Less>::type;

template <typename T, typename Predicate>
using filter = typename impl_srt::flt<Predicate, T>::type;

template <typename T, typename Predicate>
using reject = filter<T, negation<Predicate>>;

// TODO: ACCEPT A CUSTOM PREDICATE
template  <typename LHS, typename RHS>
using merge = typename impl_srt::merge<LHS, RHS>::type;

template <typename T, typename Less = applier<less>>
using sort = typename impl_srt::qs<T, Less>::type;

// TODO: MAKE IT GENERATE LESS TEMPLATES FOR THE PREDICATE
template <typename T, typename Less = applier<less>>
using map_sort = sort<
  T,
  filtered_applier<Less::template apply, first>
>;

// TODO: MAKE IT GENERATE LESS TEMPLATES FOR THE PREDICATE
template <typename T, typename Less = applier<less>>
using sequence_map_sort = sort<
  T,
  filtered_applier<sequence_compare<Less>::template apply, first>
>;

// TODO: REVIEW / MOVE SOMEWHERE ELSE / GENERALIZE / ...
template <typename T>
using invert = typename impl_srt::inv<T>::type;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_sort_h
