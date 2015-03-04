/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_time_time_h
#define FATAL_INCLUDE_fatal_time_time_h

#include <fatal/type/map.h>
#include <fatal/type/prefix_tree.h>
#include <fatal/type/string.h>
#include <fatal/type/transform.h>

#include <chrono>

namespace fatal {
namespace time {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

namespace detail {

// TODO: MOVE TO MATH
using ratio_suffixes = build_type_map<
  //std::yotta, type_string<char, 'Y'>,
  //std::zetta, type_string<char, 'Z'>,
  std::exa, type_string<char, 'E'>,
  std::peta, type_string<char, 'P'>,
  std::tera, type_string<char, 'T'>,
  std::giga, type_string<char, 'G'>,
  std::mega, type_string<char, 'M'>,
  std::kilo, type_string<char, 'k'>,
  std::hecto, type_string<char, 'h'>,
  std::deca, type_string<char, 'd', 'a'>,
  std::deci, type_string<char, 'd'>,
  std::centi, type_string<char, 'c'>,
  std::milli, type_string<char, 'm'>,
  std::micro, type_string<char, 'u'>,
  std::nano, type_string<char, 'n'>,
  std::pico, type_string<char, 'p'>,
  std::femto, type_string<char, 'f'>,
  std::atto, type_string<char, 'a'>
  //std::zepto, type_string<char, 'z'>,
  //std::yocto, type_string<char, 'y'>
>::sort<std::ratio_less>;

template <typename T>
using suffixes_impl_append_s = typename T::template push_back<'s'>;

} // namespace detail {

// TODO: DOCUMENT AND TEST
using suffixes = detail::ratio_suffixes::transform<
  detail::suffixes_impl_append_s
>::push_back<
  std::chrono::hours::period, type_string<char, 'h'>,
  std::chrono::minutes::period, type_string<char, 'm', 'i', 'n'>,
  std::chrono::seconds::period, type_string<char, 's'>
>::sort<std::ratio_less>;

// TODO: DOCUMENT AND TEST
template <typename TPeriod>
using suffix_t = typename suffixes::template get<TPeriod>;

// TODO: DOCUMENT AND TEST
template <typename T>
inline char const *suffix() {
  return suffix_t<typename T::period>::z_data();
}

// TODO: DOCUMENT AND TEST
template <typename T>
inline char const *suffix(T const &) {
  return suffix<T>();
}

} // namespace time {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_time_time_h
