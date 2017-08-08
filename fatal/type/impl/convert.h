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

// as_sequence //
template <typename...> struct s;

template <
  template <typename V, V...> class Sequence,
  template <typename...> class Variadics,
  typename... Args,
  typename T
>
struct s<Sequence<int>, Variadics<Args...>, T> {
  using type = Sequence<T, Args::value...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename...> class Variadics,
  typename Head,
  typename... Tail
>
struct s<Sequence<int>, Variadics<Head, Tail...>> {
  using type = Sequence<typename std::decay<decltype(Head::value)>::type, Head::value, Tail::value...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename V, V...> class Variadics,
  typename V,
  V... Args,
  typename T
>
struct s<Sequence<int>, Variadics<V, Args...>, T> {
  using type = Sequence<T, static_cast<T>(Args)...>;
};

template <
  template <typename V, V...> class Sequence,
  template <typename V, V...> class Variadics,
  typename T,
  T... Args
>
struct s<Sequence<int>, Variadics<T, Args...>> {
  using type = Sequence<T, Args...>;
};

// as_list //
template <template <typename...> class, typename> struct l;

template <
  template <typename...> class List,
  template <typename V, V...> class Variadics,
  typename T,
  T... Args
>
struct l<List, Variadics<T, Args...>> {
  using type = List<std::integral_constant<T, Args>...>;
};

template <
  template <typename...> class List,
  template <typename...> class Variadics,
  typename... Args
>
struct l<List, Variadics<Args...>> {
  using type = List<Args...>;
};

// to_instance //
template <typename...> struct t;

template <typename To, template <typename...> class Variadics, typename... T>
struct t<To, Variadics<T...>> {
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
struct t<To, Variadics<T, Values...>> {
  template <typename... Args>
  static constexpr To to(Args &&...args) {
    return To{Values..., std::forward<Args>(args)...};
  }
};

} // namespace impl_cv {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_convert_h
