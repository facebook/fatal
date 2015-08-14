/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_string_h
#define FATAL_INCLUDE_fatal_test_string_h

#include <stdexcept>
#include <string>
#include <type_traits>

#include <cstring>

namespace fatal {
namespace detail {
namespace string_impl {

template <typename, typename> struct parse_impl_conversion_pair;

template <>
struct parse_impl_conversion_pair<std::string, bool> {
  using quote = std::integral_constant<char, '\0'>;

  static std::string convert(bool value) { return value ? "true" : "false"; }
 
  static void append(std::string &out, bool value) {
    out.append(value ? "true" : "false");
  }
};

template <>
struct parse_impl_conversion_pair<bool, std::string> {
  static bool convert(std::string const &value) {
    // TODO: use a compile-time trie??
    if (value == "true") {
      return true;
    }
    
    if (value == "false") {
      return false;
    }

    throw std::invalid_argument("unrecognized boolean");
  }
};

#define FATAL_IMPL_PARSE_CONVERSION_PAIR(From) \
  template <> \
  struct parse_impl_conversion_pair<std::string, From> { \
    using quote = std::integral_constant<char, '\0'>; \
    \
    static std::string convert(From value) { return std::to_string(value); } \
    \
    static void append(std::string &out, From value) { \
      out.append(std::to_string(value)); \
    } \
  }

FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int8_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int16_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int32_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int64_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint8_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint16_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint32_t);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint64_t);
// TODO: FIX AS IT MIGHT CLASH WITH ONE ON THE TYPES ABOVE
FATAL_IMPL_PARSE_CONVERSION_PAIR(long long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(unsigned long long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(float);
FATAL_IMPL_PARSE_CONVERSION_PAIR(double);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long double);

#undef FATAL_IMPL_PARSE_CONVERSION_PAIR

template <typename T>
struct stoi {
  static T parse(std::string const &s) { return std::stoi(s); }
};

template <>
struct stoi<long> {
  static long parse(std::string const &s) { return std::stol(s); }
};

template <>
struct stoi<long long> {
  static long long parse(std::string const &s) { return std::stoll(s); }
};

template <typename T>
struct stou {
  static T parse(std::string const &s) { return std::stoul(s); }
};

template <>
struct stou<unsigned long long> {
  static unsigned long long parse(std::string const &s) {
    return std::stoull(s);
  }
};

template <typename T>
struct stofp {
  static T parse(std::string const &s) { return std::stof(s); }
};

template <>
struct stofp<double> {
  static double parse(std::string const &s) { return std::stod(s); }
};

template <>
struct stofp<long double> {
  static long double parse(std::string const &s) { return std::stold(s); }
};

#define FATAL_IMPL_PARSE_CONVERSION_PAIR(To, From, Converter) \
  template <> \
  struct parse_impl_conversion_pair<To, From> { \
    static To convert(From const &value) { return Converter<To>::parse(value); } \
  }

FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int8_t, std::string, stoi);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int16_t, std::string, stoi);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int32_t, std::string, stoi);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::int64_t, std::string, stoi);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint8_t, std::string, stou);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint16_t, std::string, stou);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint32_t, std::string, stou);
FATAL_IMPL_PARSE_CONVERSION_PAIR(std::uint64_t, std::string, stou);
FATAL_IMPL_PARSE_CONVERSION_PAIR(float, std::string, stofp);
FATAL_IMPL_PARSE_CONVERSION_PAIR(double, std::string, stofp);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long double, std::string, stofp);

#undef FATAL_IMPL_PARSE_CONVERSION_PAIR

template <>
struct parse_impl_conversion_pair<std::string, std::string> {
  using quote = std::integral_constant<char, '"'>;

  static std::string const &convert(std::string const &s) { return s; }
  static std::string convert(std::string &&s) { return std::move(s); }

  static void append(std::string &out, std::string const &s) { out.append(s); }
  static void append(std::string &out, std::string &&s) {
    out.append(std::move(s));
  }
};

template <>
struct parse_impl_conversion_pair<std::string, char const *> {
  using quote = std::integral_constant<char, '"'>;

  static std::string convert(char const *s) { return s; }

  static void append(std::string &out, char const *s) { out.append(s); }
};

template <>
struct parse_impl_conversion_pair<std::string, char *>:
  public parse_impl_conversion_pair<std::string, char const *>
{};

template <std::size_t N>
struct parse_impl_conversion_pair<std::string, char const [N]>:
  public parse_impl_conversion_pair<std::string, char const *>
{};

template <std::size_t N>
struct parse_impl_conversion_pair<std::string, char [N]>:
  public parse_impl_conversion_pair<std::string, char const *>
{};

// TODO: UNSIGNED CHAR
template <>
struct parse_impl_conversion_pair<std::string, char> {
  using quote = std::integral_constant<char, '\''>;

  static std::string convert(char c) { return std::string(1, c); }

  static void append(std::string &out, char c) { out.push_back(c); }
};

void to_string_impl(std::string &) {}

template <typename T, typename... Args>
void to_string_impl(std::string &out, T &&value, Args &&...args) {
  using converter = detail::string_impl::parse_impl_conversion_pair<
    std::string, typename std::decay<T>::type
  >;

  converter::append(out, std::forward<T>(value));

  to_string_impl(out, std::forward<Args>(args)...);
}

} // namespace string_impl {
} // namespace detail {

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename TTo, typename TFrom>
TTo parse(TFrom &&from) {
  using converter = detail::string_impl::parse_impl_conversion_pair<
    TTo, typename std::decay<TFrom>::type
  >;

  return converter::convert(std::forward<TFrom>(from));
}

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename... Args>
std::string &append_to_string(std::string &out, Args &&...args) {
  detail::string_impl::to_string_impl(out, std::forward<Args>(args)...);

  return out;
}

// TODO: TEST
template <typename... Args>
std::string to_string(Args &&...args) {
  std::string out;

  append_to_string(out, std::forward<Args>(args)...);

  return out;
}

std::string const &to_string(std::string const &s) { return s; }
std::string &to_string(std::string &s) { return s; }
std::string &&to_string(std::string &&s) { return std::move(s); }

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_string_h
