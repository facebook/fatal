/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_constant_array_h
#define FATAL_INCLUDE_fatal_container_constant_array_h

#include <array>

namespace fatal {

// TODO: DOCUMENT
template <typename T, T... Values>
struct constant_array {
  /**
   * Tells how many elements this array has.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr std::size_t size = sizeof...(Values);

  /**
   * Tells whether this array is empty or not.
   * This is the same as `size == 0`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr bool empty = size == 0;

  // TODO: DOCUMENT
  using value_type = T;

  // TODO: DOCUMENT
  using type = std::array<value_type, size>;

  // TODO: DOCUMENT
  static constexpr type get{{Values...}};

  // TODO: DOCUMENT
  static constexpr value_type const *data() { return get.data(); }
};

// TODO: DOCUMENT AND TEST
// TODO: ADD , template <typename...> class... Transforms
template <typename T>
class constant_array_from {
  template <T... Values>
  using sequence_impl = constant_array<T, Values...>;

public:
  // TODO: DOCUMENT AND TEST
  template <typename... Args>
  using args = constant_array<T, Args::value...>;

  // TODO: DOCUMENT AND TEST
  template <typename List>
  using list = typename List::template apply<args>;

  template <typename Sequence>
  using sequence = typename Sequence::template apply<sequence_impl>;
};

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

template <typename T, T... Values>
constexpr std::size_t constant_array<T, Values...>::size;

template <typename T, T... Values>
constexpr bool constant_array<T, Values...>::empty;

template <typename T, T... Values>
constexpr typename constant_array<T, Values...>::type
constant_array<T, Values...>::get;

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_constant_array_h
