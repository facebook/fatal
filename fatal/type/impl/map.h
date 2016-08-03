/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_map_h
#define FATAL_INCLUDE_fatal_type_impl_map_h

namespace fatal {
namespace impl_mp {

template <typename...> struct mk;

template <template <typename...> class Map, typename... Pairs>
struct mk<Map<Pairs...>> {
  using type = Map<Pairs...>;
};

// TODO: OPTIMIZE
template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0,
  typename V0,
  typename... Args
>
struct mk<Map<Pairs...>, K0, V0, Args...>:
  mk<Map<Pairs..., pair<K0, V0>>, Args...>
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename... Args
>
struct mk<Map<Pairs...>, K0, V0, K1, V1, Args...>:
  mk<Map<Pairs..., pair<K0, V0>, pair<K1, V1>>, Args...>
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename... Args
>
struct mk<Map<Pairs...>, K0, V0, K1, V1, K2, V2, Args...>:
  mk<Map<Pairs..., pair<K0, V0>, pair<K1, V1>, pair<K2, V2>>, Args...>
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename... Args
>
struct mk<Map<Pairs...>, K0, V0, K1, V1, K2, V2, K3, V3, Args...>:
  mk<
    Map<Pairs..., pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>>,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename... Args
>
struct mk<Map<Pairs...>, K0, V0, K1, V1, K2, V2, K3, V3, K4, V4, Args...>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>
    >,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename K5, typename V5,
  typename... Args
>
struct mk<
  Map<Pairs...>,
  K0, V0, K1, V1, K2, V2, K3, V3, K4, V4, K5, V5,
  Args...
>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>,
      pair<K5, V5>
    >,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename K5, typename V5,
  typename K6, typename V6,
  typename... Args
>
struct mk<
  Map<Pairs...>,
  K0, V0, K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6,
  Args...
>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>,
      pair<K5, V5>, pair<K6, V6>
    >,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename K5, typename V5,
  typename K6, typename V6,
  typename K7, typename V7,
  typename... Args
>
struct mk<
  Map<Pairs...>,
  K0, V0, K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7,
  Args...
>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>,
      pair<K5, V5>, pair<K6, V6>, pair<K7, V7>
    >,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename K5, typename V5,
  typename K6, typename V6,
  typename K7, typename V7,
  typename K8, typename V8,
  typename... Args
>
struct mk<
  Map<Pairs...>,
  K0, V0, K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8,
  Args...
>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>,
      pair<K5, V5>, pair<K6, V6>, pair<K7, V7>, pair<K8, V8>
    >,
    Args...
  >
{};

template <
  template <typename...> class Map,
  typename... Pairs,
  typename K0, typename V0,
  typename K1, typename V1,
  typename K2, typename V2,
  typename K3, typename V3,
  typename K4, typename V4,
  typename K5, typename V5,
  typename K6, typename V6,
  typename K7, typename V7,
  typename K8, typename V8,
  typename K9, typename V9,
  typename... Args
>
struct mk<
  Map<Pairs...>,
  K0, V0, K1, V1, K2, V2, K3, V3, K4, V4,
  K5, V5, K6, V6, K7, V7, K8, V8, K9, V9,
  Args...
>:
  mk<
    Map<
      Pairs...,
      pair<K0, V0>, pair<K1, V1>, pair<K2, V2>, pair<K3, V3>, pair<K4, V4>,
      pair<K5, V5>, pair<K6, V6>, pair<K7, V7>, pair<K8, V8>, pair<K9, V9>
    >,
    Args...
  >
{};

} // namespace impl_mp {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_map_h
