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

#include <fatal/type/inherit.h>
#include <fatal/type/pair.h>
#include <fatal/type/size.h>
#include <fatal/type/tag.h>

namespace fatal {
namespace impl_srch {

template <typename, template <typename> class> struct get;

template <
  template <typename...> class List,
  typename... Args,
  template <typename> class Filter
>
struct get<List<Args...>, Filter> {
  template <typename Key, typename Value>
  static Value find(pair<Key, Value>);

  template <typename Key>
  using type = decltype(find<Key>(inherit<pair<Filter<Args>, Args>...>()));
};

// TODO: OPTIMIZE COMPILE TIMES
// TODO: HIGHER LOG BASE OPTIMIZATION (3 or 4 should be enough)
// TODO: SWITCH CASE OPTIMIZATION

template <typename T, std::size_t Offset = 0, std::size_t Size = size<T>::value>
struct srt {
  template <
    template <typename> class Filter,
    template <typename> class Comparer,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool sorted_search(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + (Size / 2) < size<T>::value, "");
    using pivot = at<T, Offset + (Size / 2)>;
    return Comparer<Filter<pivot>>::greater(needle)
      ? srt<T, Offset, Size / 2>::template sorted_search<Filter, Comparer>(
        std::forward<Needle>(needle),
        std::forward<Visitor>(visitor),
        std::forward<Args>(args)...
      )
      : Comparer<Filter<pivot>>::less(needle)
        ? srt<T, (Offset + Size / 2) + 1, Size / 2 - !(Size & 1)>
          ::template sorted_search<Filter, Comparer>(
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
struct srt<T, Offset, 0> {
  template <
    template <typename> class,
    template <typename> class,
    typename... Args
  >
  static constexpr bool sorted_search(Args &&...) {
    return false;
  }
};

template <typename T, std::size_t Offset>
struct srt<T, Offset, 1> {
  template <
    template <typename> class Filter,
    template <typename> class Comparer,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool sorted_search(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset < size<T>::value,  "");
    return Comparer<Filter<at<T, Offset>>>::equal(needle) && (
      visitor(
        indexed<at<T, Offset>, Offset>(),
        std::forward<Args>(args)...
      ),
      true
    );
  }
};

template <typename T, std::size_t Offset>
struct srt<T, Offset, 2> {
  template <
    template <typename> class Filter,
    template <typename> class Comparer,
    typename Needle,
    typename Visitor,
    typename... Args
  >
  static constexpr bool sorted_search(
    Needle &&needle,
    Visitor &&visitor,
    Args &&...args
  ) {
    static_assert(Offset + 1 < size<T>::value,  "");
    return (
      Comparer<Filter<at<T, Offset>>>::equal(needle) && (
        visitor(
          indexed<at<T, Offset>, Offset>(),
          std::forward<Args>(args)...
        ), true
      )
    ) || (
      Comparer<Filter<at<T, Offset + 1>>>::equal(needle) && (
        visitor(
          indexed<at<T, Offset + 1>, Offset + 1>(),
          std::forward<Args>(args)...
        ), true
      )
    );
  }
};

template <typename...> struct fe;

template <>
struct fe<> {
  template <typename Visitor, typename... VArgs>
  static void foreach(Visitor &&, VArgs &&...) {}
};

template <typename T, typename... Args>
struct fe<T, Args...> {
  template <typename Visitor, typename... VArgs>
  static void foreach(Visitor &&visitor, VArgs &&...args) {
    visitor(args...);
    fe<Args...>::foreach(
      std::forward<Visitor>(visitor),
      std::forward<VArgs>(args)...
    );
  }
};

} // namespace impl_srch {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_search_h
