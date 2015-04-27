/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_test_h
#define FATAL_INCLUDE_fatal_test_test_h

#include <fatal/preprocessor.h>
#include <fatal/test/type.h>
#include <fatal/time/time.h>
#include <fatal/type/traits.h>

#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>

// for internal use only

namespace fatal {
namespace test {

// TODO: organize the order of components

#define FATAL_TEST(Case, Name) \
  FATAL_IMPL_TEST_CASE( \
    FATAL_UID(fatal_test_case_impl), \
    FATAL_TO_STR(FATAL_UNPARENTHESIZE(Case)), \
    FATAL_TO_STR(FATAL_UNPARENTHESIZE(Name)), \
    FATAL_UID(fatal_test_case_results_impl), \
    FATAL_UID(fatal_test_case_test_registry_placeholder_impl) \
  )

#define FATAL_IMPL_TEST_CASE(Class, Case, Name, Results, RegistryPlaceholder) \
  struct Class { \
    void operator ()(); \
    \
    ::fatal::test::results Results; \
  }; \
  \
  static auto const RegistryPlaceholder \
    = ::fatal::test::detail::test_impl::registry::get().add<Class>(); \
  \
  std::size_t operator <<( \
    ::fatal::test::detail::test_impl::registry_index_tag, \
    Class * \
  ) { \
    return RegistryPlaceholder; \
  } \
  \
  char const *operator <<( \
    ::fatal::test::detail::test_impl::group_tag, \
    Class * \
  ) { \
    return Case; \
  } \
  \
  char const *operator <<( \
    ::fatal::test::detail::test_impl::name_tag, \
    Class * \
  ) { \
    return Name; \
  } \
  \
  source_info const &operator <<( \
    ::fatal::test::detail::test_impl::source_tag, \
    Class * \
  ) { \
    static auto source = FATAL_SOURCE_INFO(); \
    return source; \
  } \
  \
  ::fatal::test::results &operator <<( \
    ::fatal::test::detail::test_impl::result_tag, Class &subject \
  ) { \
    return subject.Results; \
  } \
  \
  void Class::operator ()()

#define FATAL_WARN_UNREACHABLE() \
  FATAL_IMPL_NULLARY_TEST(warning, unreachable())

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
  FATAL_IMPL_NULLARY_TEST(expectation, unreachable())

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
  FATAL_IMPL_NULLARY_TEST(assertion, unreachable())

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
  ::fatal::test::detail::test_impl::expect_same_impl( \
    FATAL_SOURCE_INFO() \
  ).check

using clock = std::chrono::system_clock;
using duration = clock::duration;
using timestamp = clock::time_point;

// TODO: ISSUE SHOULD NOT BUILD AN INTERNAL STRING BUT RATHER A HIERARCHICAL MAP
//       TO ALLOW PRETTY PRINTERS TO CHOOSE THE FORMATTING
struct test_issue {
  enum class severity_t { warning, error, fatal };

  template <typename... Args>
  test_issue(
    severity_t severity,
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

  template <typename... Args>
  std::string const &append(Args &&...args) {
    return append_to_string(message_, std::forward<Args>(args)...);
  }

  severity_t severity() const { return severity_; }
  timestamp timestamp() const { return timestamp_; }
  source_info const &source() const { return source_; }
  std::string const &message() const { return message_; }

  char severity_signature() const {
    switch (severity_) {
      case severity_t::warning: return 'W';
      case severity_t::error: return 'E';
      case severity_t::fatal: return 'F';
      default:
        assert(false);
        return '\0';
    }
  }

private:
  severity_t const severity_;
  test::timestamp const timestamp_;
  source_info const source_;
  std::string message_;
};

class results {
  using issue_list = std::vector<test_issue>;

public:
  using const_iterator = issue_list::const_iterator;
  using size_type = issue_list::size_type;

  test_issue const &add(test_issue i) {
    issues_.push_back(std::move(i));
    return issues_.back();
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

private:
  issue_list issues_;
  duration elapsed_;

};

struct controller {
  using issue_sink = std::function<void(test_issue &&)>;

  static void add_issue(test_issue &&i) {
    auto &sink = current_issues_sink();
    assert(sink);
    sink(std::move(i));
  }

  template <typename T>
  static void set_issues_sink(T &&sink) {
    current_issues_sink() = std::forward<T>(sink);
  }

private:
  static issue_sink &current_issues_sink() {
    static issue_sink sink;
    return sink;
  }
};

namespace detail {
namespace test_impl {

struct registry_index_tag {};
struct group_tag {};
struct name_tag {};
struct source_tag {};
struct result_tag {};

struct abort_test_run_exception {};

#define FATAL_IMPL_NULLARY_TEST(Category, Predicate) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::tuple<>() \
  ).check()

#define FATAL_IMPL_UNARY_TEST(Category, Predicate, Expression) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::tuple<char const *>(FATAL_TO_STR(Expression)) \
  ).check(Expression)

#define FATAL_IMPL_BINARY_TEST(Category, Predicate, LHS, RHS) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    (::std::tuple<char const *, char const *>( \
      FATAL_TO_STR(LHS), \
      FATAL_TO_STR(RHS) \
    )) \
  ).check((LHS), (RHS))

#define FATAL_IMPL_BODY_TEST(Category, Predicate) \
  FATAL_IMPL_TEST_WRAP_CHECK( \
    Category, Predicate, \
    ::std::tuple<char const *>("<expression>") \
  ) << [&]()

#define FATAL_IMPL_TEST_WRAP_CHECK(Category, Predicate, ArgsTuple) \
  ::fatal::test::detail::test_impl::make_check_wrapper< \
    ::fatal::test::detail::test_impl::categories::Category \
  >( \
    ::fatal::test::detail::test_impl::predicates::Predicate, \
    FATAL_SOURCE_INFO(), \
    ArgsTuple \
  )

template <
  typename T,
  // TODO: don't use detail from another header
  typename TImpl = fatal::detail::string_impl::parse_impl_conversion_pair<
    std::string, T
  >,
  bool = fatal::is_complete<TImpl>::value
>
struct any_to_string_impl {
  static void append(std::string &out, T const &value) {
    if (TImpl::quote::value) {
      out.push_back(TImpl::quote::value);
    }

    TImpl::append(out, value);

    if (TImpl::quote::value) {
      out.push_back(TImpl::quote::value);
    }
  }
};

template <typename T, typename TImpl>
struct any_to_string_impl<T, TImpl, false> {
  static void append(std::string &out, T const &value) {
    out.append("<instance:");
    out.append(type_str<T>());
    out.push_back('>');
  }
};

template <typename T>
std::string &any_to_string(std::string &out, T const &value) {
  any_to_string_impl<T>::append(out, value);
  return out;
}

template <typename T>
std::string any_to_string(T const &value) {
  std::string result;
  any_to_string(result, value);
  return result;
}

// TODO: MOVE ELSEWHERE??

void handle_exception(test_issue &issue) {
  try {
    auto e = std::current_exception();
    assert(e);
    std::rethrow_exception(e);
  } catch (std::exception const &e) {
    issue.append(
      '(', type_str(typeid(e)), "): \"",
      e.what() ? e.what() : "<no message>", '"'
    );
  } catch (std::string const &e) {
    issue.append("(std::string): \"", e, '"');
  } catch (char const *e) {
    issue.append("(char const *): \"", e, '"');
  } catch (long long e) {
    issue.append("(long long): \"", e, '"');
  } catch (unsigned long long e) {
    issue.append("(unsigned long long): \"", e, '"');
  } catch (long double e) {
    issue.append("(long double): \"", e, '"');
  // TODO: ADD MORE TYPES
  } catch (...) {
    issue.append("(<unknown exception>)");
  }
}

template <typename TCategory, typename TPredicate, typename TArgsTuple>
class check_wrapper {
  using predicate = TPredicate;
  using args_tuple = TArgsTuple;
  using category = TCategory;

public:
  template <typename UPredicate, typename UArgsTuple>
  check_wrapper(
    UPredicate &&predicate,
    source_info source,
    UArgsTuple &&args
  ):
    predicate_(std::forward<UPredicate>(predicate)),
    source_(source),
    args_(std::forward<UArgsTuple>(args))
  {}

  template <typename... Args>
  void check(Args &&...args) {
    test_issue issue(category::severity(), clock::now(), source_);

    try {
      if (predicate_(args...)) {
        return;
      }

      issue.append(category::name(), " failed: ");

      // TODO: properly build message based on arity
      append_args(issue, args_, args...);
    } catch (...) {
      issue.append("unexpected exception ");
      handle_exception(issue);
    }

    controller::add_issue(std::move(issue));

    if (category::severity() == test_issue::severity_t::fatal) {
      throw abort_test_run_exception();
    }
  }

  template <typename T>
  void operator <<(T &&what) {
    check(std::forward<T>(what));
  }

private:
  // TODO: CLEANUP - see TODO on unreachable::text()
  void append_args(test_issue &issue, std::tuple<> const &) {
    issue.append("\n    ", predicate_.text());
  }

  // TODO: CLEANUP - see TODO on unreachable::text()
  template <typename Value>
  void append_args(
    test_issue &issue,
    std::tuple<char const *> const &args,
    Value const &value
  ) {
    issue.append(
      "\n    ", std::get<0>(args), " -> ", any_to_string(value),
      "\n      ", predicate_.text()
    );
  }

  // TODO: CLEANUP - see TODO on unreachable::text()
  template <typename LHS, typename RHS>
  void append_args(
    test_issue &issue,
    std::tuple<char const *, char const *> const &args,
    LHS const &lhs,
    RHS const &rhs
  ) {
    issue.append(
      "\n    lhs: ", std::get<0>(args), " -> ", any_to_string(lhs),
      "\n      ", predicate_.text(),
      "\n    rhs: ", std::get<1>(args), " -> ", any_to_string(rhs)
    );
  }

  predicate const predicate_;
  source_info const source_;
  args_tuple const args_;
};

template <typename TCategory, typename TPredicate, typename TArgsTuple>
check_wrapper<
  TCategory,
  typename std::decay<TPredicate>::type,
  typename std::decay<TArgsTuple>::type
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
  static test_issue::severity_t severity() {
    return test_issue::severity_t::warning;
  }
  static char const *name() { return "warning"; }
};

struct expectation {
  static test_issue::severity_t severity() {
    return test_issue::severity_t::error;
  }
  static char const *name() { return "expectation"; }
};

struct assertion {
  static test_issue::severity_t severity() {
    return test_issue::severity_t::fatal;
  }
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

struct unreachable {
  bool operator ()() const { return false; }
  // TODO: change to `void format(test_issue &i, tuple<*> names, args...)`
  //       with good defaults for nullary/unary/binary/...
  char const *text() const { return "unreachable"; }
};

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

  char const *text() const { return text_.c_str(); }

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
    using issue_sink = std::function<void(test_issue const &)>;

    virtual ~entry() {}

    virtual results run(issue_sink sink) = 0;

    virtual char const *group() = 0;
    virtual char const *name() = 0;
    virtual source_info const &source() = 0;
  };

  template <typename T>
  struct entry_impl:
    public entry
  {
    using type = T;

    results run(issue_sink sink) override {
      type subject;

      auto &result = (result_tag() << subject);

      controller::set_issues_sink(
        [&](test_issue &&issue) {
          sink(result.add(std::move(issue)));
        }
      );

      auto const start = clock::now();
      try {
        subject();
      } catch (abort_test_run_exception const &) {
        // test run aborted
      } catch (...) {
        test_issue issue(test_issue::severity_t::error, clock::now(), source());
        issue.append("test case aborted by unexpected exception ");
        handle_exception(issue);
        result.add(std::move(issue));
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

    source_info const &source() override {
      return source_tag() << static_cast<type *>(nullptr);
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
      groups_order_.push_back(e->group());
      groups_.emplace(groups_order_.back(), index);

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

    printer.start_run(out, size_, groups_.size(), clock::now());

    duration running_time(0);
    size_type passed = 0;
    size_type total = 0;

    for (auto const &order: groups_order_) {
      auto g = groups_.find(order);
      assert(g != groups_.end());
      assert(g->second < entries_.size());
      auto const &group = entries_[g->second];

      duration group_time(0);

      printer.start_group(out, g->first, clock::now());

      for (auto const &i: group) {
        printer.start_test(out, i->name(), i->source(), clock::now());

        std::size_t issues = 0;
        auto result = i->run(
          [&](test_issue const &issue) {
            printer.issue(out, i->name(), i->source(), issue, issues);
          }
        );

        printer.end_test(out, result, i->name(), i->source());

        group_time += result.elapsed();
        ++total;

        if (result.passed()) {
          ++passed;
        }

        summary.first[i->group()][i->name()] = std::move(result);
      }

      running_time += group_time;

      printer.end_group(out, g->first, group_time);
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
  std::vector<std::string> groups_order_;
  std::unordered_map<std::string, std::size_t> groups_;
  size_type size_;
};

} // namespace test_impl {
} // namespace detail {

struct default_printer {
  template <typename TOut>
  void start_run(
    TOut &out, std::size_t total, std::size_t groups, timestamp start
  ) {
    // TODO: pretty print test run start time??
    out << "running " << total << " tests from " << groups << " test cases\n";
    run_start_ = start;
  }

  template <typename TOut, typename TGroup>
  void start_group(TOut &out, TGroup const &group, timestamp start) {
    auto const time = start - run_start_;

    // TODO: pretty print test group start time
    out << "\n== test case: '" << group << "' at "
      << time.count() << time::suffix(time) << " ==\n";

    group_start_ = start;
  }

  template <typename TOut, typename TName>
  void start_test(
    TOut &out, TName const &name, source_info const &source, timestamp start
  ) {
    auto const time = start - group_start_;

    // TODO: pretty print unit test start time
    out << ">> test '" << name << "' (" << source.file() << ':' << source.line()
      << ") at " << time.count() << time::suffix(time) << ":\n";

    test_start_ = start;
  }

  template <typename TOut, typename TName>
  void issue(
    TOut &out, TName const &name, source_info const &source,
    test_issue const &i, std::size_t index
  ) {
    if (index) {
      out << '\n';
    }

    auto const time = i.timestamp() - test_start_;

    out << i.severity_signature()
      << " [" << i.source().file() << ':' << i.source().line() << "] at "
      << time.count() << time::suffix(time) << ":\n  " << i.message() << '\n';
  }

  template <typename TOut, typename TName>
  void end_test(
    TOut &out, results const &result,
    TName const &name, source_info const &source
  ) {
    out << "<< " << (result.passed() ? "succeeded" : "failed")
      << " after " << result.elapsed().count() << ' '
      << time::suffix(result.elapsed()) << "\n\n";
  }

  template <typename TOut, typename TGroup>
  void end_group(TOut &out, TGroup const &group, duration time) {
  }

  template <typename TOut>
  void end_run(
    TOut &out, std::size_t passed, std::size_t total, duration time
  ) {
    out << '\n' << (passed == total ? "succeeded" : "FAILED")
      << ": passed " << passed << '/' << total << " after "
      << time.count() << ' ' << time::suffix(time) << "\n\n";
  }

private:
  timestamp run_start_;
  timestamp group_start_;
  timestamp test_start_;
};

template <typename TPrinter = default_printer, typename TOut>
int run(TOut &out) {
  auto const result = detail::test_impl::registry::get().run<TPrinter>(out);

  return result.second ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace test {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_test_h
