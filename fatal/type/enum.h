/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_enum_h
#define FATAL_INCLUDE_fatal_type_enum_h

#include <fatal/preprocessor.h>
#include <fatal/type/call_traits.h>
#include <fatal/type/prefix_tree.h>
#include <fatal/type/registry.h>
#include <fatal/type/sequence.h>
#include <fatal/type/traits.h>

#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <cstring>

namespace fatal {
namespace detail {
namespace enum_impl {

struct metadata_tag {};

} // namespace enum_impl {
} // namespace detail {

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

  static_assert(
    std::is_same<Enum, typename impl::type>::value,
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
   * Provides type strings representing the name of each enumeration field.
   *
   * The members are named exactly like its respective enumeration field.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `constant_sequence<char, 'f', 'i', 'e', 'l', 'd', '0'>;
   *  using result = enum_traits<my_enum>::str::field0;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using str = typename impl::str;

  /**
   * A `type_map` from name to value for each known enumeration field.
   *
   * Field names are represented as `constant_sequence`, while field values are
   * represented as a `std::integral_constant` of the given enumeration.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `std::integral_constant<my_enum, my_enum::field0>`
   *  using result = enum_traits<my_enum>::name_to_value::get<
   *    enum_traits<my_enum>::str::field0
   *  >;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using name_to_value = typename impl::name_to_value;

  /**
   * A `type_map` from value to name for each known enumeration field.
   *
   * Field names are represented as `constant_sequence`, while field values are
   * represented as a `std::integral_constant` of the given enumeration.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `enum_traits<my_enum>::str::field0`
   *  using result = enum_traits<my_enum>::value_to_name::get<
   *    std::integral_constant<my_enum, my_enum::field0>
   *  >;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using value_to_name = typename name_to_value::template invert<>;

  /**
   * A `type_list` of type strings for the names of each known
   * enumeration fields.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `type_list<
   *  //   enum_traits<my_enum>::str::field0,
   *  //   enum_traits<my_enum>::str::field1,
   *  //   enum_traits<my_enum>::str::field2
   *  // >`
   *  using result = enum_traits<my_enum>::names;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using names = typename name_to_value::keys;

  /**
   * A `type_list` of `std::integral_constant` for each of the
   * known enumeration values.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `type_list<
   *  //   std::integral_constant<my_enum, my_enum::field0>,
   *  //   std::integral_constant<my_enum, my_enum::field1>,
   *  //   std::integral_constant<my_enum, my_enum::field2>
   *  // >`
   *  using result = enum_traits<my_enum>::values;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using values = typename name_to_value::mapped;

  /**
   * A `type_prefix_tree` containing the names of the enumeration fields.
   *
   * Example:
   *
   *  FATAL_RICH_ENUM_CLASS(my_enum, field0, field1, field2);
   *
   *  // yields `build_type_prefix_tree::from<
   *  //   enum_traits<my_enum>::str::field0,
   *  //   enum_traits<my_enum>::str::field1,
   *  //   enum_traits<my_enum>::str::field2
   *  // >`
   *  using result = enum_traits<my_enum>::prefix_tree;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using prefix_tree = typename names::template apply<
    fatal::build_type_prefix_tree<>::from
  >;

private:
  struct parser {
    template <typename TString>
    void operator ()(type_tag<TString>, Enum &out) {
      out = name_to_value::template get<TString>::value;
    }
  };

  struct to_string_visitor {
    template <typename Value, typename Name, std::size_t Index>
    void operator ()(
      indexed_type_pair_tag<Value, Name, Index>, type, char const *&out
    ) const {
      out = Name::z_data();
    }
  };

  struct to_string_fallback {
    char const *operator ()(type e, char const *fallback) const {
      value_to_name::template sort<>::template binary_search<>::exact(
        e, to_string_visitor(), fallback
      );

      return fallback;
    }
  };

public:
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
    using caller = call_traits::to_string::static_member::bind<impl>;
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

    if (!prefix_tree::template match<>::exact(
      std::forward<TBegin>(begin), std::forward<TEnd>(end), parser(), out
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
  static bool try_parse(type &out, TBegin &&begin, TEnd &&end) {
    return prefix_tree::template match<>::exact(
      std::forward<TBegin>(begin), std::forward<TEnd>(end), parser(), out
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
  static bool try_parse(type &out, TString const &s) {
    return try_parse(out, std::begin(s), std::end(s));
  }
};

/**
 * A convenient shortcut to `enum_traits::to_string()`.
 *
 * Returns a non-owning pointer to the statically allocated string
 * representation of the enumeration value given, or `fallback` when
 * the given value is not supported.
 *
 * The default value for `fallback` is `nullptr`.
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
char const *enum_to_string(Enum e, char const *fallback = nullptr) {
  return enum_traits<typename std::decay<Enum>::type>::to_string(e, fallback);
}

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
 *    struct str {
 *      FATAL_STR(field0, "field0");
 *      FATAL_STR(field1, "field1");
 *      FATAL_STR(field2, "field2");
 *    };
 *
 *    using name_to_value = build_type_map<
 *      str::field0, std::integral_constant<type, type::field0>,
 *      str::field1, std::integral_constant<type, type::field1>,
 *      str::field2, std::integral_constant<type, type::field2>
 *    >;
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
#define FATAL_REGISTER_ENUM_TRAITS(Traits) \
  FATAL_REGISTER_TYPE( \
    ::fatal::detail::enum_impl::metadata_tag, \
    Traits::type, \
    Traits \
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

#define FATAL_IMPL_EXPORT_RICH_ENUM_STR(...) \
  FATAL_STR(__VA_ARGS__, FATAL_TO_STR(__VA_ARGS__));

#define FATAL_IMPL_EXPORT_RICH_ENUM_STR_VALUE_LIST(Arg, IsFirst, Index, ...) \
  FATAL_CONDITIONAL(IsFirst)()(,) \
  str::__VA_ARGS__, ::std::integral_constant<type, type::__VA_ARGS__>

#define FATAL_IMPL_EXPORT_RICH_ENUM_TO_STR(...) \
  case type::__VA_ARGS__: return FATAL_TO_STR(__VA_ARGS__);

#define FATAL_IMPL_EXPORT_RICH_ENUM( \
  ClassName, TString, TBegin, TEnd, Enum, ... \
) \
  struct ClassName { \
    using type = Enum; \
    \
    struct str { \
      FATAL_SIMPLE_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_STR, __VA_ARGS__) \
    }; \
    \
    using name_to_value = ::fatal::build_type_map< \
      FATAL_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_STR_VALUE_LIST, ~, __VA_ARGS__) \
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

#endif // FATAL_INCLUDE_fatal_type_enum_h
