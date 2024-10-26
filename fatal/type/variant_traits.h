/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/type/apply.h>
#include <fatal/type/array.h>
#include <fatal/type/get.h>
#include <fatal/type/get_type.h>
#include <fatal/type/registry.h>
#include <fatal/type/search.h>
#include <fatal/type/slice.h>
#include <fatal/type/sort.h>
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
#define FATAL_REGISTER_VARIANT_TRAITS(Traits, ...) \
  FATAL_REGISTER_TYPE( \
    ::fatal::detail::variant_traits_impl::metadata_tag, \
    Traits::type, \
    ::fatal::curry<::fatal::applier<::fatal::list>, Traits>::apply< \
      __VA_ARGS__ \
    > \
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

template <typename, typename, typename> struct variant_traits_by;

// TODO: REMOVE name

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
class variant_traits {
  using impl = registry_lookup<detail::variant_traits_impl::metadata_tag, T>;
  using traits = at<impl, 0>;

public:
  using type = typename traits::type;
  using name = typename traits::name;
  using id = typename traits::id;

  using metadata = try_at<impl, 1, void>;

  using ids = typename traits::ids;

  using descriptors = typename traits::descriptors;

  using by_id = variant_traits_by<traits, descriptors, get_type::id>;
  using by_type = variant_traits_by<traits, descriptors, get_type::type>;

  struct array {
    /**
     * A statically allocated array containing the names of the variant members.
     *
     * See `type/array.h` for more info.
     *
     * Example:
     *
     *  using array = enum_traits<my_enum>::array::names;
     *
     *  // prints "field0 field1 field2 "
     *  for (auto s: array::data) {
     *    std::cout << s << ' ';
     *  }
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    using ids = as_array<
      transform<typename by_id::descriptors, get_type::id>,
      id
    >;
  };

  template <typename U>
  static id get_id(U &&variant) {
    return traits::get_id(std::forward<U>(variant));
  }

  // TODO: ONLY IF traits HAS empty()
  static bool empty(type const &variant) {
    return traits::empty(variant);
  }

  // TODO: ONLY IF traits HAS clear()
  static void clear(type &variant) {
    traits::clear(variant);
  }
};

/**
 * TODO: DOCUMENT
 *
 * NOTE: TODO: not supposed to be instantiated by the user
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Impl, typename Descriptors, typename KeyFilter>
struct variant_traits_by {
  // TODO: TEST
  template <typename Tag>
  using descriptor = get<Descriptors, Tag, KeyFilter>;

  template <typename Tag>
  using id = typename descriptor<Tag>::id;

  template <typename Tag>
  using type = typename descriptor<Tag>::type;

  template <typename Tag, typename U>
  static auto get(U &&variant)
    -> decltype(descriptor<Tag>::get(std::forward<U>(variant)))
  {
    return descriptor<Tag>::get(std::forward<U>(variant));
  }

  template <typename Tag, typename U, typename... Args>
  static void set(U &variant, Args &&...args) {
    descriptor<Tag>::set(variant, std::forward<Args>(args)...);
  }
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename T,
  typename Id,
  typename Getter,
  typename Setter,
  typename Metadata = void
>
struct variant_member_descriptor {
  using type = T;

  using id = Id;

  using getter = Getter;
  using setter = Setter;

  // TODO: TEST
  using metadata = Metadata;

  template <typename U>
  static auto get(U &&variant)
    -> decltype(getter()(std::forward<U>(variant)))
  {
    return getter()(std::forward<U>(variant));
  }

  template <typename U, typename... Args>
  static void set(U &variant, Args &&...args) {
    setter()(variant, std::forward<Args>(args)...);
  }
};

} // namespace fatal
