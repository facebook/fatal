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

///////////////////////
// parse_type_string //
///////////////////////

namespace detail {
namespace type_string_impl {

template <typename T, typename TChar, TChar, TChar...>
struct parse_type_string;

template <typename, typename> struct from_type_string;

} // namespace type_string_impl {
} // namespace detail {

/**
 * Parses the integral value represented by a type string.
 *
 * Arguments:
 *
 *  - T: the type of the integral value that will be parsed
 *
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T = std::size_t>
struct parse_type_string {
  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TChar>
  struct bind {
    /**
     * TODO: DOCUMENT AND TEST
     *
     * @author: Marcelo Juchem <marcelo@fb.com>
     */
    template <TChar... Chars>
    using apply = typename detail::type_string_impl::parse_type_string<
      T, TChar, Chars...
    >::type;
  };

  /**
   * A convenient alias to `bind<TChar>::apply<Chars...>`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TChar, TChar... Chars>
  using apply = typename bind<TChar>::template apply<Chars...>;

  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename TString>
  using from = typename detail::type_string_impl::from_type_string<
    T, TString
  >::type;
};

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
struct to_type_string_impl {
  template <TChar... Suffix>
  using apply = typename to_type_string_impl<
    TChar, T, Value / 10
  >::template apply<
    static_cast<TChar>("9876543210123456789"[(Value % 10) + 9]), Suffix...
  >;
};

template <typename TChar, typename T, T Value>
struct to_type_string_impl<TChar, T, Value, true> {
  template <TChar... Chars>
  using apply = type_string<TChar, Chars...>;
};

template <typename TChar, typename T, T Value>
class to_type_string {
  using str = typename to_type_string_impl<TChar, T, Value>::template apply<>;

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

///////////////////////
// parse_type_string //
///////////////////////

template <typename T, typename TChar, TChar Char>
struct parse_type_char {
  static_assert(
    Char == static_cast<TChar>('0')
    || Char == static_cast<TChar>('1')
    || Char == static_cast<TChar>('2')
    || Char == static_cast<TChar>('3')
    || Char == static_cast<TChar>('4')
    || Char == static_cast<TChar>('5')
    || Char == static_cast<TChar>('6')
    || Char == static_cast<TChar>('7')
    || Char == static_cast<TChar>('8')
    || Char == static_cast<TChar>('9'),
    "character is not a valid digit"
  );

  using type = std::integral_constant<
    T,
    Char == static_cast<TChar>('0') ? static_cast<T>(0)
    : Char == static_cast<TChar>('1') ? static_cast<T>(1)
    : Char == static_cast<TChar>('2') ? static_cast<T>(2)
    : Char == static_cast<TChar>('3') ? static_cast<T>(3)
    : Char == static_cast<TChar>('4') ? static_cast<T>(4)
    : Char == static_cast<TChar>('5') ? static_cast<T>(5)
    : Char == static_cast<TChar>('6') ? static_cast<T>(6)
    : Char == static_cast<TChar>('7') ? static_cast<T>(7)
    : Char == static_cast<TChar>('8') ? static_cast<T>(8)
    : Char == static_cast<TChar>('9') ? static_cast<T>(9)
    : 10
  >;

  static_assert(
    type::value >= 0 && type::value <= 9,
    "character is not a valid digit"
  );
};

#undef FATAL_IMPL_PARSE_TYPE_CHAR

template <typename T>
struct parse_string_add_tail {
  template <T Value, T Next>
  using positive = std::integral_constant<T, Value + Next>;

  template <T Value, T Next>
  using negative = std::integral_constant<T, Value - Next>;
};

template <typename T, T, template <T, T> class, typename TChar, TChar...>
struct parse_type_string_recursion;

template <
  typename T, T Value, template <T, T> class TAddTail,
  typename TChar, TChar Head, TChar... Tail
>
struct parse_type_string_recursion<T, Value, TAddTail, TChar, Head, Tail...> {
  using type = typename parse_type_string_recursion<
    T,
    TAddTail<Value * 10, parse_type_char<T, TChar, Head>::type::value>::value,
    TAddTail, TChar, Tail...
  >::type;
};

template <typename T, T Value, template <T, T> class TAddTail, typename TChar>
struct parse_type_string_recursion<T, Value, TAddTail, TChar> {
  using type = std::integral_constant<T, Value>;
};

template <bool, typename T, typename TChar, TChar Head, TChar... Tail>
struct parse_type_string_selector {
  using head = typename parse_type_char<T, TChar, Head>::type;

  static_assert(
    sizeof...(Tail) == 0 || head::value > 0,
    "integral can't have a leading zero"
  );

  using type = typename parse_type_string_recursion<
    T, head::value, parse_string_add_tail<T>::template positive, TChar, Tail...
  >::type;
};

template <typename T, typename TChar, TChar Sign, TChar Head, TChar... Tail>
struct parse_type_string_selector<true, T, TChar, Sign, Head, Tail...> {
  static_assert(Sign == static_cast<TChar>('-'), "unrecognized sign");

  using head = typename parse_type_char<T, TChar, Head>::type;

  static_assert(head::value > 0, "negative integral can't have a leading zero");

  using type = typename parse_type_string_recursion<
    T, -head::value, parse_string_add_tail<T>::template negative, TChar, Tail...
  >::type;
};

template <typename T, typename TChar, TChar Head, TChar... Tail>
struct parse_type_string {
  using is_sign = std::integral_constant<bool, Head == static_cast<TChar>('-')>;

  static_assert(
    std::is_signed<T>::value || !is_sign::value,
    "unsigned integral can't have a sign"
  );

  using type = typename parse_type_string_selector<
    is_sign::value, T, TChar, Head, Tail...
  >::type;
};

template <
  typename T,
  typename TChar,
  template <typename, TChar...> class U,
  TChar... Chars
>
struct from_type_string<T, U<TChar, Chars...>> {
  using type = typename parse_type_string<T, TChar, Chars...>::type;
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
