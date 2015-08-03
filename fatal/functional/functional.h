/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_functional_functional_h
#define FATAL_INCLUDE_fatal_functional_functional_h

#include <memory>
#include <tuple>
#include <utility>

namespace fatal {
namespace fn {

// TODO: MAKE CALLABLES VARIADIC WHENEVER POSSIBLE (i.e.: logical_and...)

/**
 * A convenience callable that accepts any parameters and does nothing.
 * TODO: TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct no_op {
  template <typename... UArgs>
  void operator ()(UArgs &&...) const noexcept {}

  template <typename... UArgs>
  static void call(UArgs &&...) noexcept {}
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct identity {
  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename T>
  static constexpr T &&call(T &&value) noexcept {
    return std::forward<T>(value);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct constant {
  using value_type = typename T::value_type;

  template <typename... Args>
  constexpr value_type operator ()(Args &&...) const noexcept {
    return T::value;
  }

  template <typename... Args>
  static constexpr value_type call(Args &&...) noexcept {
    return T::value;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index>
struct std_get {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(std::get<Index>(std::forward<T>(value))))
    -> decltype(std::get<Index>(std::forward<T>(value)))
  {
    return std::get<Index>(std::forward<T>(value));
  }

  template <typename T>
  static constexpr auto call(T &&value)
    noexcept(noexcept(std::get<Index>(std::forward<T>(value))))
    -> decltype(std::get<Index>(std::forward<T>(value)))
  {
    return std::get<Index>(std::forward<T>(value));
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct static_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(static_cast<T>(std::forward<U>(value))))
  {
    return static_cast<T>(std::forward<U>(value));
  }

  template <typename U>
  static constexpr T call(U &&value)
    noexcept(noexcept(static_cast<T>(std::forward<U>(value))))
  {
    return static_cast<T>(std::forward<U>(value));
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct reinterpret_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(reinterpret_cast<T>(std::forward<U>(value))))
  {
    return reinterpret_cast<T>(std::forward<U>(value));
  }

  template <typename U>
  static constexpr T call(U &&value)
    noexcept(noexcept(reinterpret_cast<T>(std::forward<U>(value))))
  {
    return reinterpret_cast<T>(std::forward<U>(value));
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct dynamic_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(dynamic_cast<T>(std::forward<U>(value))))
  {
    return dynamic_cast<T>(std::forward<U>(value));
  }

  template <typename U>
  static constexpr T call(U &&value)
    noexcept(noexcept(dynamic_cast<T>(std::forward<U>(value))))
  {
    return dynamic_cast<T>(std::forward<U>(value));
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct dereferencer {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(*std::forward<T>(value)))
    -> decltype(*std::forward<T>(value))
  {
    return *std::forward<T>(value);
  }

  template <typename T>
  static constexpr auto call(T &&value)
    noexcept(noexcept(*std::forward<T>(value)))
    -> decltype(*std::forward<T>(value))
  {
    return *std::forward<T>(value);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct address_of {
  template <typename T>
  constexpr T *operator ()(T &value) const noexcept {
    return std::addressof(value);
  }

  template <typename T>
  static constexpr T *call(T &value) noexcept {
    return std::addressof(value);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs == rhs))
    -> decltype(lhs == rhs)
  {
    return lhs == rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs == rhs))
    -> decltype(lhs == rhs)
  {
    return lhs == rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct not_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs != rhs))
    -> decltype(lhs != rhs)
  {
    return lhs != rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs != rhs))
    -> decltype(lhs != rhs)
  {
    return lhs != rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs < rhs))
    -> decltype(lhs < rhs)
  {
    return lhs < rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs < rhs))
    -> decltype(lhs < rhs)
  {
    return lhs < rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs <= rhs))
    -> decltype(lhs <= rhs)
  {
    return lhs <= rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs <= rhs))
    -> decltype(lhs <= rhs)
  {
    return lhs <= rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs > rhs))
    -> decltype(lhs > rhs)
  {
    return lhs > rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs > rhs))
    -> decltype(lhs > rhs)
  {
    return lhs > rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs >= rhs))
    -> decltype(lhs >= rhs)
  {
    return lhs >= rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs >= rhs))
    -> decltype(lhs >= rhs)
  {
    return lhs >= rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct negate {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(!value))
    -> decltype(!value)
  {
    return !value;
  }

  template <typename T>
  static constexpr auto call(T &&value)
    noexcept(noexcept(!value))
    -> decltype(!value)
  {
    return !value;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct complement {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(~value))
    -> decltype(~value)
  {
    return ~value;
  }

  template <typename T>
  static constexpr auto call(T &&value)
    noexcept(noexcept(~value))
    -> decltype(~value)
  {
    return ~value;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct logical_and {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs && rhs))
    -> decltype(lhs && rhs)
  {
    return lhs && rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs && rhs))
    -> decltype(lhs && rhs)
  {
    return lhs && rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct logical_or {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs || rhs))
    -> decltype(lhs || rhs)
  {
    return lhs || rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs || rhs))
    -> decltype(lhs || rhs)
  {
    return lhs || rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_and {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs & rhs))
    -> decltype(lhs & rhs)
  {
    return lhs & rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs & rhs))
    -> decltype(lhs & rhs)
  {
    return lhs & rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_or {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs | rhs))
    -> decltype(lhs | rhs)
  {
    return lhs | rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs | rhs))
    -> decltype(lhs | rhs)
  {
    return lhs | rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_xor {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(lhs ^ rhs))
    -> decltype(lhs ^ rhs)
  {
    return lhs ^ rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr auto call(LHS &&lhs, RHS &&rhs)
    noexcept(noexcept(lhs ^ rhs))
    -> decltype(lhs ^ rhs)
  {
    return lhs ^ rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct ternary {
  template <typename C, typename T, typename F>
  constexpr auto operator ()(C &&c, T &&t, F &&f) const
    noexcept(noexcept(c ? std::forward<T>(t) : std::forward<F>(f)))
    -> decltype(c ? std::forward<T>(t) : std::forward<F>(f))
  {
    return c ? std::forward<T>(t) : std::forward<F>(f);
  }

  template <typename C, typename T, typename F>
  static constexpr auto call(C &&c, T &&t, F &&f)
    noexcept(noexcept(c ? std::forward<T>(t) : std::forward<F>(f)))
    -> decltype(c ? std::forward<T>(t) : std::forward<F>(f))
  {
    return c ? std::forward<T>(t) : std::forward<F>(f);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_copy {
  template <typename T, typename... Args>
  constexpr T *operator ()(T *to, Args const &...args) const
    noexcept(noexcept(new (to) T(args...)))
  {
    return new (to) T(args...);
  }

  template <typename T, typename... Args>
  static constexpr T *call(T *to, Args const &...args)
    noexcept(noexcept(new (to) T(args...)))
  {
    return new (to) T(args...);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_move {
  template <typename T, typename U>
  constexpr T *operator ()(T *to, U &&from) const
    noexcept(noexcept(new (to) T(std::move(from))))
  {
    return new (to) T(std::move(from));
  }

  template <typename T, typename U>
  static constexpr T *call(T *to, U &&from)
    noexcept(noexcept(new (to) T(std::move(from))))
  {
    return new (to) T(std::move(from));
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_forward {
  template <typename T, typename... Args>
  constexpr T *operator ()(T *to, Args &&...args) const
    noexcept(noexcept(new (to) T(std::forward<Args>(args)...)))
  {
    return new (to) T(std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  static constexpr T *call(T *to, Args &&...args)
    noexcept(noexcept(new (to) T(std::forward<Args>(args)...)))
  {
    return new (to) T(std::forward<Args>(args)...);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct copy_assign {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS const &rhs) const
    noexcept(noexcept(lhs = rhs))
  {
    return lhs = rhs;
  }

  template <typename LHS, typename RHS>
  static constexpr LHS &call(LHS &lhs, RHS const &rhs)
    noexcept(noexcept(lhs = rhs))
  {
    return lhs = rhs;
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct move_assign {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS &&rhs) const
    noexcept(noexcept(lhs = std::move(rhs)))
  {
    return lhs = std::move(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr LHS &call(LHS &lhs, RHS &&rhs)
    noexcept(noexcept(lhs = std::move(rhs)))
  {
    return lhs = std::move(rhs);
  }
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct forward_assign {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS &&rhs) const
    noexcept(noexcept(lhs = std::forward<RHS>(rhs)))
  {
    return lhs = std::forward<RHS>(rhs);
  }

  template <typename LHS, typename RHS>
  static constexpr LHS &call(LHS &lhs, RHS &&rhs)
    noexcept(noexcept(lhs = std::forward<RHS>(rhs)))
  {
    return lhs = std::forward<RHS>(rhs);
  }
};

} // namespace fn {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_functional_functional_h
