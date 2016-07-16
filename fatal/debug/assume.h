/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/debug/debug.h>
#include <fatal/log/log.h>

#ifndef FATAL_INCLUDE_fatal_debug_assume_h
#define FATAL_INCLUDE_fatal_debug_assume_h

namespace fatal {

// TODO: allow assumption on types that don't have an `out << value` overload

/////////////////////////
// nullary assumptions //
/////////////////////////

#define FATAL_UNREACHABLE FATAL_NULLARY_ASSUMPTION_IMPL( \
  ::fatal::fn::constant<std::false_type>, "unreachable code reached" \
)

///////////////////////
// unary assumptions //
///////////////////////

#define FATAL_IS_ZERO(Value) FATAL_EQ(0, Value)
#define FATAL_NOT_ZERO(Value) FATAL_NE(0, Value)

#define FATAL_IS_FALSE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::negate, "is false", Value \
)

#define FATAL_IS_TRUE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::static_caster<bool>, "is true", Value \
)

#define FATAL_IS_POSITIVE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::is_positive, "is positive", Value \
)

#define FATAL_IS_NEGATIVE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::is_negative, "is negative", Value \
)

#define FATAL_NOT_NEGATIVE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::not_negative, "not negative", Value \
)

#define FATAL_NOT_POSITIVE(Value) FATAL_UNARY_ASSUMPTION_IMPL( \
  ::fatal::fn::not_positive, "not positive", Value \
)

#define FATAL_IS_NULL(Value) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::equal, "is null", nullptr, Value \
)

#define FATAL_NOT_NULL(Value) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::not_equal, "not null", nullptr, Value \
)

////////////////////////
// binary assumptions //
////////////////////////

#define FATAL_EQ(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::equal, "equal to", LHS, RHS \
)
#define FATAL_NE(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::not_equal, "not equal to", LHS, RHS \
)
#define FATAL_LT(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::less, "less than", LHS, RHS \
)
#define FATAL_LE(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::less_equal, "less than or equal to", LHS, RHS \
)
#define FATAL_GT(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::greater, "greater than", LHS, RHS \
)
#define FATAL_GE(LHS, RHS) FATAL_BINARY_ASSUMPTION_IMPL( \
  ::fatal::fn::greater_equal, "greater than or equal to", LHS, RHS \
)

////////////////////////////
// assumption aggregators //
////////////////////////////

// TODO: MAKE IT POSSIBLE TO NEST AGGREGATORS

// TODO: ALLOW SHORT-CIRCUITING

#define FATAL_ASSUME_SINGLE(Assumption) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_single(Assumption) \
)

#define FATAL_ASSUME_ONE(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_one(__VA_ARGS__) \
)

#define FATAL_ASSUME_ANY(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_any(__VA_ARGS__) \
)

#define FATAL_ASSUME_ALL(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_all(__VA_ARGS__) \
)

#define FATAL_ASSUME_NONE(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_none(__VA_ARGS__) \
)

#define FATAL_ASSUME_ALL_OR_NONE(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_all_or_none(__VA_ARGS__) \
)

#define FATAL_ASSUME_IF(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_if(__VA_ARGS__) \
)

#define FATAL_ASSUME_IF_ELSE(...) FATAL_DEBUG_STATEMENT( \
  ::fatal::detail::assume_impl::assume_if_else(__VA_ARGS__) \
)

////////////////////////////////////
// standalone nullary assumptions //
////////////////////////////////////

#define FATAL_ASSUME_UNREACHABLE FATAL_ASSUME_SINGLE(FATAL_UNREACHABLE)

//////////////////////////////////
// standalone unary assumptions //
//////////////////////////////////

#define FATAL_ASSUME_ZERO(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_ZERO(Value))
#define FATAL_ASSUME_NOT_ZERO(Value) \
  FATAL_ASSUME_SINGLE(FATAL_NOT_ZERO(Value))
#define FATAL_ASSUME_FALSE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_FALSE(Value))
#define FATAL_ASSUME_TRUE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_TRUE(Value))
#define FATAL_ASSUME_NULL(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_NULL(Value))
#define FATAL_ASSUME_NOT_NULL(Value) \
  FATAL_ASSUME_SINGLE(FATAL_NOT_NULL(Value))
#define FATAL_ASSUME_POSITIVE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_POSITIVE(Value))
#define FATAL_ASSUME_NEGATIVE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_IS_NEGATIVE(Value))
#define FATAL_ASSUME_NOT_NEGATIVE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_NOT_NEGATIVE(Value))
#define FATAL_ASSUME_NOT_POSITIVE(Value) \
  FATAL_ASSUME_SINGLE(FATAL_NOT_POSITIVE(Value))

///////////////////////////////////
// standalone binary assumptions //
///////////////////////////////////

#define FATAL_ASSUME_EQ(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_EQ(LHS, RHS))
#define FATAL_ASSUME_NE(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_NE(LHS, RHS))
#define FATAL_ASSUME_LT(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_LT(LHS, RHS))
#define FATAL_ASSUME_LE(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_LE(LHS, RHS))
#define FATAL_ASSUME_GT(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_GT(LHS, RHS))
#define FATAL_ASSUME_GE(LHS, RHS) FATAL_ASSUME_SINGLE(FATAL_GE(LHS, RHS))

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

// production build
#ifdef NDEBUG

# define FATAL_NULLARY_ASSUMPTION_IMPL(Assumption, Message) \
  ::fatal::fn::constant<std::true_type>

# define FATAL_UNARY_ASSUMPTION_IMPL(Assumption, Message, Value) \
  ::fatal::fn::constant<std::true_type>

# define FATAL_BINARY_ASSUMPTION_IMPL(Assumption, Message, LHS, RHS) \
  ::fatal::fn::constant<std::true_type>

// debug build
#else // NDEBUG

// TODO: USE source_info HERE
#define FATAL_NULLARY_ASSUMPTION_IMPL(Assumption, Message) \
  ::fatal::detail::assume_impl::create_nullary_assumption<Assumption>( \
    __FILE__, __LINE__, Message \
  )

#define FATAL_UNARY_ASSUMPTION_IMPL(Assumption, Message, Value) \
  ::fatal::detail::assume_impl::create_unary_assumption<Assumption>( \
    Value,  FATAL_TO_STR(Value), \
    __FILE__, __LINE__, Message \
  )

#define FATAL_BINARY_ASSUMPTION_IMPL(Assumption, Message, LHS, RHS) \
  ::fatal::detail::assume_impl::create_binary_assumption<Assumption>( \
    LHS,  RHS, FATAL_TO_STR(LHS), FATAL_TO_STR(RHS), \
    __FILE__, __LINE__, Message \
  )

namespace detail {
namespace assume_impl {

////////////////////////
// nullary_assumption //
////////////////////////

template <typename Assumption>
struct nullary_assumption {
  nullary_assumption(char const *file, long long line, char const *assumption):
    file_(file), line_(line), assumption_(assumption)
  {}

  template <typename TOut>
  TOut &&print(TOut &&out) const {
    out << "\nassumption " << (eval() ? "succeeded" : "failed") << " at "
      << file_ << ':' << line_ << ":\n\t" << assumption_;

    return std::forward<TOut>(out);
  }

  bool eval() const {
    return Assumption()();
  }

  explicit operator bool() const { return eval(); }

private:
  char const *file_;
  long long line_;
  char const *assumption_;
};

template <typename Assumption, typename... Args>
nullary_assumption<Assumption> create_nullary_assumption(Args &&...args) {
  return nullary_assumption<Assumption>(std::forward<Args>(args)...);
}

//////////////////////
// unary_assumption //
//////////////////////

template <typename Assumption, typename TValue>
struct unary_assumption {
  unary_assumption(
    TValue const &value, char const *value_str,
    char const *file, long long line, char const *assumption
  ):
    value_(value), value_str_(value_str),
    file_(file), line_(line), assumption_(assumption)
  {}

  template <typename TOut>
  TOut &&print(TOut &&out) const {
    out << "\nassumption " << (eval() ? "succeeded" : "failed") << " at "
      << file_ << ':' << line_ << ':'
      << "\n\t" << assumption_
      << "\n\t'" << value_str_ << "' with value '" << value_ << '\'';

    return std::forward<TOut>(out);
  }

  bool eval() const {
    return Assumption()(value_);
  }

  explicit operator bool() const { return eval(); }

private:
  TValue const &value_;
  char const *value_str_;

  char const *file_;
  long long line_;
  char const *assumption_;
};

template <typename Assumption, typename TValue, typename... Args>
unary_assumption<Assumption, TValue> create_unary_assumption(
  TValue const &value, Args &&...args
) {
  return unary_assumption<Assumption, TValue>(
    value, std::forward<Args>(args)...
  );
}

///////////////////////
// binary_assumption //
///////////////////////

template <typename Assumption, typename TLHS, typename TRHS>
struct binary_assumption {
  binary_assumption(
    TLHS const &lhs, TRHS const &rhs,
    char const *lhs_str, char const *rhs_str,
    char const *file, long long line, char const *assumption
  ):
    lhs_(lhs), rhs_(rhs),
    lhs_str_(lhs_str), rhs_str_(rhs_str),
    file_(file), line_(line), assumption_(assumption)
  {}

  template <typename TOut>
  TOut &&print(TOut &&out) const {
    out << "\nassumption " << (eval() ? "succeeded" : "failed") << " at "
      << file_ << ':' << line_ << ':'
      << "\n\t'" << lhs_str_ << "' with value '" << lhs_ << '\''
      << "\n\t" << assumption_
      << "\n\t'" << rhs_str_ << "' with value '" << rhs_ << '\'';

    return std::forward<TOut>(out);
  }

  bool eval() const {
    return Assumption()(lhs_, rhs_);
  }

  explicit operator bool() const { return eval(); }

private:
  TLHS const &lhs_;
  TRHS const &rhs_;
  char const *lhs_str_;
  char const *rhs_str_;
  char const *file_;
  long long line_;
  char const *assumption_;
};

template <typename Assumption, typename TLHS, typename TRHS, typename... Args>
binary_assumption<Assumption, TLHS, TRHS> create_binary_assumption(
  TLHS const &lhs, TRHS const &rhs, Args &&...args
) {
  return binary_assumption<Assumption, TLHS, TRHS>(
    lhs, rhs, std::forward<Args>(args)...
  );
}

///////////////////////
// print_assumptions //
///////////////////////

template <typename TOut>
void print_assumptions(TOut &&) {}

template <typename TOut, typename T, typename... Args>
void print_assumptions(TOut &&out, T &&assumption, Args &&...args) {
  assumption.print(out);
  print_assumptions(std::forward<TOut>(out), std::forward<Args>(args)...);
}

//////////////////////////
// print_assumptions_if //
//////////////////////////

template <typename TOut>
void print_assumptions_if(TOut &&, bool) {}

template <typename TOut, typename T, typename... Args>
void print_assumptions_if(
  TOut &&out, bool condition, T &&assumption, Args &&...args
) {
  if (assumption.eval() == condition) {
    assumption.print(out);
  }

  print_assumptions_if(
    std::forward<TOut>(out), condition, std::forward<Args>(args)...
  );
}

///////////////////////
// count_assumptions //
///////////////////////

template <typename T>
std::size_t count_assumptions(T &&assumption) { return assumption.eval(); }

template <typename T, typename... Args>
std::size_t count_assumptions(T &&assumption, Args &&...args) {
  return assumption.eval() + count_assumptions(std::forward<Args>(args)...);
}

///////////////////
// assume_single //
///////////////////

template <typename T>
void assume_single(T &&assumption) {
  if (!assumption.eval()) {
    assumption.print(FATAL_LOG(FATAL));
    std::abort();
  }
}

////////////////
// assume_one //
////////////////

template <typename... Args>
void assume_one(Args &&...args) {
  auto const n = count_assumptions(args...);

  if (n != 1) {
    print_assumptions(
      FATAL_LOG(FATAL) << "expected exactly one assumption to be true, got "
        << n,
      std::forward<Args>(args)...
    );
    std::abort();
  }
}

////////////////
// assume_any //
////////////////


inline bool assume_any_impl() { return false; }

template <typename T, typename... Args>
bool assume_any_impl(T &&assumption, Args &&...args) {
  if (assumption.eval()) {
    return true;
  }

  return assume_any_impl(std::forward<Args>(args)...);
}

template <typename... Args>
void assume_any(Args &&...args) {
  if (!assume_any_impl(std::forward<Args>(args)...)) {
    print_assumptions(
      FATAL_LOG(FATAL) << "expected at least one assumption to be true",
      std::forward<Args>(args)...
    );
    std::abort();
  }
}

////////////////
// assume_all //
////////////////

inline void assume_all_impl() {}

template <typename T, typename... Args>
void assume_all_impl(T &&assumption, Args &&...args) {
  if (!assumption.eval()) {
    print_assumptions_if(
      FATAL_LOG(FATAL) << "expected all assumptions to be true",
      false,
      std::forward<T>(assumption),
      std::forward<Args>(args)...
    );
    std::abort();
  } else {
    assume_all_impl(std::forward<Args>(args)...);
  }
}

template <typename T, typename... Args>
void assume_all(T &&assumption, Args &&...args) {
  assume_all_impl(std::forward<T>(assumption), std::forward<Args>(args)...);
}

/////////////////
// assume_none //
/////////////////

inline void assume_none_impl() {}

template <typename T, typename... Args>
void assume_none_impl(T &&assumption, Args &&...args) {
  if (assumption.eval()) {
    print_assumptions_if(
      FATAL_LOG(FATAL) << "expected no assumptions to be true",
      true,
      std::forward<T>(assumption),
      std::forward<Args>(args)...
    );
    std::abort();
  } else {
    assume_none_impl(std::forward<Args>(args)...);
  }
}

template <typename T, typename... Args>
void assume_none(T &&assumption, Args &&...args) {
  assume_none_impl(std::forward<T>(assumption), std::forward<Args>(args)...);
}

////////////////////////
// assume_all_or_none //
////////////////////////

inline bool assume_all_or_none_impl(bool) { return true; }

template <typename T, typename... Args>
bool assume_all_or_none_impl(bool expected, T &&assumption, Args &&...args) {
  return (assumption.eval() == expected)
    && assume_all_or_none_impl(expected, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void assume_all_or_none(T &&assumption, Args &&...args) {
  auto const result = assume_all_or_none_impl(
    assumption.eval(),
    std::forward<T>(assumption),
    std::forward<Args>(args)...
  );

  if (!result) {
    print_assumptions(
      FATAL_LOG(FATAL) << "expected either all or no assumptions to be true",
      std::forward<T>(assumption),
      std::forward<Args>(args)...
    );
    std::abort();
  }
}

///////////////
// assume_if //
///////////////

template <typename TCondition, typename TWhenTrue>
void assume_if(TCondition &&condition, TWhenTrue &&when_true) {
  if (condition.eval() && !when_true.eval()) {
    print_assumptions(
      FATAL_LOG(FATAL) << "expected second assumption to be true since first"
        " is",
      std::forward<TCondition>(condition),
      std::forward<TWhenTrue>(when_true)
    );
    std::abort();
  }
}

////////////////////
// assume_if_else //
////////////////////

template <typename TCondition, typename TWhenTrue, typename TWhenFalse>
void assume_if_else(
  TCondition &&condition,
  TWhenTrue &&when_true,
  TWhenFalse &&when_false
) {
  if (condition.eval()) {
    if (!when_true.eval()) {
      print_assumptions(
        FATAL_LOG(FATAL) << "expected second assumption to be true since first"
          " is",
        std::forward<TCondition>(condition),
        std::forward<TWhenTrue>(when_true)
      );
      std::abort();
    }
  } else if (!when_false.eval()) {
    print_assumptions(
      FATAL_LOG(FATAL) << "expected third assumption to be true since first"
        " isn't",
      std::forward<TCondition>(condition),
      std::forward<TWhenFalse>(when_false)
    );
    std::abort();
  }
}

} // namespace assume_impl {
} // namespace detail {

#endif // NDEBUG

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_debug_assume_h
