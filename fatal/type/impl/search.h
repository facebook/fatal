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
    template <typename...> class Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + (Size / 2) < size<T>::value, "");
    using pivot = at<T, Offset + (Size / 2)>;
    return Comparer::template greater<Filter<pivot>>(needle)
      ? s<T, Offset, Size / 2>::template S<Comparer, Filter>(
        std::forward<Needle>(needle),
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      )
      : Comparer::template less<Filter<pivot>>(needle)
        ? s<T, (Offset + Size / 2) + 1, Size / 2 - !(Size & 1)>
          ::template S<Comparer, Filter>(
            std::forward<Needle>(needle),
            std::forward<Visitor>(visitor),
            std::forward<Args>(args)...
          )
        : (
          visitor(
            indexed<pivot, Offset + (Size / 2)>(),
            std::forward<Args>(args)...
          ), true
        );
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 0> {
  template <typename, template <typename...> class, typename... Args>
  static constexpr bool S(Args &&...) {
    return false;
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 1> {
  template <
    typename Comparer,
    template <typename...> class Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset < size<T>::value,  "");
    return Comparer::template equal<Filter<at<T, Offset>>>(needle) && (
      visitor(
        indexed<at<T, Offset>, Offset>(),
        std::forward<Args>(args)...
      ),
      true
    );
  }
};

template <typename T, std::size_t Offset>
struct s<T, Offset, 2> {
  template <
    typename Comparer,
    template <typename...> class Filter,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool S(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + 1 < size<T>::value,  "");
    return (
      Comparer::template equal<Filter<at<T, Offset>>>(needle) && (
        visitor(
          indexed<at<T, Offset>, Offset>(),
          std::forward<Args>(args)...
        ), true
      )
    ) || (
      Comparer::template equal<Filter<at<T, Offset + 1>>>(needle) && (
        visitor(
          indexed<at<T, Offset + 1>, Offset + 1>(),
          std::forward<Args>(args)...
        ), true
      )
    );
  }
};

} // namespace i_S {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_search_h
