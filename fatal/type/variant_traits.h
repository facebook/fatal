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

#include <type_traits>
#include <utility>

namespace fatal {
namespace detail {
namespace variant_traits_impl {
struct metadata_tag {};
} // namespace variant_traits_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * NOTE: this macro must be called from the same namespace of the variant (as
 * returned by `Traits::type`), or some other namespace that respects the C++
 * rules for Argument Dependent Lookup
 * (http://en.cppreference.com/w/cpp/language/adl).
 *
 * @author
 */
#define FATAL_REGISTER_VARIANT_TRAITS(Traits) \
  FATAL_REGISTER_TYPE( \
    ::fatal::detail::variant_traits_impl::metadata_tag, \
    Traits::type, \
    Traits \
  )

/**
 * Tells whether the given type has `variant_traits` support available.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `std::false_type`
 *  using result1 = has_variant_traits<foo>;
 *
 *  struct bar { ...implementation omitted... };
 *
 *  struct bar_traits {
 *    using type = bar;
 *    ...implementation omitted...
 *  };
 *
 *  FATAL_REGISTER_VARIANT_TRAITS(bar_traits);
 *
 *  // yields `std::true_type`
 *  using result2 = has_variant_traits<bar>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using has_variant_traits = std::integral_constant<
  bool,
  !std::is_same<
    try_registry_lookup<detail::variant_traits_impl::metadata_tag, T, void>,
    void
  >::value
>;

template <typename> class variant_traits_by;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
class variant_traits {
  using impl = registry_lookup<detail::variant_traits_impl::metadata_tag, T>;

public:
  using type = typename impl::type;
  using id = typename impl::id;

  using names = typename impl::names;
  using ids = typename impl::ids;

  using descriptors = typename impl::descriptors;

  using by_name = variant_traits_by<
    typename type_map_from<get_member_type::name>
      ::template list<descriptors>
  >;

  using by_id = variant_traits_by<
    typename type_map_from<get_member_type::id>
      ::template list<descriptors>
      ::template sort<>
  >;

  using by_type = variant_traits_by<
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

/**
 * TODO: DOCUMENT
 *
 * NOTE: TODO: not supposed to be instantiated by the user
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Map>
struct variant_traits_by {
  using map = Map;

  using tags = typename map::keys;

  template <typename Tag>
  using name = typename map::template get<Tag>::name;

  template <typename Tag>
  using id = typename map::template get<Tag>::id;

  template <typename Tag>
  using type = typename map::template get<Tag>::type;

  template <typename Tag, typename U>
  static auto get(U &&variant)
    -> decltype(
      typename map::template get<Tag>::getter()(std::forward<U>(variant))
    )
  {
    return typename map::template get<Tag>::getter()(std::forward<U>(variant));
  }

  template <typename Tag, typename U, typename... Args>
  static void set(U &variant, Args &&...args) {
    typename map::template get<Tag>::setter()(
      variant, std::forward<Args>(args)...
    );
  }
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
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
