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
#include <fatal/type/sequence.h>

namespace fatal {

/**
 * A compile-time string for template metaprogramming. This class inherits
 * from `constant_sequence` so all the functionality provided by the latter
 * is also present on the former, be it compile or runtime.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: Operations that would
 * mutate the sequence upon which they are performed actually create a new
 * sequence.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T... Chars>
class type_string:
  public constant_sequence<T, Chars...>
{
  typedef constant_sequence<T, Chars...> as_sequence;

  typedef std::char_traits<T> default_char_traits;
  typedef std::allocator<T> default_allocator;

public:
  /**
   * The type of the characters for this `type_string`.
   */
  typedef typename as_sequence::type char_type;

  /**
   * Constructs a `std::basic_string` corresponding to this
   * `type_string`, using the given character traits and allocator.
   * Defaults to `std::char_traits` and `std::allocator`.
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  typedef type_string<char, 'h', 'i'> hi;
   *
   *  // yields `std::string("hi")`
   *  auto result = hi::string();
   */
  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  using string_type = std::basic_string<char_type, TTraits, TAllocator>;

  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  static string_type<TTraits, TAllocator> string() {
    return string_type<TTraits, TAllocator>{Chars...};
  }
};

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

/**
 * Creates a `type_string` typedef out of a regular string.
 *
 * Example:
 *
 *  // this is equivalent to
 *  // `typedef type_string<char, 'h', 'i'> hi;`
 *  FATAL_STR("hi") hi;
 *
 *  // this is equivalent to
 *  // `typedef type_string<char32_t, U'h', U'e', U'y'> hey;`
 *  FATAL_STR(U"hey") hey;
 */
#define FATAL_STR(Id, String) \
  FATAL_BUILD_STR_IMPL(String, FATAL_CAT(Id, FATAL_UID(build_cstr_impl_))) Id

///////////////////////////////////////
// IMPLEMENTATION DETAILS DEFINITION //
///////////////////////////////////////

namespace detail {
namespace type_string_impl {

///////////////
// FATAL_STR //
///////////////

template <typename T, std::size_t Size>
constexpr std::size_t size(T const (&)[Size]) {
  static_assert(
    Size > 0,
    "expecting a string containing at least the null terminator"
  );
  return Size - 1;
}

#define FATAL_BUILD_STR_IMPL(String, Class) \
  template <std::size_t... Indexes> \
  struct Class { \
    static_assert( \
      !((String)[sizeof...(Indexes)]), \
      "expecting a valid null-terminated string" \
    ); \
    typedef typename ::std::decay<decltype(*(String))>::type char_type; \
    typedef ::fatal::type_string< \
      char_type, \
      (String)[Indexes]... \
    > type; \
  }; \
  \
  typedef typename ::fatal::constant_range< \
    std::size_t, 0, ::fatal::detail::type_string_impl::size(String) \
  >::apply<Class>::type

} // namespace type_string_impl {
} // namespace detail {
} // namespace fatal {
