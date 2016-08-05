/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_convert_h
#define FATAL_INCLUDE_fatal_type_impl_convert_h

namespace fatal {
namespace impl_cv {

template <template <typename V, V...> class, typename...> struct seq;

template <
  template <typename V, V...> class Sequence,
  template <typename...> class Variadics,
  typename... Args,
  typename T
>
struct seq<Sequence, Variadics<Args...>, T> {
  using type = Sequence<T, Args::value...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename...> class Variadics,
  typename Head,
  typename... Tail
>
struct seq<Sequence, Variadics<Head, Tail...>> {
  using type = Sequence<typename std::decay<decltype(Head::value)>::type, Head::value, Tail::value...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename V, V...> class Variadics,
  typename V,
  V... Args,
  typename T
>
struct seq<Sequence, Variadics<V, Args...>, T> {
  using type = Sequence<T, static_cast<T>(Args)...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename V, V...> class Variadics,
  typename T,
  T... Args
>
struct seq<Sequence, Variadics<T, Args...>> {
  using type = Sequence<T, Args...>;
};

template <template <typename...> class, typename> struct lst;

template <
  template <typename...> class List,
  template <typename V, V...> class Variadics,
  typename T,
  T... Args
>
struct lst<List, Variadics<T, Args...>> {
  using type = List<std::integral_constant<T, Args>...>;
};

template <
  template <typename...> class List,
  template <typename...> class Variadics,
  typename... Args
>
struct lst<List, Variadics<Args...>> {
  using type = List<Args...>;
};

template <typename, template <typename...> class...> struct mp;

template <
  template <typename...> class Variadics,
  typename... Args,
  template <typename...> class Map,
  template <typename...> class Pair,
  template <typename...> class Key,
  template <typename...> class Value
>
struct mp<Variadics<Args...>, Map, Pair, Key, Value> {
  using type = Map<Pair<Key<Args>, Value<Args>>...>;
};

template <typename...> struct toi;

template <typename To, template <typename...> class Variadics, typename... T>
struct toi<To, Variadics<T...>> {
  template <typename... Args>
  static constexpr To to(Args &&...args) {
    return To{T::value..., std::forward<Args>(args)...};
  }
};

template <
  typename To,
  template <typename V, V...> class Variadics,
  typename T,
  T... Values
>
struct toi<To, Variadics<T, Values...>> {
  template <typename... Args>
  static constexpr To to(Args &&...args) {
    return To{Values..., std::forward<Args>(args)...};
  }
};

} // namespace impl_cv {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_convert_h
