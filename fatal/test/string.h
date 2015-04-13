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

FATAL_IMPL_PARSE_CONVERSION_PAIR(int);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(unsigned long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(unsigned long long);
FATAL_IMPL_PARSE_CONVERSION_PAIR(float);
FATAL_IMPL_PARSE_CONVERSION_PAIR(double);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long double);

#undef FATAL_IMPL_PARSE_CONVERSION_PAIR

#define FATAL_IMPL_PARSE_CONVERSION_PAIR(To, From, Fn) \
  template <> \
  struct parse_impl_conversion_pair<To, From> { \
    static To convert(From const &value) { return Fn(value); } \
  }

FATAL_IMPL_PARSE_CONVERSION_PAIR(int, std::string, std::stoi);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long, std::string, std::stol);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long long, std::string, std::stoll);
FATAL_IMPL_PARSE_CONVERSION_PAIR(unsigned long, std::string, std::stoul);
FATAL_IMPL_PARSE_CONVERSION_PAIR(unsigned long long, std::string, std::stoull);
FATAL_IMPL_PARSE_CONVERSION_PAIR(float, std::string, std::stof);
FATAL_IMPL_PARSE_CONVERSION_PAIR(double, std::string, std::stod);
FATAL_IMPL_PARSE_CONVERSION_PAIR(long double, std::string, std::stold);

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
