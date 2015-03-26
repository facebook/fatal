/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_string_h
#define FATAL_INCLUDE_fatal_type_string_h

#include <fatal/preprocessor.h>
#include <fatal/type/sequence.h>

namespace fatal {

/////////////////
// type_string //
/////////////////

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
  public detail::constant_sequence_impl::constant_sequence<
    type_string, T, Chars...
  >
{
  using as_sequence = constant_sequence<T, Chars...>;

  using default_char_traits = std::char_traits<T>;
  using default_allocator = std::allocator<T>;

public:
  /**
   * The type of the characters for this `type_string`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using char_type = typename as_sequence::type;

  /**
   * The `std::basic_string` type returned by the `string()` method.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  using string_type = std::basic_string<char_type, TTraits, TAllocator>;

  /**
   * Constructs a `std::basic_string` corresponding to this
   * `type_string`, using the given character traits and allocator.
   * Defaults to `std::char_traits` and `std::allocator`.
   *
   * Note: this is a runtime facility.
   *
   * Example:
   *
   *  using hi = type_string<char, 'h', 'i'>;
   *
   *  // yields `std::string("hi")`
   *  auto result = hi::string();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename TTraits = default_char_traits,
    typename TAllocator = default_allocator
  >
  static string_type<TTraits, TAllocator> string() {
    return string_type<TTraits, TAllocator>{Chars...};
  }
};

////////////////////
// to_type_string //
////////////////////

namespace detail {
namespace type_string_impl {

template <typename, typename T, T> class to_type_string;

} // namespace type_string_impl {
} // namespace detail {

/**
 * Converts an integral value to its string representation, as a type string.
 *
 * Arguments:
 *
 *  - T: the type of the integral value
 *  - Value: the value to be converted
 *  - TChar: the type of the string's character. Defaults to `char`.
 *
 * Example:
 *
 *  // yields `type_string<char, '4', '2'>`
 *  using result1 = to_type_string<std::size_t, 42>;
 *
 *  // yields `type_string<wchar_t, L'-', L'5', L'6'>`
 *  using result2 = to_type_string<int, -56, wchar_t>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Value, typename TChar = char>
using to_type_string = typename detail::type_string_impl::to_type_string<
  TChar, T, Value
>::type;

///////////////
// FATAL_STR //
///////////////

/**
 * Instantiates a `type_string` class template out of a regular string.
 *
 * Example:
 *
 *  // this is equivalent to
 *  // `using hi = type_string<char, 'h', 'i'>;`
 *  FATAL_STR("hi") hi;
 *
 *  // this is equivalent to
 *  // `using hey = type_string<char32_t, U'h', U'e', U'y'>;`
 *  FATAL_STR(U"hey") hey;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_STR(Id, String) \
  FATAL_BUILD_STR_IMPL( \
    Id, \
    String, \
    FATAL_UID(FATAL_CAT(type_string_impl_, Id)), \
    FATAL_UID(Indexes) \
  )

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace type_string_impl {

////////////////////
// to_type_string //
////////////////////

template <typename TChar, typename T, T Value, bool Done = (Value == 0)>
struct to_string {
  template <TChar... Suffix>
  using apply = typename to_string<TChar, T, Value / 10>::template apply<
    static_cast<TChar>("9876543210123456789"[(Value % 10) + 9]), Suffix...
  >;
};

template <typename TChar, typename T, T Value>
struct to_string<TChar, T, Value, true> {
  template <TChar... Chars>
  using apply = type_string<TChar, Chars...>;
};

template <typename TChar, typename T, T Value>
class to_type_string {
  using str = typename to_string<TChar, T, Value>::template apply<>;

public:
  using type = typename std::conditional<
    Value == 0,
    type_string<TChar, static_cast<TChar>('0')>,
    typename std::conditional<
      (Value < 0),
      typename str::template push_front<static_cast<TChar>('-')>,
      str
    >::type
  >::type;
};

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

#define FATAL_BUILD_STR_IMPL(Id, String, Class, Indexes) \
  template <::std::size_t... Indexes> \
  struct Class { \
    static_assert( \
      !((String)[sizeof...(Indexes)]), \
      "expecting a valid null-terminated string" \
    ); \
    \
    using char_type = typename ::std::decay<decltype(*(String))>::type; \
    \
    using type = ::fatal::type_string< \
      char_type, \
      (String)[Indexes]... \
    >; \
  }; \
  \
  using Id = typename ::fatal::constant_range< \
    ::std::size_t, 0, ::fatal::detail::type_string_impl::size(String) \
  >::apply<Class>::type

} // namespace type_string_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_string_h
