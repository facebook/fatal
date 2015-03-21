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

#include <fatal/preprocessor.h>
#include <fatal/time/time.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
std::string &append_to_string(std::string &out, Args &&...args) {
  detail::to_string_impl(out, std::forward<Args>(args)...);

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

  logger(TOut *out, source_info source) noexcept:
    writer_(out),
    source_(source)
  {}

  logger(logger const &) = delete;
  logger(logger &&rhs) = default;

  template <typename T>
  writer operator <<(T &&value) {
    writer_ << info::signature::value;

    if (info::show_level::value) {
      writer_ << info::value;
    }

    // TODO: output date in a more useful format
    writer_ << std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
      ).count() << "| " << source_.file() << ':' << source_.line() << "] "
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
  source_info source_;
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
using level_WARNING = level_info<log_tag, 3, 'W', false>;
using level_INFO = level_info<log_tag, 4, 'I', false>;
using level_DEBUG = level_info<log_tag, 5, 'D', false>;

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
log::detail::logger<std::ostream, TInfo> log(source_info source) {
  return log::detail::logger<std::ostream, TInfo>(
    TInfo::value <= detail::by_category<typename TInfo::category>::level::get()
      ? std::addressof(std::cerr)
      : nullptr,
    source
  );
}

} // namespace log {

#define FATAL_LOG(Level) \
  ::fatal::log::log<::fatal::log::detail::level_##Level>(FATAL_SOURCE_INFO())

#define FATAL_VLOG(Level) \
  ::fatal::log::log<::fatal::log::detail::level_verbose<Level>>( \
    FATAL_SOURCE_INFO() \
  )

// for internal tests only

// TODO: organize the order of components

namespace test {

#define FATAL_TEST(Case, Name, ...) \
  FATAL_IMPL_TEST_CASE( \
    FATAL_UID(FATAL_CAT(FATAL_CAT(Case, FATAL_CAT(_, Name)), _test_impl)), \
    Case, \
    Name, \
    FATAL_UID(FATAL_CAT(Case, FATAL_CAT(_, FATAL_CAT(Name, _test_results)))) \
  )

#define FATAL_IMPL_TEST_CASE(Class, Case, Name, Results) \
  struct Class { \
    void operator ()(); \
    \
    ::fatal::test::results Results; \
  }; \
  \
  namespace { \
  static auto const FATAL_UID(FATAL_CAT(Class, FATAL_CAT(_, test_registry))) \
    = ::fatal::test::detail::registry::get().add<Class>(); \
  }; \
  \
  char const *operator <<(::fatal::test::detail::group_tag, Class *) { \
    return FATAL_TO_STR(Case); \
  } \
  \
  char const *operator <<(::fatal::test::detail::name_tag, Class *) { \
    return FATAL_TO_STR(Name); \
  } \
  \
  ::fatal::test::results &operator <<( \
    ::fatal::test::detail::result_tag, Class &subject \
  ) { \
    return subject.Results; \
  } \
  \
  void Class::operator ()()

#define FATAL_WARN_UNREACHABLE() \
  FATAL_IMPL_NULLARY_TEST(warning, unreachable)

#define FATAL_WARN_TRUE(Expression) \
  FATAL_IMPL_UNARY_TEST(warning, is_true(), Expression)

#define FATAL_WARN_FALSE(Expression) \
  FATAL_IMPL_UNARY_TEST(warning, is_false(), Expression)

#define FATAL_WARN_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(warning, is_null(), Expression)

#define FATAL_WARN_NOT_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(warning, not_null(), Expression)

#define FATAL_WARN_NO_THROW \
  FATAL_IMPL_BODY_TEST(warning, no_throw())

#define FATAL_WARN_THROW(Exception) \
  FATAL_IMPL_BODY_TEST(warning, exception<Exception>(FATAL_TO_STR(Exception)))

#define FATAL_WARN_EQ(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, is_equal(), LHS, RHS)

#define FATAL_WARN_NE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, not_equal(), LHS, RHS)

#define FATAL_WARN_LT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, less_than(), LHS, RHS)

#define FATAL_WARN_LE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, less_equal(), LHS, RHS)

#define FATAL_WARN_GT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, greater_than(), LHS, RHS)

#define FATAL_WARN_GE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(warning, greater_equal(), LHS, RHS)


#define FATAL_EXPECT_UNREACHABLE() \
  FATAL_IMPL_NULLARY_TEST(expectation, unreachable)

#define FATAL_EXPECT_TRUE(Expression) \
  FATAL_IMPL_UNARY_TEST(expectation, is_true(), Expression)

#define FATAL_EXPECT_FALSE(Expression) \
  FATAL_IMPL_UNARY_TEST(expectation, is_false(), Expression)

#define FATAL_EXPECT_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(expectation, is_null(), Expression)

#define FATAL_EXPECT_NOT_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(expectation, not_null(), Expression)

#define FATAL_EXPECT_NO_THROW \
  FATAL_IMPL_BODY_TEST(expectation, no_throw())

#define FATAL_EXPECT_THROW(Exception) \
  FATAL_IMPL_BODY_TEST( \
    expectation, exception<Exception>(FATAL_TO_STR(Exception)) \
  )

#define FATAL_EXPECT_EQ(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, is_equal(), LHS, RHS)

#define FATAL_EXPECT_NE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, not_equal(), LHS, RHS)

#define FATAL_EXPECT_LT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, less_than(), LHS, RHS)

#define FATAL_EXPECT_LE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, less_equal(), LHS, RHS)

#define FATAL_EXPECT_GT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, greater_than(), LHS, RHS)

#define FATAL_EXPECT_GE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(expectation, greater_equal(), LHS, RHS)


#define FATAL_ASSERT_UNREACHABLE() \
  FATAL_IMPL_NULLARY_TEST(assertion, unreachable)

#define FATAL_ASSERT_TRUE(Expression) \
  FATAL_IMPL_UNARY_TEST(assertion, is_true(), Expression)

#define FATAL_ASSERT_FALSE(Expression) \
  FATAL_IMPL_UNARY_TEST(assertion, is_false(), Expression)

#define FATAL_ASSERT_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(assertion, is_null(), Expression)

#define FATAL_ASSERT_NOT_NULL(Expression) \
  FATAL_IMPL_UNARY_TEST(assertion, not_null(), Expression)

#define FATAL_ASSERT_NO_THROW \
  FATAL_IMPL_BODY_TEST(assertion, no_throw())

#define FATAL_ASSERT_THROW(Exception) \
  FATAL_IMPL_BODY_TEST(assertion, exception<Exception>(FATAL_TO_STR(Exception)))

#define FATAL_ASSERT_EQ(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, is_equal(), LHS, RHS)

#define FATAL_ASSERT_NE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, not_equal(), LHS, RHS)

#define FATAL_ASSERT_LT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, less_than(), LHS, RHS)

#define FATAL_ASSERT_LE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, less_equal(), LHS, RHS)

#define FATAL_ASSERT_GT(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, greater_than(), LHS, RHS)

#define FATAL_ASSERT_GE(LHS, RHS) \
  FATAL_IMPL_BINARY_TEST(assertion, greater_equal(), LHS, RHS)

/**
 * Unit test helper for checking whether two types are the same.
 *
 * Example:
 *
 *  // passes the test
 *  FATAL_EXPECT_SAME<int, int>();
 *
 *  // yields an error
 *  FATAL_EXPECT_SAME<int, void>();
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: REWRITE USING THE NEW FRAMEWORK
#define FATAL_EXPECT_SAME \
  ::fatal::test::detail::expect_same_impl(FATAL_SOURCE_INFO()).check

using clock = std::chrono::system_clock;
using duration = clock::duration;
using timestamp = clock::time_point;

struct issue {
  enum class severity { warning, error, fatal };

  template <typename... Args>
  issue(
    severity severity,
    timestamp timestamp,
    source_info source,
    Args &&...args
  ):
    severity_(severity),
    timestamp_(timestamp),
    source_(source)
  {
    append_to_string(message_, std::forward<Args>(args)...);
  }

  template <typename TOut>
  void print(TOut &out) const {
    // TODO: output date in a more useful format
    out << severity_signature() << timestamp_.time_since_epoch().count()
      << "| " << source_.file() << ':' << source_.line()
      << "\n  " << message_;
  }

  template <typename... Args>
  std::string const &append(Args &&...args) {
    return append_to_string(message_, std::forward<Args>(args)...);
  }

private:
  severity const severity_;
  timestamp const timestamp_;
  source_info const source_;
  std::string message_;

  char severity_signature() const {
    switch (severity_) {
      case severity::warning: return 'W';
      case severity::error: return 'E';
      case severity::fatal: return 'F';
      default:
        assert(false);
        return '\0';
    }
  }
};

class results {
  using issue_list = std::vector<issue>;

public:
  using const_iterator = issue_list::const_iterator;
  using size_type = issue_list::size_type;

  void add(issue i) {
    issues_.push_back(std::move(i));
  }

  const_iterator cbegin() const { return issues_.cbegin(); }
  const_iterator begin() const { return issues_.begin(); }
  const_iterator cend() const { return issues_.cend(); }
  const_iterator end() const { return issues_.end(); }

  size_type size() const { return issues_.size(); }
  bool empty() const { return issues_.empty(); }

  bool passed() const { return issues_.empty(); }

  duration elapsed() const { return elapsed_; }

  void set_elapsed(duration elapsed) { elapsed_ = elapsed; }

  static results &current() {
    results *p = current_results();
    assert(p);
    return *p;
  }

  static results &set_current(results &result) {
    current_results() = std::addressof(result);
    return result;
  }

private:
  issue_list issues_;
  duration elapsed_;

  static std::atomic<results *> &current_results() {
    static std::atomic<results *> current(nullptr);
    return current;
  }
};

namespace detail {

struct group_tag {};
struct name_tag {};
struct result_tag {};

struct abort_test_run_exception {};

#define FATAL_IMPL_NULLARY_TEST(Category, Predicate) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::make_tuple() \
  ).check()

#define FATAL_IMPL_UNARY_TEST(Category, Predicate, Expression) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::make_tuple(FATAL_TO_STR(Expression)) \
  ).check(Expression)

#define FATAL_IMPL_BINARY_TEST(Category, Predicate, LHS, RHS) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::make_tuple(FATAL_TO_STR(LHS), FATAL_TO_STR(RHS)) \
  ).check((LHS), (RHS))

#define FATAL_IMPL_BODY_TEST(Category, Predicate) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::make_tuple("<expression>") \
  ) << [&]()

#define FATAL_IMPL_TEST_WRAP_CHECK(Category, Predicate, ArgsTuple) \
  ::fatal::test::detail::make_check_wrapper< \
    ::fatal::test::detail::categories::Category \
  >( \
    ::fatal::test::detail::predicates::Predicate, \
    FATAL_SOURCE_INFO(), \
    ArgsTuple \
  )

template <std::size_t N, typename... Args>
struct build_args_tuple {
  static_assert(N > 0, "out of bounds");
  using type = typename build_args_tuple<N - 1, std::string, Args...>::type;
};

template <typename... Args>
struct build_args_tuple<0, Args...> {
  using type = std::tuple<Args...>;
};

template <typename TCategory, typename TPredicate, std::size_t Arity>
class check_wrapper {
  using predicate = TPredicate;
  using args_tuple = typename build_args_tuple<Arity>::type;
  using category = TCategory;

public:
  template <typename UPredicate, typename TArgsTuple>
  check_wrapper(
    UPredicate &&predicate,
    source_info source,
    TArgsTuple &&args
  ):
    predicate_(std::forward<UPredicate>(predicate)),
    source_(source),
    args_(std::forward<TArgsTuple>(args))
  {}

  template <typename... Args>
  void check(Args &&...args) {
    auto const passed = predicate_(args...);

    if (!passed) {
      issue result(
        category::severity(), clock::now(), source_,
        category::name(), " failed: "
      );

      // TODO: properly build message based on arity

      results::current().add(std::move(result));

      if (category::severity() == issue::severity::fatal) {
        throw abort_test_run_exception();
      }
    }
  }

  template <typename T>
  void operator <<(T &&what) {
    check(std::forward<T>(what));
  }

private:
  predicate const predicate_;
  source_info const source_;
  args_tuple const args_;
};

template <typename TCategory, typename TPredicate, typename TArgsTuple>
check_wrapper<
  TCategory,
  typename std::decay<TPredicate>::type,
  std::tuple_size<typename std::decay<TArgsTuple>::type>::value
> make_check_wrapper(
  TPredicate &&predicate,
  source_info source,
  TArgsTuple &&args
) {
  return {
    std::forward<TPredicate>(predicate),
    source,
    std::forward<TArgsTuple>(args)
  };
}

namespace categories {

struct warning {
  static issue::severity severity() { return issue::severity::warning; }
  static char const *name() { return "warning"; }
};

struct expectation {
  static issue::severity severity() { return issue::severity::error; }
  static char const *name() { return "expectation"; }
};

struct assertion {
  static issue::severity severity() { return issue::severity::fatal; }
  static char const *name() { return "assertion"; }
};

} // namespace categories {

namespace predicates {

template <typename TLHS, typename TRHS>
using both_integral = std::integral_constant<
  bool,
  std::is_integral<typename std::decay<TLHS>::type>::value
    && std::is_integral<typename std::decay<TRHS>::type>::value
>;

template <typename T, typename TLHS, typename TRHS, bool Expected>
using if_both_integral = typename std::enable_if<
  both_integral<TLHS, TRHS>::value == Expected,
  T
>::type;

template <typename TLHS, typename TRHS>
using common = typename std::common_type<
  typename std::decay<TLHS>::type,
  typename std::decay<TRHS>::type
>::type;

struct is_true {
  template <typename T>
  bool operator ()(T &&what) const { return static_cast<bool>(what); }
  char const *text() const { return "is true"; }
};

struct is_false {
  template <typename T>
  bool operator ()(T &&what) const { return !what; }
  char const *text() const { return "is false"; }
};

struct is_null {
  template <typename T>
  bool operator ()(T &&what) const { return what == nullptr; }
  char const *text() const { return "is null"; }
};

struct not_null {
  template <typename T>
  bool operator ()(T &&what) const { return what != nullptr; }
  char const *text() const { return "is not null"; }
};

struct is_equal {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs == rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) == static_cast<type>(rhs);
  }

  char const *text() const { return "is equal to"; }
};

struct not_equal {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs != rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) != static_cast<type>(rhs);
  }

  char const *text() const { return "is not equal to"; }
};

struct less_than {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs < rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) < static_cast<type>(rhs);
  }

  char const *text() const { return "is less than"; }
};

struct less_equal {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs <= rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) <= static_cast<type>(rhs);
  }

  char const *text() const { return "is less than or equal to"; }
};

struct greater_than {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs > rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) > static_cast<type>(rhs);
  }

  char const *text() const { return "is greater than"; }
};

struct greater_equal {
  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, false> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const { return lhs >= rhs; }

  template <typename TLHS, typename TRHS>
  if_both_integral<bool, TLHS, TRHS, true> operator ()(
    TLHS &&lhs, TRHS &&rhs
  ) const {
    using type = common<TLHS, TRHS>;
    return static_cast<type>(lhs) >= static_cast<type>(rhs);
  }

  char const *text() const { return "is greater than or equal to"; }
};

struct no_throw {
  template <typename T>
  bool operator ()(T &&what) const {
    try {
      what();
      return true;
    } catch (...) {
      return false;
    }
  }

  char const *text() const { return "does not throw"; }
};

template <typename TException>
struct exception {
  explicit exception(char const *name):
    text_("throws ")
  {
    text_.append(name);
  }

  template <typename T>
  bool operator ()(T &&what) const {
    try {
      what();
    } catch (TException const &) {
      return true;
    } catch (...) {
    }

    return false;
  }

  std::string text() const { return text_; }

private:
  std::string text_;
};

} // namespace predicates {

///////////////////////
// FATAL_EXPECT_SAME //
///////////////////////

struct expect_same_impl {
  explicit expect_same_impl(source_info source): source_(source) {}

  template <typename TExpected, typename TActual>
  void check(
    char const *file_name = __FILE__,
    std::size_t line_number = __LINE__
  ) {
    // TODO: reimplement using basic test framework
    if (!std::is_same<TExpected, TActual>::value) {
      std::cerr << "at " << file_name << ':' << line_number << ':'
        << std::endl << std::endl
        << "  expected: '" << type_str<TExpected>() << '\''
        << std::endl << std::endl
        << "  actual:   '" << type_str<TActual>() << '\''
        << std::endl << std::endl;

      using result = std::is_same<TExpected, TActual>;

      FATAL_EXPECT_TRUE(result::value);
    }
  }

private:
  source_info source_;
};

class registry {
  struct entry {
    virtual results run() = 0;
    virtual char const *group() = 0;
    virtual char const *name() = 0;
  };

  template <typename T>
  struct entry_impl:
    public entry
  {
    using type = T;

    results run() override {
      type subject;

      auto &result = results::set_current(result_tag() << subject);

      auto const start = clock::now();
      try {
        subject();
      } catch (abort_test_run_exception const &) {
      }
      auto const elapsed = clock::now() - start;

      result.set_elapsed(elapsed);

      return result;
    }

    char const *group() override {
      return group_tag() << static_cast<type *>(nullptr);
    }

    char const *name() override {
      return name_tag() << static_cast<type *>(nullptr);
    }
  };

public:
  using case_map = std::unordered_map<std::string, results>;
  using result_map = std::unordered_map<std::string, case_map>;
  using run_result = std::pair<result_map, bool>;
  using size_type = std::size_t;

  registry(): size_(0) {}

  template <typename T>
  size_type add() {
    std::unique_ptr<entry> e(new entry_impl<T>());

    auto i = groups_.find(e->group());
    if (i == groups_.end()) {
      auto const index = entries_.size();
      groups_[e->group()] = index;

      entries_.emplace_back();
      entries_.back().emplace_back(std::move(e));

      assert(entries_.size() == index + 1);
    } else {
      entries_[i->second].emplace_back(std::move(e));
    }

    return ++size_;
  }

  template <typename TPrinter, typename TOut>
  run_result run(TOut &out) const {
    run_result summary;

    TPrinter printer;

    printer.start_run(out, size_, groups_.size());

    duration running_time(0);
    size_type passed = 0;
    size_type total = 0;

    for (auto const &g: groups_) {
      assert(g.second < entries_.size());
      auto const &group = entries_[g.second];

      duration group_time(0);

      printer.start_group(out, g.first);

      for (auto const &i: group) {
        printer.start_test(out, i->name());

        auto result = i->run();

        printer.end_test(out, i->name(), result);

        group_time += result.elapsed();
        ++total;

        if (result.passed()) {
          ++passed;
        }

        summary.first[i->group()][i->name()] = std::move(result);
      }

      running_time += group_time;

      printer.end_group(out, g.first, group_time);
    }

    printer.end_run(out, passed, total, running_time);

    summary.second = passed == total;

    return summary;
  }

  size_type size() const { return size_; }

  static registry &get() {
    static registry instance;
    return instance;
  }

private:
  std::vector<std::vector<std::unique_ptr<entry>>> entries_;
  std::unordered_map<std::string, std::size_t> groups_;
  size_type size_;
};

} // namespace detail {

struct default_printer {
  template <typename TOut>
  void start_run(TOut &out, std::size_t total, std::size_t groups) {
    out << "running " << total << " tests from " << groups << " test cases\n";
  }

  template <typename TOut>
  void end_run(
    TOut &out, std::size_t passed, std::size_t total, duration time
  ) {
    out << '\n' << (passed == total ? "succeeded" : "FAILED")
      << ": passed " << passed << '/' << total << " after "
      << time.count() << ' ' << time::suffix(time) << "\n\n";
  }

  template <typename TOut, typename TGroup>
  void start_group(TOut &out, TGroup const &group) {
    out << "\n== test case: " << group << " ==\n";
  }

  template <typename TOut, typename TGroup>
  void end_group(TOut &out, TGroup const &group, duration time) {
  }

  template <typename TOut, typename TName>
  void start_test(TOut &out, TName const &name) {
    out << ">> test '" << name << "':\n";
  }

  template <typename TOut, typename TName>
  void end_test(TOut &out, TName const &name, results const &result) {
    for (auto const &i: result) {
      i.print(out);
      out << '\n';
    }

    out << "<< " << (result.passed() ? "succeeded" : "failed")
      << " after " << result.elapsed().count() << ' '
      << time::suffix(result.elapsed()) << "\n\n";
  }
};

template <typename TPrinter = default_printer, typename TOut>
int run(TOut &out) {
  auto const result = detail::registry::get().run<TPrinter>(out);

  return result.second ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace test {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_lib_h
