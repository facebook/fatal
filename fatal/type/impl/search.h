/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_search_h
#define FATAL_INCLUDE_fatal_type_impl_search_h

#include <fatal/type/size.h>
#include <fatal/type/slice.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace i_S {

// TODO: OPTIMIZE COMPILE TIMES
// TODO: HIGHER LOG BASE OPTIMIZATION (3 or 4 should be enough)
// TODO: SWITCH CASE OPTIMIZATION

template <typename T, std::size_t Offset = 0, std::size_t Size = size<T>::value>
struct s {
  template <
    typename Comparer,
    typename Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr inline bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + (Size / 2) < size<T>::value, "");
    using pivot = at<T, Offset + (Size / 2)>;
    using filtered = typename Filter::template apply<pivot>;
    return Comparer::template greater<filtered>(needle)
      ? s<T, Offset, Size / 2>::template S<Comparer, Filter>(
        static_cast<Needle &&>(needle),
        static_cast<Visitor &&>(visitor),
        static_cast<Args &&>(args)...
      )
      : Comparer::template less<filtered>(needle)
        ? s<T, (Offset + Size / 2) + 1, Size / 2 - !(Size & 1)>
          ::template S<Comparer, Filter>(
            static_cast<Needle &&>(needle),
            static_cast<Visitor &&>(visitor),
            static_cast<Args &&>(args)...
          )
        : (
          visitor(
            indexed<pivot, Offset + (Size / 2)>(),
            static_cast<Args &&>(args)...
          ), true
        );
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 0> {
  template <typename, typename, typename... Args>
  static constexpr inline bool S(Args &&...) {
    return false;
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 1> {
  template <
    typename Comparer,
    typename Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr inline bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset < size<T>::value,  "");
    using filtered = typename Filter::template apply<at<T, Offset>>;
    return Comparer::template equal<filtered>(needle) && (
      visitor(
        indexed<at<T, Offset>, Offset>(),
        static_cast<Args &&>(args)...
      ),
      true
    );
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 2> {
  template <
    typename Comparer,
    typename Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr inline bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + 1 < size<T>::value,  "");
    using filtered = typename Filter::template apply<at<T, Offset>>;
    using filtered_next = typename Filter::template apply<at<T, Offset + 1>>;
    return (
      Comparer::template equal<filtered>(needle) && (
        visitor(
          indexed<at<T, Offset>, Offset>(),
          static_cast<Args &&>(args)...
        ), true
      )
    ) || (
      Comparer::template equal<filtered_next>(needle) && (
        visitor(
          indexed<at<T, Offset + 1>, Offset + 1>(),
          static_cast<Args &&>(args)...
        ), true
      )
    );
  }
};

} // namespace i_S {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_search_h
