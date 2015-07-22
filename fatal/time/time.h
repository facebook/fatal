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
#include <ratio>
#include <type_traits>

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

using day_ratio = std::ratio_multiply<
  std::chrono::hours::period,
  std::ratio<24, 1>
>;

using week_ratio = std::ratio_multiply<day_ratio, std::ratio<7, 1>>;

} // namespace detail {

// TODO: DOCUMENT AND TEST
using suffixes = detail::ratio_suffixes::transform<
  detail::suffixes_impl_append_s
>::push_back<
  detail::week_ratio, type_string<char, 'w', 'k'>,
  detail::day_ratio, type_string<char, 'd'>,
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

namespace detail {

template <typename...> struct pretty;

template <typename T, typename... Args>
struct pretty<T, Args...> {
  template <typename Out, typename P, typename R>
  static void print(Out &out, std::chrono::duration<R, P> time) {
    auto const local = std::chrono::duration_cast<
      std::chrono::duration<R, T>
    >(time);

    auto const remaining = time - local;

    if (local.count()) {
      out << local.count() << suffix_t<T>::z_data();

      if (sizeof...(Args) && remaining.count()) {
        out << ' ';
      }
    }

    pretty<Args...>::print(out, remaining);
  }
};

template <>
struct pretty<> {
  template <typename Out, typename D>
  static void print(Out &out, D) {}
};

using pretty_print_ratios = type_list<
  week_ratio,
  day_ratio,
  std::chrono::hours::period,
  std::chrono::minutes::period,
  std::chrono::seconds::period,
  std::chrono::milliseconds::period,
  std::chrono::microseconds::period,
  std::chrono::nanoseconds::period
>::sort<std::ratio_greater>;

} // namespace detail {

template <typename Out, typename R, typename P>
Out &&pretty_print(Out &&out, std::chrono::duration<R, P> time) {
  using ratios = typename detail::pretty_print_ratios::reject<
    transform_alias<std::ratio_greater, P>::template apply
  >;
  static_assert(!ratios::empty, "unsupported duration");

  using impl = typename ratios::template apply<detail::pretty>;
  impl::print(out, time);

  return std::forward<Out>(out);
}

} // namespace time {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_time_time_h
