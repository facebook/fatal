/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_lib_h
#define FATAL_INCLUDE_fatal_test_lib_h

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <type_traits>

#include <iostream>

#include <cassert>

namespace fatal {

std::string type_str(std::type_info const &type) {
  // TODO: de-mangle
  return type.name();
}

template <typename T>
std::string type_str() {
  auto s = type_str(typeid(T));

  using type = typename std::remove_reference<T>::type;

  if (std::is_const<type>::value) {
    s.append(" const");
  }

  if (std::is_volatile<type>::value) {
    s.append(" volatile");
  }

  if (std::is_lvalue_reference<T>::value) {
    s.append(" &");
  } else if (std::is_rvalue_reference<T>::value) {
    s.append(" &&");
  }

  return s;
}

/**
 * Unit test helper for checking whether two types are the same.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TExpected, typename TActual>
void expect_same(
  char const *file_name = __FILE__,
  std::size_t line_number = __LINE__
) {
  if (!std::is_same<TExpected, TActual>::value) {
    std::cerr << "at " << file_name << ':' << line_number << ':'
      << std::endl << std::endl
      << "  expected: '" << type_str<TExpected>() << '\''
      << std::endl << std::endl
      << "  actual:   '" << type_str<TActual>() << '\''
      << std::endl << std::endl;

    EXPECT_TRUE((std::is_same<TExpected, TActual>::value));
  }
}

namespace detail {

struct expect_same_impl {
  expect_same_impl(char const *file_name, std::size_t line_number):
    file_name_(file_name),
    line_number_(line_number)
  {}

  template <typename TExpected, typename TActual>
  void check() const {
    expect_same<TExpected, TActual>(file_name_, line_number_);
  }

private:
  char const *file_name_;
  std::size_t line_number_;
};

} // namespace detail {

#define FATAL_EXPECT_SAME \
  ::fatal::detail::expect_same_impl{__FILE__, __LINE__}.check

namespace detail {

template <typename, typename> struct parse_impl_conversion_pair;

#define FATAL_IMPL_PARSE_CONVERSION_PAIR(From) \
  template <> \
  struct parse_impl_conversion_pair<std::string, From> { \
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
  static std::string const &convert(std::string const &s) { return s; }
  static std::string convert(std::string &&s) { return std::move(s); }

  static void append(std::string &out, std::string const &s) { out.append(s); }
  static void append(std::string &out, std::string &&s) {
    out.append(std::move(s));
  }
};

template <>
struct parse_impl_conversion_pair<std::string, char const *> {
  static std::string convert(char const *s) { return s; }

  static void append(std::string &out, char const *s) { out.append(s); }
};

template <>
struct parse_impl_conversion_pair<std::string, char *> {
  static std::string convert(char const *s) { return s; }

  static void append(std::string &out, char const *s) { out.append(s); }
};

template <>
struct parse_impl_conversion_pair<std::string, char> {
  static std::string convert(char c) { return std::string(1, c); }

  static void append(std::string &out, char c) { out.push_back(c); }
};

void to_string_impl(std::string &) {}

template <typename T, typename... Args>
void to_string_impl(std::string &out, T &&value, Args &&...args) {
  using converter = detail::parse_impl_conversion_pair<
    std::string, typename std::decay<T>::type
  >;

  converter::append(out, std::forward<T>(value));

  to_string_impl(out, std::forward<Args>(args)...);
}

} // namespace detail {

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename TTo, typename TFrom>
TTo parse(TFrom &&from) {
  using converter = detail::parse_impl_conversion_pair<
    TTo, typename std::decay<TFrom>::type
  >;

  return converter::convert(std::forward<TFrom>(from));
}

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename... Args>
std::string to_string(Args &&...args) {
  std::string out;

  detail::to_string_impl(out, std::forward<Args>(args)...);

  return out;
}

std::string to_string(std::string s) { return s; }

// for internal tests only

namespace log {
namespace detail {

template <typename TOut, typename TInfo>
struct logger {
  using info = TInfo;

  struct writer {
    explicit writer(TOut *out) noexcept: out_(out) {}

    writer(writer const &) = delete;
    writer(writer &&rhs) noexcept: out_(rhs.out_) { rhs.out_ = nullptr; }

    template <typename T>
    writer &operator <<(T &&value) & {
      if (out_) {
        *out_ << std::forward<T>(value);
      }

      return *this;
    }

    template <typename T>
    writer &&operator <<(T &&value) && {
      if (out_) {
        *out_ << std::forward<T>(value);
      }

      return std::move(*this);
    }

    ~writer() {
      if (out_) {
        *out_ << '\n';
      }
    }

  private:
    TOut *out_;
  };

  explicit logger(TOut *out, char const *file, std::size_t line) noexcept:
    writer_(out),
    file_(file),
    line_(line)
  {
    assert(file_);
    for (auto i = file_; *i; ++i) {
      if (*i == '/') {
        file_ = i + 1;
      }
    }
  }

  logger(logger const &) = delete;
  logger(logger &&rhs) = default;

  template <typename T>
  writer operator <<(T &&value) {
    writer_ << info::signature::value;

    if (info::show_level::value) {
      writer_ << info::value;
    }

    // TODO: output date in a more useful format
    writer_ << ' '
      << std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
      ).count() << ' ' << file_ << ':' << line_ << " | "
      << std::forward<T>(value);

    return std::move(writer_);
  }

  ~logger() {
    if (info::abort::value) {
      std::abort();
    }
  }

private:
  writer writer_;
  char const *file_;
  std::size_t line_;
};

using level_t = unsigned;

template <typename TCategory, level_t Level>
struct log_level {
  static void set(level_t level) {
    value() = level;
  }

  static level_t get() {
    return value();
  }

private:
  static std::atomic<level_t> &value() {
    static std::atomic<level_t> instance(Level);
    return instance;
  }
};

template <
  typename TCategory,
  level_t Level,
  char Signature,
  bool ShowLevel,
  bool Abort = false
>
struct level_info:
  std::integral_constant<level_t, Level>
{
  using category = TCategory;
  using show_level = std::integral_constant<bool, ShowLevel>;
  using signature = std::integral_constant<char, Signature>;
  using abort = std::integral_constant<bool, Abort>;
};

struct log_tag {};
struct verbose_tag {};

using level_FATAL = level_info<log_tag, 0, 'F', false, true>;
using level_CRITICAL = level_info<log_tag, 1, 'C', false>;
using level_ERROR = level_info<log_tag, 2, 'E', false>;
using level_INFO = level_info<log_tag, 3, 'I', false>;
using level_DEBUG = level_info<log_tag, 4, 'D', false>;

template <level_t Level>
using level_verbose = level_info<verbose_tag, Level, 'V', true>;

template <typename> struct by_category;

template <>
struct by_category<log_tag> {
  using level = detail::log_level<detail::log_tag, detail::level_INFO::value>;
};

template <>
struct by_category<verbose_tag> {
  using level = detail::log_level<detail::verbose_tag, 0>;
};

} // namespace detail {

using level = detail::by_category<detail::log_tag>::level;
using v_level = detail::by_category<detail::verbose_tag>::level;

template <typename TInfo>
log::detail::logger<std::ostream, TInfo> log(
  char const *file,
  std::size_t line
) {
  return log::detail::logger<std::ostream, TInfo>(
    TInfo::value <= detail::by_category<typename TInfo::category>::level::get()
      ? std::addressof(std::cerr)
      : nullptr,
    file, line
  );
}

} // namespace log {

#define FATAL_LOG(Level) \
  ::fatal::log::log<::fatal::log::detail::level_##Level>(__FILE__, __LINE__)

#define FATAL_VLOG(Level) \
  ::fatal::log::log<::fatal::log::detail::level_verbose<Level>>( \
    __FILE__, __LINE__ \
  )

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_lib_h
