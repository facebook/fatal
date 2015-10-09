/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_static_array_h
#define FATAL_INCLUDE_fatal_container_static_array_h

#include <array>
#include <type_traits>

namespace fatal {
namespace detail {
namespace static_array_impl {

template <typename T, template <typename...> class Factory, typename... Args>
struct array {
  using value_type = T;
  using type = std::array<value_type, sizeof...(Args)>;

  static type const get;
};

template <typename T, template <typename...> class Factory, typename... Args>
typename array<
  T, Factory, Args...
>::type const array<
  T, Factory, Args...
>::get{{ Factory<Args>()()... }};

template <typename...> struct head;
template <typename T, typename... Args>
struct head<T, Args...> {
  using type = T;
};

template <template <typename...> class, typename...> struct value_type;
template <template <typename...> class Factory>
struct value_type<Factory> {
  template <typename... Args>
  using apply = typename std::decay<
    decltype(Factory<typename head<Args...>::type>()())
  >::type;
};

template <template <typename...> class Factory, typename T>
struct value_type<Factory, T> {
  template <typename...>
  using apply = T;
};

template <typename T>
struct type_value_factory {
  constexpr decltype(T::value) operator ()() const { return T::value; }
};

template <typename T>
struct type_data_factory {
  constexpr decltype(T::data()) operator ()() const { return T::data(); }
};

template <typename T>
struct type_z_data_factory {
  constexpr decltype(T::z_data()) operator ()() const { return T::z_data(); }
};

} // namespace static_array_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT - T is the optional explicit type of the array values
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... T>
struct static_array {
  static_assert(
    sizeof...(T) <= 1,
    "static array elements can have only one type"
  );

  // TODO: PROVIDE A CONVENIENT WAY TO GET A static_array of string_view
  // without including that file

  /**
   * TODO: DOCUMENT - T is the optional explicit type of the array values
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Factory>
  struct with {
    /**
     * TODO: DOCUMENT - T is the optional explicit type of the array values
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <typename... Args>
    using apply = typename detail::static_array_impl::array<
      typename detail::static_array_impl::value_type<Factory, T...>
        ::template apply<Args...>,
      Factory,
      Args...
    >;
  };

  /**
   * TODO: DOCUMENT - T is the optional explicit type of the array values
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using value = typename with<detail::static_array_impl::type_value_factory>
    ::template apply<Args...>;

  /**
   * TODO: DOCUMENT - T is the optional explicit type of the array values
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using data = typename with<detail::static_array_impl::type_data_factory>
    ::template apply<Args...>;

  /**
   * TODO: DOCUMENT - T is the optional explicit type of the array values
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... Args>
  using z_data = typename with<detail::static_array_impl::type_z_data_factory>
    ::template apply<Args...>;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_static_array_h
