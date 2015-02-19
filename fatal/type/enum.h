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
#include <fatal/type/prefix_tree.h>
#include <fatal/type/string.h>

#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <cstring>

namespace fatal {

/**
 * Declares an enum named `Enum`, containing the given fields.
 *
 * To give an explicit value to a field, provide a tuple `(FieldName, Value)`.
 *
 * Example:
 *
 *  // yields `enum class my_enum { field0, field1, field2 };`
 *  FATAL_ENUM(my_enum, field0, field1, field2);
 *
 *  // yields `enum class my_enum { field0, field1 = 37, field2 };`
 *  FATAL_ENUM(my_enum, field0, (field1, 37), field2);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_ENUM(Enum, ...) \
  enum class Enum { \
    FATAL_MAP(FATAL_IMPL_ENUM_DECLARE_FIELD_ENTRY_POINT, ~, __VA_ARGS__) \
  }

/**
 * Declares an enum named `Enum` containing the given fields, as well as
 * a companion class that can be used for enum <-> string conversion and
 * template meta-programming / compile-time reflection.
 *
 * To give an explicit value to a field, provide a tuple `(FieldName, Value)`.
 * Refer to `FATAL_ENUM` above for more information.
 *
 * For more information on the companion class, refer to the documentation
 * for `FATAL_EXPORT_RICH_ENUM` below.
 *
 * Example:
 *
 *  // yields `enum class my_enum { field0, field1, field2 };`
 *  // and a companion class called `my_companion_class`
 *  FATAL_RICH_ENUM(my_companion_class, my_enum, field0, field1, field2);
 *
 *  // yields `enum class my_enum { field0, field1 = 37, field2 };`
 *  // and a companion class called `my_companion_class`
 *  FATAL_RICH_ENUM(my_companion_class, field0, (field1, 37), field2);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_RICH_ENUM(ClassName, Enum, ...) \
  FATAL_ENUM(Enum, __VA_ARGS__); \
  \
  FATAL_EXPORT_RICH_ENUM( \
    ClassName, \
    Enum \
    FATAL_SIMPLE_MAP(FATAL_IMPL_RICH_ENUM_EXTRACT_FIELD, __VA_ARGS__) \
  )

/**
 * Provides a companion class named `ClassName` for an existing
 * enum `Enum` that can be used for efficient enum <-> string
 * conversion and template meta-programming / compile-time reflection.
 *
 * Only the specified fields will be supported.
 *
 * Example:
 *
 *  enum class my_enum { field0, field1 = 37, field2 };
 *
 *  // generates the class `my_companion_class`, as specified below
 *  FATAL_EXPORT_RICH_ENUM(my_companion_class, my_enum, field0, field1, field2);
 *
 * Output of example:
 *
 *  struct my_companion_class {
 *    using type = my_enum;
 *
 *    struct str {
 *      FATAL_STR(field0, "field0");
 *      FATAL_STR(field1, "field1");
 *      FATAL_STR(field2, "field2");
 *    };
 *
 *    using name_to_value = fatal::build_type_map<
 *      str::field0, std::integral_constant<my_enum, my_enum::field0>,
 *      str::field1, std::integral_constant<my_enum, my_enum::field1>,
 *      str::field2, std::integral_constant<my_enum, my_enum::field2>
 *    >;
 *
 *    using names = name_to_value::keys;
 *    using values = name_to_value::mapped;
 *
 *    using value_to_name = name_to_value::invert<>;
 *
 *    using prefix_tree = fatal::build_type_prefix_tree<>::from<
 *      str::field0, str::field1, str::field2
 *    >;
 *
 *    // returns the string representation of the enum value given
 *    // by `e`, or `nullptr` when the given value is not supported
 *    static char const *to_str(my_enum e);
 *
 *    // parses the string given by the iterators `[begin, end)`
 *    // and returns the corresponding enum value.
 *    // throws `std::invalid_argument` if the given string is not
 *    // a valid enum field
 *    template <typename TBegin, typename TEnd>
 *    static my_enum parse(TBegin &&begin, TEnd &&end);
 *
 *    // equivalent to `parse(std::begin(s), std::end(s))`
 *    template <typename TString>
 *    static my_enum parse(TString const &s);
 *
 *    // parses the string given by the iterators `[begin, end)`
 *    // and sets `out` with the appropriate enum value.
 *    // returns `true` if successful or `false` otherwise
 *    // when `false` is returned, `out` is untouched.
 *    template <typename TBegin, typename TEnd>
 *    static bool try_parse(my_enum &out, TBegin &&begin, TEnd &&end);
 *
 *    // equivalent to `try_parse(out, std::begin(s), std::end(s))`
 *    template <typename TString>
 *    static bool try_parse(my_enum &out, TString const &s);
 *  };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_EXPORT_RICH_ENUM(ClassName, Enum, ...) \
  FATAL_IMPL_EXPORT_RICH_ENUM( \
    ClassName, \
    FATAL_UID(TString), \
    FATAL_UID(TBegin), \
    FATAL_UID(TEnd), \
    Enum, \
    __VA_ARGS__ \
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
    using names = typename name_to_value::keys; \
    using values = typename name_to_value::mapped; \
    \
    using value_to_name = typename name_to_value::template invert<>; \
    \
    using prefix_tree = names::apply< \
      ::fatal::build_type_prefix_tree<>::from \
    >; \
    \
  private: \
    struct parse_visitor_impl { \
      template <typename TString> \
      void operator ()(::fatal::type_tag<TString>, Enum &out) { \
        using value = typename name_to_value::template find<TString>; \
        \
        static_assert( \
          !::std::is_same<value, ::fatal::type_not_found_tag>::value, \
          "missing enum mapping" \
        ); \
        \
        out = value::value; \
      } \
    }; \
    \
  public: \
    static char const *to_str(Enum e) { \
      switch (e) { \
        default: return nullptr; \
        \
        FATAL_SIMPLE_MAP(FATAL_IMPL_EXPORT_RICH_ENUM_TO_STR, __VA_ARGS__) \
      } \
    } \
    \
    template <typename TBegin, typename TEnd> \
    static Enum parse(TBegin &&begin, TEnd &&end) { \
      Enum out; \
      \
      if (!prefix_tree::match<>::exact( \
        ::std::forward<TBegin>(begin), ::std::forward<TEnd>(end), \
        parse_visitor_impl{}, out \
      )) { \
        throw ::std::invalid_argument("unrecognized enum value"); \
      } \
      \
      return out; \
    } \
    \
    template <typename TString> \
    static Enum parse(TString const &s) { \
      return parse(::std::begin(s), ::std::end(s)); \
    } \
    \
    template <typename TBegin, typename TEnd> \
    static bool try_parse(Enum &out, TBegin &&begin, TEnd &&end) { \
      return prefix_tree::match<>::exact( \
        ::std::forward<TBegin>(begin), ::std::forward<TEnd>(end), \
        parse_visitor_impl{}, out \
      ); \
    } \
    \
    template <typename TString> \
    static bool try_parse(Enum &out, TString const &s) { \
      return try_parse(out, ::std::begin(s), ::std::end(s)); \
    } \
  } \

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_enum_h
