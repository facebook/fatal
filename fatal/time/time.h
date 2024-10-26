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
#include <fatal/type/array.h>
#include <fatal/type/get.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>
#include <fatal/type/sort.h>

#include <chrono>
#include <ratio>
#include <type_traits>
#include <utility>

namespace fatal {
namespace time {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

namespace detail {

template <typename Ratio, char... Suffix>
using t_s = pair<Ratio, char_sequence<Suffix...>>;

} // namespace detail {

// TODO: MOVE TO MATH
using suffixes = list<
  //detail::t_s<std::yocto, 'y', 's'>,
  //detail::t_s<std::zepto, 'z', 's'>,
  detail::t_s<std::atto, 'a', 's'>,
  detail::t_s<std::femto, 'f', 's'>,
  detail::t_s<std::pico, 'p', 's'>,
  detail::t_s<std::nano, 'n', 's'>,
  detail::t_s<std::micro, 'u', 's'>,
  detail::t_s<std::milli, 'm', 's'>,
  detail::t_s<std::centi, 'c', 's'>,
  detail::t_s<std::deci, 'd', 's'>,
  detail::t_s<std::chrono::seconds::period, 's'>,
  detail::t_s<std::deca, 'd', 'a', 's'>,
  detail::t_s<std::chrono::minutes::period, 'm', 'i', 'n'>,
  detail::t_s<std::hecto, 'h', 's'>,
  detail::t_s<std::kilo, 'k', 's'>,
  detail::t_s<std::chrono::hours::period, 'h'>,
  detail::t_s<
    std::ratio_multiply<std::chrono::hours::period, std::ratio<24, 1>>, 'd'
  >,
  detail::t_s<
    std::ratio_multiply<std::chrono::hours::period, std::ratio<24 * 7, 1>>,
    'w', 'k'
  >,
  detail::t_s<std::mega, 'M', 's'>,
  detail::t_s<std::giga, 'G', 's'>,
  detail::t_s<std::tera, 'T', 's'>,
  detail::t_s<std::peta, 'P', 's'>,
  detail::t_s<std::exa, 'E', 's'>
  //detail::t_s<std::zetta, 'Z', 's'>,
  //detail::t_s<std::yotta, 'Y', 's'>
>;

// TODO: DOCUMENT AND TEST
template <typename TPeriod>
using suffix_t = second<get<suffixes, TPeriod>>;

// TODO: DOCUMENT AND TEST
template <typename T>
inline char const *suffix() {
  return z_data<suffix_t<typename T::period>>();
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
      out << local.count() << z_data<suffix_t<T>>();

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
  static void print(Out &, D) {}
};

using pretty_print_ratios = list<
  std::ratio_multiply<std::chrono::hours::period, std::ratio<24 * 7, 1>>,
  std::ratio_multiply<std::chrono::hours::period, std::ratio<24, 1>>,
  std::chrono::hours::period,
  std::chrono::minutes::period,
  std::chrono::seconds::period,
  std::chrono::milliseconds::period,
  std::chrono::microseconds::period,
  std::chrono::nanoseconds::period
>;

} // namespace detail {

template <typename Out, typename R, typename P>
Out &&pretty_print(Out &&out, std::chrono::duration<R, P> time) {
  using ratios = reject<
    typename detail::pretty_print_ratios, curry<applier<std::ratio_greater>, P>
  >;
  static_assert(!empty<ratios>::value, "unsupported duration");

  using impl = apply_to<ratios, detail::pretty>;
  impl::print(out, time);

  return std::forward<Out>(out);
}

} // namespace time {
} // namespace fatal {
