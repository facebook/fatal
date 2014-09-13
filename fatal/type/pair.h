/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/traits.h>

#include <type_traits>
#include <utility>

namespace fatal {

/**
 * Type pair for template metaprogramming.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TFirst, typename TSecond>
struct type_pair {
  typedef TFirst first;
  typedef TSecond second;

  /**
   * A `type_pair` with `first` and `second` swapped.
   *
   * Example:
   *
   *  typedef type_pair<int, double> pair;
   *
   *  // yields `type_pair<double, int>`
   *  typedef pair::invert result;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  typedef type_pair<TSecond, TFirst> invert;

  /**
   * Applies an optional transform to the elements of this pair.
   *
   * Example:
   *
   *  typedef type_pair<int, bool> pair;
   *
   *  // yields `type_pair<Foo<int>, Bar<bool>>`
   *  typdef pair::transform<Foo, Bar> result1;
   *
   *  // yields `type_pair<Foo<int>, bool>`
   *  typdef pair::transform<Foo> result2;
   *
   *  // yields `type_pair<int, bool>`
   *  typdef pair::transform<> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TFirstTransform = transform::identity,
    template <typename...> class TSecondTransform = transform::identity
  >
  using transform = type_pair<TFirstTransform<first>, TSecondTransform<second>>;
};

/**
 * Creates a `type_pair` out of given types. Takes optional transforms that will
 * be independently applied to the `first` and `second` members of the pair.
 *
 * See individual members below for detailed info.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TFirstTransform = transform::identity,
  template <typename...> class TSecondTransform = transform::identity
>
struct type_pair_from {
  /**
   * Creates a `type_pair` out of a single type by individually applying
   * transforms to it in order to get the `first` and `second` members of
   * the pair.
   *
   * Example:
   *
   *  // yields `type_pair<Foo<int>, Bar<int>>`
   *  typdef type_pair_from<Foo, Bar>::type<int> result1;
   *
   *  // yields `type_pair<Foo<int>, int>`
   *  typdef type_pair_from<Foo>::type<int> result2;
   *
   *  // yields `type_pair<int, int>`
   *  typdef type_pair_from<>::type<int> result3;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename U>
  using type = type_pair<TFirstTransform<U>, TSecondTransform<U>>;
};

/**
 * Specialization of `type_get_traits` so that `type_get` supports `type_pair`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TFirst, typename TSecond, std::size_t Index>
struct type_get_traits<type_pair<TFirst, TSecond>, Index> {
  static_assert(Index < 2, "index out of bounds");
  typedef typename std::conditional<Index == 0, TFirst, TSecond>::type type;
};

} // namespace fatal
