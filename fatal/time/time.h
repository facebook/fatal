/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_time_time_h
#define FATAL_INCLUDE_fatal_time_time_h

#include <fatal/type/deprecated/transform.h>
#include <fatal/type/deprecated/type_map.h>
#include <fatal/type/sequence.h>

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
  //std::yotta, constant_sequence<char, 'Y'>,
  //std::zetta, constant_sequence<char, 'Z'>,
  std::exa, constant_sequence<char, 'E'>,
  std::peta, constant_sequence<char, 'P'>,
  std::tera, constant_sequence<char, 'T'>,
  std::giga, constant_sequence<char, 'G'>,
  std::mega, constant_sequence<char, 'M'>,
  std::kilo, constant_sequence<char, 'k'>,
  std::hecto, constant_sequence<char, 'h'>,
  std::deca, constant_sequence<char, 'd', 'a'>,
  std::deci, constant_sequence<char, 'd'>,
  std::centi, constant_sequence<char, 'c'>,
  std::milli, constant_sequence<char, 'm'>,
  std::micro, constant_sequence<char, 'u'>,
  std::nano, constant_sequence<char, 'n'>,
  std::pico, constant_sequence<char, 'p'>,
  std::femto, constant_sequence<char, 'f'>,
  std::atto, constant_sequence<char, 'a'>
  //std::zepto, constant_sequence<char, 'z'>,
  //std::yocto, constant_sequence<char, 'y'>
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
  detail::week_ratio, constant_sequence<char, 'w', 'k'>,
  detail::day_ratio, constant_sequence<char, 'd'>,
  std::chrono::hours::period, constant_sequence<char, 'h'>,
  std::chrono::minutes::period, constant_sequence<char, 'm', 'i', 'n'>,
  std::chrono::seconds::period, constant_sequence<char, 's'>
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
