/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/preprocessor.h>
#include <fatal/string/string_view.h>
#include <fatal/type/apply.h>
#include <fatal/type/array.h>
#include <fatal/type/call_traits.h>
#include <fatal/type/get.h>
#include <fatal/type/get_type.h>
#include <fatal/type/list.h>
#include <fatal/type/push.h>
#include <fatal/type/registry.h>
#include <fatal/type/search.h>
#include <fatal/type/sequence.h>
#include <fatal/type/slice.h>
#include <fatal/type/traits.h>
#include <fatal/type/transform.h>
#include <fatal/type/trie.h>

#include <iterator>
#include <stdexcept>
#include <type_traits>

#include <cstring>

namespace fatal {
namespace detail {
namespace enum_impl {

struct metadata_tag {};

} // namespace enum_impl {
} // namespace detail {

/**
 * Tells whether the given type has `enum_traits` support available.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `std::false_type`
 *  using result1 = has_enum_traits<foo>;
 *
 *  FATAL_RICH_ENUM_CLASS(bar, field0, field1, field2);
 *
 *  // yields `std::true_type`
 *  using result2 = has_enum_traits<bar>;
 *
 *  enum class baz { field0, field1, field2 };
 *  FATAL_EXPORT_RICH_ENUM(baz, field0, field1, field2);
 *
 *  // yields `std::true_type`
 *  using result3 = has_enum_traits<baz>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Enum>
using has_enum_traits = std::integral_constant<
  bool,
  !std::is_same<
    try_registry_lookup<detail::enum_impl::metadata_tag, Enum, void>,
    void
  >::value
>;

/**
 * Provides additional functionality for enumerations like efficient
 * enum <-> string conversion, template meta-programming metadata and
 * compile-time reflection.
 *
 * Only enumerations exported with `FATAL_EXPORT_RICH_ENUM`, `FATAL_RICH_ENUM`
 * and `FATAL_RICH_ENUM_CLASS` are available to `enum_traits`.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  my_enum e = my_enum::field0;
 *
 *  // yields `"field0"`
 *  auto result = enum_traits<my_enum>::to_string(e);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Enum>
class enum_traits {
  static_assert(std::is_enum<Enum>::value, "enumeration expected");

  using impl = registry_lookup<detail::enum_impl::metadata_tag, Enum>;
  using traits = at<impl, 0>;

  static_assert(
    std::is_same<Enum, typename traits::type>::value,
    "enum type mismatch"
  );

public:
  /**
   * A type alias for the enumeration itself.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `my_enum`
   *  using result = enum_traits<my_enum>::type;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = Enum;

  /**
   * A compile-time string representing the name of this enum.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `sequence<char, 'm', 'y', '_', 'e', 'n', 'u', 'm'>`
   *  using result = enum_traits<my_enum>::name;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using name = typename traits::name;

  /**
   * The metadata, if any, that has been registered with this enum traits.
   *
   * When no metadata is registered, returns `void`.
   *
   * See `FATAL_REGISTER_ENUM_TRAITS`'s documentation for more information.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `void`
   *  using result1 = enum_traits<my_enum>::metadata;
   *
   *  struct my_metadata {
   *    // ...
   *  };
   *
   *  // assume there's a custom traits implementation called `my_traits`
   *  // for an enum called `my_enum_2`.
   *
   *  FATAL_REGISTER_ENUM_TRAITS(my_enum_traits, my_metadata);
   *
   *  // yields `my_metadata`
   *  using result2 = enum_traits<my_enum_2>::metadata;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using metadata = try_at<impl, 1, void>;

  /**
   * The underlying integral type of the enumeration.
   *
   * This is equivalent to `std::underlying_type`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `unsigned`
   *  using result = enum_traits<my_enum>::int_type;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using int_type = typename std::underlying_type<type>::type;

  /**
   * Provides compile-time representation of name and value for each
   * enumeration field.
   *
   * The members are named exactly like its respective enumeration field.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `sequence<char, 'f', 'i', 'e', 'l', 'd', '0'>`;
   *  using result = enum_traits<my_enum>::member::field0::name;
   *
   *  // yields `std::integral_constant<my_enum, my_enum::field1>`;
   *  using result = enum_traits<my_enum>::member::field1::value;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using member = typename traits::member;

  /**
   * A type list of compile-time representation of metadata for the enumeration
   * fields.
   *
   * This is basically a type list of all entries in `member`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `list<
   *  //   enum_traits<my_enum>::member::field0,
   *  //   enum_traits<my_enum>::member::field1,
   *  //   enum_traits<my_enum>::member::field2
   *  // >`
   *  using result = enum_traits<my_enum>::fields;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using fields = typename traits::fields;

  /**
   * Gets the name associated with the field with the given `Value`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *  using traits = enum_traits<my_enum>;
   *
   *  // yields `fatal::sequence<char, 'f', 'i', 'e', 'l', 'd', '0'>`
   *  using result = traits::name_of<traits::member::field0::value::value>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type Value>
  using name_of = typename get<
    fields,
    std::integral_constant<type, Value>,
    get_type::value
  >::name;

  /**
   * Gets the value associated with the field with the given `Name`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *  using traits = enum_traits<my_enum>;
   *
   *  // yields `std::integral_constant<my_enum, my_enum::field0>`
   *  using result = traits::value_of<traits::member::field0::name>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Name>
  using value_of = typename get<fields, Name, get_type::name>::value;

private:
  struct parser {
    template <typename Field>
    void operator ()(tag<Field>, Enum &out) {
      out = Field::value::value;
    }
  };

  struct to_string_visitor {
    template <typename Field, std::size_t Index>
    void operator ()(indexed<Field, Index>, char const *&out) const {
      out = z_data<typename Field::name>();
    }
  };

  struct to_string_fallback {
    char const *operator ()(type e, char const *fallback) const {
      scalar_search<fields, get_type::value>(e, to_string_visitor(), fallback);

      return fallback;
    }
  };

public:
  /**
   * Tells if the given value is a valid value for this enum.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  my_enum out;
   *
   *  // returns `true` and sets `out` to `my_enum::field0`
   *  bool result1 = enum_traits<my_enum>::is_valid(my_enum::field0);
   *
   *  // returns `false` and leaves `out` untouched
   *  bool result2 = enum_traits<my_enum>::is_valid(
   *    static_cast<my_enum>(99999)
   *  );
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr bool is_valid(type e) {
    return scalar_search<fields, get_type::value>(e);
  }

  /**
   * Returns a non-owning pointer to the statically allocated string
   * representation of the enumeration value given, or `fallback` when
   * the given value is not supported.
   *
   * The default value for `fallback` is `nullptr`.
   *
   * See also `enum_to_string()` for a convenient shortcut.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `"field0"`
   *  auto result = enum_traits<my_enum>::to_string(my_enum::field0);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static char const *to_string(type e, char const *fallback = nullptr) {
    using caller = call_traits::to_string::static_member::bind<traits>;
    return call_if_supported<caller, to_string_fallback>(e, fallback);
  }

  /**
   * Parses the string given by the iterators `[begin, end)`
   * and returns the corresponding enumeration value.
   *
   * Throws `std::invalid_argument` if the given string is not
   * a valid enumeration field.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  std::string f1("field0");
   *  std::string f2("yolo");
   *
   *  // returns `my_enum::field0`
   *  auto result1 = enum_traits<my_enum>::parse(f1.begin(), f1.end());
   *
   *  // throws `std::invalid_argument`
   *  auto result2 = enum_traits<my_enum>::parse(f2.begin(), f2.end());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TBegin, typename TEnd>
  static type parse(TBegin &&begin, TEnd &&end) {
    type out;

    if (!trie_find<fields, get_type::name>(
      static_cast<TBegin &&>(begin), static_cast<TEnd &&>(end), parser(), out
    )) {
      throw std::invalid_argument("unrecognized enum value");
    }

    return out;
  }

  /**
   * Equivalent to `parse(std::begin(s), std::end(s))`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // returns `my_enum::field0`
   *  auto result1 = enum_traits<my_enum>::parse("field0");
   *
   *  // throws `std::invalid_argument`
   *  auto result2 = enum_traits<my_enum>::parse("yolo");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TString>
  static type parse(TString const &s) {
    return parse(std::begin(s), std::end(s));
  }

  /**
   * Parses the string given by the iterators `[begin, end)`.
   * and sets `out` with the appropriate enumeration value.
   *
   * Returns `true` if successful or `false` otherwise.
   * When `false` is returned, `out` remains untouched.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  my_enum out;
   *  std::string f1("field0");
   *  std::string f2("yolo");
   *
   *  // returns `true` and sets `out` to `my_enum::field0`
   *  bool result1 = enum_traits<my_enum>::try_parse(out, f1.begin(), f1.end());
   *
   *  // returns `false` and leaves `out` untouched
   *  bool result2 = enum_traits<my_enum>::try_parse(out, f2.begin(), f2.end());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TBegin, typename TEnd>
  static constexpr bool try_parse(type &out, TBegin &&begin, TEnd &&end) {
    return trie_find<fields, get_type::name>(
      static_cast<TBegin &&>(begin), static_cast<TEnd &&>(end), parser(), out
    );
  }

  /**
   * Equivalent to `try_parse(out, std::begin(s), std::end(s))`.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  my_enum out;
   *
   *  // returns `true` and sets `out` to `my_enum::field0`
   *  bool result1 = enum_traits<my_enum>::try_parse(out, "field0");
   *
   *  // returns `false` and leaves `out` untouched
   *  bool result2 = enum_traits<my_enum>::try_parse(out, "yolo");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TString>
  static constexpr bool try_parse(type &out, TString const &s) {
    return try_parse(out, std::begin(s), std::end(s));
  }
};

/**
 * A convenient shortcut to `enum_traits::is_valid()`.
 *
 * Tells if the given value is a valid value for this enum.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  // yields `"field0"`
 *  auto result = enum_to_string(my_enum::field0);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Enum>
static constexpr bool is_valid_enum(Enum e) {
  return enum_traits<typename std::decay<Enum>::type>::is_valid(e);
}

/**
 * A convenient shortcut to `enum_traits::to_string()`.
 *
 * Returns a non-owning pointer to the statically allocated string
 * representation of the enumeration value given, or `fallback` when
 * the given value is not supported.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  // yields `"field0"`
 *  auto result = enum_to_string(my_enum::field0, nullptr);
 *
 *  // yields `"field0"`
 *  auto result = enum_to_string(my_enum::field0, "<unknown>");
 *
 *  // yields `nullptr`
 *  auto result = enum_to_string(my_enum(-1), nullptr);
 *
 *  // yields `"<unknown>"`
 *  auto result = enum_to_string(my_enum(-1), "<unknown>");
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Enum>
static constexpr char const *enum_to_string(
  Enum const e,
  char const *fallback
) {
  return enum_traits<typename std::decay<Enum>::type>::to_string(e, fallback);
}

/**
 * A convenient shortcut to `enum_traits::to_string()`.
 *
 * Returns a non-owning pointer to the statically allocated string
 * representation of the enumeration value given, or `nullptr` when
 * the given value is not supported. Equivalent to `enum_to_string`
 * with a `fallback` argument of `nullptr`.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  // yields `"field0"`
 *  auto result = try_enum_to_string(my_enum::field0);
 *
 *  // yields `nullptr`
 *  auto result = try_enum_to_string(my_enum(-1));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Enum>
static constexpr char const *try_enum_to_string(Enum const e) {
  return enum_traits<typename std::decay<Enum>::type>::to_string(e, nullptr);
}

/**
 * A statically allocated array containing the names of the enumeration
 * fields as `fatal::string_view`.
 *
 * See `type/array.h` and `string/string_view.h` for more info.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  using array = enum_names_array<my_enum>;
 *
 *  // yields `3`
 *  auto const size = array::size::value;
 *
 *  // prints "field0(6) field1(6) field2(6) "
 *  for (auto s: array::data) {
 *    std::cout << s << '[' << s.size() << "] ";
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using enum_names_array = string_view_array<
  fatal::transform<typename enum_traits<T>::fields, get_type::name>,
  string_view
>;

/**
 * A statically allocated array containing the values of the enumeration
 * fields.
 *
 * See `type/array.h` for more info.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  using traits = enum_traits<my_enum>;
 *  using array = enum_values_array<my_enum>;
 *
 *  // yields `3`
 *  auto const size = array::size::value;
 *
 *  // prints "0 1 2 "
 *  for (auto i: array::data) {
 *    std::cout << static_cast<traits::int_type>(i) << ' ';
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using enum_values_array = as_array<
  fatal::transform<typename enum_traits<T>::fields, get_type::value>
>;

/**
 * Declares an `enum` named `Enum`, containing the given fields.
 *
 * To give an explicit value to a field, provide a tuple `(FieldName, Value)`.
 *
 * Example:
 *
 *  // yields `enum my_enum { field0, field1, field2 };`
 *  FATAL_ENUM(my_enum, field0, field1, field2);
 *
 *  // yields `enum my_enum { field0, field1 = 37, field2 };`
 *  FATAL_ENUM(my_enum, field0, (field1, 37), field2);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_ENUM(Enum, ...) \
  enum Enum { \
    FATAL_MAP(FATAL_IMPL_ENUM_DECLARE_FIELD_ENTRY_POINT, ~, __VA_ARGS__) \
  }

/**
 * The same as `FATAL_ENUM`, but declares an `enum class` instead of an `enum`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_ENUM_CLASS(Enum, ...) \
  enum class Enum { \
    FATAL_MAP(FATAL_IMPL_ENUM_DECLARE_FIELD_ENTRY_POINT, ~, __VA_ARGS__) \
  }

/**
 * Declares an `enum` named `Enum` containing the given fields. It also
 * adds support for this enumeration to `enum_traits`.
 *
 * To give an explicit value to a field, provide a tuple `(FieldName, Value)`.
 * Refer to `FATAL_ENUM` above for more information.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM(my_enum, field0, field1, field2);
 *
 *  my_enum e = my_enum::field0;
 *
 *  // yields `"field0"`
 *  auto result = enum_traits<my_enum>::to_string(e);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_RICH_ENUM(Enum, ...) \
  FATAL_ENUM(Enum, __VA_ARGS__); \
  \
  FATAL_IMPL_EXPORT_RICH_ENUM_CALL( \
    Enum \
    FATAL_SIMPLE_MAP(FATAL_IMPL_RICH_ENUM_EXTRACT_FIELD, __VA_ARGS__) \
  )

/**
 * The same as `FATAL_RICH_ENUM`, but declares
 * an `enum class` instead of an `enum`.
 *
 * Example:
 *
 *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
 *
 *  my_enum e = my_enum::field0;
 *
 *  // yields `"field0"`
 *  auto result = enum_traits<my_enum>::to_string(e);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_RICH_ENUM_CLASS(Enum, ...) \
  FATAL_ENUM_CLASS(Enum, __VA_ARGS__); \
  \
  FATAL_IMPL_EXPORT_RICH_ENUM_CALL( \
    Enum \
    FATAL_SIMPLE_MAP(FATAL_IMPL_RICH_ENUM_EXTRACT_FIELD, __VA_ARGS__) \
  )

/**
 * Adds `enum_traits` support for an existing enum `Enum` to `enum_traits`.
 *
 * Only the specified fields will be supported.
 *
 * Example:
 *
 *  enum class my_enum { field0, field1 = 37, field2 };
 *
 *  FATAL_EXPORT_RICH_ENUM(my_enum, field0, field1, field2);
 *
 *  // yields `"field0"`
 *  auto result = enum_traits<my_enum>::to_string(my_enum::field0);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_EXPORT_RICH_ENUM(Enum, ...) \
  FATAL_IMPL_EXPORT_RICH_ENUM( \
    FATAL_UID(FATAL_CAT(enum_metadata_impl_, Enum)), \
    FATAL_UID(TString), \
    FATAL_UID(TBegin), \
    FATAL_UID(TEnd), \
    Enum, \
    __VA_ARGS__ \
  )

/**
 * TODO: DOCUMENT
 *
 * NOTE: this macro must be called from the same namespace of the enum (as
 * returned by `Traits::type`), or some other namespace that respects the C++
 * rules for Argument Dependent Lookup
 * (http://en.cppreference.com/w/cpp/language/adl).
 *
 * Example:
 *
 *  enum class my_enum { field0, field1 = 37, field2 };
 *
 *  struct my_traits {
 *    using type = my_enum;
 *
 *    // see FATAL_S
 *    using name = sequence<char, 'm', 'y', '_', 'e', 'n', 'u', 'm'>;
 *
 *    struct member {
 *      struct field0 {
 *        FATAL_S(name, "field0");
 *        using value = std::integral_constant<type, type::field0>;
 *      };
 *      struct field1 {
 *        FATAL_S(name, "field1");
 *        using value = std::integral_constant<type, type::field1>;
 *      };
 *      struct field2 {
 *        FATAL_S(name, "field2");
 *        using value = std::integral_constant<type, type::field2>;
 *      };
 *    };
 *
 *    using fields = list<member::field0, member::field1, member::field2>;
 *
 *    // this function is optional but its presence greatly
 *    // improves build times and runtime performance
 *    static char const *to_string(type e, char const *fallback) {
 *      switch (e) {
 *        case type::field0: return "field0";
 *        case type::field1: return "field1";
 *        case type::field2: return "field2";
 *        default: return fallback;
 *      }
 *    }
 *  };
 *
 *  // adds my_enum support to enum_traits via my_traits
 *  FATAL_REGISTER_ENUM_TRAITS(my_traits);
 *
 *  // yields `my_enum::field0`
 *  auto const value = enum_traits::parse("field0");
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_REGISTER_ENUM_TRAITS(Traits, ...) \
  FATAL_REGISTER_TYPE( \
    ::fatal::detail::enum_impl::metadata_tag, \
    Traits::type, \
    ::fatal::push<::fatal::list<Traits>>::back<__VA_ARGS__> \
  )

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

#define FATAL_IMPL_ENUM_DECLARE_FIELD_ENTRY_POINT(Arg, IsFirst, Index, ...) \
  FATAL_CONDITIONAL(IsFirst)()(,) \
  FATAL_IMPL_ENUM_DECLARE_FIELD(FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_IMPL_ENUM_DECLARE_FIELD(...) \
  FATAL_ARG_1(__VA_ARGS__) \
  FATAL_CONDITIONAL(FATAL_HAS_ARGS(FATAL_SKIP_1_ARG(__VA_ARGS__)))( \
    = FATAL_DEFER_2(FATAL_ARG_2)(__VA_ARGS__) \
  )()

#define FATAL_IMPL_RICH_ENUM_EXTRACT_FIELD(...) \
  , FATAL_DEFER_1(FATAL_ARG_1)(FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_IMPL_EXPORT_RICH_ENUM_CALL(...) \
  FATAL_EXPORT_RICH_ENUM(__VA_ARGS__)

#define FATAL_IMPL_EXPORT_RICH_ENUM_MEMBER(...) \
  FATAL_CONDITIONAL(IsFirst)()(,) struct __VA_ARGS__ { \
    FATAL_S(name, FATAL_TO_STR(__VA_ARGS__)); \
    using value = std::integral_constant<type, type::__VA_ARGS__>; \
  };


#define FATAL_IMPL_EXPORT_RICH_ENUM_FIELDS(Arg, IsFirst, Index, ...) \
  FATAL_CONDITIONAL(IsFirst)()(,) member::__VA_ARGS__

#define FATAL_IMPL_EXPORT_RICH_ENUM_TO_STR(...) \
  case type::__VA_ARGS__: return FATAL_TO_STR(__VA_ARGS__);

#define FATAL_IMPL_EXPORT_RICH_ENUM( \
  ClassName, TString, TBegin, TEnd, Enum, ... \
) \
  struct ClassName { \
    using type = Enum; \
    \
    FATAL_S(name, FATAL_TO_STR(Enum)); \
    \
    struct member { \
      FATAL_SIMPLE_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_MEMBER, __VA_ARGS__) \
    }; \
    \
    using fields = ::fatal::list< \
      FATAL_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_FIELDS, ~, __VA_ARGS__) \
    >; \
    \
    static char const *to_string(type e, char const *fallback) { \
      switch (e) { \
        FATAL_SIMPLE_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_TO_STR, __VA_ARGS__) \
        default: return fallback; \
      } \
    } \
  }; \
  \
  FATAL_REGISTER_ENUM_TRAITS(ClassName)

} // namespace fatal {
