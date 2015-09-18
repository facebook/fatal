/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_variant_traits_h
#define FATAL_INCLUDE_fatal_type_variant_traits_h


#include <fatal/type/fast_pass.h>
#include <fatal/type/map.h>
#include <fatal/type/registry.h>
#include <fatal/type/transform.h>

#include <utility>

namespace fatal {
namespace detail {
namespace variant_traits_impl {

struct metadata_tag {};

template <typename Map>
class by {
  using map = Map;

public:
  using tags = typename map::keys;

  template <typename Tag>
  using type = typename Map::template get<Tag>::type;

  template <typename Tag>
  using id = typename Map::template get<Tag>::id;

  template <typename Tag, typename U>
  static auto get(U &&variant)
    -> decltype(
      typename Map::template get<Tag>::getter()(std::forward<U>(variant))
    )
  {
    return typename Map::template get<Tag>::getter()(std::forward<U>(variant));
  }

  template <typename Tag, typename U, typename... Args>
  static void set(U &variant, Args &&...args) {
    typename Map::template get<Tag>::setter()(
      variant, std::forward<Args>(args)...
    );
  }
};

} // namespace variant_traits_impl {
} // namespace detail {

template <typename T>
class variant_traits {
  using impl = registry_lookup<detail::variant_traits_impl::metadata_tag, T>;

public:
  using type = typename impl::type;
  using id = typename impl::id;

  using descriptors = typename impl::descriptors;

  using by_name = detail::variant_traits_impl::by<
    typename type_map_from<get_member_type::name>
      ::template list<descriptors>
  >;

  using by_id = detail::variant_traits_impl::by<
    typename type_map_from<get_member_type::id>
      ::template list<descriptors>
      ::template sort<>
  >;

  using by_type = detail::variant_traits_impl::by<
    typename type_map_from<get_member_type::type>
      ::template list<descriptors>
  >;

  // TODO: ONLY IF impl HAS empty()
  static bool empty(fast_pass<type> variant) {
    return impl::empty(variant);
  }

  // TODO: ONLY IF impl HAS clear()
  static void clear(type &variant) {
    impl::clear(variant);
  }
};

#define FATAL_REGISTER_VARIANT_TRAITS(Traits) \
  FATAL_REGISTER_TYPE( \
    ::fatal::detail::variant_traits_impl::metadata_tag, \
    Traits::type, \
    Traits \
  )

template <
  typename T, typename Id, typename Name, typename Getter, typename Setter
>
struct variant_type_descriptor {
  using type = T;
  using id = Id;
  using name = Name;

  using getter = Getter;
  using setter = Setter;
};

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_variant_traits_h
