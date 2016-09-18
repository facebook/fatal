/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_transform_h
#define FATAL_INCLUDE_fatal_type_impl_transform_h

namespace fatal {
namespace i_t {

template <typename, template <typename...> class...> struct t;

template <template <typename...> class Variadic, typename... Args>
struct t<Variadic<Args...>> {
  using type = Variadic<Args...>;
};

template <
  template <typename...> class Variadic, typename... Args,
  template <typename...> class T0
>
struct t<Variadic<Args...>, T0> {
  using type = Variadic<T0<Args>...>;
};

template <
  template <typename...> class Variadic, typename... Args,
  template <typename...> class T0,
  template <typename...> class T1
>
struct t<Variadic<Args...>, T0, T1> {
  using type = Variadic<T1<T0<Args>>...>;
};

template <
  template <typename...> class Variadic, typename... Args,
  template <typename...> class T0,
  template <typename...> class T1,
  template <typename...> class T2
>
struct t<Variadic<Args...>, T0, T1, T2> {
  using type = Variadic<T2<T1<T0<Args>>>...>;
};

template <
  template <typename...> class Variadic, typename... Args,
  template <typename...> class T0,
  template <typename...> class T1,
  template <typename...> class T2,
  template <typename...> class T3
>
struct t<Variadic<Args...>, T0, T1, T2, T3> {
  using type = Variadic<T3<T2<T1<T0<Args>>>>...>;
};

template <
  template <typename...> class Variadic, typename... Args,
  template <typename...> class T0,
  template <typename...> class T1,
  template <typename...> class T2,
  template <typename...> class T3,
  template <typename...> class T4,
  template <typename...> class... Tn
>
struct t<Variadic<Args...>, T0, T1, T2, T3, T4, Tn...>:
  t<Variadic<T4<T3<T2<T1<T0<Args>>>>>...>, Tn...>
{};

} // namespace i_t {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_transform_h
