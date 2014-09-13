/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/preprocessor.h>
#include <fatal/type/string.h>
#include <fatal/type/prefix_tree.h>

#include <type_traits>
#include <iterator>
#include <utility>
#include <stdexcept>

#include <cstring>

namespace fatal {

/**
 * TL;DR version of this file:
 *
 * Say you want to declare an enum called `my_enum`, with fields `field0` with
 * default value, `field1` with value `4`, `field2` with value `97` and `field3`
 * with default value, as in:
 *
 *  enum class my_enum {
 *    field0,
 *    field1 = 4,
 *    field2 = 97,
 *    field3
 *  };
 *
 * Say you also want to be able to convert this enum to its string
 * representation and back.
 *
 * This is what you do:
 *
 *  // this is where you specify the enum fields
 *  // the name you give to this macro is irrelevant
 *  #define ENUMIFY_MY_ENUM(FIRST, MID, LAST, ...) \
 *    FIRST(field0, __VA_ARGS__) \
 *    MID(field1, 4, __VA_ARGS__) \
 *    MID(field2, 97, __VA_ARGS__) \
 *    LAST(field3, __VA_ARGS__)
 *
 *  // this code declares the enum `my_enum` for you and also a class
 *  // `my_enum_str` with methods to convert between `my_enum` <-> string
 *  // look for its documentation below for more information
 *  // on the class it creates
 *  FATAL_RICH_ENUM(my_enum, my_enum_str, ENUMIFY_MY_ENUM);
 *
 *  // our macro is not needed from this point onwards
 *  // so it's perfectly fine to clear it:
 *  #undef ENUMIFY_MY_ENUM
 *
 * And that's it, you're ready to go. You have both the enum declaration and
 * a helper class to perform string conversions with your enum, plus some
 * declarations of type_lists and type_maps in case you want to perform some
 * template metaprogramming.
 *
 * Now, this is the TL;DR on how to handle enum <-> strings operations:
 *
 *  // this is how you convert from an enum to its string representation:
 *  void foo(my_enum e) {
 *    char const *s = my_enum_str::to_str(e);
 *    if (!s) {
 *      cout << "e is an invalid value for this enum" << endl;
 *      // ...
 *    }
 *    // ...
 *  }
 *
 *  // these are two ways to parse a string into an enum
 *
 *  void foo(std::string const &s) {
 *    try {
 *      my_enum e = my_enum_str::parse(s);
 *      // ...
 *    } catch (std::invalid_argument const &e) {
 *      cout << "s is an invalid representation of this enum" << endl;
 *    }
 *  }
 *
 *  void foo(std::string const &s) {
 *    my_enum e;
 *    if (!my_enum_str::try_parse(e, s)) {
 *      cout << "s is an invalid representation of this enum" << endl;
 *      // ...
 *    }
 *    // ...
 *  }
 *
 *  // the same ways above work for iterators to the
 *  // range [begin, end) of a string:
 *
 *  template <typename iterator>
 *  void foo(iterator begin, iterator end) {
 *    try {
 *      my_enum e = my_enum_str::parse(begin, end);
 *      // ...
 *    } catch (std::invalid_argument const &e) {
 *      cout << "s is an invalid representation of this enum" << endl;
 *    }
 *  }
 *
 *  template <typename iterator>
 *  void foo(iterator begin, iterator end) {
 *    my_enum e;
 *    if (!my_enum_str::try_parse(e, begin, end)) {
 *      cout << "s is an invalid representation of this enum" << endl;
 *      // ...
 *    }
 *    // ...
 *  }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

/**
 * The code below is dense and not for the faint of heart, stick to comments
 * only and you should be fine. Otherwise, abandon all hope ye who enter here.
 */

#define FATAL_ENUMIFY_GET_1ST_ARG(Arg0, ...) Arg0
#define FATAL_ENUMIFY_GET_2ND_ARG(Arg0, Arg1, ...) Arg1
#define FATAL_ENUMIFY_IGNORE_ARGS_IMPL(...)
#define FATAL_ENUMIFY_DECLARE_LAST_IMPL(Field, Value, Handler, ...) \
  Field Handler(= Value, __VA_ARGS__)
#define FATAL_ENUMIFY_DECLARE_IMPL(...) FATAL_ENUMIFY_DECLARE_LAST_IMPL(__VA_ARGS__),
/**
 * Higher-order macro to declare enumerations. This can be used in
 * conjunction with FATAL_DEFINE_ENUM_STR_CLASS (further below) to
 * create helpers that convert enums to/from strings.
 *
 * You need to provide the ENUMIFY_FN higher-order macro, which
 * declares the enum fields.

 * For fields where you want to explicitly specify their values, just provide
 * the value following the field name:
 *
 * This is the general format of the higher-order macro definition that you
 * must provide:
 *
 *  #define MACRO_NAME(FIRST, MID, LAST, ...) \
 *    FIRST(field0, [value0, ] __VA_ARGS__) \
 *    MID(field1, [value1, ] __VA_ARGS__) \
 *    MID(field2, [value2, ] __VA_ARGS__) \
 *    LAST(field3, [value3, ] __VA_ARGS__)
 *
 * Example:
 *
 *  // first we declare the enum fields
 *  // `field1` will have value `17`, `field3` will have value `101`, the
 *  // rest (`field0` and `field2`) will have the default value
 *  #define ENUMIFY_MY_ENUM(F, M, L, ...) \
 *    F(field0, __VA_ARGS__) \
 *    M(field1, 17, __VA_ARGS__) \
 *    M(field2, __VA_ARGS__) \
 *    L(field3, 101, __VA_ARGS__)
 *
 *  // then we call the macro which declares the enum
 *  FATAL_DECLARE_ENUM(my_enum, ENUMIFY_MY_ENUM);
 *
 *  // our macro is no longer needed but this step is optional
 *  #undef ENUMIFY_MY_ENUM
 *
 * This will generate code equivalent to the following:
 *
 *  enum class my_enum { field0, field1 = 17, field2, field3 = 101 };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_DECLARE_ENUM(Enum, ENUMIFY_FN) \
  enum class Enum { \
    ENUMIFY_FN( \
      FATAL_ENUMIFY_DECLARE_IMPL, \
      FATAL_ENUMIFY_DECLARE_IMPL, \
      FATAL_ENUMIFY_DECLARE_LAST_IMPL, \
      FATAL_ENUMIFY_GET_1ST_ARG, \
      FATAL_ENUMIFY_IGNORE_ARGS_IMPL \
    ) \
  }

#define FATAL_ENUM_TO_STR_CASE_IMPL(Field, Value, Enum, Handler, ...) \
  case Handler(Value, Enum)::Field: return FB_STRINGIZE(Field);
#define FATAL_ENUM_TO_CSTR_IMPL(Field, ...) FATAL_STR(Field, FATAL_AS_STR(Field));
#define FATAL_ENUM_VALUE_TO_LIST_LAST_IMPL(Field, Value, Enum, Handler, ...) \
  Handler(Value, Enum)::Field
#define FATAL_ENUM_VALUE_TO_LIST_IMPL(...) \
  FATAL_ENUM_VALUE_TO_LIST_LAST_IMPL(__VA_ARGS__),
#define FATAL_ENUM_CSTR_TO_LIST_LAST_IMPL(Field, ...) \
  cstr::Field
#define FATAL_ENUM_CSTR_TO_LIST_IMPL(...) \
  FATAL_ENUM_CSTR_TO_LIST_LAST_IMPL(__VA_ARGS__),
/**
 * Higher-order macro to define a class to handle enum <-> string conversions.
 * This can be used in conjunction with ENUMIFY_DECLARE_ENUM.
 *
 * These are the parameters needed by this macro:
 *
 *  - Enum: the type name of the enum
 *  - ClassName: the name of the class to be created by this macro
 *  - ENUMIFY_FN: the higher-order macro which declares the enum fields. For
 *    more details, see ENUMIFT_DECLARE_ENUM
 *
 * The following call to this macro (assume that ENUMIFY_MY_ENUM declares
 * enum fields `field0`, `field1` and `field2`):
 *
 *  FATAL_DEFINE_ENUM_STR_CLASS(Enum, ClassName, ENUMIFY_MY_ENUM);
 *
 * will create a class following this declaration:
 *
 *  struct ClassName {
 *    struct cstr {
 *      FATAL_STR(field0, "field0");
 *      FATAL_STR(field1, "field1");
 *      FATAL_STR(field2, "field2");
 *      FATAL_STR(field3, "field3");
 *    };
 *
 *    typedef type_list<
 *      cstr::field0, cstr::field1, cstr::field2, cstr::field3
 *    > strings;
 *
 *    typedef build_type_map<
 *      cstr::field0, std::integral_constant<Enum, Enum::field0>,
 *      cstr::field1, std::integral_constant<Enum, Enum::field1>,
 *      cstr::field2, std::integral_constant<Enum, Enum::field2>,
 *      cstr::field3, std::integral_constant<Enum, Enum::field3>
 *    > str_to_value;
 *
 *    typedef type_prefix_tree_builder<>::build<
 *      cstr::field0, cstr::field1, cstr::field2, cstr::field3
 *    > prefix_tree;
 *
 *    // returns the string representation of `e`, or
 *    // `nullptr` if `e` is not a valid enum value
 *    // note: the caller doesn't own the string returned,
 *    // nor does it need to deallocate it
 *    char const *to_str(Enum e);
 *
 *    // if the string represented by iterators `begin` and
 *    // `end` represents a valid enum value, returns it
 *    // otherwise, throws `std::invalid_argument`
 *    template <typename TBegin, typename TEnd>
 *    Enum parse(TBegin &&begin, TEnd &&end);
 *
 *    // same as above, with `std::begin(s)`
 *    // and `std::end(s)` for the iterators
 *    template <typename TString>
 *    Enum parse(TString const &s);
 *
 *    // if the string represented by iterators `begin` and `end` represents
 *    // a valid enum value, returns `true` and sets `out` appropriately
 *    // otherwise, returns `false` and leaves `out` untouched
 *    template <typename TBegin, typename TEnd>
 *    bool try_parse(Enum &out, TBegin &&begin, TEnd &&end);
 *
 *    // same as above, with `std::begin(s)`
 *    // and `std::end(s)` for the iterators
 *    template <typename TString>
 *    bool try_parse(Enum &out, TString const &s);
 *  };
 *
 * `to_str()` complexity is generally O(1) - it's implemented with a
 * straightforward switch/case statement.
 *
 * `parse()` and `try_parse()` complexity is `O(m lg k)`, where `m` is the size
 * of the largest enum value name and `k` is the total number of enum values.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_DEFINE_ENUM_STR_CLASS(Enum, ClassName, ENUMIFY_FN) \
  struct ClassName { \
    struct cstr { \
      ENUMIFY_FN( \
        FATAL_ENUM_TO_CSTR_IMPL, \
        FATAL_ENUM_TO_CSTR_IMPL, \
        FATAL_ENUM_TO_CSTR_IMPL \
      ) \
    }; \
    typedef ::fatal::type_list< \
      ENUMIFY_FN( \
        FATAL_ENUM_CSTR_TO_LIST_IMPL, \
        FATAL_ENUM_CSTR_TO_LIST_IMPL, \
        FATAL_ENUM_CSTR_TO_LIST_LAST_IMPL \
      ) \
    > strings; \
    typedef constant_sequence< \
      Enum, \
      ENUMIFY_FN( \
        FATAL_ENUM_VALUE_TO_LIST_IMPL, \
        FATAL_ENUM_VALUE_TO_LIST_IMPL, \
        FATAL_ENUM_VALUE_TO_LIST_LAST_IMPL, \
        Enum, \
        FATAL_ENUMIFY_GET_2ND_ARG, \
        FATAL_ENUMIFY_GET_1ST_ARG \
      ) \
    > values; \
    typedef values::list::apply<strings::zip>::apply<build_type_map> str_to_value; \
    typedef strings::apply<values::list::zip>::apply<build_type_map> value_to_str; \
    typedef strings::apply< \
      ::fatal::type_prefix_tree_builder<>::build \
    > prefix_tree; \
  private: \
    struct parse_visitor_impl { \
      template <typename TString> \
      void operator ()(type_tag<TString>, Enum &out) { \
        typedef typename str_to_value::template find<TString> value; \
        static_assert( \
          !::std::is_same<value, ::fatal::type_not_found_tag>::value, \
          "missing enum mapping" \
        ); \
        out = value::value; \
      } \
    }; \
  public: \
    static char const *to_str(Enum e) { \
      switch (e) { \
        default: return nullptr; \
        ENUMIFY_FN( \
          FATAL_ENUM_TO_STR_CASE_IMPL, \
          FATAL_ENUM_TO_STR_CASE_IMPL, \
          FATAL_ENUM_TO_STR_CASE_IMPL, \
          Enum, \
          FATAL_ENUMIFY_GET_2ND_ARG, \
          FATAL_ENUMIFY_GET_1ST_ARG \
        ) \
      } \
    } \
    template <typename TBegin, typename TEnd> \
    static Enum parse(TBegin &&begin, TEnd &&end) { \
      Enum out; \
      if (!prefix_tree::match<>::exact( \
        std::forward<TBegin>(begin), std::forward<TEnd>(end), \
        parse_visitor_impl{}, out \
      )) { \
        throw std::invalid_argument("unrecognized enum value"); \
      } \
      return out; \
    } \
    template <typename TString> \
    static Enum parse(TString const &s) { \
      return parse(std::begin(s), std::end(s)); \
    } \
    template <typename TBegin, typename TEnd> \
    static bool try_parse(Enum &out, TBegin &&begin, TEnd &&end) { \
      return prefix_tree::match<>::exact( \
        std::forward<TBegin>(begin), std::forward<TEnd>(end), \
        parse_visitor_impl{}, out \
      ); \
    } \
    template <typename TString> \
    static bool try_parse(Enum &out, TString const &s) { \
      return try_parse(out, std::begin(s), std::end(s)); \
    } \
  } \

/**
 * A convenience macro that combines both `FATAL_DECLARE_ENUM` and
 * `FATAL_DECLARE_ENUM` into a single one.
 *
 * See the documentation of both for more info on how they work.
 */
#define FATAL_RICH_ENUM(Enum, ClassName, ENUMIFY_FN) \
  FATAL_DECLARE_ENUM(Enum, ENUMIFY_FN); \
  FATAL_DEFINE_ENUM_STR_CLASS(Enum, ClassName, ENUMIFY_FN) \

} // namespace fatal
